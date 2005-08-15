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
* @author William E. Johnson II
*/
#ifndef DELTA_ENVIRONMENT_ACTOR_PROXY
#define DELTA_ENVIRONMENT_ACTOR_PROXY

#include "dtDAL/plugin_export.h"
#include "dtDAL/actorproxy.h"
#include <dtCore/environment.h>
#include "dtActors/enveffectactorproxy.h"
#include "dtDAL/exception.h"
#include "dtUtil/enumeration.h"

namespace dtActors 
{
    /**
     * @class EnvironmentActorProxy
     * @brief This proxy wraps the Environment Delta3D object.
     */
    class DT_PLUGIN_EXPORT EnvironmentActorProxy : public dtDAL::ActorProxy 
    {
    public:

        class DT_PLUGIN_EXPORT FogModeEnum : public dtUtil::Enumeration
        {
            DECLARE_ENUM(FogModeEnum)
        public:
            static FogModeEnum LINEAR;
            static FogModeEnum EXP;
            static FogModeEnum EXP2;
            static FogModeEnum ADV;
        private:
            FogModeEnum(const std::string &name) : dtUtil::Enumeration(name)
            {
                AddInstance(this);
            }
        };

        /**
         * Constructor
         */
        EnvironmentActorProxy() { SetClassName("dtCore::Environment"); }

        /**
         * Adds the properties that are common to all Delta3D environment objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Environments cannot be placed in a scene
         */
        virtual bool IsPlaceable() const { return false; }

        /**
         * Sets the sky color
         * @param color The color to Set
         */
        void SetSkyColor(const osg::Vec3 &color);

        /**
         * Gets the sky color
         * @return The current color
         */
        osg::Vec3 GetSkyColor();

        /**             
         * Sets the fog color
         * @param color The color to Set
         */
        void SetFogColor(const osg::Vec3 &color);

        /**
         * Gets the fog color
         * @return The current color
         */
        osg::Vec3 GetFogColor();

        /**
         * Sets the fog mode
         * @param mode The desired fog mode
         */
        void SetFogMode(FogModeEnum &mode);

        /**
         * Gets the fog mode
         * @return The current fog mode
         */
        FogModeEnum& GetFogMode();

        /**
         * Sets the lat/long
         * @param latlong The desired lat/long
         */
        void SetRefLatLong(const osg::Vec2 &latlong);

        /**
         * Gets the lat/long
         * @return The current lat/long
         */
        osg::Vec2 GetRefLatLong();

        /**
         * Adds an effect to the environment
         * @param effect The effect to add
         */
        void AddEffect(osg::ref_ptr<EnvEffectActorProxy> effect);

        /**
         * Removes an effect from the list
         * @param effect The effect to remove
         */
        void RemoveEffect(osg::ref_ptr<EnvEffectActorProxy> effect);

        /**
         * Retrieves the list of effects the environment contains
         * @return The list of effects
         */
        std::vector< osg::ref_ptr<EnvEffectActorProxy> >& GetEffects();

    private:
        /**
         * The list of effects for the environment
         */
        std::vector< osg::ref_ptr<EnvEffectActorProxy> > mEffects;

    protected:

        /**
         * Creates the global Delta3D environment actor.
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~EnvironmentActorProxy() { }
    };
}

#endif
