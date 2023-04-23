#ifndef _EXPR_H_
#define _EXPR_H_

#include "ast/CType.h"
#include <memory>

class ArrayExpr;
class ConstExpr;
class IdentExpr;
class IROperand;
class ASTVisitor;


class Expr
{
public:
    virtual ~Expr() {}

    virtual void Accept(ASTVisitor*) {}
    virtual bool IsLVal() const { return false; }
    virtual bool IsConstant() const { return false; }
    virtual bool IsIdentifier() const { return false; }
    virtual bool IsSubscript() const { return false; }

    virtual ConstExpr* ToConstant() { return nullptr; }
    virtual IdentExpr* ToIdentifier() { return nullptr; }
    virtual ArrayExpr* ToSubscript() { return nullptr; }
    virtual const ConstExpr* ToConstant() const { return nullptr; }
    virtual const IdentExpr* ToIdentifier() const { return nullptr; }
    virtual const ArrayExpr* ToSubscript() const { return nullptr; }

    const auto& Type() const { return type_; }
    auto& Type() { return type_; }
    const CType* RawType() const { return type_.get(); }
    auto Val() const { return val_; }
    auto& Val() { return val_; }


protected:
    const IROperand* val_{};
    std::shared_ptr<CType> type_;
};

#endif // _EXPR_H_
