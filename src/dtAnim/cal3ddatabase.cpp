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

#include <dtAnim/macros.h>
#include <dtAnim/cal3ddatabase.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/posemeshdatabase.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/xercesparser.h>

#include <osgDB/FileNameUtils>
#include <osg/Texture2D>

#include <cal3d/model.h>
#include <cal3d/skeleton.h>
#include <cal3d/coreskeleton.h>

#include <dtUtil/threadpool.h>

namespace dtAnim
{

   class LoadTask : public dtUtil::ThreadPoolTask
   {
   public:
      LoadTask(Cal3DDatabase& db, const std::string& fileName)
      : mDatabase(db)
      {
         SetName(fileName);
      }

      virtual void operator()()
      {
         dtCore::RefPtr<Cal3DModelData> loadedData;
         mDatabase.InternalLoad(GetName(), loadedData);
      }
   private:
      Cal3DDatabase& mDatabase;
   };

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
   void Cal3DDatabase::Destroy()
   {
      mInstance = NULL;
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

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DDatabase::IsFileValid(const std::string& filename)
   {
      dtUtil::XercesParser parser;
      CharacterFileHandler handler;

      return parser.Parse(filename, handler, "animationdefinition.xsd");
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

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.CleanupFileString(filename);

      dtCore::RefPtr<Cal3DModelData> data;

      if (!InternalLoad(filename, data))
      {
         LOG_ERROR("Unable to load Character XML file '" + filename + "'.");
         return NULL;
      }

      CalModel* model = new CalModel(data->GetCoreModel());
      dtCore::RefPtr<Cal3DModelWrapper> wrapper = new Cal3DModelWrapper(model);
      wrapper->SetScale(data->GetScale());

      GetPoseMeshDatabase(*wrapper);

      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////
   void Cal3DDatabase::LoadAsynchronously(const std::string& file)
   {

      dtCore::RefPtr<Cal3DModelData> data;
      {
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
         data = Find(file);
      }

      if (!data.valid())
      {
         dtCore::RefPtr<LoadTask> loadTask = new LoadTask(*this, file);
         dtUtil::ThreadPool::AddTask(*loadTask, dtUtil::ThreadPool::IO);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DDatabase::Save(const std::string& file, const Cal3DModelWrapper& wrapper)
   {
#ifdef CAL3D_VERSION_DEVELOPMENT
      std::string filename = osgDB::convertFileNameToNativeStyle(file);

      dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();
      writer->CreateDocument("character");

      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();

      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* root = doc->getDocumentElement();

      // Root
      {
         std::string characterName = wrapper.GetCalModel()->getCoreModel()->getName();

         XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("name");
         XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(characterName.c_str());
         root->setAttribute(name, value);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);
      }

      // Skeleton
      {
         XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("skeleton");
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

         std::string skeletonName = wrapper.GetCalModel()->getSkeleton()->getCoreSkeleton()->getName();

         XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
         XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(skeletonName.c_str());
         groupElement->setAttribute(name, value);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

         root->appendChild(groupElement);
      }

      // Meshes
      {
         int count = wrapper.GetMeshCount();
         for (int index = 0; index < count; ++index)
         {
            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("mesh");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            std::string meshName = wrapper.GetCoreMeshName(index);

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(meshName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      // Materials
      {
         int count = wrapper.GetCoreMaterialCount();
         for (int index = 0; index < count; ++index)
         {
            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("material");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            std::string materialName = wrapper.GetCoreMaterialName(index);

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(materialName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      // Bone Animations
      {
         int count = wrapper.GetCoreAnimationCount();
         for (int index = 0; index < count; ++index)
         {
            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("animation");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            std::string animName = wrapper.GetCoreAnimationName(index);

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(animName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      // Morph Animations
      {
         int count = wrapper.GetCalModel()->getMorphTargetMixer()->getMorphTargetCount();
         for (int index = 0; index < count; ++index)
         {
            XMLCh* groupName = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("morph");
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* groupElement = doc->createElement(groupName);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&groupName);

            std::string morphName = wrapper.GetCalModel()->getMorphTargetMixer()->getMorphName(index);

            XMLCh* name = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("fileName");
            XMLCh* value = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(morphName.c_str());
            groupElement->setAttribute(name, value);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&name);
            XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&value);

            root->appendChild(groupElement);
         }
      }

      writer->WriteFile(filename);
      return true;
#else
      return false;
#endif
   }

   /////////////////////////////////////////////////////////////////////////////
   bool Cal3DDatabase::InternalLoad(const std::string& filename, dtCore::RefPtr<Cal3DModelData>& data_in)
   {
      bool result = true;
      // have to lock for this entire call because the load function is not thread safe
      // plus it also prevents anyone from adding data to the cache unless they just loaded it.
      OpenThreads::ScopedLock<OpenThreads::Mutex> lockLoad(mLoadingLock);

      {
         OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
         data_in = Find(filename);
      }

      if (!data_in.valid())
      {
         result = mFileLoader->Load(filename, data_in);

         if (data_in.valid())
         {
            OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
            mModelData.push_back(data_in);
         }
      }

      return result;
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
      mPoseMeshMap.clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData* Cal3DDatabase::GetModelData(const Cal3DModelWrapper& wrapper) const
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return Find(wrapper.GetCalModel()->getCoreModel());
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::GetModelData(const Cal3DModelWrapper& wrapper)
   {
      if (!wrapper.GetCalModel())
      {
         return NULL;
      }
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return Find(wrapper.GetCalModel()->getCoreModel());
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::GetModelData(const std::string& filename)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> lock(mAsynchronousLoadLock);
      return Find(filename);
   }

   ////////////////////////////////////////////////////////////////////////////////
   /// Each core model needs to work with its own pose mesh database.
   /// This is not necessary for this example since they all share the same mesh but
   /// is important in a real application that has more than one core model
   dtAnim::PoseMeshDatabase* Cal3DDatabase::GetPoseMeshDatabase(Cal3DModelWrapper& wrapper)
   {
      dtAnim::PoseMeshDatabase* poseDatabase = NULL;
      CalCoreModel* coreModel = wrapper.GetCalModel()->getCoreModel();

      // See if this core model already has a pose mesh database that can be shared
      PoseDatabaseMap::iterator mapIter = mPoseMeshMap.find(coreModel);

      if (mapIter != mPoseMeshMap.end())
      {
         poseDatabase = mapIter->second.get();
      }
      else
      {
         // Get access to the pose mesh file name
         dtAnim::Cal3DDatabase& calDatabase = dtAnim::Cal3DDatabase::GetInstance();
         dtAnim::Cal3DModelData* modelData = calDatabase.GetModelData(wrapper);

         if ( ! modelData->GetPoseMeshFilename().empty())
         {
            // Load up the pose mesh data
            dtCore::RefPtr<dtAnim::PoseMeshDatabase> newPoseDatabase
               = new dtAnim::PoseMeshDatabase(&wrapper);
            if (newPoseDatabase->LoadFromFile(modelData->GetPoseMeshFilename()))
            {
               mPoseMeshMap.insert(std::make_pair(coreModel, newPoseDatabase));

               poseDatabase = newPoseDatabase;
            }

            if (poseDatabase == NULL)
            {
               LOG_ERROR("Cannot access pose mesh data for character model \"" + coreModel->getName() + "\"");
            }
         }
      }

      return poseDatabase;
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::Find(const std::string& filename)
   {
      return const_cast<Cal3DModelData*>(FindWithFunctor(mModelData, findWithFilename(filename)));
   }

   /////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData* Cal3DDatabase::Find(const std::string& filename) const
   {
      return FindWithFunctor(mModelData, findWithFilename(filename));
   }

   /////////////////////////////////////////////////////////////////////////////
   Cal3DModelData* Cal3DDatabase::Find(const CalCoreModel* coreModel)
   {
      return const_cast<Cal3DModelData*>(FindWithFunctor(mModelData, findWithCoreModel(coreModel)));
   }

   /////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData* Cal3DDatabase::Find(const CalCoreModel* coreModel) const
   {
      return FindWithFunctor(mModelData, findWithCoreModel(coreModel));
   }

   void Cal3DDatabase::ReleaseGLObjects() 
    { 
      mFileLoader->ReleaseGLObjects();
	  for (ModelDataArray::iterator i = mModelData.begin(); i != mModelData.end(); i++) {
		  (*i)->SetVertexBufferObject(0);
		  (*i)->SetElementBufferObject(0);
	  }
    }
} // namespace dtAnim
