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

#ifndef __DELTA_DTDIS_LIBRARY_ENTRY_H__
#define __DELTA_DTDIS_LIBRARY_ENTRY_H__

#include <dtUtil/librarysharingmanager.h>        // for member
#include <dtCore/refptr.h>                       // 

namespace dtDIS
{
   ///@cond DOXYGEN_SHOULD_SKIP_THIS
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
   }  // end namespace details
   ///@endcond
}

#endif  // __DELTA_DTDIS_LIBRARY_ENTRY_H__

