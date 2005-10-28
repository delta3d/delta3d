/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
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
*/

#ifndef DELTA_REF_PTR
#define DELTA_REF_PTR

// Delta3D RefPtr, a wrap of the OSG RefPtr but in the dtCore namespace.
//
//////////////////////////////////////////////////////////////////////

#include <ostream>

namespace dtCore {

   /** Smart pointer for handling referenced counted objects.*/
   template<class T>
   class RefPtr
   {

   public:
      typedef T element_type;

      RefPtr() :_ptr(0L) {}
      RefPtr(T* t):_ptr(t)              { if (_ptr) _ptr->ref(); }
      RefPtr(const RefPtr& rp):_ptr(rp._ptr)  { if (_ptr) _ptr->ref(); }
      ~RefPtr()                           { if (_ptr) _ptr->unref(); _ptr=0; }

      inline RefPtr& operator = (const RefPtr& rp)
      {
      if (_ptr==rp._ptr) return *this;
      T* tmp_ptr = _ptr;
      _ptr = rp._ptr;
      if (_ptr) _ptr->ref();
      // unref second to prevent any deletion of any object which might
      // be referenced by the other object. i.e rp is child of the
      // original _ptr.
      if (tmp_ptr) tmp_ptr->unref();
      return *this;
      }

      inline RefPtr& operator = (T* ptr)
      {
      if (_ptr==ptr) return *this;
      T* tmp_ptr = _ptr;
      _ptr = ptr;
      if (_ptr) _ptr->ref();
      // unref second to prevent any deletion of any object which might
      // be referenced by the other object. i.e rp is child of the
      // original _ptr.
      if (tmp_ptr) tmp_ptr->unref();
      return *this;
      }

      // comparison operators for RefPtr.
      inline bool operator == (const RefPtr& rp) const { return (_ptr==rp._ptr); }
      inline bool operator != (const RefPtr& rp) const { return (_ptr!=rp._ptr); }
      inline bool operator < (const RefPtr& rp) const { return (_ptr<rp._ptr); }
      inline bool operator > (const RefPtr& rp) const { return (_ptr>rp._ptr); }

      // comparison operator for const T*.
      inline bool operator == (const T* ptr) const { return (_ptr==ptr); }
      inline bool operator != (const T* ptr) const { return (_ptr!=ptr); }
      inline bool operator < (const T* ptr) const { return (_ptr<ptr); }
      inline bool operator > (const T* ptr) const { return (_ptr>ptr); }

      //added for by Delta3D
      friend inline std::ostream &operator<<(std::ostream &os,
         const RefPtr& rp)
      {
         os << rp._ptr;
         return os;
      }

      inline T& operator*()  { return *_ptr; }

      inline const T& operator*() const { return *_ptr; }

      inline T* operator->() { return _ptr; }

      inline const T* operator->() const   { return _ptr; }

      inline bool operator!() const	{ return _ptr==0L; }

      inline bool valid() const	{ return _ptr!=0L; }

      inline T* get() { return _ptr; }

      inline const T* get() const { return _ptr; }

      inline T* take() { return release();}

      inline T* release() { T* tmp=_ptr; if (_ptr) _ptr->unref_nodelete(); _ptr=0; return tmp;}

      private:
      T* _ptr;
     
   };
}

#endif // DELTA_REF_PTR
