#ifndef _DTDIS_LIBRARY_ENTRY_H_
#define _DTDIS_LIBRARY_ENTRY_H_

#include <dtUtil/librarysharingmanager.h>        // for member
#include <dtCore/refptr.h>                       // 

namespace dtDIS
{
   namespace details
   {
      /// a data structure for holding library symbols to create and destroy an object.
      template<typename CreateT>
      struct LibraryRegistry
      {
         typedef CreateT CreatedType;
         typedef CreatedType* (*CREATE_FUNCTION)();
         typedef void (*DESTROY_FUNCTION)(CreatedType* plugin);

         /// does not allocate any memory or do any symbol assignments
         LibraryRegistry()
            : mCreator( NULL )
            , mDestroyer( NULL )
            , mHandle( NULL )
            , mCreated( NULL )
         {
         }

         /// does not attempt to clean up memory for the CreatedType member, mCreated.
         ~LibraryRegistry()
         {
            mCreator = NULL;
            mDestroyer = NULL;
            mHandle = NULL;
            mCreated = NULL;
         }

         /// the function object to request allocation of the CreatedType
         CREATE_FUNCTION mCreator;

         /// the function object to request deallocation of the CreatedType
         DESTROY_FUNCTION mDestroyer;

         /// the utility instance which finds the library symbols
         dtCore::RefPtr<dtUtil::LibrarySharingManager::LibraryHandle> mHandle;

         /// the object to be managed with the create and destroy library symbols
         CreatedType* mCreated;
      };
   }
}

#endif  // _DTDIS_LIBRARY_ENTRY_H_

