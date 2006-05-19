/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author Matthew W. Campbell
*/
#ifndef __Functor__h
#define __Functor__h

#include <dtUtil/functor.h>

namespace dtDAL 
{

    /**
     * This class is the base Functor class.  A Functor is an object that acts
     * like a function.  It is therefore, an object-oriented way of encapsulating
     * function pointers.  These can be quite handy for complex callback
     * mechanisms. Much of this Functor and its set of classes was inspired by
     * the paper: CALLBACKS IN C++ USING TEMPLATE FUNCTORS, by Rich Hickey
     * @par
     *    The current implementation of Functor supports the following types:<br>
     *    Functor0 - Function with no parameters and no return value. <br>
     *    Functor1 - Function with 1 parameter and no return value. <br>
     *    Functor2 - Function with 2 parameters and no return value. <br>
     *    Functor3 - Function with 3 parameters and no return value. <br>
     *    Functor4 - Function with 4 parameters and no return value. <br>
     *    Functor0Ret - Function with no parameters and a return value. <br>
     *    Functor1Ret - Function with 1 parameter and a return value. <br>
     *    Functor2Ret - Function with 2 parameters and a return value. <br>
     *    Functor3Ret - Function with 3 parameters and a return value. <br>
     *    Functor4Ret - Function with 4 parameters and a return value. <br>
     *    Functors with no return value should be created using the MakeFunctor
     *    function.  The functors with a return value should be created using
     *    the MakeFunctorRet function. Each function has three versions for each
     *    of the functor types.  These versions support const class member functions,
     *    non-const class member functions, and regular functions.  Once created,
     *    a functor can be treated like an ordinary function since each functor has
     *    an operator() which is paramaterized on both the parameters and return
     *    type.
     *  @note
     *    The functors are completely type-safe and work correctly when used
     *    with virtual functions.  They also are flexible such that a function
     *    with a return value on a non-return value functor will be safely
     *    ignored.
     */
    class Functor 
    {
    public:
        typedef void (Functor::*BaseMemFunc)();
        typedef void (*BaseFunc)();

        BaseFunc getBaseFunc() const { return f; }
        void *getCallee() const { return callee; }
        const char *getMemFunc() const { return memFunc; }

    protected:
        Functor() : f(0), callee(0) { }
        Functor(const void *c, BaseFunc f, const void *mf, size_t sz) {
            this->callee = (void *)c;
            if (this->callee) 
            {
                memcpy(this->memFunc,mf,sz);
            }
            else 
            {
                this->f = f;
            }
        }

        /**
         * Since each functor could hold a pointer to a regular functino or
         * a member function, we use a union.
         */
        union 
        {
            BaseFunc f;
            char memFunc[16];
        };

        /**
         * Generic pointer to a class instance who's member function the functor
         * should invoke.  This is typecast'd to the appropriate class in the
         * specific functor implementation.
         */
        void *callee;
    };

    ////////////////////////////////FUNCTOR0////////////////////////////////////
    /**
     * Functor which takes no parameters and returns no value.
     */
    class Functor0 : public Functor
    {
    public:
        Functor0() { }
        void operator()() const { call(*this);  }
    protected:
        typedef void (*CallFunc)(const Functor &);
        Functor0(CallFunc cf, const void *c, BaseFunc f, const void *mf, size_t sz) :
            Functor(c,f,mf,sz), call(cf) { }
    private:
        CallFunc call;
    };

    /**
     * Special case of Functor0 which is used to store a member function of a
     * class.
     */
    template <class Callee, class MemFunc>
    class MemberFunctor0 : public Functor0 
    {
    public:
        MemberFunctor0(Callee &c, const MemFunc &m) :
            Functor0(caller,&c,0,&m,sizeof(MemFunc)) { }
        static void caller(const Functor &functor) 
        {
            Callee *callee = (Callee *)functor.getCallee();
            MemFunc &memFunc(*(MemFunc*)(void *)(functor.getMemFunc()));
            (callee->*memFunc)();
        }

        typedef dtUtil::Functor<void, TYPELIST_0()> UtilFunctor0;
        UtilFunctor0 ToUtilFunctor()
        {
            return UtilFunctor0((MemFunc*)(void *)(getMemFunc()));
        }
        
    };

    /**
     * Special case of Functor0 which is used to store a regular function.
     */
    template <class Func>
    class FunctionFunctor0 : public Functor0 
    {
    public:
        FunctionFunctor0(Func f) : Functor0(caller,0,(BaseFunc)f,0,0) { }
        static void caller(const Functor &functor) 
        {
            (Func(functor.getBaseFunc()))();
        }
    };

    /**
     * Creates a Functor0 for a non-const member function.
     */
    template <class Callee, class TRT, class CallType>
    MemberFunctor0<Callee, TRT (CallType::*)()>
    MakeFunctor(Callee &callee, TRT (CallType::*f)()) 
    {
        typedef TRT (CallType::*MemFunc)();
        return MemberFunctor0<Callee,MemFunc>(callee,f);
    }

    /**
     * Creates a Functor0 for a const member function.
     */
    template <class Callee, class TRT, class CallType>
    MemberFunctor0<const Callee, TRT (CallType::*)() const>
    MakeFunctor(const Callee &callee, TRT (CallType::* const &f)() const) 
    {
        typedef TRT (CallType::*MemFunc)() const;
        return MemberFunctor0<const Callee,MemFunc>(callee,f);
    }

    /**
     * Creates a Functor0 for a regular function.
     */
    template <class TRT>
    FunctionFunctor0<TRT (*)()>
    MakeFunctor(TRT (*f)()) 
    {
        return FunctionFunctor0<TRT (*)()>(f);
    }

