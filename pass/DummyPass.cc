#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <utility>

// LLVM
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;
namespace {

class DummyPass : public ModulePass {
 public:
  static char ID;

  FunctionCallee DummyFuncIcmp;
  FunctionCallee DummyFuncGetc;

  void initVariables(Module &M);
  DummyPass() : ModulePass(ID) {}
  bool runOnModule(Module &M) override;
};

}  // namespace

char DummyPass::ID = 0;

void DummyPass::initVariables(Module &M) {
  LLVMContext &C = M.getContext();

  Type *Int8Ty = IntegerType::getInt8Ty(C);
  {
    FunctionType *DummyFuncTy = FunctionType::get(Int8Ty, {}, false);
    AttributeList AL;
    AL = AL.addAttribute(M.getContext(), AttributeList::FunctionIndex,
                         Attribute::NoUnwind);
    AL = AL.addAttribute(M.getContext(), AttributeList::FunctionIndex,
                         Attribute::ReadOnly);
    DummyFuncIcmp = M.getOrInsertFunction("dummy_func_icmp", DummyFuncTy, AL);
    DummyFuncGetc = M.getOrInsertFunction("dummy_func_getc", DummyFuncTy, AL);
  }
};

bool DummyPass::runOnModule(Module &M) {
  errs() << "Dummy pass\n";
  initVariables(M);

  for (auto &F : M) {
    std::vector<BasicBlock *> bb_list;
    for (auto bb = F.begin(); bb != F.end(); bb++) bb_list.push_back(&(*bb));

    std::vector<Instruction *> inst_list;
    for (BasicBlock *BB : bb_list) {
      for (auto inst = BB->begin(); inst != BB->end(); inst++) {
        inst_list.push_back(&(*inst));
      }
    }

    for (Instruction *Inst : inst_list) {
      IRBuilder<> IRB(Inst);
      if (isa<CmpInst>(Inst)) {
        CallInst *rand_call = IRB.CreateCall(DummyFuncIcmp, {});
      } else if (CallInst *call = dyn_cast<CallInst>(Inst)) {
        if (call->getCalledFunction()->getName() == "_IO_getc") {
          CallInst *rand_call = IRB.CreateCall(DummyFuncGetc, {});
        }
      }
    }
  }
  return true;
}

static void registerDummyPass(const PassManagerBuilder &,
                              legacy::PassManagerBase &PM) {
  PM.add(new DummyPass());
}

static RegisterPass<DummyPass> X("dummy_pass", "Dummy Pass", false, false);

static RegisterStandardPasses RegisterDummyPass(
    PassManagerBuilder::EP_FullLinkTimeOptimizationLast, registerDummyPass);

/*
static RegisterStandardPasses RegisterDummyPass(
    PassManagerBuilder::EP_OptimizerLast, registerDummyPass);

static RegisterStandardPasses RegisterDummyPass0(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registerDummyPass);
*/