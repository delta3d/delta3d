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

   typedef ResourceLoader<ResourceKey, Resource> ResourceLoaderBaseType;
   typedef ResourceManager<ResourceKey, Resource> ResourceManagerType;
   typedef typename ResourceLoaderBaseType::LoadingState LoadingState;

   struct ResourceData
   {
      dtCore::RefPtr<Resource> mResource;
      LoadingState mLoadingState;
   };

   typedef std::pair<ResourceKey, ResourceData > ResourceHandle;
   typedef std::map<ResourceKey, ResourceData > ResourceMap;
   typedef typename ResourceMap::iterator ResourceIterator;
   typedef typename ResourceMap::iterator ResourceConstIterator;


////////////////////////////////////////////////////////////////////////////////////
//Functions
////////////////////////////////////////////////////////////////////////////////////
public:

   ResourceManager()
      : mLoader(NULL)
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
         ResourceData& data = iter->second;
         if (data.mResource.valid())
         {
            mLoader->FreeResource(data.mResource);
         }
         mResource.erase(iter);
      }
   }

   void SetResourceLoader(ResourceLoaderBaseType* pLoader)
   {
      mLoader = pLoader;
   }

   virtual void AddResource(const ResourceKey& pHandle, Resource* pResource)
   {
      ResourceIterator iter = mResource.find(pHandle);
      if (iter == mResource.end())
      {
         ResourceData data;
         data.mLoadingState = ResourceLoaderBaseType::COMPLETE;
         data.mResource = pResource;
         mResource.insert(ResourceHandle(pHandle, data));
      }
      else if (iter->second.mLoadingState == ResourceLoaderBaseType::LOADING
            || iter->second.mLoadingState == ResourceLoaderBaseType::FAILED)
      {
         ResourceData& data = iter->second;
         data.mLoadingState = ResourceLoaderBaseType::COMPLETE;
         data.mResource = pResource;
      }
      else
      {
         LOG_WARNING("Resource Handle already exists in ResourceManager.");
      }
   }

   virtual void LoadResource(const ResourceKey& pHandle)
   {
      ResourceConstIterator iter = mResource.find(pHandle);
      if (iter == mResource.end())
      {
         mLoader->LoadResource(pHandle, dtUtil::MakeFunctor(&ResourceManagerType::OnResourceLoaded, this));
      }
      else
      {
         LOG_WARNING("Resource Handle already exists in ResourceManager.");
      }
   }

   virtual void OnResourceLoaded(const ResourceKey& pHandle, Resource* loadedResource, LoadingState state)
   {
      if (state == ResourceLoaderBaseType::COMPLETE)
      {
         AddResource(pHandle, loadedResource);
      }
      else if (state == ResourceLoaderBaseType::FAILED)
      {
         ResourceIterator iter = mResource.find(pHandle);
         if (iter == mResource.end())
         {
            ResourceData data;
            data.mLoadingState = ResourceLoaderBaseType::FAILED;
            data.mResource = NULL;
            mResource.insert(ResourceHandle(pHandle, data));
         }
         else if (iter->second.mLoadingState == ResourceLoaderBaseType::LOADING)
         {
            ResourceData& data = iter->second;
            data.mLoadingState = ResourceLoaderBaseType::FAILED;
            data.mResource = NULL;
         }
      }

   }

   virtual void FreeResource(const ResourceKey& pHandle)
   {
      ResourceIterator iter = mResource.find(pHandle);
      if (iter != mResource.end())
      {
         ResourceData& data = iter->second;
         if (data.mResource.valid())
         {
            mLoader->FreeResource(data.mResource);
         }
         mResource.erase(iter);
      }
      else
      {
         LOG_WARNING("Resource Handle not found.");
      }
   }

   Resource* GetResource(const ResourceKey& pHandle)
   {
      ResourceConstIterator iter = mResource.find(pHandle);
      if (iter != mResource.end())
      {
         return (*iter).second.mResource;
      }
      else
      {
         LOG_ERROR("Cannot find resource");
         return NULL;
      }
   }

   const Resource* GetResource(const ResourceKey& pHandle) const
   {
      ResourceConstIterator iter = mResource.find(pHandle);
      if (iter != mResource.end())
      {
         return (*iter).second.mResource;
      }
      else
      {
         LOG_ERROR("Cannot find resource: " + pHandle);
         return NULL;
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

