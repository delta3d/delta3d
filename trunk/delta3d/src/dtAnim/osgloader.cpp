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
#include <dtAnim/osgmodelwrapper.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <osgDB/FileNameUtils>



namespace dtAnim
{
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
      dtCore::RefPtr<OsgModelData> modelData;

      const std::string& path = handler.mPath;

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      
      // load meshes
      std::vector<CharacterFileHandler::MixedResourceStruct>::iterator curIter = handler.mMixedResources.begin();
      std::vector<CharacterFileHandler::MixedResourceStruct>::iterator endIter = handler.mMixedResources.end();
      for (; curIter != endIter; ++curIter)
      {
         std::string filename(GetAbsolutePath(path + (*curIter).mFileName));
         if (!filename.empty())
         {
            // Load the mesh and get its id for further error checking
            dtCore::RefPtr<osgDB::ReaderWriter::Options> options = dtAnim::OsgLoader::GetOrCreateOSGOptions();
            dtCore::RefPtr<osg::Node> node = fileUtils.ReadNode(filename, options.get());
            if ( ! node.valid())
            {
               LOG_ERROR("Can't load mixed resource file '" + filename +"'");
            }
            else
            {
               // Handle resource specific setup.
               modelData = new OsgModelData(handler.mName,filename);
               modelData->SetCoreModel(node.get());
            }
         }
         else
         {
            LOG_ERROR("Can't find mixed resource file named:'" + path + (*curIter).mFileName + "'.");
         }
      }
      
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
