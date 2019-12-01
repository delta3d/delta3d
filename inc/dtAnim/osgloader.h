/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2007 MOVES Institute
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
 * Chris Rodgers
 */

#ifndef __DELTA_OSGLOADER_H__
#define __DELTA_OSGLOADER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtAnim/basemodelloader.h>
#include <osgDB/ReadFile>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtAnim
{
   class BaseModelData;
   class OsgModelData;
   class OsgModelResourceFinder;


   
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgExtensionEnum : public dtUtil::Enumeration
   {
         DECLARE_ENUM(OsgExtensionEnum);
      public:
         typedef dtUtil::Enumeration BaseClass;

         static OsgExtensionEnum TARGET;

         const std::string& GetDescription() const;

         dtAnim::ModelResourceType GetResourceType() const;

      private:
         OsgExtensionEnum(const std::string& name,
            dtAnim::ModelResourceType resourceType,
            const std::string& description);

         dtAnim::ModelResourceType mResourceType;
         std::string mDescription;
   };

   typedef OsgExtensionEnum OsgExtEnum;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgLoader: public dtAnim::BaseModelLoader
   {
   public:
      typedef dtAnim::BaseModelLoader BaseClass;

      OsgLoader();

      // Override
      virtual dtCore::RefPtr<dtAnim::BaseModelData> CreateModelData(CharacterFileHandler& handler);
      
      virtual dtCore::RefPtr<dtAnim::BaseModelWrapper> CreateModel(dtAnim::BaseModelData& data);
      
      virtual void Clear();

      virtual void CreateAttachments(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData);

      static dtCore::RefPtr<osgDB::ReaderWriter::Options> GetOrCreateOSGOptions();

      dtCore::RefPtr<osg::Node> LoadResourceFile(
         const std::string& filepath,
         dtAnim::ModelResourceType resType);

   protected:
      virtual ~OsgLoader();

   private:
   };

} // namespace dtAnim

#endif
