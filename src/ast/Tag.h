#ifndef _TAG_H_
#define _TAG_H_

enum class Tag
{
    _char = 1, _short = 2, _int = 4, _long = 8, _longlong = 24,
    _float = 32, _double = 64, _unsigned = 128, _signed = 256,
    _bool = 512, _void = 1024,

    _typedef, _extern, _static, _thread_local,
    
    _auto, _register, _complex, _imaginary,
    
    _const, _restrict, _volatile, _atomic,

    arrow, inc, dec, dot,
    _and, _or, star, plus, minus, tilde, exclamation, 
    slash, percent, lshift, rshift, greathan,
    lessthan, greatequal, lessequal, equal,
    notequal, cap,
    incl_or, logical_and, logical_or,
    _sizeof, _alignof,
    left, right,

    assign, mul_assign, div_assign, mod_assign, add_assign,
    sub_assign, left_assign, right_assign, and_assign,
    xor_assign, or_assign
};

#endif // _TAG_H_