#ifndef _IR_GEN_H_
#define _IR_GEN_H_

#include "IR/IRBuilder.h"
#include "IR/Value.h"
#include "visitast/Scope.h"
#include "visitast/ASTVisitor.h"
#include <list>
#include <memory>
#include <stack>
#include <variant>

class Declaration;
class EnumSpec;
class StructUnionSpec;
class IROperand;
class IRType;
class Statement;


class IRGen : public ASTVisitor
{
public:
    IRGen() {}
    IRGen(std::string name) :
        transunit_(std::make_unique<Module>(name)) {}

    void VisitArrayDef(ArrayDef*) override;
    void VisitDeclSpec(DeclSpec*) override;
    void VisitDeclList(DeclList*) override;
    void VisitFuncDef(FuncDef*) override;
    void VisitObjDef(ObjDef*) override;
    void VisitParamList(ParamList*) override;
    void VisitPtrDef(PtrDef*) override;

    void VisitArrayExpr(ArrayExpr*) override;
    void VisitAssignExpr(AssignExpr*) override;
    void VisitBinaryExpr(BinaryExpr*) override;
    void VisitCallExpr(CallExpr*) override;
    void VisitCastExpr(CastExpr*) override;
    void VisitCondExpr(CondExpr*) override;
    void VisitConstant(ConstExpr*) override;
    void VisitEnumConst(EnumConst*) override;
    void VisitEnumList(EnumList*) override;
    void VisitExprList(ExprList*) override;
    void VisitIdentExpr(IdentExpr*) override;
    void VisitLogicalExpr(LogicalExpr*) override;
    void VisitUnaryExpr(UnaryExpr*) override;

    void VisitBreakStmt(BreakStmt*) override;
    void VisitCaseStmt(CaseStmt*) override;
    void VisitCompoundStmt(CompoundStmt*) override;
    void VisitContinueStmt(ContinueStmt*) override;
    void VisitDeclStmt(DeclStmt*) override;
    void VisitDoWhileStmt(DoWhileStmt*) override;
    void VisitExprStmt(ExprStmt*) override;
    void VisitForStmt(ForStmt*) override;
    void VisitGotoStmt(GotoStmt*) override;
    void VisitIfStmt(IfStmt*) override;
    void VisitLabelStmt(LabelStmt*) override;
    void VisitRetStmt(RetStmt*) override;
    void VisitSwitchStmt(SwitchStmt*) override;
    void VisitTransUnit(TransUnit*) override;
    void VisitWhileStmt(WhileStmt*) override;

    auto GetModule() { return std::move(transunit_); }


private:
    class CurrentEnv
    {
    public:
        CurrentEnv() {}
        CurrentEnv(std::variant<Function*, GlobalVar*> v) : env_(v) {}

        Function* GetFunction() { return std::get<0>(env_); }
        GlobalVar* GetGlobalVar() { return std::get<1>(env_); }

        std::string GetRegName() { return '%' + std::to_string(index_++); }
        std::string GetLabelName() { return std::to_string(index_++); }

        void PushStmt(Statement* s) { brkcntn_.push(s); }
        void PopStmt() { brkcntn_.pop(); }
        Statement* StmtStackTop() { return brkcntn_.top(); }

        void PushSwitch(SwitchInstr* i) { swtch_.push(i); }
        void PopSwitch() { swtch_.pop(); }
        SwitchInstr* SwitchStackTop() { return swtch_.top(); }

        void AddBrInstr4Ret(BrInstr* br) { ret_.push_back(br); }
        void AddLabelBlkPair(const std::string& s, BasicBlock* bb) { labelmap_.emplace(s, bb); }
        void AddBrLabelPair(BrInstr* br, const std::string& s) { gotomap_.emplace(br, s); }

        void Epilog(BasicBlock*);

    private:
        std::variant<Function*, GlobalVar*> env_{};

        // store pointers to statement nodes to make
        // br instructions generated by break and
        // continue statements to find their way to go
        std::stack<Statement*> brkcntn_{};
        // Map case and break in switch.
        std::stack<SwitchInstr*> swtch_{};
        // To which basic block does a label map?
        std::unordered_map<std::string, BasicBlock*> labelmap_{};
        // Where will this BrInstr go to?
        std::unordered_map<BrInstr*, std::string> gotomap_{};
        // store BrInstr* generated by RetInstr
        std::list<BrInstr*> ret_{};
        size_t index_{};
    };


    std::unique_ptr<CEnumType> EnumHelper(const EnumSpec*);
    void StructUnionHelper(const StructUnionSpec*);

    const Register* AllocaObject(const CType*, const std::string&);
    Function* AllocaFunc(const CFuncType*, const std::string&);

    const IROperand* LoadVal(Expr*);
    const Register* LoadAddr(Expr*);

    static void FillNullBlk(BrInstr*, BasicBlock*);
    static void Backpatch(std::list<BrInstr*>&, BasicBlock*);
    static void Merge(const std::list<BrInstr*>&, std::list<BrInstr*>&);

    ScopeStack scopestack_{};
    InstrBuilder ibud_{};
    BlockBuilder bbud_{};
    CurrentEnv env_{};

    std::unique_ptr<Module> transunit_{};
};

#endif // _IR_GEN_H_
