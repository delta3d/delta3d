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

#include <dtAnim/modeldatabase.h>
#include <dtAnim/cal3dloader.h>
#include <dtAnim/constants.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtAnim/osgloader.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/xercesparser.h>
#include <dtCore/project.h>
#include <osgDB/FileNameUtils>



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

   struct findWithResourceDescriptor
   {
      findWithResourceDescriptor(const dtCore::ResourceDescriptor& rd) : mResource(rd) {}

      bool operator()(dtAnim::BaseModelData* data)
      {
         if (data == NULL)
         {
            return false;
         }
         else
         {
            return data->GetResource() == mResource;
         }
      }

      const dtCore::ResourceDescriptor& mResource;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ModelDatabase> ModelDatabase::mInstance = NULL;
   
   ModelDatabase::ModelDatabase()
      : mHardwareMode(true)
      , mNodeBuilder(new dtAnim::AnimNodeBuilder)
      , mFileLoader(new dtAnim::CharacterFileLoader)
   {
      dtCore::RefPtr<Cal3dLoader> calLoader = new Cal3dLoader();
      AddModelLoader(*calLoader);

      dtCore::RefPtr<OsgLoader> osgLoader = new OsgLoader();
      AddModelLoader(*osgLoader);
   }

   ModelDatabase::~ModelDatabase()
   {}

   void ModelDatabase::Destroy()
   {
      mInstance = NULL;
   }

   bool ModelDatabase::IsAvailable()
   {
      return mInstance.valid();
   }

   ModelDatabase& ModelDatabase::GetInstance()
   {
      if (!mInstance.valid())
      {
         mInstance = new ModelDatabase();
      }
      return *mInstance;
   }

   bool ModelDatabase::IsFileValid(const dtCore::ResourceDescriptor& resource)
   {
      dtUtil::XercesParser parser;
      CharacterFileHandler handler;
      handler.mResource = resource;

      std::string filename;
      try
      {
         filename = dtCore::Project::GetInstance().GetResourcePath(handler.mResource);
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, "cal3dloader.cpp");
         return false;
      }

      return parser.Parse(filename, handler, "animationdefinition.xsd");
   }

   void ModelDatabase::SetHardwareMode(bool hardwareMode)
   {
      if (mHardwareMode != hardwareMode)
      {
         mHardwareMode = hardwareMode;

         if (hardwareMode)
         {
            mNodeBuilder->SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(mNodeBuilder.get(), &dtAnim::AnimNodeBuilder::CreateHardware));
         }
         else
         {
            mNodeBuilder->SetCreate(dtAnim::AnimNodeBuilder::CreateFunc(mNodeBuilder.get(), &dtAnim::AnimNodeBuilder::CreateSoftware));
         }
      }
   }

   bool ModelDatabase::IsHardwareMode() const
   {
      return mHardwareMode;
   }

   bool ModelDatabase::IsHardwareSupported() const
   {
      // Not really a great way to see what method we're using.  A better way would
      // be to query a hardware skinning object to see if its being used or something.
      // This'll have to do for now.
      return mNodeBuilder->SupportsHardware();
   }

   bool ModelDatabase::Load(const dtCore::ResourceDescriptor& resource)
   {
      dtCore::RefPtr<dtAnim::BaseModelData> data;

      return InternalLoad(resource, data);
   }
      
   void ModelDatabase::TruncateDatabase()
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      mModelData.clear();

      // TODO: Determine if this is thread safe...
      ModelLoaderMap::iterator curIter = mModelLoaders.begin();
      ModelLoaderMap::iterator endIter = mModelLoaders.end();
      for (; curIter != endIter; ++curIter)
      {
         curIter->second->Clear();
      }
   }
   
   bool ModelDatabase::RegisterModelData(dtAnim::BaseModelData& modelData)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);

      bool success = false;

      ModelDataArray::iterator foundIter 
         = std::find(mModelData.begin(), mModelData.end(), &modelData);

      if (foundIter == mModelData.end())
      {
         mModelData.push_back(&modelData);
         success = true;
      }

      return success;
   }

   bool ModelDatabase::UnregisterModelData(dtAnim::BaseModelData& modelData)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);

      bool success = false;
      
      ModelDataArray::iterator foundIter 
         = std::find(mModelData.begin(), mModelData.end(), &modelData);

      if (foundIter != mModelData.end())
      {
         mModelData.erase(foundIter);
         success = true;
      }

      return success;
   }

   dtAnim::BaseModelData* ModelDatabase::GetModelData(const dtCore::ResourceDescriptor& resource)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return Find(resource);
   }

   dtAnim::BaseModelData* ModelDatabase::Find(const dtCore::ResourceDescriptor& resource)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return const_cast<dtAnim::BaseModelData*>(FindWithFunctor(mModelData, findWithResourceDescriptor(resource)));
   }

   const dtAnim::BaseModelData* ModelDatabase::Find(const dtCore::ResourceDescriptor& resource) const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return FindWithFunctor(mModelData, findWithResourceDescriptor(resource));
   }

   bool ModelDatabase::InternalLoad(const dtCore::ResourceDescriptor& resource, dtCore::RefPtr<dtAnim::BaseModelData>& outModelData)
   {
      // have to lock for this entire call because the load function is not thread safe
      // plus it also prevents anyone from adding data to the cache unless they just loaded it.
      OpenThreads::ScopedLock<OpenThreads::Mutex> lockLoad(mLoadingLock);

      outModelData = Find(resource);

      if (!outModelData.valid())
      {
         BaseModelLoader* loader = NULL;
         dtCore::RefPtr<CharacterFileHandler> handler = mFileLoader->LoadCharacterFile(resource);
         if (handler.valid())
         {
            // TODO: Determine if thread safe...
            loader = GetModelLoader(handler->GetCharacterSystemType());
         }

         if (loader != NULL)
         {
            // TODO: Determine if thread safe...
            outModelData = loader->CreateModelData(*handler);

            if (outModelData.valid())
            {
               // TODO: Move the channel and sequence setup out of the file loader.
               mFileLoader->CreateChannelsAndSequences(*handler, *outModelData);
            }
         }

         if (outModelData.valid())
         {
            RegisterModelData(*outModelData);
         }
      }

      return outModelData.valid();
   }

   dtCore::RefPtr<dtAnim::BaseModelWrapper> ModelDatabase::CreateModelWrapper(dtAnim::BaseModelData& data)
   {
      const std::string& characterSystem = data.GetCharacterSystemType();

      dtCore::RefPtr<dtAnim::BaseModelWrapper> model;
      dtAnim::BaseModelLoader* loader = GetModelLoader(characterSystem);

      if (loader != NULL)
      {
         model = loader->CreateModel(data);
      }

      return model;
   }

   dtAnim::BaseModelLoader* ModelDatabase::GetModelLoader(const std::string& characterSystem) const
   {
      dtAnim::BaseModelLoader* loader = NULL;

      ModelLoaderMap::const_iterator foundIter = mModelLoaders.find(characterSystem);
      if (foundIter != mModelLoaders.end())
      {
         loader = foundIter->second.get();
      }

      return loader;
   }

   bool ModelDatabase::AddModelLoader(dtAnim::BaseModelLoader& modelLoader)
   {
      bool success = false;

      const std::string characterSystem = modelLoader.GetCharacterSystemType();
      ModelLoaderMap::iterator foundIter = mModelLoaders.find(characterSystem);
      if (foundIter == mModelLoaders.end())
      {
         mModelLoaders.insert(std::make_pair(characterSystem, &modelLoader));
         success = true;
      }

      return success;
   }

   bool ModelDatabase::RemoveModelLoader(dtAnim::BaseModelLoader& modelLoader)
   {
      bool success = false;

      const std::string characterSystem = modelLoader.GetCharacterSystemType();
      ModelLoaderMap::iterator foundIter = mModelLoaders.find(characterSystem);
      if (foundIter != mModelLoaders.end())
      {
         mModelLoaders.erase(foundIter);
         success = true;
      }

      return success;
   }

   dtAnim::AnimNodeBuilder* ModelDatabase::GetNodeBuilder() const
   {
      return mNodeBuilder.get();
   }
   
   dtCore::RefPtr<osg::Node> ModelDatabase::CreateNode(
      dtAnim::BaseModelWrapper& model, bool immediate)
   {
      return mNodeBuilder->CreateNode(&model, immediate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   /// Each core model needs to work with its own pose mesh database.
   /// This is not necessary for this example since they all share the same mesh but
   /// is important in a real application that has more than one core model
   dtAnim::PoseMeshDatabase* ModelDatabase::GetPoseMeshDatabase(dtAnim::BaseModelWrapper& wrapper)
   {
      // Get access to the pose mesh file name
      //dtAnim::ModelDatabase& database = dtAnim::ModelDatabase::GetInstance();
      dtAnim::BaseModelData* modelData = wrapper.GetModelData();
      const std::string& poseMeshFile = modelData->GetPoseMeshFilename();

      dtAnim::PoseMeshDatabase* poseDatabase = NULL;

      // See if this core model already has a pose mesh database that can be shared
      PoseDatabaseMap::iterator mapIter = mPoseMeshMap.find(poseMeshFile);

      if (mapIter != mPoseMeshMap.end())
      {
         poseDatabase = mapIter->second.get();
      }
      else // Database not found. Try to create one from file.
      {
         if ( ! poseMeshFile.empty())
         {
            // Load up the pose mesh data
            dtCore::RefPtr<dtAnim::PoseMeshDatabase> newPoseDatabase
               = new dtAnim::PoseMeshDatabase(&wrapper);
            if (newPoseDatabase->LoadFromFile(poseMeshFile))
            {
               mPoseMeshMap.insert(std::make_pair(poseMeshFile, newPoseDatabase));

               poseDatabase = newPoseDatabase;
            }

            if (poseDatabase == NULL)
            {
               LOG_ERROR("Cannot access pose mesh data for character model \"" + modelData->GetModelName() + "\"");
            }
         }
      }

      return poseDatabase;
   }

} // namespace dtAnim
