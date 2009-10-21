/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * David Guthrie and Bradley Anderegg
 */

#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtUtil/log.h>

#include <osgDB/FileNameUtils>
#include <osg/Texture2D>

#include <cal3d/model.h>

namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   //a custom find function that uses a functor
   template<class T, class Array>
   const typename Array::value_type::element_type* FindWithFunctor(Array a, T functor)
   {
      typename Array::const_iterator iter = a.begin();
      typename Array::const_iterator end  = a.end();

      for (;iter != end; ++iter)
      {
         if (functor((*iter).get()))
         {
            return (*iter).get();
         }
      }

      return 0;
   };

   struct findWithFilename
   {
      findWithFilename(const std::string& filename) : mFilename(filename) {}

      bool operator()(Cal3DModelData* data)
      {
         if (data == NULL)
         {
            return false;
         }
         else
         {
            return data->GetFilename() == mFilename;
         }
      }

      const std::string& mFilename;
   };

   struct findWithCoreModel
   {
      findWithCoreModel(const CalCoreModel* model) : mModel(model) {}

      bool operator()(Cal3DModelData* data)
      {
         return data->GetCoreModel() == mModel;
      }

      const CalCoreModel* mModel;
   };

   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   Cal3DDatabase::Cal3DDatabase()
      : mModelData()
      , mFileLoader(new Cal3DLoader())
      , mNodeBuilder(new AnimNodeBuilder())
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DDatabase::~Cal3DDatabase()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Cal3DDatabase> Cal3DDatabase::mInstance;

   /////////////////////////////////////////////////////////////////////////////
   Cal3DDatabase& Cal3DDatabase::GetInstance()
   {
      if (!mInstance.valid())
      {
         mInstance = new Cal3DDatabase();
      }
      return *mInstance;
   }

   /////////////////////////////////////////////////////////////////////////////
   AnimNodeBuilder& Cal3DDatabase::GetNodeBuilder()
   {
      return *mNodeBuilder;
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<Cal3DModelWrapper> Cal3DDatabase::Load(const std::string& file)
   {
      std::string filename = osgDB::convertFileNameToNativeStyle(file);
      Cal3DModelData* data = Find(filename);
      if (!data)
      {
         if (mFileLoader->Load(filename, data))
         {
            // Protect the data in case another thread is accessing it
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
            mModelData.push_back(data);
         }
         else
         {
            LOG_ERROR("Unable to load Character XML file '" + filename + "'.");
            return NULL;
         }
      }

      CalModel* model = new CalModel(data->GetCoreModel());
      dtCore::RefPtr<Cal3DModelWrapper> wrapper = new Cal3DModelWrapper(model);
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DDatabase::LoadAsynchronously(const std::string& file)
   {
      dtUtil::Functor<void, TYPELIST_1(Cal3DModelData*)> loadCallback =
         dtUtil::MakeFunctor(&Cal3DDatabase::OnAsynchronousLoadCompleted, this);
      
      mFileLoader->LoadAsynchronously(file, loadCallback);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DDatabase::OnAsynchronousLoadCompleted(Cal3DModelData* loadedModelData)
   {
      if (loadedModelData == NULL) 
      { 
         return; 
      }

      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);

      dtCore::RefPtr<Cal3DModelData> newModelData = loadedModelData;
      mModelData.push_back(newModelData);
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DDatabase::PurgeLoaderCaches()
   {
      mFileLoader->PurgeAllCaches();
   }
   /////////////////////////////////////////////////////////////////////////////
   void Cal3DDatabase::TruncateDatabase()
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      mModelData.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData* Cal3DDatabase::GetModelData(const Cal3DModelWrapper& wrapper) const
   {
      return Find(wrapper.GetCalModel()->getCoreModel());
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::GetModelData(const Cal3DModelWrapper& wrapper)
   {
      if (!wrapper.GetCalModel())
      {
         return NULL;
      }
      return Find(wrapper.GetCalModel()->getCoreModel());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::GetModelData(const std::string& filename)
   {
      return Find(filename);
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::Find(const std::string& filename)
   {
      // todo- this is ugly, get rid of it
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return const_cast<Cal3DModelData*>(FindWithFunctor(mModelData, findWithFilename(filename)));
   }

   /////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData* Cal3DDatabase::Find(const std::string& filename) const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return FindWithFunctor(mModelData, findWithFilename(filename));
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::Find(const CalCoreModel* coreModel)
   {
      // todo- this is ugly, get rid of it
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return const_cast<Cal3DModelData*>(FindWithFunctor(mModelData, findWithCoreModel(coreModel)));
   }

   /////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData* Cal3DDatabase::Find(const CalCoreModel* coreModel) const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return FindWithFunctor(mModelData, findWithCoreModel(coreModel));
   }
} // namespace dtAnim
