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
#ifndef DELTA_SOUND_ACTOR_PROXY
#define DELTA_SOUND_ACTOR_PROXY

#include "dtDAL/plugin_export.h"
#include "dtDAL/transformableactorproxy.h"
#include "dtDAL/exception.h"

namespace dtActors 
{
    /**
     * @class SoundActorProxy
     * @brief This proxy wraps the Sound Delta3D object.
     */
    class DT_PLUGIN_EXPORT SoundActorProxy : public dtDAL::TransformableActorProxy 
    {
    public:

        /**
         * Constructor
         */
        SoundActorProxy()
        {
            /**
             * @note You must instantiate, configure, and shutdown the
             * audiomanager in your application
             * ex.
             * \code
             dtAudio::AudioManager::Instantiate();
             dtAudio::AudioManager::GetManager()->Config(AudioConfigData&)
             * \endcode
             */
            SetClassName("dtAudio::Sound");
        }

        /**
         * Adds the properties that are common to all Delta3D physical objects.
         */
        virtual void BuildPropertyMap();

        /**
         * Loads in a sound file
         * @param fileName The file to load
         */
        void LoadFile(const std::string &fileName);

        /**
         * Sets the direction
         * @param dir The direction to Set
         */
        void SetDirection(const osg::Vec3 &dir);

        /**
         * Gets the direction
         * @return The current direction
         */
        osg::Vec3 GetDirection();

        /**
         * Sets the velocity
         * @param vel The velocity to Set
         */
        void SetVelocity(const osg::Vec3 &vel);

        /**
         * Gets the velocity
         * @return The current velocity
         */
        osg::Vec3 GetVelocity();

        /**
         * Plays the sound
         */
        void Play();

        /**
         * Gets the render mode of sound actor proxies.
         * @return RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON.  Although this may seem a little
         *  strange considering you cannot actually draw a sound, however, this informs the
         *  world that this proxy's actor and billboard should be represented in the scene.
         */
        virtual const ActorProxy::RenderMode& GetRenderMode() 
        {
            return ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON;
        }

        /**
         * Gets the billboard icon associated with sound actor proxies.
         * @return
         */
        virtual dtDAL::ActorProxyIcon* GetBillBoardIcon();

    protected:

        /**
         * Creates a new positional sound.
         */
        virtual void CreateActor();

        /**
         * Destructor
         */
        virtual ~SoundActorProxy();

    };
}

#endif
