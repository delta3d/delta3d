
#include <dtCore/sigslot.h>

#include <OpenThreads/ReentrantMutex>

namespace sigslot
{
   static OpenThreads::ReentrantMutex gMutex;

   /////////////////////////////////////////////////
   // The multi threading policies only get compiled in if they are enabled.
   multi_threaded_global::multi_threaded_global()
   {
   }

   /////////////////////////////////////////////////
   multi_threaded_global::multi_threaded_global(const multi_threaded_global&)
   {
   }

   /////////////////////////////////////////////////
   multi_threaded_global::~multi_threaded_global()
   {
   }

   /////////////////////////////////////////////////
   void multi_threaded_global::lock()
   {
      gMutex.lock();
   }

   /////////////////////////////////////////////////
   void multi_threaded_global::unlock()
   {
      gMutex.unlock();
   }


   class multi_threaded_local_impl
   {
   public:
      OpenThreads::ReentrantMutex mMutex;
   };

   /////////////////////////////////////////////////
   multi_threaded_local::multi_threaded_local()
   : m_impl(new multi_threaded_local_impl)
   {
   }

   /////////////////////////////////////////////////
   multi_threaded_local::multi_threaded_local(const multi_threaded_local&)
   : m_impl(new multi_threaded_local_impl)
   {
   }

   /////////////////////////////////////////////////
   multi_threaded_local::~multi_threaded_local()
   {
      delete m_impl;
      m_impl = NULL;
   }

   /////////////////////////////////////////////////
   void multi_threaded_local::lock()
   {
      m_impl->mMutex.lock();
   }

   /////////////////////////////////////////////////
   void multi_threaded_local::unlock()
   {
      m_impl->mMutex.unlock();
   }

}
