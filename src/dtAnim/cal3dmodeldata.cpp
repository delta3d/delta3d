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
 * Bradley Anderegg 07/17/2007
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/macros.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/animatable.h>
#include <dtAnim/animationwrapper.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>

#include <osg/BufferObject>
#include <osgDB/FileNameUtils>

#include <cal3d/coremodel.h>
#include <cal3d/hardwaremodel.h>

#include <algorithm>

 namespace dtAnim
 {
   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::Cal3DModelData(const std::string& modelName, const std::string& filename)
      : mScale(1.0f)
      , mFilename(filename)
      , mStride(-1)
      , mIndexArray(NULL)
      , mVertexArray(NULL)
      , mVertexBufferObject(NULL)
      , mElementBufferObject(NULL)
      , mCoreModel(NULL)
      , mHardwareModel(NULL)
      , mAnimWrappers()
      , mAnimatables()
      , mShaderMaxBones(72)
   {
      mCoreModel = new CalCoreModel(modelName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::~Cal3DModelData()
   {
      delete mCoreModel;
      mCoreModel = NULL;

      mAnimWrappers.clear();
      mAnimatables.clear();

      delete mHardwareModel;
      mHardwareModel = NULL;

      DestroySourceArrays();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Add(AnimationWrapper* wrapper)
   {
      mAnimWrappers.push_back(wrapper);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Add(Animatable* anim)
   {
      mAnimatables.push_back(anim);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Add(const std::pair<dtUtil::HotSpotDefinition, std::string>& attachment)
   {
      mAttachments.push_back(attachment);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Remove(AnimationWrapper* wrapper)
   {
      mAnimWrappers.erase(std::remove(mAnimWrappers.begin(), mAnimWrappers.end(), wrapper));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Remove(Animatable* anim)
   {
      mAnimatables.erase(std::remove(mAnimatables.begin(), mAnimatables.end(), anim));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::Remove(unsigned int which)
   {
      mAttachments.erase(mAttachments.begin() + which);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetFilename(const std::string& file)
   {
      mFilename = file;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetFilename() const
   {
      return mFilename;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetModelName(const std::string& name)
   {
      mModelName = name;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetModelName() const
   {
      return mModelName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CalCoreModel* Cal3DModelData::GetCoreModel()
   {
      return mCoreModel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const CalCoreModel* Cal3DModelData::GetCoreModel() const
   {
      return mCoreModel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetScale(float scale)
   {
      mScale = scale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float Cal3DModelData::GetScale() const
   {
      return mScale;
   }

   ////////////////////////////////////////////////////////////////////////////////
   AnimationWrapper* Cal3DModelData::GetAnimationWrapperByName(const std::string& name)
   {
      AnimationWrapper* anim = NULL;

      AnimationWrapper* curAnim = NULL;
      AnimationWrapperArray::iterator curIter = mAnimWrappers.begin();
      AnimationWrapperArray::iterator endIter = mAnimWrappers.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const AnimationWrapper* Cal3DModelData::GetAnimationWrapperByName(const std::string& name) const
   {
      const AnimationWrapper* anim = NULL;

      const AnimationWrapper* curAnim = NULL;
      AnimationWrapperArray::const_iterator curIter = mAnimWrappers.begin();
      AnimationWrapperArray::const_iterator endIter = mAnimWrappers.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::AnimationWrapperArray& Cal3DModelData::GetAnimationWrappers()
   {
      return mAnimWrappers;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData::AnimationWrapperArray& Cal3DModelData::GetAnimationWrappers() const
   {
      return mAnimWrappers;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Animatable* Cal3DModelData::GetAnimatableByName(const std::string& name)
   {
      Animatable* anim = NULL;

      Animatable* curAnim = NULL;
      AnimatableArray::iterator curIter = mAnimatables.begin();
      AnimatableArray::iterator endIter = mAnimatables.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Animatable* Cal3DModelData::GetAnimatableByName(const std::string& name) const
   {
      const Animatable* anim = NULL;

      Animatable* curAnim = NULL;
      AnimatableArray::const_iterator curIter = mAnimatables.begin();
      AnimatableArray::const_iterator endIter = mAnimatables.end();
      for (;curIter != endIter; ++curIter)
      {
         curAnim = curIter->get();
         if(curAnim->GetName() == name)
         {
            anim = curAnim;
            break;
         }
      }

      return anim;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::AnimatableArray& Cal3DModelData::GetAnimatables()
   {
      return mAnimatables;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData::AnimatableArray& Cal3DModelData::GetAnimatables() const
   {
      return mAnimatables;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::AttachmentArray& Cal3DModelData::GetAttachments()
   {
      return mAttachments;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Cal3DModelData::AttachmentArray& Cal3DModelData::GetAttachments() const
   {
      return mAttachments;
   }

   /////////////////////////////////////////////////////////////////////////////
   CalHardwareModel* Cal3DModelData::GetOrCreateCalHardwareModel()
   {
      if (!mHardwareModel)
      {
         // We need a core model to make the hardware model
         if (mCoreModel)
         {
            mHardwareModel = new CalHardwareModel(mCoreModel);
         }
         else
         {
            LOG_ERROR("Unable to create hardware model.");
         }
      }

      return mHardwareModel;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::CreateSourceArrays(int numberOfVertices, int numberOfIndices, int stride)
   {
      if (!mVertexArray && !mIndexArray)
      {
         mIndexArray = new CalIndex[numberOfIndices];
         mVertexArray = new osg::FloatArray(stride * numberOfVertices);

         mStride = stride;
      }
      else
      {
         LOG_ERROR("Unable to create source arrays.")
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::DestroySourceArrays()
   {
      mVertexArray = NULL;
      delete[] mIndexArray;  mIndexArray = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::VertexBufferObject* Cal3DModelData::GetVertexBufferObject()
   {
      return mVertexBufferObject.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetVertexBufferObject(osg::VertexBufferObject* vbo)
   {
      mVertexBufferObject = vbo;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::ElementBufferObject* Cal3DModelData::GetElementBufferObject() const
   {
      return mElementBufferObject.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetElementBufferObject(osg::ElementBufferObject* ebo)
   {
      mElementBufferObject = ebo;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::DrawElements* Cal3DModelData::GetDrawElements() const
   {
      return mElementBufferDrawElements;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetDrawElements(osg::DrawElements* drawElements)
   {
      // Osg does not hold a reference to drawElements, we must make sure to hold our own
      if (mElementBufferObject.valid())
      {
         mElementBufferObject->addDrawElements(drawElements);
      }

      mElementBufferDrawElements = drawElements;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetShaderGroupName() const
   {
      return mShaderGroupName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetShaderGroupName(const std::string& groupName)
   {
      mShaderGroupName = groupName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetShaderName() const
   {
      return mShaderName;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetShaderName(const std::string& name)
   {
      mShaderName = name;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string& Cal3DModelData::GetPoseMeshFilename() const
   {
      return mPoseMeshFilename;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetPoseMeshFilename(const std::string& name)
   {
      mPoseMeshFilename = name;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::GetShaderMaxBones() const
   {
      return mShaderMaxBones;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Cal3DModelData::SetShaderMaxBones(unsigned maxBones)
   {
      mShaderMaxBones = maxBones;
   }

#ifdef CAL3D_VERSION_DEVELOPMENT
   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::LoadCoreSkeletonBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      bool success = mCoreModel->loadCoreSkeleton(buffer);
      if (success)
      {
         mCoreModel->getCoreSkeleton()->setName(objectName);
         if (!file.empty())
         {
            RegisterFile(file, objectName);
         }
      }
      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMorphBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      /*int result = mCoreModel->loadCoreAnimatedMorph(buffer, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;*/

      // Currently, the buffer overload for loading morphs does not exist.
      return LoadCoreMorph(file, objectName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMaterialBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      int result = mCoreModel->loadCoreMaterial(buffer, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMeshBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      int result = mCoreModel->loadCoreMesh(buffer, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreAnimationBuffer(void* buffer, const std::string& file, const std::string& objectName)
   {
      int result = mCoreModel->loadCoreAnimation(buffer, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMorph(const std::string& file, const std::string& objectName)
   {
      /*int result = mCoreModel->loadCoreAnimatedMorph(file, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;*/

      // Currently, the overload for loading morphs with an object name does not exist.
      RegisterFile(file, objectName);
      return mCoreModel->loadCoreAnimatedMorph(file);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::LoadCoreSkeleton(const std::string& file, const std::string& objectName)
   {
      bool success = mCoreModel->loadCoreSkeleton(file);
      if (success)
      {
         mCoreModel->getCoreSkeleton()->setName(objectName);
         if (!file.empty())
         {
            RegisterFile(file, objectName);
         }
      }
      return success;
   }
#else

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::LoadCoreSkeleton(const std::string& file)
   {
      bool success = mCoreModel->loadCoreSkeleton(file);
      if (success)
      {
         RegisterFile(file, "skeleton");
      }
      return success;
   }
#endif

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMaterial(const std::string& file, const std::string& objectName)
   {
      int result = mCoreModel->loadCoreMaterial(file, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreMesh(const std::string& file, const std::string& objectName)
   {
      int result = mCoreModel->loadCoreMesh(file, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int Cal3DModelData::LoadCoreAnimation(const std::string& file, const std::string& objectName)
   {
      int result = mCoreModel->loadCoreAnimation(file, objectName);
      if (result > -1 && !file.empty())
      {
         RegisterFile(file, objectName);
      }
      return result;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Cal3DModelData::CalFileType Cal3DModelData::GetFileType(const std::string& file) const
   {
      CalFileType fileType = NO_FILE;

      if (file.length() >= 4)
      {
         std::string ext = osgDB::getLowerCaseFileExtension(file);

         // Determine the file type from the extension on the file.
         if (ext == "csf" || ext == "xsf")
         {
            fileType = SKEL_FILE;
         }
         else if (ext == "crf" || ext == "xrf")
         {
            fileType = MAT_FILE;
         }
         else if (ext == "cmf" || ext == "xmf")
         {
            fileType = MESH_FILE;
         }
         else if (ext == "caf" || ext == "xaf")
         {
            fileType = ANIM_FILE;
         }
         else if (ext == "cpf" || ext == "xpf")
         {
            fileType = MORPH_FILE;
         }
      }

      return fileType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::GetFileCount(CalFileType fileType) const
   {
      unsigned num = 0;

      FileToObjectMap::const_iterator curIter = mFileObjectMap.begin();
      FileToObjectMap::const_iterator endIter = mFileObjectMap.end();
      for ( ; curIter != endIter; ++curIter)
      {
         if (curIter->second->mType == fileType)
         {
            ++num;
         }
      }

      return num;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::GetFileListForFileType(CalFileType fileType, StrArray& outFiles) const
   {
      const ObjectNameAndFileType* curItem = NULL;
      FileToObjectMap::const_iterator curIter = mFileObjectMap.begin();
      FileToObjectMap::const_iterator endIter = mFileObjectMap.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curItem = curIter->second.get();

         if (curItem->mType == fileType)
         {
            outFiles.push_back(curIter->first);
         }
      }

      return unsigned(outFiles.size());
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::GetObjectNameListForFileType(CalFileType fileType, StrArray& outNames) const
   {
      const ObjectNameAndFileType* curItem = NULL;
      FileToObjectMap::const_iterator curIter = mFileObjectMap.begin();
      FileToObjectMap::const_iterator endIter = mFileObjectMap.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curItem = curIter->second.get();

         if (curItem->mType == fileType)
         {
            outNames.push_back(curItem->mName);
         }
      }

      return unsigned(outNames.size());
   }

   ////////////////////////////////////////////////////////////////////////////////
   // Helper Set
   typedef std::map<int, std::string> CalIdNameMap;

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::GetObjectNameListForFileTypeSorted(CalFileType fileType, StrArray& outNames) const
   {
      CalIdNameMap sortedMap;

      const ObjectNameAndFileType* curItem = NULL;
      FileToObjectMap::const_iterator curIter = mFileObjectMap.begin();
      FileToObjectMap::const_iterator endIter = mFileObjectMap.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curItem = curIter->second.get();

         // If mapped object name matched the file type...
         if (curItem->mType == fileType)
         {
            // ...acquire the current object name...
            const std::string& name = curItem->mName;

            // ...determine the id...
            int id = -1;
            switch (fileType)
            {
            case MAT_FILE:
               id = mCoreModel->getCoreMaterialId(name);
               break;
            case MESH_FILE:
               id = mCoreModel->getCoreMeshId(name);
               break;
            case ANIM_FILE:
               id = mCoreModel->getCoreAnimationId(name);
               break;
            case SKEL_FILE:
               ++id; // order irrelevant.
               break;
            case MORPH_FILE:
               ++id; // order irrelevant (currently).
               break;
            default: break;
            }

            sortedMap[id] = name;
         }
      }

      // Transfer the names.
      CalIdNameMap::iterator curIterMap = sortedMap.begin();
      CalIdNameMap::iterator endIterMap = sortedMap.end();
      for (; curIterMap != endIterMap; ++curIterMap)
      {
         outNames.push_back(curIterMap->second);
      }

      return unsigned(outNames.size());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::ReplaceObjectName(CalFileType fileType, const std::string& oldObjectName,
      const std::string& newObjectName)
   {
      bool success = false;

      // Only set the new name if the object name does not already exist for the
      // current file type.
      if (GetFileForObjectName(fileType, newObjectName).empty())
      {
         ObjectNameAndFileType* curItem = NULL;
         FileToObjectMap::iterator curIter = mFileObjectMap.begin();
         FileToObjectMap::iterator endIter = mFileObjectMap.end();
         for ( ; curIter != endIter; ++curIter)
         {
            curItem = curIter->second.get();

            if (curItem->mType == fileType && curItem->mName == oldObjectName)
            {
               curItem->mName = newObjectName;
               success = true;
               break;
            }
         }
      }

      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::SetFileForObjectName(CalFileType fileType, const std::string& objectName,
      const std::string& file)
   {
      bool success = false;

      std::string oldFile(GetFileForObjectName(fileType, objectName));
      if (!oldFile.empty() && oldFile != file)
      {
         unsigned numRemoved = UnregisterObjectName(objectName, oldFile, NULL);
         if (numRemoved == 0)
         {
            LOG_WARNING("Could not unregister object name \"" + objectName + "\"");
         }
         else if (numRemoved > 1)
         {
            LOG_WARNING("More than one items were unregistered for object name \"" + objectName + "\"");
         }

         success = RegisterFile(file, objectName);
      }

      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string Cal3DModelData::GetFileForObjectName(CalFileType fileType, const std::string& objectName) const
   {
      std::string file;

      const ObjectNameAndFileType* curItem = NULL;
      FileToObjectMap::const_iterator curIter = mFileObjectMap.begin();
      FileToObjectMap::const_iterator endIter = mFileObjectMap.end();
      for ( ; curIter != endIter; ++curIter)
      {
         curItem = curIter->second.get();

         if (curItem->mType == fileType && curItem->mName == objectName)
         {
            file = curIter->first;
            break;
         }
      }

      return file;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::GetObjectNameListForFile(const std::string& file, StrArray& outObjectNames) const
   {
      const ObjectNameAndFileType* curItem = NULL;
      FileToObjectMap::const_iterator curIter = mFileObjectMap.lower_bound(file);
      FileToObjectMap::const_iterator endIter = mFileObjectMap.upper_bound(file);
      for ( ; curIter != endIter; ++curIter)
      {
         curItem = curIter->second.get();
         outObjectNames.push_back(curItem->mName);
      }

      return unsigned(outObjectNames.size());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool Cal3DModelData::RegisterFile(const std::string& file, const std::string& objectName)
   {
      bool success = false;

      CalFileType fileType = GetFileType(file);

      if (fileType != NO_FILE)
      {
         // Convert the file to a relative path.
         std::string relativeFile(file);
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.CleanupFileString(relativeFile);

         // Get the location of the model file.
         std::string modelContext(osgDB::getFilePath(mFilename));
         if (!modelContext.empty())
         {
            modelContext += '/';
         }

         // Get the file path relative to the model file's location.
         std::string relativePath(fileUtils.RelativePath(modelContext, file));
         if (!relativePath.empty())
         {
            relativeFile = relativePath;
         }

         // Map the relative file path.
         size_t prevSize = mFileObjectMap.size();
         mFileObjectMap.insert(std::make_pair(relativeFile, new ObjectNameAndFileType(objectName, fileType)));

         success = mFileObjectMap.size() > prevSize;
      }
      else
      {
         LOG_WARNING("Cannot register a file without an extension for object \"" + objectName +"\"\n");
      }

      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::UnregisterFile(const std::string& file, StrArray* outObjectNames)
   {
      unsigned numNames = 0;

      if (outObjectNames != NULL)
      {
         numNames = GetObjectNameListForFile(file, *outObjectNames);
      }

      mFileObjectMap.erase(file);

      return numNames;
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned Cal3DModelData::UnregisterObjectName(const std::string& objectName,
      const std::string& file, StrArray* outFileNames)
   {
      unsigned numNames = 0;

      CalFileType fileType = NO_FILE;
      FileToObjectMap::iterator curIter = mFileObjectMap.end();
      FileToObjectMap::iterator endIter = mFileObjectMap.end();

      // Determine the range of entries to scan.
      if (mFileObjectMap.find(file) != mFileObjectMap.end())
      {
         curIter = mFileObjectMap.lower_bound(file);
         endIter = mFileObjectMap.upper_bound(file);
      }
      else if (file.length() <= 4)
      {
         if (!file.empty())
         {
            fileType = GetFileType(file);
         }
         curIter = mFileObjectMap.begin();
      }

      // Collect all matches
      const ObjectNameAndFileType* curMapping = NULL;
      for (; curIter != endIter; ++curIter)
      {
         curMapping = curIter->second.get();
         if (curMapping->mName == objectName
            && (fileType == NO_FILE || curMapping->mType == fileType))
         {
            ++numNames;

            // If the caller has specified a list to capture the associate file name...
            if (outFileNames != NULL)
            {
               // ...add the file name to the capture list before cleanup happens.
               outFileNames->push_back(curIter->first);
            }

            // Mark this match for delete.
            curIter->second = NULL;
         }
      }

      // Cleanup the nullified mappings.
      FileToObjectMap::iterator curEraseIter = mFileObjectMap.begin();
      for (; curEraseIter != mFileObjectMap.end();)
      {
         if (curEraseIter->second.get() == NULL)
         {
            FileToObjectMap::iterator tmp = curEraseIter;
            mFileObjectMap.erase(tmp);
            curEraseIter = mFileObjectMap.begin();
         }
         else
         {
            ++curEraseIter;
         }
      }

      return numNames;
   }

   /////////////////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////////////////
   LODOptions::LODOptions():
      mStartDistance(10.0), mEndDistance(500.0), mMaxVisibleDistance(1000.0)
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   void LODOptions::SetStartDistance(double newDistance)
   {
      mStartDistance = newDistance;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LODOptions::SetEndDistance(double newDistance)
   {
      mEndDistance = newDistance;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LODOptions::SetMaxVisibleDistance(double newDistance)
   {
      mMaxVisibleDistance = newDistance;
   }

} //namespace dtAnim
