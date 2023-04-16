#ifndef _SIMPLE_ALLOC_H_
#define _SIMPLE_ALLOC_H_

#include "pass/Pass.h"
#include "pass/RegAlloc.h"
#include "IR/Instr.h"
#include <unordered_map>


// the SimpleAlloc class do register allocation using the 3-TOSCA algorithm.
// DO NOT use it if any optimization pass is used, since the allocator just
// perform allocation over the original not-so-SSAic IR, and it treats the
// registers that are not given by alloca as temp registers. It'll run into
// problems if some of these registers are used more than once.
// For the TSOCA algorithm, see https://www.zhihu.com/question/29355187/answer/51935409.

class SimpleAlloc : public Pass, private x64Alloc
{
public:
    void Init(Module*) override;
    void Execute() override;

private:
    const x64* IRTox64(const IROperand*);

    int Allocate(BasicBlock*, const Register*);
    int FAllocate(BasicBlock*, const Register*);
    inline void BinaryAllocaHelper(BinaryInstr*);
    inline void FBinaryAllocaHelper(BinaryInstr*);

    Function* curfunc_{};
    BasicBlock* curbb_{};

    // map a virtual register to where?
    std::unordered_map<const Register*, const x64*> addrmap_{};

    // the stack cache; implemented as a ring buffer.
    // the three registers used are rax, rdx, rcx, respectively.
    const x64Reg* scache_[3]{};
    int index_{};
    // for float-points, the three registers used is xmm0, xmm1, xmm2.
    const x64Reg* fscache_[3]{};
    int findex_{};

private:
    void VisitFunction(Function*) override;
    void VisitBasicBlock(BasicBlock*) override;

    void VisitAddInstr(AddInstr*) override;
    void VisitFaddInstr(FaddInstr*) override;
    void VisitSubInstr(SubInstr*) override;
    void VisitFsubInstr(FsubInstr*) override;
    void VisitMulInstr(MulInstr*) override;
    void VisitFmulInstr(FmulInstr*) override;
    void VisitDivInstr(DivInstr*) override;
    void VisitFdivInstr(FdivInstr*) override;
    void VisitModInstr(ModInstr*) override;
    void VisitShlInstr(ShlInstr*) override;
    void VisitLshrInstr(LshrInstr*) override;
    void VisitAshrInstr(AshrInstr*) override;
    void VisitAndInstr(AndInstr*) override;
    void VisitOrInstr(OrInstr*) override;
    void VisitXorInstr(XorInstr*) override;

    void VisitAllocaInstr(AllocaInstr*) override;
    void VisitLoadInstr(LoadInstr*) override;
    void VisitStoreInstr(StoreInstr*) override;

    void VisitTruncInstr(TruncInstr*) override;
    void VisitFtruncInstr(FtruncInstr*) override;

    void VisitZextInstr(ZextInstr*) override;
    void VisitSextInstr(SextInstr*) override;
    void VisitFextInstr(FextInstr*) override;
    void VisitFtoUInstr(FtoUInstr*) override;
    void VisitFtoSInstr(FtoSInstr*) override;

    void VisitUtoFInstr(UtoFInstr*) override;
    void VisitStoFInstr(StoFInstr*) override;
    void VisitPtrtoIInstr(PtrtoIInstr*) override;
    void VisitItoPtrInstr(ItoPtrInstr*) override;
    void VisitBitcastInstr(BitcastInstr*) override;
};

#endif // _SIMPLE_ALLOC_H_
