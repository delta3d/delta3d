#ifndef _DTDIS_CREATE_DESTROY_POLICY_H_
#define _DTDIS_CREATE_DESTROY_POLICY_H_

#include <dtDIS/libraryregistry.h>          // for typedef
#include <dtUtil/librarysharingmanager.h>    // for loading/unloading libraries
#include <dtUtil/log.h>               // for debugging
#include <cstddef>                           // for NULL definition

namespace dtDIS
{
   namespace details
   {
      /// responsible for obtaining the symbols to be used by client code.
      template<typename PlugT>
      class CreateDestroyPolicy
      {
         const std::string mCreateName;
         const std::string mDestroyName;

      public:
         typedef dtDIS::details::LibraryRegistry<PlugT> LibraryInterface;

         CreateDestroyPolicy();

         bool LoadSymbols( dtUtil::LibrarySharingManager::LibraryHandle* handle, LibraryInterface& pi );
      };
#include <dtDIS/createdestroypolicy.inl>

   }

}

#endif  // _DTDIS_CREATE_DESTROY_POLICY_H_
