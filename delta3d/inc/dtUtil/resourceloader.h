//////////////////////////////////////////////////////////////////////////////////
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Originally created on 08/06/2006 by Bradley G Anderegg
// Copyright (C) 2006 Bradley Anderegg, all rights reserved.
///////////////////////////////////////////////////////////////////////////////////


#ifndef __RESOURCELOADER_H__
#define __RESOURCELOADER_H__

#include <dtUtil/referencedinterface.h>
#include <dtUtil/functor.h>

namespace dtUtil
{

   template <class ResourceDescriptor, class Resource>
   class ResourceLoader: public dtUtil::ReferencedInterface
   {
   protected:
      virtual ~ResourceLoader(){}

   public:
      enum LoadingState
      {
         IDLE,
         LOADING,
         FAILED,
         COMPLETE
      };

      typedef dtUtil::Functor<void, TYPELIST_3(const ResourceDescriptor&, Resource*, LoadingState)> CallbackFunctor;

      virtual void LoadResource(const ResourceDescriptor&, CallbackFunctor) = 0;

      virtual void FreeResource(Resource*) = 0;


   };

}//namespace dtUtil


#endif //__RESOURCELOADER_H__

