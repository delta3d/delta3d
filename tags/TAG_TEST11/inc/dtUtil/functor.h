// Generalized functor implementation. Concept inspired by Andrei Alexandrescu. 
// Copyright Aleksei Trunov 2005 
// Use, copy, modify, distribute and sell it for free.

#ifndef _DTUTIL_FUNCTOR_H_
#define _DTUTIL_FUNCTOR_H_

#include <dtUtil/generic.h>
#include <dtUtil/funtraits.h>
#include <dtUtil/funcall.h>
#include <cstddef>
#include <utility>  // for std::pair<>

namespace dtUtil
{

// Generalized functor class template 

template <typename R, class TList, unsigned int size = 4 * sizeof(void*)>
class Functor 
{
public:
   typedef R ResultType;
   typedef TList TypeListType;
   typedef typename CallParms<TList>::ParmsListType ParmsListType;
   // default construction, assignment and destruction
   Functor() : vptr_(0) {}
   ~Functor()
   {
      if (vptr_) vptr_->destroy_(*this); 
   }
   Functor(Functor const& src) 
   {
      vptr_ = src.vptr_ ? src.vptr_->clone_(src, *this) : NULL;
   }
   Functor& operator=(Functor const& src)
   {
      if (this != &src) {
         if (vptr_) vptr_->destroy_(*this); 
         vptr_ = src.vptr_ ? src.vptr_->clone_(src, *this) : NULL;
      }
      return *this;
   }
   // is-empty selector
   bool operator!() const { return vptr_ == NULL; }

   bool valid() const { return vptr_ != NULL; }

   // ctor for static fns and arbitrary functors 
   template <typename F> explicit Functor(F const& fun) 
   { 
      typedef FunctorImpl<F> StoredType;
      vptr_ = _init<StoredType>(fun);
   }
   // ctor for member fns (note: raw ptrs and smart ptrs are equally welcome in pobj)
   template <class P, typename MF> explicit Functor(P const& pobj, MF memfun) 
   {
      typedef MemberFnImpl<P, MF> StoredType;
      vptr_ = _init<StoredType>(std::pair<P, MF>(pobj, memfun));
   }
   // calls 
   typedef typename dtUtil::TypeAtNonStrict<TList, 0, dtUtil::NullType>::Result Parm1;
   typedef typename dtUtil::TypeAtNonStrict<TList, 1, dtUtil::NullType>::Result Parm2;
   typedef typename dtUtil::TypeAtNonStrict<TList, 2, dtUtil::NullType>::Result Parm3;
   typedef typename dtUtil::TypeAtNonStrict<TList, 3, dtUtil::NullType>::Result Parm4;
   typedef typename dtUtil::TypeAtNonStrict<TList, 4, dtUtil::NullType>::Result Parm5;
   typedef typename dtUtil::TypeAtNonStrict<TList, 5, dtUtil::NullType>::Result Parm6;
   typedef typename dtUtil::TypeAtNonStrict<TList, 6, dtUtil::NullType>::Result Parm7;
#define DoCall(parms) return vptr_->call_(*this, parms);
   inline R operator()(ParmsListType const& parms) const { DoCall(parms) }
   inline R operator()() const { DoCall(CallParms<TList>::Make()) }
   inline R operator()(Parm1 p1) const { DoCall(CallParms<TList>::Make(p1)) }
   inline R operator()(Parm1 p1, Parm2 p2) const { DoCall(CallParms<TList>::Make(p1, p2)) }
   inline R operator()(Parm1 p1, Parm2 p2, Parm3 p3) const { DoCall(CallParms<TList>::Make(p1, p2, p3)) }
   inline R operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4) const { DoCall(CallParms<TList>::Make(p1, p2, p3, p4)) }
   inline R operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5) const { DoCall(CallParms<TList>::Make(p1, p2, p3, p4, p5)) }
   inline R operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6) const { DoCall(CallParms<TList>::Make(p1, p2, p3, p4, p5, p6)) }
   inline R operator()(Parm1 p1, Parm2 p2, Parm3 p3, Parm4 p4, Parm5 p5, Parm6 p6, Parm7 p7) const { DoCall(CallParms<TList>::Make(p1, p2, p3, p4, p5, p6, p7)) }
