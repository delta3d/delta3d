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
 * Erik Johnson 03/20/2007
 */

#ifndef DELTA_CAL3DLOADER
#define DELTA_CAL3DLOADER

#include <dtAnim/export.h>
#include <dtAnim/basemodelloader.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/characterfilehandler.h>
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Texture2D>

#include <string>
#include <map>
#include <queue>


class CalCoreModel;

namespace dtAnim
{
   class Animatable;
   
   ////////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   ////////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dExtensionEnum : public dtUtil::Enumeration
   {
         DECLARE_ENUM(Cal3dExtensionEnum);
      public:
         typedef dtUtil::Enumeration BaseClass;
         
         static Cal3dExtensionEnum ANIMATION_XML;
         static Cal3dExtensionEnum ANIMATION_BINARY;
         static Cal3dExtensionEnum MATERIAL_XML;
         static Cal3dExtensionEnum MATERIAL_BINARY;
         static Cal3dExtensionEnum MESH_XML;
         static Cal3dExtensionEnum MESH_BINARY;
         static Cal3dExtensionEnum MORPH_XML;
         static Cal3dExtensionEnum MORPH_BINARY;
         static Cal3dExtensionEnum SKELETON_XML;
         static Cal3dExtensionEnum SKELETON_BINARY;

         const std::string& GetDescription() const;

         dtAnim::ModelResourceType GetResourceType() const;

      private:
         Cal3dExtensionEnum(const std::string& name,
            dtAnim::ModelResourceType resourceType,
            const std::string& description);

         dtAnim::ModelResourceType mResourceType;
         std::string mDescription;
   };

   /**
    * Loads a animation definition file and returns a valid CalModel.  Caches
    * the CalCoreModel defined by the file to make it faster to create additional
    * instances of CalModels.  If you call Load() with the same filename twice,
    * it actually only loads once.
    */
   class DT_ANIM_EXPORT Cal3dLoader: public dtAnim::BaseModelLoader
   {
   public:
      typedef dtAnim::BaseModelLoader BaseClass;

      Cal3dLoader();

      // Override
      virtual dtCore::RefPtr<dtAnim::BaseModelData> CreateModelData(CharacterFileHandler& handler);

      virtual dtCore::RefPtr<dtAnim::BaseModelWrapper> CreateModel(dtAnim::BaseModelData& data);
      
      virtual bool Save(const std::string& filename, const dtAnim::BaseModelWrapper& wrapper);

      ///empty all containers of CalCoreModels and the stored textures
      virtual void Clear();

      virtual void CreateAttachments(dtAnim::CharacterFileHandler& handler, dtAnim::BaseModelData& modelData);

      void LoadHardwareData(Cal3DModelData* modelData);

   protected:
      virtual ~Cal3dLoader();

   private:
      // this is an unpleasant place to load texture files.  Needs to be handled some other way
      void LoadAllTextures(CalCoreModel& model, const std::string& path);

      void InvertTextureCoordinates(CalHardwareModel* hardwareModel, const size_t stride,
         float* vboVertexAttr, Cal3DModelData* modelData, CalIndex*& indexArray);

      unsigned int GetMaxBoneID(CalCoreMesh& mesh);
      
      typedef std::map<std::string, osg::ref_ptr<osg::Texture2D> > TextureMap;
      typedef std::map<std::string, cal3d::RefPtr<CalCoreAnimation> > AnimationMap;

      TextureMap mTextureCache;
      AnimationMap mAnimationCache;
   };

} // namespace dtAnim

#endif // __DELTA_CAL3DLOADER_H__
