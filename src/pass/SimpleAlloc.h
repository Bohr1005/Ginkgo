#ifndef _SIMPLE_ALLOC_H_
#define _SIMPLE_ALLOC_H_

#include "pass/Pass.h"
#include "pass/DUInfo.h"
#include "pass/x64Alloc.h"
#include "visitir/IRVisitor.h"
#include "visitir/x64.h"
#include <cassert>
#include <set>
#include <unordered_map>

class BinaryInstr;
class ConvertInstr;
class Instr;
class Register;


// The SimpleAlloc is a naive register allocator inspired from the 3-TOSCA algorithm.
// It uses only three registers, and maps one of the spare registers to an IR operator.
// Once the IR operator is used, the mapped register is considered free and marked
// as spare again. This method can lead to problems in many situations. However,
// luckily, the original not-so-SSAic IR generated by the front-end holds
// a very strong guarantee that every operand, expect those given by alloca
// instructions, is assigned once and used exactly once. Therefore, this naive
// approach is bound to make correct and efficient allocations.
// Due to the limitation above, DO NOT use the allocator if some optimization
// passes that will make some IR operands reused are run. The allocator may simply
// overwrite the reused operands, leading to problems.
// For more information about 3-TOSCA, see https://www.zhihu.com/question/29355187/answer/51935409
// or the original paper (https://www.eecg.utoronto.ca/~jzhu/csc467/readings/ra-for-free.pdf).

class SimpleAlloc : public x64Alloc
{
public:
    SimpleAlloc(Module* m, Pass* du) : x64Alloc(m),
        info_(static_cast<DUInfo*>(du)), stackcache_(this, info_) {}

private:
    class StackCache
    {
    public:
        StackCache(SimpleAlloc* sa, DUInfo* d) : alloc_(sa), info_(d) {}

        RegTag SpareReg() const;
        RegTag SpareFReg() const;
        void Access(const Register*, const Instr*) const;
        void Map2Reg(const Register*, RegTag);
        void Map2Stack(const Register*, long offset);
        void Map2Stack(const Register*, size_t size, long offset);

    private:
        SimpleAlloc* alloc_;
        DUInfo* info_{};

        // map virtual registers to where? the value can be
        // either a register or a stack address.
        std::unordered_map<const Register*, const x64*> regmap_{};

        // the three registers used are rbx, r12, r13, respectively.
        mutable std::set<RegTag> intreg_{ RegTag::rbx, RegTag::r12, RegTag::r13 };
        // for float-points, the three registers used is xmm8, xmm9, xmm10.
        mutable std::set<RegTag> vecreg_{ RegTag::xmm8, RegTag::xmm9, RegTag::xmm10 };
    };

    void Allocate(const Register*);
    long AllocateOnX64Stack(x64Stack&, size_t, size_t);

    void BinaryAllocaHelper(BinaryInstr*);
    void ConvertAllocaHelper(ConvertInstr*);

    Function* curfunc_{};
    DUInfo* info_{};
    StackCache stackcache_{ this, nullptr };

private:
    void VisitFunction(Function*) override;
    void VisitBasicBlock(BasicBlock*) override;

    void VisitRetInstr(RetInstr*) override;
    void VisitBrInstr(BrInstr*) override;
    void VisitSwitchInstr(SwitchInstr*) override;
    void VisitCallInstr(CallInstr*) override;

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
    // void VisitGetValInstr(GetValInstr*) override;
    // void VisitSetValInstr(SetValInstr*) override;
    void VisitGetElePtrInstr(GetElePtrInstr*) override;

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

    void VisitIcmpInstr(IcmpInstr*) override;
    void VisitFcmpInstr(FcmpInstr*) override;
    void VisitSelectInstr(SelectInstr*) override;
    void VisitPhiInstr(PhiInstr*) override;
};

#endif // _SIMPLE_ALLOC_H_
