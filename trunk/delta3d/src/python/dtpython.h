// dtpython.h: Private header file for bindings.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>
#include <osg/ref_ptr>

#if !defined(_WIN32) && !defined(WIN32) && !defined(__WIN32__)
#include "ode/src/collision_kernel.h"
#include "ode/src/objects.h"
#endif

#include "dt.h"

namespace boost
{
   namespace python
   {
      template <class T> T* get_pointer(const osg::ref_ptr<T>& p)
      {
         return (T*)p.get();
      }
   }
}
