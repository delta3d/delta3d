/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* William E. Johnson II
*/
#ifndef DELTA_SKYBOX_ACTOR_PROXY
#define DELTA_SKYBOX_ACTOR_PROXY

#include <dtCore/plugin_export.h>
#include <dtCore/actorproxy.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/skybox.h>
#include <dtCore/resourcedescriptor.h>
#include <dtActors/enveffectactorproxy.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/getsetmacros.h>

namespace dtActors
{

   /**
   * @class SkyBoxActorProxy
   * @brief This proxy exposes skyboxes to the level editor.
   */
   class DT_PLUGIN_EXPORT SkyBoxActorProxy : public EnvEffectActorProxy
   {
   public:

      class DT_PLUGIN_EXPORT RenderProfile : public dtUtil::Enumeration
      {
         DECLARE_ENUM(RenderProfile);

      public:
         static RenderProfile FIXED_FUNCTION;
         static RenderProfile CUBE_MAP;
         static RenderProfile ANGULAR_MAP;
         static RenderProfile DEFAULT;

      private:
         RenderProfile(const std::string& name)
            : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      SkyBoxActorProxy();

      /**
      * Adds the properties that are common to all sky box objects.
      */
      virtual void BuildPropertyMap();

      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, TopTexture)
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, BottomTexture)
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, FrontTexture)
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, BackTexture)
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, LeftTexture)
      DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, RightTexture)


      /// Sets the texture on the bottom of the sky box
      void SetTexture(dtCore::SkyBox::SkyBoxSideEnum which, const std::string& path)
      {
         dtCore::SkyBox *skyBox = dynamic_cast<dtCore::SkyBox *>(GetDrawable());
         if (skyBox == NULL)
            throw dtCore::BaseException(
            "Expected a skybox actor.", __FILE__, __LINE__);

         skyBox->SetTexture(which,path);
      }

      /// Set the render profile to be used with the skybox
      void SetRenderProfilePreference(RenderProfile& profile);

      /// Gets the render profile to be used with the skybox
      RenderProfile& GetRenderProfilePreference() const
      {
         return *mRenderProfile;
      }

   protected:

      RenderProfile* mRenderProfile;

      virtual ~SkyBoxActorProxy() {}

      virtual void CreateDrawable();
   };
}

#endif
