/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
*
* This library is open source and may be redistributed and/or modified under  
* the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
* (at your option) any later version.  The full license is in LICENSE file
* included with this distribution, and on the openscenegraph.org website.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
* OpenSceneGraph Public License for more details.
*/

#ifndef DELTA_OBSERVER_PTR
#define DELTA_OBSERVER_PTR

// Delta3D ObserverPtr, a wrap of the OSG ObserverPtr but in the dtCore namespace.
//
//////////////////////////////////////////////////////////////////////

namespace dtCore 
{
   class Observer
   {
      public:
         virtual ~Observer() {}
         virtual void objectDeleted(void*) {}
   };

   /** Smart pointer for observed objects, that automatically set pointers to them to null when they deleted.*/
   template<class T>
   class ObserverPtr : public Observer
   {

   public:
      typedef T element_type;

      ObserverPtr() :_ptr(0L)                            {}
      ObserverPtr(T* t):_ptr(t)                          { if (_ptr) _ptr->addObserver(this); }
      ObserverPtr(const ObserverPtr& rp):Observer(), _ptr(rp._ptr)  { if (_ptr) _ptr->addObserver(this); }
      ~ObserverPtr()                                     { if (_ptr) _ptr->removeObserver(this); _ptr=0; }

      inline ObserverPtr& operator = (const ObserverPtr& rp)
      {
         if (_ptr==rp._ptr) return *this;
         if (_ptr) _ptr->removeObserver(this);

         _ptr = rp._ptr;
         if (_ptr) _ptr->addObserver(this);
         return *this;
      }

      inline ObserverPtr& operator = (T* ptr)
      {
         if (_ptr==ptr) return *this;
         if (_ptr) _ptr->removeObserver(this);

         _ptr = ptr;
         if (_ptr) _ptr->addObserver(this);

         return *this;
      }

      virtual void ObserverPtr(void* ptr)
      {
         if (_ptr==ptr)
         {
            _ptr = 0;
         }
      }

      // comparison operators for observer_ptr.
      inline bool operator == (const ObserverPtr& rp) const { return (_ptr==rp._ptr); }
      inline bool operator != (const ObserverPtr& rp) const { return (_ptr!=rp._ptr); }
      inline bool operator < (const ObserverPtr& rp) const { return (_ptr<rp._ptr); }
      inline bool operator > (const ObserverPtr& rp) const { return (_ptr>rp._ptr); }

      // comparison operator for const T*.
      inline bool operator == (const T* ptr) const { return (_ptr==ptr); }
      inline bool operator != (const T* ptr) const { return (_ptr!=ptr); }
      inline bool operator < (const T* ptr) const { return (_ptr<ptr); }
      inline bool operator > (const T* ptr) const { return (_ptr>ptr); }


      inline T& operator*()  { return *_ptr; }

      inline const T& operator*() const { return *_ptr; }

      inline T* operator->() { return _ptr; }

      inline const T* operator->() const   { return _ptr; }

      inline bool operator!() const   { return _ptr==0L; }

      inline bool valid() const       { return _ptr!=0L; }

      inline T* get() { return _ptr; }

      inline const T* get() const { return _ptr; }

   private:
      T* _ptr;
   };

}

#endif
