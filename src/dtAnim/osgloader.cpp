/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/osgloader.h>
#include <dtAnim/constants.h>
#include <dtAnim/osgmodeldata.h>
#include <dtAnim/osgmodelwrapper.h>
#include <dtAnim/osgmodelresourcefinder.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtCore/project.h>
#include <osgDB/FileNameUtils>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   IMPLEMENT_ENUM(OsgExtensionEnum);
   OsgExtensionEnum OsgExtensionEnum::TARGET("target", MORPH_FILE, "MakeHuman Morph Target File (text)");

   OsgExtensionEnum::OsgExtensionEnum(const std::string& name,
      dtAnim::ModelResourceType resourceType, const std::string& description)
      : BaseClass(name)
      , mResourceType(resourceType)
      , mDescription(description)
   {
      OsgExtensionEnum::AddInstance(this);
   }

   const std::string& OsgExtensionEnum::GetDescription() const
   {
      return mDescription;
   }

   dtAnim::ModelResourceType OsgExtensionEnum::GetResourceType() const
   {
      return mResourceType;
   }



   /////////////////////////////////////////////////////////////////////////////
   // HELPER FUNCTIONS
   /////////////////////////////////////////////////////////////////////////////
   template <class T_FileStruct>
   int LoadFiles(OsgLoader& loader, const std::string& path,
      const std::vector<T_FileStruct>& fileArray, dtAnim::ModelResourceType resType,
      dtAnim::OsgModelData& modelData)
   {
      int results = 0;
      
      typedef std::vector<T_FileStruct> FileArray;
      typename FileArray::const_iterator curIter = fileArray.begin();
      typename FileArray::const_iterator endIter = fileArray.end();
      for (; curIter != endIter; ++curIter)
      {
         std::string filepath(path + curIter->mFileName);
         dtCore::RefPtr<osg::Node> node
            = loader.LoadResourceFile(filepath, resType);

         if (node.valid() && modelData.GetFileForObjectName(resType, node->getName()).empty())
         {
            modelData.RegisterFile(curIter->mFileName, node->getName(), resType);

            results += modelData.ApplyNodeToModel(resType, *node);
         }
      }

      return results;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   OsgLoader::OsgLoader()
      : BaseClass(Constants::CHARACTER_SYSTEM_OSG)
   {}

   OsgLoader::~OsgLoader()
   {
      Clear();
   }

   dtCore::RefPtr<dtAnim::BaseModelData> OsgLoader::CreateModelData(CharacterFileHandler& handler)
   {
      dtCore::RefPtr<OsgModelData> modelData = new OsgModelData(handler.mName, handler.mResource);

      std::string path;
      try
      {
         path = dtCore::Project::GetInstance().GetResourcePath(handler.mResource);
         path = osgDB::getFilePath(path) + "/";
      }
      catch(const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR, "cal3dloader.cpp");
         path = "./";
      }

      // Load mixed resources, such as single character resource files.
      dtAnim::ModelResourceType resType = dtAnim::MIXED_FILE;
      LoadFiles(*this, path, handler.mMixedResources, resType, *modelData);


      // Load skeleton
      resType = dtAnim::SKEL_FILE;
      std::string filepath(path + handler.mSkeletonFilename);
      dtCore::RefPtr<osg::Node> node = LoadResourceFile(filepath, resType);
      if (node.valid())
      {
         modelData->ApplyNodeToModel(resType, *node);
      }


      // Load meshes
      resType = dtAnim::MESH_FILE;
      LoadFiles(*this, path, handler.mMeshes, resType, *modelData);


      // Load materials
      resType = dtAnim::MAT_FILE;
      LoadFiles(*this, path, handler.mMaterials, resType, *modelData);


      // Load animations
      resType = dtAnim::ANIM_FILE;
      LoadFiles(*this, path, handler.mAnimations, resType, *modelData);


      // Load morph targets
      resType = dtAnim::MORPH_FILE;
      LoadFiles(*this, path, handler.mMorphAnimations, resType, *modelData);
      

      // Set last few model parameters and attachments.
      BaseClass::SetModelParameters(handler, *modelData);
      CreateAttachments(handler, *modelData);
      
      return modelData.get();
   }
   
   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtAnim::BaseModelWrapper> OsgLoader::CreateModel(dtAnim::BaseModelData& data)
   {
      dtAnim::OsgModelData* osgModelData = dynamic_cast<dtAnim::OsgModelData*>(&data);
      return new dtAnim::OsgModelWrapper(*osgModelData);
   }
      
   /////////////////////////////////////////////////////////////////////////////
   void OsgLoader::Clear()
   {
      // TODO:
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgLoader::CreateAttachments(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData)
   {
      // TODO:
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Node> OsgLoader::LoadResourceFile(
      const std::string& filepath,
      dtAnim::ModelResourceType resType)
   {
      dtCore::RefPtr<osg::Node> node;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string filename(GetAbsolutePath(filepath));
      if (!filename.empty())
      {
         // Load the mesh and get its id for further error checking
         dtCore::RefPtr<osgDB::ReaderWriter::Options> options = dtAnim::OsgLoader::GetOrCreateOSGOptions();
         node = fileUtils.ReadNode(filename, options.get());
         if ( ! node.valid())
         {
            LOG_ERROR("Can't load " + BaseModelData::GetResourceTypeName(resType)
               + " file '" + filename +"'");
         }
      }
      else
      {
         LOG_ERROR("Can't find " + BaseModelData::GetResourceTypeName(resType)
            + " file named:'" + filepath + "'.");
      }

      return node;
   }



   /////////////////////////////////////////////////////////////////////////////
   // CLASS METHODS
   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osgDB::ReaderWriter::Options> OsgLoader::GetOrCreateOSGOptions()
   {
      dtCore::RefPtr<osgDB::ReaderWriter::Options> newOptions;
      const osgDB::ReaderWriter::Options* globalOptions = osgDB::Registry::instance()->getOptions();

      if (globalOptions != NULL)
      {
         newOptions = static_cast<osgDB::ReaderWriter::Options*>(globalOptions->clone(0));
      }
      else
      {
         newOptions = new osgDB::ReaderWriter::Options;
      }

      return newOptions;
   }

} // namespace dtAnim
