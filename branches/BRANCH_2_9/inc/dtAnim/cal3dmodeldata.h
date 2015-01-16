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
#ifndef DELTA_CAL3D_MODEL_DATA
#define DELTA_CAL3D_MODEL_DATA

#include <dtAnim/export.h>
#include <dtAnim/basemodeldata.h>
#include <dtCore/refptr.h>

DT_DISABLE_WARNING_ALL_START
#include <cal3d/global.h>
DT_DISABLE_WARNING_END

#include <dtUtil/hotspotdefinition.h>

#include <vector>

class CalCoreModel;
class CalHardwareModel;

namespace dtAnim
{
   class Animatable;
   
   typedef std::vector<std::pair<dtUtil::HotSpotDefinition, std::string> > AttachmentArray;

   class DT_ANIM_EXPORT Cal3DModelData : public dtAnim::BaseModelData
   {
   public:
      typedef dtAnim::BaseModelData BaseClass;

      Cal3DModelData(const std::string& modelName, const dtCore::ResourceDescriptor& resource);

      /**
       * Adds a non-skinned attachment to a bone for this character data.  Often this is just an invisible transformable to attach something.
       */
      void AddHotspot(const std::pair<dtUtil::HotSpotDefinition, std::string>& attachment);

      /**
       * Removes a non-skinned attachment to a bone from this character data.
       */
      void RemoveHotspot(unsigned int which);

      CalCoreModel* GetCoreModel();
      const CalCoreModel* GetCoreModel() const;

      virtual float GetAnimationDuration(const std::string& name) const;

      /// @return the list of non-character bone attachments
      AttachmentArray& GetAttachments();
      /// @return the list of non-character bone attachments as const
      const AttachmentArray& GetAttachments() const;

      CalHardwareModel* GetCalHardwareModel();
      CalHardwareModel* GetOrCreateCalHardwareModel();

      virtual int LoadResource(dtAnim::ModelResourceType resourceType,
         const std::string& file, const std::string& objectName);

      virtual int UnloadResource(dtAnim::ModelResourceType resourceType, const std::string& objectName);

#if defined(CAL3D_VERSION) && CAL3D_VERSION >= 1300
      bool LoadCoreSkeletonBuffer(void* buffer, const std::string& file, const std::string& objectName);
      int LoadCoreMorphBuffer(void* buffer, const std::string& file, const std::string& objectName);
      int LoadCoreMaterialBuffer(void* buffer, const std::string& file, const std::string& objectName);
      int LoadCoreMeshBuffer(void* buffer, const std::string& file, const std::string& objectName);
      int LoadCoreAnimationBuffer(void* buffer, const std::string& file, const std::string& objectName);

      int LoadCoreMorph(const std::string& file, const std::string& objectName);
      bool LoadCoreSkeleton(const std::string& file, const std::string& objectName);
#else
      bool LoadCoreSkeleton(const std::string& file);
#endif
      int LoadCoreMaterial(const std::string& file, const std::string& objectName);
      int LoadCoreMesh(const std::string& file, const std::string& objectName);
      int LoadCoreAnimation(const std::string& file, const std::string& objectName);

      /**
       * Convenience method for determining the file type from the specified file.
       */
      virtual dtAnim::ModelResourceType GetFileType(const std::string& file) const;

      virtual int GetIndexForObjectName(ModelResourceType fileType, const std::string& objectName) const;

   protected:
      virtual ~Cal3DModelData();

      Cal3DModelData(const Cal3DModelData&); //not implemented
      Cal3DModelData& operator=(const Cal3DModelData&); //not implemented

   private:
      CalCoreModel* mCoreModel;
      CalHardwareModel* mHardwareModel;
      AttachmentArray mAttachments;
   };

} // namespace dtAnim

#endif /*DELTA_CAL3D_MODEL_DATA*/
