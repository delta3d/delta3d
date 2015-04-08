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
* Matthew W. Campbell
*/
#ifndef DELTA_BASELIGHTACTORPROXY
#define DELTA_BASELIGHTACTORPROXY

#include <dtUtil/enumeration.h>
#include <dtCore/light.h>
#include <dtCore/plugin_export.h>
#include <dtCore/transformableactorproxy.h>
#include <osg/Vec4>

namespace dtActors
{

    /**
     * This actor proxy is a wrapper for the dtCore::Light object.  Since the
     * light object cannot be created directly, neither can the proxy. It merely
     * binds the properties that are common to all lights in one place.  So any
     * light proxy object should derive from this one.  Assuming, however, that
     * the underlying Delta3D object is derived from dtCore::Light.
     */
    class DT_PLUGIN_EXPORT BaseLightActorProxy : public dtCore::TransformableActorProxy
    {
    public:

        /**
         * Constructs the light actor proxy.
         */
        BaseLightActorProxy() { SetClassName("dtCore::Light"); }

        /**
         * Abstract lights are not placeable.
         * @return False.
         */
        virtual bool IsPlaceable() const {  return false; }

        /**
         * Adds the properties that are common to all Delta3D light objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Gets the current render mode for lights.
         * @return Note, this returns RenderMode::DRAW_ACTOR by default.  Other
         *  types of lights such as positional lights have different render modes.
         */
        virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode()
        {
            return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
        }

        /**
         * Gets the billboard icon for lights.
         * @return The billboard icon common to all lights.
         */
        virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

    public:

        //dtCore::Light has an enumeration.  We define our own enumeration here
        //which can be exported as a property to the editor.
        class DT_PLUGIN_EXPORT LightModeEnum : public dtUtil::Enumeration
        {
            DECLARE_ENUM(LightModeEnum);
        public:
            static LightModeEnum GLOBAL;
            static LightModeEnum LOCAL;
        private:
            LightModeEnum(const std::string &name) : dtUtil::Enumeration(name)
            {
                AddInstance(this);
            }
        };

        //The methods are for setting and getting properties. They are needed when
        //the actor object itself does not provide the necessary getter and setter
        //for a particular property.

        ///Sets the light number of the light actor.
        void SetNumber(int num)
        {
            dtCore::Light* l = GetDrawable<dtCore::Light>();

            l->SetNumber(num);
        }

        ///Sets the ambient color of the light actor.
        void SetAmbient(const osg::Vec4 &color)
        {
            dtCore::Light* l = GetDrawable<dtCore::Light>();

            l->SetAmbient(color[0],color[1],color[2],color[3]);
        }

        ///Gets the ambient color of the light actor.
        osg::Vec4 GetAmbient() const
        {
            const dtCore::Light* l = GetDrawable<dtCore::Light>();

            float r,g,b,a;
            l->GetAmbient(r,g,b,a);
            return osg::Vec4(r,g,b,a);
        }

        ///Sets the diffuse color of the light actor.
        void SetDiffuse(const osg::Vec4& color)
        {
            dtCore::Light* l = GetDrawable<dtCore::Light>();

            l->SetDiffuse(color[0],color[1],color[2],color[3]);
        }

        ///Gets the diffuse color of the light actor.
        osg::Vec4 GetDiffuse() const
        {
            const dtCore::Light* l = GetDrawable<dtCore::Light>();

            float r,g,b,a;
            l->GetDiffuse(r,g,b,a);
            return osg::Vec4(r,g,b,a);
        }

        ///Sets the specular color of the light actor.
        void SetSpecular(const osg::Vec4& color)
        {
            dtCore::Light* l = GetDrawable<dtCore::Light>();

            l->SetSpecular(color[0],color[1],color[2],color[3]);
        }

        ///Gets the specular color of the light actor.
        osg::Vec4 GetSpecular() const
        {
            const dtCore::Light* l = GetDrawable<dtCore::Light>();

            float r,g,b,a;
            l->GetSpecular(r,g,b,a);
            return osg::Vec4(r,g,b,a);
        }

        ///Sets the lighting mode for this light actor.
        void SetLightingMode(LightModeEnum& mode)
        {
            dtCore::Light* l = GetDrawable<dtCore::Light>();

            if (mode == LightModeEnum::GLOBAL)
            {
                l->SetLightingMode(dtCore::Light::GLOBAL);
            }
            else if (mode == LightModeEnum::LOCAL)
            {
                l->SetLightingMode(dtCore::Light::LOCAL);
            }
        }

        ///Gets the lighting mode for this light actor.
        LightModeEnum& GetLightingMode() const
        {
            const dtCore::Light* l = GetDrawable<dtCore::Light>();

            if (l->GetLightingMode() == dtCore::Light::GLOBAL)
            {
                return LightModeEnum::GLOBAL;
            }
            else
            {
                return LightModeEnum::LOCAL;
            }
        }

    protected:

        /**
         * Initializes the actor
         */
        virtual void CreateDrawable() = 0;

        ///Protected since this object is wrapped with a smart pointer.
        virtual ~BaseLightActorProxy() { }
    };
}

#endif
