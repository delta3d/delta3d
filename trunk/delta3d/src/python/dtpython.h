// dtpython.h: Private header file for bindings.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>
#include <osg/ref_ptr>

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
