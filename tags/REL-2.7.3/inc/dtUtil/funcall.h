// Generalized functor implementation helpers.
// Copyright Aleksei Trunov 2005
// Use, copy, modify, distribute and sell it for free.

#ifndef _DTUTIL_FUNCALL_H_
#define _DTUTIL_FUNCALL_H_

#include <dtUtil/generic.h>

namespace dtUtil
{

// Functor calls helpers

template <class TList> struct CallParms;
template <>
struct CallParms<TYPELIST_0()>
{
   typedef dtUtil::InstantiateH<dtUtil::NullType, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make() { return ParmsListType(); }
};
template <typename P1>
struct CallParms<TYPELIST_1(P1)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1>::Type, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make(P1 p1)
   {
      return ParmsListType(p1);
   }
};
template <typename P1, typename P2>
struct CallParms<TYPELIST_2(P1, P2)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2>::Type, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make(P1 p1, P2 p2)
   {
      return ParmsListType(p1,
         typename dtUtil::TailAt<ParmsListType, 0>::Result(p2));
   }
};
template <typename P1, typename P2, typename P3>
struct CallParms<TYPELIST_3(P1, P2, P3)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3>::Type, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make(P1 p1, P2 p2, P3 p3)
   {
      return ParmsListType(p1,
         typename dtUtil::TailAt<ParmsListType, 0>::Result(p2,
         typename dtUtil::TailAt<ParmsListType, 1>::Result(p3)));
   }
};
template <typename P1, typename P2, typename P3, typename P4>
struct CallParms<TYPELIST_4(P1, P2, P3, P4)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4>::Type, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make(P1 p1, P2 p2, P3 p3, P4 p4)
   {
      return ParmsListType(p1,
         typename dtUtil::TailAt<ParmsListType, 0>::Result(p2,
         typename dtUtil::TailAt<ParmsListType, 1>::Result(p3,
         typename dtUtil::TailAt<ParmsListType, 2>::Result(p4))));
   }
};
template <typename P1, typename P2, typename P3, typename P4, typename P5>
struct CallParms<TYPELIST_5(P1, P2, P3, P4, P5)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4, P5>::Type, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
   {
      return ParmsListType(p1,
         typename dtUtil::TailAt<ParmsListType, 0>::Result(p2,
         typename dtUtil::TailAt<ParmsListType, 1>::Result(p3,
         typename dtUtil::TailAt<ParmsListType, 2>::Result(p4,
         typename dtUtil::TailAt<ParmsListType, 3>::Result(p5)))));
   }
};
template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct CallParms<TYPELIST_6(P1, P2, P3, P4, P5, P6)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4, P5, P6>::Type, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
   {
      return ParmsListType(p1,
         typename dtUtil::TailAt<ParmsListType, 0>::Result(p2,
         typename dtUtil::TailAt<ParmsListType, 1>::Result(p3,
         typename dtUtil::TailAt<ParmsListType, 2>::Result(p4,
         typename dtUtil::TailAt<ParmsListType, 3>::Result(p5,
         typename dtUtil::TailAt<ParmsListType, 4>::Result(p6))))));
   }
};
template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct CallParms<TYPELIST_7(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4, P5, P6, P7>::Type, dtUtil::TupleHolder> ParmsListType;
   static inline ParmsListType Make(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
   {
      return ParmsListType(p1,
         typename dtUtil::TailAt<ParmsListType, 0>::Result(p2,
         typename dtUtil::TailAt<ParmsListType, 1>::Result(p3,
         typename dtUtil::TailAt<ParmsListType, 2>::Result(p4,
         typename dtUtil::TailAt<ParmsListType, 3>::Result(p5,
         typename dtUtil::TailAt<ParmsListType, 4>::Result(p6,
         typename dtUtil::TailAt<ParmsListType, 5>::Result(p7)))))));
   }
};

template <typename CallType, typename R, class TList> struct FunctorCall;
template <typename CallType, typename R>
struct FunctorCall<CallType, R, TYPELIST_0()>
{
   typedef dtUtil::InstantiateH<dtUtil::NullType, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& /*parms*/)
   {
      return fun();
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& /*parms*/)
   {
      return ((*pobj).*memfun)();
   }
};
template <typename CallType, typename R, typename P1>
struct FunctorCall<CallType, R, TYPELIST_1(P1)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1>::Type, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& parms)
   {
      return fun(dtUtil::GetH<0>(parms).value);
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& parms)
   {
      return ((*pobj).*memfun)(dtUtil::GetH<0>(parms).value);
   }
};
template <typename CallType, typename R, typename P1, typename P2>
struct FunctorCall<CallType, R, TYPELIST_2(P1, P2)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2>::Type, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& parms)
   {
      return fun(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value);
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& parms)
   {
      return ((*pobj).*memfun)(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value);
   }
};
template <typename CallType, typename R, typename P1, typename P2, typename P3>
struct FunctorCall<CallType, R, TYPELIST_3(P1, P2, P3)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3>::Type, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& parms)
   {
      return fun(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value);
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& parms)
   {
      return ((*pobj).*memfun)(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value);
   }
};
template <typename CallType, typename R, typename P1, typename P2, typename P3, typename P4>
struct FunctorCall<CallType, R, TYPELIST_4(P1, P2, P3, P4)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4>::Type, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& parms)
   {
      return fun(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value);
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& parms)
   {
      return ((*pobj).*memfun)(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value);
   }
};
template <typename CallType, typename R, typename P1, typename P2, typename P3, typename P4, typename P5>
struct FunctorCall<CallType, R, TYPELIST_5(P1, P2, P3, P4, P5)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4, P5>::Type, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& parms)
   {
      return fun(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value,
         dtUtil::GetH<4>(parms).value);
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& parms)
   {
      return ((*pobj).*memfun)(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value,
         dtUtil::GetH<4>(parms).value);
   }
};
template <typename CallType, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct FunctorCall<CallType, R, TYPELIST_6(P1, P2, P3, P4, P5, P6)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4, P5, P6>::Type, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& parms)
   {
      return fun(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value,
         dtUtil::GetH<4>(parms).value,
         dtUtil::GetH<5>(parms).value);
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& parms)
   {
      return ((*pobj).*memfun)(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value,
         dtUtil::GetH<4>(parms).value,
         dtUtil::GetH<5>(parms).value);
   }
};
template <typename CallType, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct FunctorCall<CallType, R, TYPELIST_7(P1, P2, P3, P4, P5, P6, P7)>
{
   typedef dtUtil::InstantiateH<typename dtUtil::CreateTL<P1, P2, P3, P4, P5, P6, P7>::Type, dtUtil::TupleHolder> ParmsListType;
   template <class Fun> static inline R Call(Fun const& fun, ParmsListType& parms)
   {
      return fun(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value,
         dtUtil::GetH<4>(parms).value,
         dtUtil::GetH<5>(parms).value,
         dtUtil::GetH<6>(parms).value);
   }
   template <class PObj> static inline R Call(PObj const& pobj, CallType memfun, ParmsListType& parms)
   {
      return ((*pobj).*memfun)(
         dtUtil::GetH<0>(parms).value,
         dtUtil::GetH<1>(parms).value,
         dtUtil::GetH<2>(parms).value,
         dtUtil::GetH<3>(parms).value,
         dtUtil::GetH<4>(parms).value,
         dtUtil::GetH<5>(parms).value,
         dtUtil::GetH<6>(parms).value);
   }
};

}

#endif // _DTUTIL_FUNCALL_H_
