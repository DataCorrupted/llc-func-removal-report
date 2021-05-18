#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <unordered_set>
#include <utility>

// LLVM
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;
namespace {

class DummyPass : public ModulePass {
 public:
  static char ID;

  FunctionCallee DummyFunc;

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
    DummyFunc = M.getOrInsertFunction("__dummy_func", DummyFuncTy, AL);
  }
};

bool DummyPass::runOnModule(Module &M) {
  errs() << "Dummy pass";
  initVariables(M);

  for (auto &F : M) {
    std::vector<BasicBlock *> bb_list;
    for (auto bb = F.begin(); bb != F.end(); bb++) bb_list.push_back(&(*bb));

    for (auto bi = bb_list.begin(); bi != bb_list.end(); bi++) {
      BasicBlock *BB = *bi;
      std::vector<Instruction *> inst_list;

      for (auto inst = BB->begin(); inst != BB->end(); inst++) {
        Instruction *Inst = &(*inst);
        inst_list.push_back(Inst);
      }

      for (auto inst = inst_list.begin(); inst != inst_list.end(); inst++) {
        Instruction *Inst = *inst;
        errs() << *Inst << "\n";
        IRBuilder<> IRB(Inst);
        CallInst *rand_call = IRB.CreateCall(DummyFunc, {});
      }
    }
  }
  return true;
}

static void registerDummyPass(const PassManagerBuilder &,
                              legacy::PassManagerBase &PM) {
  PM.add(new DummyPass());
}

#ifdef LTO
static RegisterPass<DummyPass> X("dummy_pass", "Dummy Pass", false, false);

static RegisterStandardPasses RegisterDummyPass(
    PassManagerBuilder::EP_FullLinkTimeOptimizationLast, registerDummyPass);
#else
static RegisterStandardPasses RegisterDummyPass(
    PassManagerBuilder::EP_OptimizerLast, registerDummyPass);

static RegisterStandardPasses RegisterDummyPass0(
    PassManagerBuilder::EP_EnabledOnOptLevel0, registerDummyPass);
#endif