    ////////////////////////////////FUNCTOR1////////////////////////////////////
    /**
     * Functor which takes 1 parameter and returns no value.
     */
    template <class P1>
    class Functor1 : public Functor 
    {
    public:
        Functor1() { }
        void operator()(P1 p1) const { call(*this,p1);  }
    protected:
        typedef void (*CallFunc)(const Functor &, P1);
        Functor1(CallFunc cf, const void *c, BaseFunc f, const void *mf, size_t sz) :
            Functor(c,f,mf,sz), call(cf) { }
    private:
        CallFunc call;
    };

    /**
     * Special case of Functor1 which is used to store a member function of a
     * class.
     */
    template <class Callee, class MemFunc, class P1>
    class MemberFunctor1 : public Functor1<P1> 
    {
    public:
        MemberFunctor1(Callee &c, const MemFunc &m) :
            Functor1<P1>(caller,&c,0,&m,sizeof(MemFunc)) { }
        static void caller(const Functor &functor, P1 p1) 
        {
            Callee *callee = (Callee *)functor.getCallee();
            MemFunc &memFunc(*(MemFunc*)(void *)(functor.getMemFunc()));
            (callee->*memFunc)(p1);
        }
    };

    /**
     * Special case of Functor1 which is used to store a regular function.
     */
    template <class Func, class P1>
    class FunctionFunctor1 : public Functor1<P1> 
    {
    public:
        FunctionFunctor1(Func f) : Functor1<P1>(caller,0,(Functor::BaseFunc)f,0,0) { }
        static void caller(const Functor &functor, P1 p1) 
        {
            (Func(functor.getBaseFunc()))(p1);
        }
    };

    /**
     * Creates a Functor1 for a non-const member function.
     */
    template <class Callee, class TRT, class CallType, class P1>
    MemberFunctor1<Callee, TRT (CallType::*)(P1 p1),P1>
    MakeFunctor(Callee &callee, TRT (CallType::*f)(P1)) 
    {
        typedef TRT (CallType::*MemFunc)(P1);
        return MemberFunctor1<Callee,MemFunc,P1>(callee,f);
    }

    /**
     * Creates a Functor1 for a const member function.
     */
    template <class Callee, class TRT, class CallType, class P1>
    MemberFunctor1<const Callee, TRT (CallType::*)(P1 p1) const,P1>
    MakeFunctor(const Callee &callee, TRT (CallType::* const &f)(P1 p1) const) 
    {
        typedef TRT (CallType::*MemFunc)(P1) const;
        return MemberFunctor1<const Callee,MemFunc,P1>(callee,f);
    }

    /**
     * Creates a Functor1 for a regular function.
     */
    template <class TRT, class P1>
    FunctionFunctor1<TRT (*)(P1 p1),P1>
    MakeFunctor(TRT (*f)(P1)) 
    {
        return FunctionFunctor1<TRT (*)(P1 p1),P1>(f);
    }


    //////////////////////////FUNCTOR WITH RETURN VALUE/////////////////////////
    /**
     * Functor which takes no parameters and returns a value.
     */
    template <class RT>
    class Functor0Ret : public Functor 
    {
    public:
        Functor0Ret() { }
        RT operator()() const { return call(*this); }
    protected:
        typedef RT (*CallFunc)(const Functor &);
        Functor0Ret(CallFunc cf, const void *c, BaseFunc f, const void *mf, size_t sz) :
            Functor(c,f,mf,sz), call(cf) { }
    private:
        CallFunc call;
    };

    /**
     * Special case of Functor0Ret which is used to store a member function of a
     * class.
     */
    template <class Callee, class MemFunc, class RT>
    class MemberFunctor0Ret : public Functor0Ret<RT>
    {
    public:
        MemberFunctor0Ret(Callee &c, const MemFunc &m) :
            Functor0Ret<RT>(caller,&c,0,&m,sizeof(MemFunc)) { }
        static RT caller(const Functor &functor) {
            Callee *callee = (Callee *)functor.getCallee();
            MemFunc &memFunc(*(MemFunc*)(void *)(functor.getMemFunc()));
            return (callee->*memFunc)();
        }
    };

    /**
     * Special case of Functor0Ret which is used to store a regular function.
     */
    template <class Func, class RT>
    class FunctionFunctor0Ret : public Functor0Ret<RT> 
    {
    public:
        FunctionFunctor0Ret(Func f) : Functor0Ret<RT>(caller,0,(Functor::BaseFunc)f,0,0) { }
        static RT caller(const Functor &functor) 
        {
            return (Func(functor.getBaseFunc()))();
        }
    };

    /**
     * Creates a Functor0Ret for a non-const member function.
     */
    template <class Callee, class CallType, class RT>
    MemberFunctor0Ret<Callee, RT (CallType::*)(), RT>
    MakeFunctorRet(Callee &callee, RT (CallType::*f)()) 
    {
        typedef RT (CallType::*MemFunc)();
        return MemberFunctor0Ret<Callee,MemFunc,RT>(callee,f);
    }

    /**
     * Creates a Functor0Ret for a const member function.
     */
    template <class Callee, class CallType, class RT>
    MemberFunctor0Ret<const Callee, RT (CallType::*)() const, RT>
    MakeFunctorRet(const Callee &callee, RT (CallType::* const &f)() const) 
    {
        typedef RT (CallType::*MemFunc)() const;
        return MemberFunctor0Ret<const Callee,MemFunc,RT>(callee,f);
    }

    /**
     * Creates a Functor0Ret for a regular function.
     */
    template <class RT>
    FunctionFunctor0Ret<RT (*)(), RT>
    MakeFunctorRet(RT (*f)()) 
    {
        return FunctionFunctor0Ret<RT (*)(), RT>(f);
    }
}

#endif

