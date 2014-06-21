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
#include <osgDB/FileNameUtils>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // HELPER FUNCTIONS
   /////////////////////////////////////////////////////////////////////////////
   int ApplyResourcesToModel(OsgLoader& loader, dtAnim::ModelResourceType resType,
      OsgModelResourceFinder& finder, dtAnim::OsgModelData& modelData)
   {
      int results = 0;

      switch (resType)
      {
      case dtAnim::ANIM_FILE:
         results += loader.ApplyAnimationsToModel(finder, modelData);
         break;

      case dtAnim::MAT_FILE:
         results += loader.ApplyMaterialsToModel(finder, modelData);
         break;

      case dtAnim::MESH_FILE:
         results += loader.ApplyMeshesToModel(finder, modelData);
         break;

      case dtAnim::MORPH_FILE:
         results += loader.ApplyMorphTargetsToModel(finder, modelData);
         break;

      case dtAnim::SKEL_FILE:
         results += loader.ApplySkeletonToModel(finder, modelData);
         break;

      case dtAnim::MIXED_FILE:
      default:
         results += loader.ApplyAllResourcesToModel(finder, modelData);
         break;
      }

      return results;
   }

   int ApplyNodeToModel(OsgLoader& loader, osg::Node& node, dtAnim::ModelResourceType resType, dtAnim::OsgModelData& modelData)
   {
      int results = 0;
      
      // Apply resources from the node but only search for
      // resources if a model node already exists; otherwise,
      // the node will be the new model.

      if (modelData.GetCoreModel() == NULL)
      {
         modelData.SetCoreModel(&node);
         ++results;
      }
      else
      {
         OsgModelResourceFinder newFinder;
         loader.SetFinderMode(resType, newFinder);
         node.accept(newFinder);

         results += ApplyResourcesToModel(loader, resType, newFinder, modelData);
      }

      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   template <class T_FileStruct>
   int LoadFiles(OsgLoader& loader, const std::string& path,
      const std::vector<T_FileStruct>& fileArray, dtAnim::ModelResourceType resType,
      dtAnim::OsgModelData& modelData)
   {
      int results = 0;
      
      typedef std::vector<T_FileStruct> FileArray;
      FileArray::const_iterator curIter = fileArray.begin();
      FileArray::const_iterator endIter = fileArray.end();
      for (; curIter != endIter; ++curIter)
      {
         dtCore::RefPtr<osg::Node> node
            = loader.LoadResourceFile(path, curIter->mFileName, resType);

         if (node.valid())
         {
            results += ApplyNodeToModel(loader, *node, resType, modelData);
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
      dtCore::RefPtr<OsgModelData> modelData = new OsgModelData(handler.mName, handler.mFilename);

      const std::string& path = handler.mPath;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      
      // Load mixed resources, such as single character resource files.
      dtAnim::ModelResourceType resType = dtAnim::MIXED_FILE;
      LoadFiles(*this, path, handler.mMixedResources, resType, *modelData);


      // Load skeleton
      resType = dtAnim::SKEL_FILE;
      dtCore::RefPtr<osg::Node> node = LoadResourceFile(path, handler.mSkeletonFilename, resType);
      if (node.valid())
      {
         ApplyNodeToModel(*this, *node, resType, *modelData);
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
   int OsgLoader::ApplyAnimationsToModel(OsgModelResourceFinder& finder, dtAnim::OsgModelData& modelData)
   {
      int results = 0;

      typedef dtAnim::OsgModelResourceFinder::OsgAnimationArray OsgAnimationArray;
      typedef dtAnim::OsgModelResourceFinder::OsgAnimManagerArray OsgAnimManagerArray;

      osg::Node* node = GetOrCreateModelNode(modelData);

      // Acquire or create an animation manager in the original model.
      OsgModelResourceFinder finderOfOriginal;
      node->accept(finderOfOriginal);
      dtCore::RefPtr<osgAnimation::BasicAnimationManager> originalAnimManager = finderOfOriginal.mAnimManagers.front();
      if ( ! originalAnimManager.valid())
      {
         originalAnimManager = new osgAnimation::BasicAnimationManager;
         node->addUpdateCallback(originalAnimManager);
      }

      // Find all the new animations and add them to the original
      // animation manager on the current model.
      osgAnimation::BasicAnimationManager* animManager = NULL;
      OsgAnimManagerArray::iterator curIter = finder.mAnimManagers.begin();
      OsgAnimManagerArray::iterator endIter = finder.mAnimManagers.end();
      for (; curIter != endIter; ++curIter)
      {
         animManager = curIter->get();

         osgAnimation::Animation* anim = NULL;
         const osgAnimation::AnimationList& animList = animManager->getAnimationList();
         osgAnimation::AnimationList::const_iterator curAnimIter = animList.begin();
         osgAnimation::AnimationList::const_iterator endAnimIter = animList.end();
         for (; curAnimIter != endAnimIter; ++curAnimIter)
         {
            anim = curAnimIter->get();

            originalAnimManager->registerAnimation(anim);
            ++results;
         }
      }

      return results;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgLoader::ApplyMaterialsToModel(OsgModelResourceFinder& finder, dtAnim::OsgModelData& modelData)
   {
      // TODO:
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgLoader::ApplyMeshesToModel(OsgModelResourceFinder& finder, dtAnim::OsgModelData& modelData)
   {
      // TODO:
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgLoader::ApplyMorphTargetsToModel(OsgModelResourceFinder& finder, dtAnim::OsgModelData& modelData)
   {
      // TODO:
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgLoader::ApplySkeletonToModel(OsgModelResourceFinder& finder, dtAnim::OsgModelData& modelData)
   {
      // TODO:
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   int OsgLoader::ApplyAllResourcesToModel(OsgModelResourceFinder& finder, dtAnim::OsgModelData& modelData)
   {
      int results = 0;

      results += ApplySkeletonToModel(finder, modelData);
      results += ApplyMeshesToModel(finder, modelData);
      results += ApplyMaterialsToModel(finder, modelData);
      results += ApplyAnimationsToModel(finder, modelData);
      results += ApplyMorphTargetsToModel(finder, modelData);

      return results;
   }
   
   /////////////////////////////////////////////////////////////////////////////
   osg::Node* OsgLoader::GetOrCreateModelNode(dtAnim::OsgModelData& modelData)
   {
      dtCore::RefPtr<osg::Node> node = modelData.GetCoreModel();

      if (node == NULL)
      {
         node = new osg::Group;
         node->setName(modelData.GetModelName());
         modelData.SetCoreModel(node);
      }

      return node;
   }

   /////////////////////////////////////////////////////////////////////////////
   void OsgLoader::SetFinderMode(dtAnim::ModelResourceType resType, OsgModelResourceFinder& finder)
   {
      switch (resType)
      {
      case dtAnim::ANIM_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_ANIMATIONS;
         break;

      case dtAnim::MAT_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_MATERIALS;
         break;

      case dtAnim::MESH_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_MESHES;
         break;

      case dtAnim::MORPH_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_MORPHS;
         break;

      case dtAnim::SKEL_FILE:
         finder.mMode = OsgModelResourceFinder::SEARCH_SKELETON;
         break;

      default:
         finder.mMode = OsgModelResourceFinder::SEARCH_ALL;
         break;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<osg::Node> OsgLoader::LoadResourceFile(
      const std::string& path, const std::string& file,
      dtAnim::ModelResourceType resType)
   {
      dtCore::RefPtr<osg::Node> node;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string filename(GetAbsolutePath(path + file));
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
            + " file named:'" + path + file + "'.");
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
