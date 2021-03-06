/*
 * ###########################################################################
 * Copyright (c) 2015, Los Alamos National Security, LLC.
 * All rights reserved.
 * 
 *  Copyright 2010. Los Alamos National Security, LLC. This software was
 *  produced under U.S. Government contract DE-AC52-06NA25396 for Los
 *  Alamos National Laboratory (LANL), which is operated by Los Alamos
 *  National Security, LLC for the U.S. Department of Energy. The
 *  U.S. Government has rights to use, reproduce, and distribute this
 *  software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY,
 *  LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
 *  FOR THE USE OF THIS SOFTWARE.  If software is modified to produce
 *  derivative works, such modified software should be clearly marked,
 *  so as not to confuse it with the version available from LANL.
 *
 *  Additionally, redistribution and use in source and binary forms,
 *  with or without modification, are permitted provided that the
 *  following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * 
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided 
 *      with the distribution.
 *
 *    * Neither the name of Los Alamos National Security, LLC, Los
 *      Alamos National Laboratory, LANL, the U.S. Government, nor the
 *      names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior
 *      written permission.
 * 
 *  THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
 *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 * ########################################################################### 
 * 
 * Notes
 *
 * ##### 
 */ 

#ifndef CLANG_CODEGEN_IDEAS_RUNTIME_H
#define CLANG_CODEGEN_IDEAS_RUNTIME_H

#include "CodeGenModule.h"

namespace llvm {
  class Function;
}

namespace clang {
namespace CodeGen {
  class CodeGenModule;

  class CGIdeasRuntime{
  public:
    typedef std::vector<llvm::Type*> TypeVec;
    
    CGIdeasRuntime(llvm::Module& module);

    CGIdeasRuntime(CodeGen::CodeGenModule &CGM);
    
    ~CGIdeasRuntime();
    
    llvm::PointerType* PointerTy(llvm::Type* elementType);
    
    llvm::Value* GetNull(llvm::Type* T);
    
    llvm::Function* GetFunc(const std::string& funcName,
                            const TypeVec& argTypes,
                            llvm::Type* retType=0);
    
    llvm::PointerType* VoidPtrTy;
    llvm::IntegerType* Int1Ty;
    llvm::IntegerType* Int8Ty;
    llvm::IntegerType* Int32Ty;
    llvm::IntegerType* Int64Ty;
    llvm::Type* FloatTy;
    llvm::Type* DoubleTy;
    llvm::Type* VoidTy;
    llvm::Type* StringTy;

    llvm::FunctionType* QueueFuncTy;

    llvm::Function* CreateSynchFunc();
    llvm::Function* QueueFuncFunc();
    llvm::Function* FinishFuncFunc();
    llvm::Function* AwaitSynchFunc();

    llvm::Function* Debug1Func();

    llvm::Function* CudaInitKernelFunc();
    llvm::Function* CudaInitKernel2Func();
    llvm::Function* CudaAddViewFunc();
    llvm::Function* CudaAddArrayFunc();
    llvm::Function* CudaAddKernelViewFunc();
    llvm::Function* CudaAddKernelArrayFunc();
    llvm::Function* CudaCopyViewToDeviceFunc();
    llvm::Function* CudaCopyViewFromDeviceFunc();
    llvm::Function* CudaCopyArrayToDeviceFunc();
    llvm::Function* CudaCopyArrayFromDeviceFunc();
    llvm::Function* CudaAddKernelVarFunc();
    llvm::Function* CudaRunKernelFunc();
    llvm::Function* CudaAwaitKernelFunc();
    llvm::Function* CudaRunKernel2Func();

    llvm::Function* CudaLaunchDeviceFunc();

    llvm::Function* getSREGFunc(const std::string& suffix);
 
    llvm::Function* getBarrierFunc();   

  private:
    llvm::Module& module_;
  };
}
}
#endif // CLANG_CODEGEN_IDEAS_RUNTIME_H
