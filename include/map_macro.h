//
// Created by umar on 17.03.24.
//

#ifndef DBG_MAP_MACRO_H
#define DBG_MAP_MACRO_H

#define MAP_OUT

#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0 (EVAL0 (EVAL0 (__VA_ARGS__))) // Evaluates 4 times, 3 EVAL0 calls and 1 EVAL1 call
#define EVAL2(...) EVAL1 (EVAL1 (EVAL1 (__VA_ARGS__))) //13, Each EVAL1 call is evaluated 4 times, 1 evaluation at EVAL2 call 4*3+1
#define EVAL3(...) EVAL2 (EVAL2 (EVAL2 (__VA_ARGS__))) //40
#define EVAL4(...) EVAL3 (EVAL3 (EVAL3 (__VA_ARGS__))) //121
#define EVAL(...)  EVAL4 (EVAL4 (EVAL4 (__VA_ARGS__))) //364
//In total 364 MACRO evauluations, one additional evaluation with A(x) => 365 'x' in total
// toy recursive call;
#define A(x) x GET_NEXT(x,B) MAP_OUT(x)
#define B(x) x GET_NEXT(x,B) MAP_OUT(x)

//if peek is () GETEND() is invoked which resolve in GET_NEXT0(0,GETEND2,MACFUNC,0) and next becomes GETEND2
//however remember in MAPX call would be "#x GETEND2 ((),)" where peek is () and VA_ARGS are nothing
//to get red of that GETEND2(...) to nothing is needed.
//if peek is not (), then GET_NEXT1 will resolve in GET_NEXT0(GETEND peek , MACFUNC, 0) which will resolve in alternate MACFUNC call
//MAP_OUT is needed here otherwise the next call won't proceed. gcc -E option lets you see the result.
#define GETEND2(...)
#define GETEND() 0, GETEND2
#define GET_NEXT0(peek, next, ...) next MAP_OUT //Not sure why MAP_OUT is needed; check https://github.com/swansontec/map-macro
#define GET_NEXT1(peek, MACFUNC,...) GET_NEXT0(peek, MACFUNC, 0) // if not then GET
#define GET_NEXT(peek, MACFUNC) GET_NEXT1(GETEND peek, MACFUNC) //if peek is () then MACRO call to GETEND ();


//GET_NEXT has to get based on peek value, either a call to alternate MAP0/1 macro (provided as input to GET_NEXT) or No call

#define MAP1(x,peek,...) #x, GET_NEXT(peek, MAP0)(peek,__VA_ARGS__)
#define MAP0(x,peek,...) #x, GET_NEXT(peek, MAP1)(peek,__VA_ARGS__)

#define MK_STRING(...) EVAL(MAP1(__VA_ARGS__,()))

#define ENUM_MAP1(name,x,peek,...) name::x, GET_NEXT(peek, ENUM_MAP0)(name,peek,__VA_ARGS__)
#define ENUM_MAP0(name,x,peek,...) name::x, GET_NEXT(peek, ENUM_MAP1)(name,peek,__VA_ARGS__)

#define MK_ENUM_ARR(name,...) EVAL(ENUM_MAP1(name, __VA_ARGS__,()))

#define MK_ENUM(name, ... )                     \
            enum class name {__VA_ARGS__};     \
            constexpr std::array name##_to_strings {MK_STRING(__VA_ARGS__)}; \
			constexpr std::array name##_enum_arr {MK_ENUM_ARR(name,__VA_ARGS__)};  \

#endif //DBG_MAP_MACRO_H
