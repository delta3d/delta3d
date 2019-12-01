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

#ifndef DELTA_BASE_MODEL_LOADER
#define DELTA_BASE_MODEL_LOADER

#include <dtAnim/export.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/characterfilehandler.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT BaseModelLoader : public osg::Referenced
   {
      public:
         typedef osg::Referenced BaseClass;

         BaseModelLoader(const std::string& characterSystem);

         const std::string& GetCharacterSystemType() const;

         // Override
         virtual dtCore::RefPtr<dtAnim::BaseModelData> CreateModelData(dtAnim::CharacterFileHandler& handler) = 0;
         
         // Override
         virtual dtCore::RefPtr<dtAnim::BaseModelWrapper> CreateModel(dtAnim::BaseModelData& data) = 0;

         virtual void SetModelParameters(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData);

         virtual bool Save(const std::string& filename, const dtAnim::BaseModelWrapper& wrapper);

         virtual void Clear();

         virtual void CreateAttachments(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData);

         // Convenience function
         static std::string GetAbsolutePath(const std::string& filePath);
      
      protected:
         virtual ~BaseModelLoader();

         std::string mCharacterSystem;
   };

}

#endif
