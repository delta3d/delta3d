/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007 John K. Grant
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
 * John K. Grant, April 2007.
 */

#ifndef __DELTA_DTDIS_CREATE_DESTROY_POLICY_H__
#define __DELTA_DTDIS_CREATE_DESTROY_POLICY_H__

#include <dtDIS/libraryregistry.h>          // for typedef
#include <dtUtil/librarysharingmanager.h>    // for loading/unloading libraries
#include <dtUtil/log.h>               // for debugging
#include <cstddef>                           // for NULL definition

namespace dtDIS
{
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
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

         bool LoadSymbols(const dtUtil::LibrarySharingManager::LibraryHandle* handle,
                          LibraryInterface& pi );
      };
#include <dtDIS/createdestroypolicy.inl>

   }
   ///@endcond

}

#endif  // __DELTA_DTDIS_CREATE_DESTROY_POLICY_H__
