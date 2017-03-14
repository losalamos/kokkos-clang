//===--- CloneChecker.cpp - Clone detection checker -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// CloneChecker is a checker that reports clones in the current translation
/// unit.
///
//===----------------------------------------------------------------------===//

#include "ClangSACheckers.h"
#include "clang/Analysis/CloneDetection.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/AnalysisManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"

using namespace clang;
using namespace ento;

namespace {
class CloneChecker
    : public Checker<check::ASTCodeBody, check::EndOfTranslationUnit> {
  mutable CloneDetector Detector;
  mutable std::unique_ptr<BugType> BT_Exact, BT_Suspicious;

public:
  void checkASTCodeBody(const Decl *D, AnalysisManager &Mgr,
                        BugReporter &BR) const;

  void checkEndOfTranslationUnit(const TranslationUnitDecl *TU,
                                 AnalysisManager &Mgr, BugReporter &BR) const;

  /// \brief Reports all clones to the user.
  void reportClones(BugReporter &BR, AnalysisManager &Mgr,
                    int MinComplexity) const;

  /// \brief Reports only suspicious clones to the user along with informaton
  ///        that explain why they are suspicious.
  void reportSuspiciousClones(BugReporter &BR, AnalysisManager &Mgr,
                              int MinComplexity) const;
};
} // end anonymous namespace

void CloneChecker::checkASTCodeBody(const Decl *D, AnalysisManager &Mgr,
                                    BugReporter &BR) const {
  // Every statement that should be included in the search for clones needs to
  // be passed to the CloneDetector.
  Detector.analyzeCodeBody(D);
}

void CloneChecker::checkEndOfTranslationUnit(const TranslationUnitDecl *TU,
                                             AnalysisManager &Mgr,
                                             BugReporter &BR) const {
  // At this point, every statement in the translation unit has been analyzed by
  // the CloneDetector. The only thing left to do is to report the found clones.

  int MinComplexity = Mgr.getAnalyzerOptions().getOptionAsInteger(
      "MinimumCloneComplexity", 10, this);
  assert(MinComplexity >= 0);

  bool ReportSuspiciousClones = Mgr.getAnalyzerOptions().getBooleanOption(
      "ReportSuspiciousClones", true, this);

  bool ReportNormalClones = Mgr.getAnalyzerOptions().getBooleanOption(
      "ReportNormalClones", true, this);

  if (ReportSuspiciousClones)
    reportSuspiciousClones(BR, Mgr, MinComplexity);

  if (ReportNormalClones)
    reportClones(BR, Mgr, MinComplexity);
}

static PathDiagnosticLocation makeLocation(const StmtSequence &S,
                                           AnalysisManager &Mgr) {
  ASTContext &ACtx = Mgr.getASTContext();
  return PathDiagnosticLocation::createBegin(
      S.front(), ACtx.getSourceManager(),
      Mgr.getAnalysisDeclContext(ACtx.getTranslationUnitDecl()));
}

void CloneChecker::reportClones(BugReporter &BR, AnalysisManager &Mgr,
                                int MinComplexity) const {

  std::vector<CloneDetector::CloneGroup> CloneGroups;
  Detector.findClones(CloneGroups, MinComplexity);

  if (!BT_Exact)
    BT_Exact.reset(new BugType(this, "Exact code clone", "Code clone"));

  for (CloneDetector::CloneGroup &Group : CloneGroups) {
    // We group the clones by printing the first as a warning and all others
    // as a note.
    auto R = llvm::make_unique<BugReport>(
        *BT_Exact, "Duplicate code detected",
        makeLocation(Group.Sequences.front(), Mgr));
    R->addRange(Group.Sequences.front().getSourceRange());

    for (unsigned i = 1; i < Group.Sequences.size(); ++i)
      R->addNote("Similar code here",
                      makeLocation(Group.Sequences[i], Mgr),
                      Group.Sequences[i].getSourceRange());
    BR.emitReport(std::move(R));
  }
}

void CloneChecker::reportSuspiciousClones(BugReporter &BR,
                                          AnalysisManager &Mgr,
                                          int MinComplexity) const {

  std::vector<CloneDetector::SuspiciousClonePair> Clones;
  Detector.findSuspiciousClones(Clones, MinComplexity);

  if (!BT_Suspicious)
    BT_Suspicious.reset(
        new BugType(this, "Suspicious code clone", "Code clone"));

  ASTContext &ACtx = BR.getContext();
  SourceManager &SM = ACtx.getSourceManager();
  AnalysisDeclContext *ADC =
      Mgr.getAnalysisDeclContext(ACtx.getTranslationUnitDecl());

  for (CloneDetector::SuspiciousClonePair &Pair : Clones) {
    // FIXME: We are ignoring the suggestions currently, because they are
    // only 50% accurate (even if the second suggestion is unavailable),
    // which may confuse the user.
    // Think how to perform more accurate suggestions?

    auto R = llvm::make_unique<BugReport>(
        *BT_Suspicious,
        "Potential copy-paste error; did you really mean to use '" +
            Pair.FirstCloneInfo.Variable->getNameAsString() + "' here?",
        PathDiagnosticLocation::createBegin(Pair.FirstCloneInfo.Mention, SM,
                                            ADC));
    R->addRange(Pair.FirstCloneInfo.Mention->getSourceRange());

    R->addNote("Similar code using '" +
                   Pair.SecondCloneInfo.Variable->getNameAsString() + "' here",
               PathDiagnosticLocation::createBegin(Pair.SecondCloneInfo.Mention,
                                                   SM, ADC),
               Pair.SecondCloneInfo.Mention->getSourceRange());

    BR.emitReport(std::move(R));
  }
}

//===----------------------------------------------------------------------===//
// Register CloneChecker
//===----------------------------------------------------------------------===//

void ento::registerCloneChecker(CheckerManager &Mgr) {
  Mgr.registerChecker<CloneChecker>();
}
