// Generalized functor implementation helpers.
// Copyright Aleksei Trunov 2005
// Use, copy, modify, distribute and sell it for free.

#ifndef _DTUTIL_FUNTRAITS_H_
#define _DTUTIL_FUNTRAITS_H_

#include <dtUtil/generic.h>

namespace dtUtil
{

// Function traits

template <typename T> struct FunTraits;

#if (defined(_MSC_VER)) && (_MANAGED != 1) && (!defined (_WIN64))
template <typename R>
struct FunTraits<R (__cdecl *)()>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <typename R>
struct FunTraits<R (__stdcall *)()>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <typename R>
struct FunTraits<R (__fastcall *)()>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <typename R, typename P1>
struct FunTraits<R (__cdecl *)(P1)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <typename R, typename P1>
struct FunTraits<R (__stdcall *)(P1)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <typename R, typename P1>
struct FunTraits<R (__fastcall *)(P1)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <typename R, typename P1, typename P2>
struct FunTraits<R (__cdecl *)(P1, P2)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <typename R, typename P1, typename P2>
struct FunTraits<R (__stdcall *)(P1, P2)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <typename R, typename P1, typename P2>
struct FunTraits<R (__fastcall *)(P1, P2)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__cdecl *)(P1, P2, P3)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__stdcall *)(P1, P2, P3)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__fastcall *)(P1, P2, P3)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__cdecl *)(P1, P2, P3, P4)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__stdcall *)(P1, P2, P3, P4)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__fastcall *)(P1, P2, P3, P4)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__cdecl *)(P1, P2, P3, P4, P5)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__stdcall *)(P1, P2, P3, P4, P5)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__fastcall *)(P1, P2, P3, P4, P5)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__cdecl *)(P1, P2, P3, P4, P5, P6)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__stdcall *)(P1, P2, P3, P4, P5, P6)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__fastcall *)(P1, P2, P3, P4, P5, P6)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__cdecl *)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__stdcall *)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__fastcall *)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <class O, typename R>
struct FunTraits<R (__cdecl O::*)() const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R>
struct FunTraits<R (__stdcall O::*)() const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R>
struct FunTraits<R (__fastcall O::*)() const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (__cdecl O::*)(P1) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (__stdcall O::*)(P1) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (__fastcall O::*)(P1) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (__cdecl O::*)(P1, P2) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (__stdcall O::*)(P1, P2) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (__fastcall O::*)(P1, P2) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4, P5) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4, P5) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4, P5) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4, P5, P6) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4, P5, P6) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4, P5, P6) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4, P5, P6, P7) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4, P5, P6, P7) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4, P5, P6, P7) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <class O, typename R>
struct FunTraits<R (__cdecl O::*)()>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R>
struct FunTraits<R (__stdcall O::*)()>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R>
struct FunTraits<R (__fastcall O::*)()>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (__cdecl O::*)(P1)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (__stdcall O::*)(P1)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (__fastcall O::*)(P1)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (__cdecl O::*)(P1, P2)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (__stdcall O::*)(P1, P2)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (__fastcall O::*)(P1, P2)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4, P5)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4, P5)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4, P5)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4, P5, P6)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4, P5, P6)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4, P5, P6)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__cdecl O::*)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__stdcall O::*)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (__fastcall O::*)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
#else // #if (defined(_MSC_VER)) && (_MANAGED != 1) && (!defined (_WIN64))
template <typename R>
struct FunTraits<R (*)()>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <typename R, typename P1>
struct FunTraits<R (*)(P1)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <typename R, typename P1, typename P2>
struct FunTraits<R (*)(P1, P2)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (*)(P1, P2, P3)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (*)(P1, P2, P3, P4)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (*)(P1, P2, P3, P4, P5)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (*)(P1, P2, P3, P4, P5, P6)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (*)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef NullType ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P6) TypeListType;
};
#endif //#if (defined(_MSC_VER)) && (_MANAGED != 1) && (!defined (_WIN64))

template <class O, typename R>
struct FunTraits<R (O::*)()>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (O::*)(P1)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (O::*)(P1, P2)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (O::*)(P1, P2, P3)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (O::*)(P1, P2, P3, P4)>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (O::*)(P1, P2, P3, P4, P5)>
{
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (O::*)(P1, P2, P3, P4, P5, P6)>
{
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (O::*)(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};
template <class O, typename R>
struct FunTraits<R (O::*)() const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef NullType TypeListType;
};
template <class O, typename R, typename P1>
struct FunTraits<R (O::*)(P1) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef TYPELIST_1(P1) TypeListType;
};
template <class O, typename R, typename P1, typename P2>
struct FunTraits<R (O::*)(P1, P2) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef TYPELIST_2(P1, P2) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3>
struct FunTraits<R (O::*)(P1, P2, P3) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef TYPELIST_3(P1, P2, P3) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunTraits<R (O::*)(P1, P2, P3, P4) const>
{
   typedef O ObjType;
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef TYPELIST_4(P1, P2, P3, P4) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunTraits<R (O::*)(P1, P2, P3, P4, P5) const>
{
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef TYPELIST_5(P1, P2, P3, P4, P5) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunTraits<R (O::*)(P1, P2, P3, P4, P5, P6) const>
{
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef TYPELIST_6(P1, P2, P3, P4, P5, P6) TypeListType;
};
template <class O, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunTraits<R (O::*)(P1, P2, P3, P4, P5, P6, P7) const>
{
   typedef R ResultType;
   typedef P1 Parm1;
   typedef P2 Parm2;
   typedef P3 Parm3;
   typedef P4 Parm4;
   typedef P5 Parm5;
   typedef P6 Parm6;
   typedef P7 Parm7;
   typedef TYPELIST_7(P1, P2, P3, P4, P5, P6, P7) TypeListType;
};

}

#endif // _DTUTIL_FUNTRAITS_H_
