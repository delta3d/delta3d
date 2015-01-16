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
#include <dtAnim/basemodeldata.h>
#include <dtAnim/animatable.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtCore/project.h>
#include <dtCore/exceptionenum.h>

#include <osg/BufferObject>
#include <osgDB/FileNameUtils>

#include <algorithm>

 namespace dtAnim
 {
    ////////////////////////////////////////////////////////////////////////////////
    // CLASS METHODS
    ////////////////////////////////////////////////////////////////////////////////
    std::string BaseModelData::GetResourceTypeName(dtAnim::ModelResourceType resourceType)
    {
       std::string resourceTypeName = "NONE";
       switch (resourceType)
       {
       case dtAnim::SKEL_FILE: // Skeleton
          resourceTypeName = "SKELETON"; break;

       case dtAnim::MAT_FILE:  // Material
          resourceTypeName = "MATERIAL"; break;

       case dtAnim::MESH_FILE: // Mesh
          resourceTypeName = "MESH"; break;

       case dtAnim::ANIM_FILE: // Animation
          resourceTypeName = "ANIMATION"; break;

       case dtAnim::MORPH_FILE: // Morph Animation
          resourceTypeName = "MORPH"; break;
          
       case dtAnim::NO_FILE:
       default:
          break;
       }

       return resourceTypeName;
    }



   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   BaseModelData::BaseModelData(const std::string& modelName, const dtCore::ResourceDescriptor& resource,
      const std::string& characterSystemType)
      : mResource(resource)
      , mScale(1.0f)
      , mCharacterSystemType(characterSystemType)
      , mStride(-1)
      , mIndexArray(NULL)
      , mVertexArray(NULL)
      , mShaderMaxBones(72)
      , mVertexBufferObject(NULL)
      , mElementBufferObject(NULL)
      , mAnimatables()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   BaseModelData::~BaseModelData()
   {
      mAnimatables.clear();

      DestroySourceArrays();
   }
    
   ////////////////////////////////////////////////////////////////////////////////
   const std::string& BaseModelData::GetCharacterSystemType() const
   {
      return mCharacterSystemType;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::Add(Animatable& animatable)
   {
      mAnimatables.push_back(&animatable);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::Remove(Animatable& animatable)
   {
      mAnimatables.erase(std::remove(mAnimatables.begin(), mAnimatables.end(), &animatable));
   }

   DT_IMPLEMENT_ACCESSOR(BaseModelData, dtCore::ResourceDescriptor, Resource);

   DT_IMPLEMENT_ACCESSOR(BaseModelData, std::string, ModelName);

   DT_IMPLEMENT_ACCESSOR_GETTER(BaseModelData, float, Scale);
   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::SetScale(float scale)
   {
      // Ensure scale never goes to 0, to prevent the NAN plague.
      if(scale <= 0.0f)
      {
         scale = 0.001f;
      }

      mScale = scale;
   }

   DT_IMPLEMENT_ACCESSOR(BaseModelData, std::string, ShaderGroupName);
   DT_IMPLEMENT_ACCESSOR(BaseModelData, std::string, ShaderName);
   DT_IMPLEMENT_ACCESSOR(BaseModelData, std::string, PoseMeshFilename);
   /// Sets the maximum number of bones the shader supports
   DT_IMPLEMENT_ACCESSOR(BaseModelData, unsigned, ShaderMaxBones);


   ////////////////////////////////////////////////////////////////////////////////
   Animatable* BaseModelData::GetAnimatableByName(const std::string& name)
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
   const Animatable* BaseModelData::GetAnimatableByName(const std::string& name) const
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
   dtAnim::AnimatableArray& BaseModelData::GetAnimatables()
   {
      return mAnimatables;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const dtAnim::AnimatableArray& BaseModelData::GetAnimatables() const
   {
      return mAnimatables;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::CreateSourceArrays(int numberOfVertices, int numberOfIndices, int stride)
   {
      if (!mVertexArray && !mIndexArray)
      {
         mIndexArray = new osg::IntArray(numberOfIndices);
         mVertexArray = new osg::FloatArray(stride * numberOfVertices);
         
         mStride = stride;
      }
      else
      {
         LOG_ERROR("Unable to create source arrays.")
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::DestroySourceArrays()
   {
      mVertexArray = NULL;
      mIndexArray = NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::VertexBufferObject* BaseModelData::GetVertexBufferObject()
   {
      return mVertexBufferObject.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::SetVertexBufferObject(osg::VertexBufferObject* vbo)
   {
      mVertexBufferObject = vbo;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::ElementBufferObject* BaseModelData::GetElementBufferObject() const
   {
      return mElementBufferObject.get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::SetElementBufferObject(osg::ElementBufferObject* ebo)
   {
      mElementBufferObject = ebo;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::DrawElements* BaseModelData::GetDrawElements() const
   {
      return mElementBufferDrawElements;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseModelData::SetDrawElements(osg::DrawElements* drawElements)
   {
      // Osg does not hold a reference to drawElements, we must make sure to hold our own
      if (mElementBufferObject.valid())
      {
         mElementBufferObject->addDrawElements(drawElements);
      }

      mElementBufferDrawElements = drawElements;
   }


   ////////////////////////////////////////////////////////////////////////////////
   unsigned BaseModelData::GetFileCount(ModelResourceType fileType) const
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
   unsigned BaseModelData::GetFileListForFileType(ModelResourceType fileType, StrArray& outFiles) const
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
   unsigned BaseModelData::GetObjectNameListForFileType(ModelResourceType fileType, StrArray& outNames) const
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
   unsigned BaseModelData::GetObjectNameListForFileTypeSorted(ModelResourceType fileType, StrArray& outNames) const
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
            int id = GetIndexForObjectName(fileType, name);
            if (id < 0)
            {
               id = sortedMap.size();
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
   bool BaseModelData::ReplaceObjectName(ModelResourceType fileType, const std::string& oldObjectName,
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
   bool BaseModelData::SetFileForObjectName(ModelResourceType fileType, const std::string& objectName,
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
   std::string BaseModelData::GetFileForObjectName(ModelResourceType fileType, const std::string& objectName) const
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
   unsigned BaseModelData::GetObjectNameListForFile(const std::string& file, StrArray& outObjectNames) const
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
   bool BaseModelData::RegisterFile(const std::string& file, const std::string& objectName, ModelResourceType fileType)
   {
      bool success = false;

      if (fileType != NO_FILE)
      {
         // Convert the file to a relative path.
         std::string relativeFile(file);
         dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
         fileUtils.CleanupFileString(relativeFile);

         std::string resourceFile;
         // Let it throw exceptions for now.
         if (!mResource.IsEmpty())
         {
            try
            {
               resourceFile = dtCore::Project::GetInstance().GetResourcePath(mResource);
            }
            catch (const dtCore::ProjectInvalidContextException& ex)
            {
               ex.LogException(dtUtil::Log::LOG_ERROR, "basemodeldata.cpp");
            }
         }

         // Get the location of the model file.
         std::string modelContext(osgDB::getFilePath(resourceFile));
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
   bool BaseModelData::RegisterFile(const std::string& file, const std::string& objectName)
   {
      return RegisterFile(file, objectName, GetFileType(file));
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned BaseModelData::UnregisterFile(const std::string& file, StrArray* outObjectNames)
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
   unsigned BaseModelData::UnregisterObjectName(const std::string& objectName,
      dtAnim::ModelResourceType fileType, StrArray* outFileNames)
   {
      unsigned numNames = 0;

      FileToObjectMap::iterator curIter = mFileObjectMap.begin();
      FileToObjectMap::iterator endIter = mFileObjectMap.end();

      // Collect all matches
      const ObjectNameAndFileType* curMapping = NULL;
      for (; curIter != endIter; ++curIter)
      {
         curMapping = curIter->second.get();
         if (curMapping->mName == objectName && curMapping->mType == fileType)
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

   ////////////////////////////////////////////////////////////////////////////////
   unsigned BaseModelData::UnregisterObjectName(const std::string& objectName,
      const std::string& file, StrArray* outFileNames)
   {
      unsigned numNames = 0;

      ModelResourceType fileType = NO_FILE;
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
