////////////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////

#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include <map>
#include <string>

#include <dtUtil/resourceloader.h>
#include <dtUtil/log.h>
#include <dtCore/refptr.h>

#include <osg/Referenced>

namespace dtUtil
{

template <class ResourceKey, class Resource>
class ResourceManager: public osg::Referenced
{

///////////////////////////////////////////////////////////////////////////////////
//Data Structures
///////////////////////////////////////////////////////////////////////////////////
public:

   typedef std::pair<ResourceKey, dtCore::RefPtr<Resource> > ResourceHandle;
   typedef std::map<ResourceKey, dtCore::RefPtr<Resource> > ResourceMap;
   typedef typename ResourceMap::iterator ResourceIterator;
   typedef typename ResourceMap::iterator ResourceConstIterator;


////////////////////////////////////////////////////////////////////////////////////
//Functions
////////////////////////////////////////////////////////////////////////////////////
public:

   ResourceManager()
      : mLoader(0)
   {}

protected:

   virtual ~ResourceManager()
   {
      FreeAll();
   }

public:

   void FreeAll()
   {
      while (!mResource.empty())
      {
         ResourceIterator iter = mResource.begin();
         mLoader->FreeResource((*iter).second.get());
         mResource.erase(iter);
      }
   }

   void SetResourceLoader(ResourceLoader<ResourceKey, Resource>* pLoader)
   {
      mLoader = pLoader;
   }

   virtual void AddResource(const ResourceKey& pHandle, Resource* pResource)
   {
      if (mResource.find(pHandle) == mResource.end())
      {
         mResource.insert(ResourceHandle(pHandle, pResource));
      }
      else
      {
         LOG_WARNING("Resource Handle: " + pHandle + " already exists in ResourceManager.");
      }
   }

   virtual bool LoadResource(const ResourceKey& pHandle, const std::string& pFilename)
   {
      ResourceConstIterator iter = mResource.find(pHandle);
      if (iter == mResource.end())
      {
         Resource* pResource = mLoader->LoadResource(pFilename);
         if (pResource)
         {
            mResource.insert(ResourceHandle(pHandle, pResource));
            return true;
         }
      }
      else
      {
         LOG_WARNING("Resource Handle: " + pHandle + " already exists in ResourceManager.");
      }

      return false;
   }

   virtual void FreeResource(const ResourceKey& pHandle)
   {
      ResourceIterator iter = mResource.find(pHandle);
      if (iter != mResource.end())
      {
         mLoader->FreeResource((*iter).second.get());
         mResource.erase(iter);
      }
      else
      {
         LOG_WARNING("Resource Handle: " + pHandle + " not found.");
      }
   }

   Resource* GetResource(const ResourceKey& pHandle)
   {
      ResourceConstIterator iter = mResource.find(pHandle);
      if (iter != mResource.end())
      {
         return (*iter).second.get();
      }
      else
      {
         LOG_ERROR("Cannot find resource: " + pHandle);
         return 0;
      }
   }

   const Resource* GetResource(const ResourceKey& pHandle) const
   {
      ResourceConstIterator iter = mResource.find(pHandle);
      if (iter != mResource.end())
      {
         return (*iter).second.get();
      }
      else
      {
         LOG_ERROR("Cannot find resource: " + pHandle);
         return 0;
      }
   }

   ///todo add a clone to the loader and call it on copy
   //Resource* CopyResource(const String& pHandle) const
   //{
   //}

private:
   ResourceMap mResource;
   dtCore::RefPtr<ResourceLoader<ResourceKey, Resource> > mLoader;
};


} // namespace dtUtil


#endif // __RESOURCEMANAGER_H__