private:
   // copying/destruction and calls implementation
   struct FunImplBase
   {
      struct VTable;
      struct VTable
      {
         void (*destroy_)(Functor const&);
         VTable* (*clone_)(Functor const&, Functor&);
         R (*call_)(Functor const&, ParmsListType);
      };
      // VTable vtbl_;   // not needed here and actually wastes space!
   };
   template <typename V, class Derived>
   struct FunStorageImpl : public FunImplBase
   {
      V val_;
      FunStorageImpl(V const& val) : val_(val) {}
      static void Destroy(Functor const& src) { src.val_.template destroy<Derived>(); }
      static typename FunImplBase::VTable* Clone(Functor const& src, Functor& dest) 
      { 
         Derived const& this_ = src.val_.template get<Derived const>();
         return dest._init<Derived>(this_.val_); 
      }
   };
   template <typename T>
   struct FunctorImpl : public FunStorageImpl<T, FunctorImpl<T> >
   {
      FunctorImpl(T const& val) : FunStorageImpl<T, FunctorImpl>(val) {}
      static R Call(Functor const& src, ParmsListType parms) 
      { 
         FunctorImpl const& this_ = src.val_.template get<FunctorImpl const>();
         return FunctorCall<T, R, TList>::Call(this_.val_, parms); 
      }
   };
   template <class P, typename T>
   struct MemberFnImpl : public FunStorageImpl<std::pair<P, T>, MemberFnImpl<P, T> >
   {
      MemberFnImpl(std::pair<P, T> const& val) : FunStorageImpl<std::pair<P, T>, MemberFnImpl>(val) {}
      static R Call(Functor const& src, ParmsListType parms) 
      { 
         MemberFnImpl const& this_ = src.val_.template get<MemberFnImpl const>();
         return FunctorCall<T, R, TList>::Call(this_.val_.first, this_.val_.second, parms); 
      }
   };
   // initialization helper
   template <class T, class V>
   typename FunImplBase::VTable* _init(V const& v)
   {
      // gcc obviously complained about the below code due to unused variables
      // so I'm removing it for now. As long as the unit tests pass, we should
      // be good. -osb
      //FunImplBase* pimpl = val_.template init<T>(v);
      //pimpl;
      val_.template init<T>(v);
      // throw away pimpl, we don't need it in this implementation
      static typename FunImplBase::VTable vtbl =
      {
         &T::Destroy,
         &T::Clone,
         &T::Call,
      };
      return &vtbl;
   }
   // typeless storage support
   struct Typeless
   {
      template <typename T> inline T* init1(T* v) { return new(getbuf()) T(v); }
      template <typename T, typename V> inline T* init(V const& v) { return new(getbuf()) T(v); }
       template <typename T> inline void destroy() const { (*reinterpret_cast<T const*>(getbuf())).~T(); }
      template <typename T> inline T const& get() const { return *reinterpret_cast<T const*>(getbuf()); }
      template <typename T> inline T& get() { return *reinterpret_cast<T*>(getbuf()); }
      void* getbuf() { return &buffer_[0]; }
      void const* getbuf() const { return &buffer_[0]; }
      unsigned char buffer_[size];
   };
   template <typename T>
   struct ByValue
   {
      template <typename V> inline static T* init(Typeless& val, V const& v) { return val.template init<T>(v); }
      inline static void destroy(Typeless const& val) { val.template destroy<T>(); }
      inline static T const& get(Typeless const& val) { return val.template get<T>(); }
      inline static T& get(Typeless& val) { return val.template get<T>(); }
   };
   template <typename T>
   struct NewAlloc
   {
      template <typename V> inline static T* init(Typeless& val, V const& v) { return *val.template init<T*>(new T(v)); }
      inline static void destroy(Typeless const& val) { delete val.template get<T*>(); }
      inline static T const& get(Typeless const& val) { return *val.template get<T const*>(); }
      inline static T& get(Typeless& val) { return *val.template get<T*>(); }
   };
   template <typename T>
   struct SelectStored 
   { 
      // TODO: it seems this is a good place to add alignment calculations
      typedef typename dtUtil::Select<
         sizeof(T)<=sizeof(Typeless), 
         ByValue<T>, 
         NewAlloc<T> 
      >::Result Type; 
   };
   struct Stored 
   { 
      template <typename T, typename V> inline T* init(V const& v) { return SelectStored<T>::Type::init(val_, v); }
      template <typename T> inline void destroy() const { SelectStored<T>::Type::destroy(val_); }
      template <typename T> inline T const& get() const { return SelectStored<T>::Type::get(val_); }
      template <typename T> inline T& get() { return SelectStored<T>::Type::get(val_); }
      Typeless val_;
   };
   Stored val_;
   typename FunImplBase::VTable* vptr_;
};

// Helper functor creation functions

template <typename CallType> inline
Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>
MakeFunctor(CallType fun)
{
   return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(fun);
}
template <typename CallType, class PObj> inline
Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType> 
MakeFunctor(CallType memfun, PObj* const pobj)
{
   return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(pobj, memfun);
}
template <typename CallType, class Fun> inline
Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>
MakeFunctor(Fun const& fun)
{
   return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(fun);
}
template <typename CallType, class PObj> inline
Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType> 
MakeFunctor(CallType memfun, PObj& pobj)
{
   return dtUtil::Functor<typename dtUtil::FunTraits<CallType>::ResultType, typename dtUtil::FunTraits<CallType>::TypeListType>(&pobj, memfun);
}

}

#endif // _DTUTIL_FUNCTOR_H_
