#include "ast/CType.h"
#include "ast/Declaration.h"
#include "IR/IRType.h"
#include <memory>


bool QualType::SetToken(Tag t)
{
    switch (t)
    {
        case Tag::_const:
            token_ |= static_cast<unsigned>(QualTag::_const); return true;
        case Tag::_restrict:
            token_ |= static_cast<unsigned>(QualTag::_restrict); return true;
        case Tag::_volatile:
            token_ |= static_cast<unsigned>(QualTag::_volatile); return true;
        case Tag::_atomic:
            token_ |= static_cast<unsigned>(QualTag::_atomic); return true;
        default:
            return false;
    }
}


bool StorageType::SetToken(Tag t)
{
    if (token_) return false;
    switch (t)
    {
        case Tag::_static:
            token_ |= static_cast<unsigned>(StorageTag::_static); return true;
        case Tag::_extern:
            token_ |= static_cast<unsigned>(StorageTag::_extern); return true;
        case Tag::_typedef:
            token_ |= static_cast<unsigned>(StorageTag::_typedef); return true;
        case Tag::_thread_local:
            token_ |= static_cast<unsigned>(StorageTag::_thread_local); return true;
        case Tag::_auto:
            token_ |= static_cast<unsigned>(StorageTag::_auto); return true;
        case Tag::_register:
            token_ |= static_cast<unsigned>(StorageTag::_register); return true;
        default:
            return false;
    }
}


bool FuncSpec::SetSpec(Tag t)
{
    switch (t)
    {
    case Tag::_inline:
        token_ |= (unsigned)FuncTag::_inline; return true;
    case Tag::_noreturn:
        token_ |= (unsigned)FuncTag::_noreturn; return true;
    default: return false;
    }
}


CArithmType::CArithmType(TypeTag tt) : CType(CTypeId::arithm), type_(tt)
{
    switch (tt)
    {
    case TypeTag::int8: case TypeTag::uint8:
        size_ = 1; break;
    case TypeTag::int16: case TypeTag::uint16:
        size_ = 2; break;
    case TypeTag::int32: case TypeTag::uint32:
        size_ = 4; break;
    case TypeTag::int64: case TypeTag::uint64:
        size_ = 8; break;
    default: break;
    }
}

const IRType* CArithmType::ToIRType(Pool<IRType>*) const
{
    switch (type_)
    {
    case TypeTag::int8: return IntType::GetInt8(true);
    case TypeTag::int16: return IntType::GetInt16(true);
    case TypeTag::int32: return IntType::GetInt32(true);
    case TypeTag::int64: return IntType::GetInt64(true);
    case TypeTag::uint8: return IntType::GetInt8(false);
    case TypeTag::uint16: return IntType::GetInt16(false);
    case TypeTag::uint32: return IntType::GetInt32(false);
    case TypeTag::uint64: return IntType::GetInt64(false);
    case TypeTag::flt32: return FloatType::GetFloat32();
    case TypeTag::flt64: return FloatType::GetFloat64();
    }
    return nullptr;
}

bool CArithmType::Compatible(const CType& other) const
{
    auto arithm = other.As<CArithmType>();
    if (arithm) return size_ == arithm->size_;
    auto ptr = other.As<CPtrType>();
    if (ptr) return size_ == ptr->Size();
    return false;
}

bool CArithmType::operator>(const CArithmType& rhs) const
{
    if (IsInteger() && rhs.IsFloat()) return false;
    else if (IsFloat() && rhs.IsInteger()) return true;
    else if (IsInteger() && rhs.IsInteger())
        return this->size_ > rhs.size_;
    else if (IsFloat() && rhs.IsFloat())
        return this->size_ > rhs.size_;
    return false;
}

bool CArithmType::operator<(const CArithmType& rhs) const
{
    return !(*this > rhs);
}

std::string CArithmType::ToString() const
{
    /*std::string name = IsUnsigned() ? "unsigned " : "";
    if (raw_ & int(Tag::_bool))
        return "bool";
    if (raw_ & int(Tag::_char))
    {
        name += "char";
        return name;
    }
    if (raw_ & int(Tag::_short))
        name += "short ";
    if (raw_ & int(Tag::_long))
        name += "long ";
    if (raw_ & int(Tag::_longlong))
        name += "long long ";
    if (raw_ & int(Tag::_int))
    {
        name += "int";
        return name;
    }
    if (raw_ & int(Tag::_double))
        name += "double ";
    if (raw_ & int(Tag::_float))
        name += "float";
    
    return name;*/
    return "";
}


void CFuncType::AddParam(const CType* t)
{
    paramlist_[index_++] = std::move(t);
}

const FuncType* CFuncType::ToIRType(Pool<IRType>* pool) const
{
    auto functy = FuncType::GetFuncType(
        pool, ReturnType()->ToIRType(pool), Variadic());
    for (auto& param : paramlist_)
        if (!param->Is<CVoidType>())
            functy->AddParam(param->ToIRType(pool));
    return functy;
}

std::unique_ptr<CType> CFuncType::Clone() const
{
    auto func = std::make_unique<CFuncType>();
    func->Qual() = Qual();
    func->Storage() = Storage();
    func->variadic_ = variadic_;
    func->inline_ = inline_;
    func->noreturn_ = noreturn_;
    func->paramlist_ = paramlist_;
    func->return_ = std::move(return_->Clone());
    return std::move(func);
}


std::string CPtrType::ToString() const
{
    return "";
}

const PtrType* CPtrType::ToIRType(Pool<IRType>* pool) const
{
    auto point2 = point2_->ToIRType(pool);
    return PtrType::GetPtrType(pool, point2);
}

std::unique_ptr<CType> CPtrType::Clone() const
{
    auto ptr = std::make_unique<CPtrType>();
    ptr->Qual() = Qual();
    ptr->Storage() = Storage();
    ptr->point2_ = std::move(point2_->Clone());
    return std::move(ptr);
}


const ArrayType* CArrayType::ToIRType(Pool<IRType>* pool) const
{
    auto arrayof = arrayof_->ToIRType(pool);
    auto array = ArrayType::GetArrayType(pool, count_, arrayof);
    array->VariableLen() = variable_;
    array->Static() = static_;
    return array;
}

std::string CArrayType::ToString() const
{
    return "";
}

std::unique_ptr<CType> CArrayType::Clone() const
{
    auto array = std::make_unique<CArrayType>();
    array->Qual() = Qual();
    array->Storage() = Storage();
    array->arrayof_ = std::move(arrayof_->Clone());
    array->count_ = count_;
    array->variable_ = variable_;
    array->static_ = static_;
    return std::move(array);
}


std::string CEnumType::ToString() const
{
    return "";
}

const IntType* CEnumType::ToIRType(Pool<IRType>* pool) const
{
    return static_cast<const IntType*>(underlying_->ToIRType(pool));
}


const VoidType* CVoidType::ToIRType(Pool<IRType>*) const
{
    return VoidType::GetVoidType();
}
