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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/basemodelloader.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   BaseModelLoader::BaseModelLoader(const std::string& characterSystem)
      : mCharacterSystem(characterSystem)
   {}

   BaseModelLoader::~BaseModelLoader()
   {
      Clear();
   }

   void BaseModelLoader::SetModelParameters(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData)
   {
      // Set shader scale and visibility parameters
      modelData.SetModelName(handler.mName);
      modelData.SetShaderGroupName(handler.mShaderGroup);
      modelData.SetShaderName(handler.mShaderName);
      modelData.SetShaderMaxBones(handler.mShaderMaxBones);

      LODOptions& lodOptions = modelData.GetLODOptions();

      if (handler.mFoundLODOptions)
      {
         lodOptions.SetStartDistance(handler.mLODStartDistance);
         lodOptions.SetEndDistance(handler.mLODEndDistance);
         lodOptions.SetMaxVisibleDistance(handler.mLODMaxVisibleDistance);
      }

      if (handler.mFoundScale)
      {
         modelData.SetScale(handler.mScale);
      }
   }

   const std::string& BaseModelLoader::GetCharacterSystemType() const
   {
      return mCharacterSystem;
   }

   bool BaseModelLoader::Save(const std::string& filename, const dtAnim::BaseModelWrapper& wrapper)
   {
      // OVERRIDE:
      return false;
   }

   void BaseModelLoader::Clear()
   {
      // OVERRIDE:
   }
   
   void BaseModelLoader::CreateAttachments(
      dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData)
   {
      // OVERRIDE:
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string BaseModelLoader::GetAbsolutePath(const std::string& filePath)
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      dtUtil::FileInfo fi = fileUtils.GetFileInfo(filePath, true);

      return fi.fileType == dtUtil::REGULAR_FILE
         ? fi.fileName
         : dtUtil::FindFileInPathList(filePath);
   }

}
