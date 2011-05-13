// Copyright (C) 2002-2003 Gideon May (gideon@computer.org)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.
//
// patched by Brett Hartshorn and Loic Dachary
// Hacked up for use with Producer's Reference node, same as osg::Node

#ifndef PYOSG_HELD_PTR
#define PYOSG_HELD_PTR 1

#include <boost/python/to_python_converter.hpp>
#include <boost/python/implicit.hpp>
#include <iostream>

const int DEBUG = 0;
#include <osg/Node>

template<class T> class held_ptr
{
    public:

        typedef T element_type;

        held_ptr(T* t):_ptr(t) {
            if (DEBUG)  std::cerr << "held_ptr::held_ptr(T *):" << typeid(*_ptr).name() << std::endl;
            if (_ptr) _ptr->ref();
        }

        ~held_ptr() {
            if (DEBUG) {
                    //std::string name("");
                    //osg::Node * node = dynamic_cast<osg::Node*>(_ptr);
                    //if (node) name = node->getName();
                    std::cerr << "held_ptr::~held_ptr()  :"  << typeid(*_ptr).name() << "->" << std::endl;
            }
            if (_ptr) _ptr->unref();
        }

        inline T& operator*()  { 
            if (DEBUG) {
                    //std::string name("");
                    //osg::Node * node = dynamic_cast<osg::Node*>(_ptr);
                    //if (node) name = node->getName();
                    std::cerr << "held_ptr::*          :" << typeid(*_ptr).name() << "->" << std::endl;
            }
            return *_ptr; 
        }

        inline T * get() const {
            return _ptr;
        }

    private:
        T* _ptr;
};

// patch by Loic Dachary (loic@gnu.org) 29 Mar 2004
namespace boost {
  namespace python {
    template <class T> struct pointee<held_ptr<T> >
    {
      typedef T type;
    };

    namespace objects {
      template<class T> T* get_pointer( held_ptr<T> const & p )
      {
	return p.get();
      }

      template<class T> const volatile T * get_pointer(const volatile T *& p)
      {
	return p;
      }

      template<class T> T * get_pointer(T * const & p)
      {
	return p;
      }

      template<class T> T * get_pointer(std::auto_ptr<T> const& p)
      {
	return p.get();
      }
    }
  }
}

template <class T>
struct enum_to_int_converter
{
   static PyObject* convert(T const& x)
   {
          return PyInt_FromLong(x);
   }
};

template <class T> 
void enum_as_int()
{
        boost::python::to_python_converter<T, enum_to_int_converter<T> >();
        boost::python::implicitly_convertible<int, T>();
};

#endif // PYOSG_HELD_PTR

