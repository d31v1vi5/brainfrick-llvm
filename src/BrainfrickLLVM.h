#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include <iostream>
#include <string>
#include <vector>
#include <stack>

class BrainfrickLLVM {
  public:
    BrainfrickLLVM() {
      moduleInit();
      setupExternFunctions();
      setupMainFunction();
      setupData();
    }

    void exec(const std::string& file) {
      compile("file");
      saveModuleToFile("./out.ll");
    }

    void compile(const std::string& file) {
      char sym;
      std::fstream fin(file, fstream::in);
      while (fin >> sym) {
        switch(sym) {
          case '<':
            decrementDataPointer();
            break;
          case '>':
            incrementDataPointer();
            break;
          case '+'
            incrementByteAtDataPointer();
            break;
          case '-':
            decrementByteAtDataPointer();
            break;
          case '.':
            outputByteAtDataPointer();
            break;
          case ',':
            break;
          case '[':
            beginLoop();
            break;
          case ']':
            endLoop();
            break;
        }
      }
      builder->CreateRet(builder->getInt32(0));
    }

  private:
    std::unique_ptr<llvm::LLVMContext> ctx;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    llvm::Function* mainFunc;
    llvm::AllocaInst* cellPtr;
    std::stack<llvm::BasicBlock *> basicblocks, exit_bbs;
    static const size_t data_size = 30000;

    void moduleInit() {
      ctx = std::make_unique<llvm::LLVMContext>();
      module = std::make_unique<llvm::Module>("BrainfrickLLVM", *ctx);
      builder = std::make_unique<llvm::IRBuilder<>>(*ctx);
    }

    void setupMainFunction() {
      auto mainFuncType = llvm::FunctionType::get(builder->getInt32Ty(), false);
      mainFunc = llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", *module);
      verifyFunction(*mainFunc);
      auto entry = llvm::BasicBlock::Create(*ctx, "entry", mainFunc);
      builder->SetInsertPoint(entry);
    }


    void setupData() {
      llvm::ArrayType *dataType = llvm::ArrayType::get(builder->getInt8Ty(), 30000);
      llvm::ConstantAggregateZero* constArray = llvm::ConstantAggregateZero::get(dataType);
      llvm::GlobalVariable *data =
			  new llvm::GlobalVariable(*module, dataType, false, llvm::GlobalValue::PrivateLinkage, constArray, "data");
      cellPtr = builder->CreateAlloca(builder->getInt8PtrTy(), 0, "cell_ptr");
    }


    void setupExternFunctions() {
      std::vector<llvm::Type *> putcharArgs = {builder->getInt8Ty()};
      llvm::FunctionType* putcharType = llvm::FunctionType::get(builder->getInt32Ty(), putcharArgs, false);
      llvm::FunctionCallee putcharFunc = module->getOrInsertFunction("putchar", putcharType);

      std::vector<llvm::Type* > getcharArgs = {};
      llvm::FunctionType* getcharType = llvm::FunctionType::get(builder->getInt32Ty(), getcharArgs, false);
      llvm::FunctionCallee getcharFunc = module->getOrInsertFunction("getchar", getcharType);
    }

    void incrementDataPointer() {
      llvm::Value* ptr = builder->CreateLoad(cellPtr->getAllocatedType(), cellPtr, "cell_ptr");
			llvm::Value* value = builder->CreateGEP(builder->getInt8Ty(), ptr, builder->getInt32(1));
      builder->CreateStore(value, cellPtr);
    }

    void decrementDataPointer() {
      llvm::Value* ptr = builder->CreateLoad(cellPtr->getAllocatedType(), cellPtr, "cell_ptr");
      llvm::Value* value = builder->CreateGEP(builder->getInt8Ty(), ptr, builder->getInt32(1));
      builder->CreateStore(value, cellPtr);
    }

    void incrementByteAtDataPointer() {
      llvm::Value* ptr = builder->CreateLoad(cellPtr->getAllocatedType(), cellPtr, "cell_value_ptr");
      llvm::Value* value = builder->CreateLoad(builder->getInt8Ty(), ptr, "cell_value");
      value = builder->CreateAdd(value, builder->getInt8(1));
      builder->CreateStore(value, ptr);
    }

    void decrementByteAtDataPointer() {
      llvm::Value* ptr = builder->CreateLoad(cellPtr->getAllocatedType(), cellPtr, "cell_value_ptr");
      llvm::Value* value = builder->CreateLoad(builder->getInt8Ty(), ptr, "cell_value");
      value = builder->CreateAdd(value, builder->getInt8(-1));
      builder->CreateStore(value, ptr);
    }

    void outputByteAtDataPointer() {
      llvm::Value* ptr = builder->CreateLoad(cellPtr->getAllocatedType(), cellPtr, "cell_value_ptr");
      llvm::Value* value = builder->CreateLoad(builder->getInt8Ty(), ptr, "cell_value");
      llvm::Function* putcharFunc = module->getFunction("putchar");
      std::vector<llvm::Value *> args = {value};
      builder->CreateCall(putcharFunc, args);
    }

    void beginLoop() {
      llvm::Value* ptr = builder->CreateLoad(cellPtr->getAllocatedType(), cellPtr, "cell_value_ptr");
      llvm::Value* value = builder->CreateLoad(builder->getInt8Ty(), ptr, "cell_value");
			llvm::Value* cmp = builder->CreateICmpNE(value, builder->getInt8(0));

      llvm::BasicBlock* bbLoop = llvm::BasicBlock::Create(*ctx, "", mainFunc);
			llvm::BasicBlock* bbExit = llvm::BasicBlock::Create(*ctx, "", mainFunc);
      builder->CreateCondBr(cmp, bbLoop, bbExit);
			builder->SetInsertPoint(bbLoop);
    }

    void endLoop() {
			llvm::Value* ptr = builder->CreateLoad(cellPtr->getAllocatedType(), cellPtr, "cell_value_ptr");
      llvm::Value* value = builder->CreateLoad(builder->getInt8Ty(), ptr, "cell_value");
			llvm::Value* cmp = builder->CreateICmpNE(value, builder->getInt8(0));

			llvm::BasicBlock* bbLoop = basicblocks.top();
			basicblocks.pop();
			llvm::BasicBlock *bbExit = exit_bbs.top();
			exit_bbs.pop();
			builder->CreateCondBr(cmp, bbLoop, bbExit);
			builder->SetInsertPoint(bbExit);
    }

    void saveModuleToFile(const std::string& fileName) {
      std::error_code errorCode;
      llvm::raw_fd_ostream outLL(fileName, errorCode);
    }
};

