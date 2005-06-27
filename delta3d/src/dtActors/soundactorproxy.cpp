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

#include "dtActors/soundactorproxy.h"
#include "dtDAL/enginepropertytypes.h"

#include <dtAudio/dtaudio.h>

using namespace dtAudio;
using namespace dtDAL;

namespace dtActors 
{
    SoundActorProxy::~SoundActorProxy()
    {
        dtAudio::Sound *snd = dynamic_cast<dtAudio::Sound*>(mActor.get());
        if(!snd)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        if(snd->GetFilename())
            snd->UnloadFile();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::CreateActor()
    {
        mActor = dtAudio::AudioManager::GetManager()->NewSound();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Sound";
        TransformableActorProxy::BuildPropertyMap();

        Sound *sound = dynamic_cast<Sound*>(mActor.get());
        if(!sound)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        // This property toggles whether or not a sound loops. A
        // value of true will loop the sound, while a value of false
        // will not loop/stop looping a sound.
        // Default is false
        AddProperty(new BooleanActorProperty("Looping", "Looping",
            MakeFunctor(*sound, &Sound::SetLooping),
            MakeFunctorRet(*sound, &Sound::IsLooping),
            "Toggles if a sound loops continuously.", GROUPNAME));

        // This property manipulates the gain of a sound. It uses
        // a float type to represent the gain value.
        // Clamped between 0 - 1 by default.
        // Default is 1.0f
        AddProperty(new FloatActorProperty("Gain", "Gain",
            MakeFunctor(*sound, &Sound::SetGain),
            MakeFunctorRet(*sound, &Sound::GetGain),
            "Sets the gain of a sound.", GROUPNAME));

        // This property manipulates the pitch of a sound. It uses
        // a float type to represent the pitch value.
        // Default is 1.0f
        AddProperty(new FloatActorProperty("Pitch", "Pitch",
            MakeFunctor(*sound, &Sound::SetPitch),
            MakeFunctorRet(*sound, &Sound::GetPitch),
            "Sets the pitch of a sound.", GROUPNAME));

        // This property toggles whether or not a sound is listerner
        // relative.
        // Default is false
        AddProperty(new BooleanActorProperty("Listener Relative", "Listener Relative",
            MakeFunctor(*sound, &Sound::ListenerRelative),
            MakeFunctorRet(*sound, &Sound::IsListenerRelative),
            "Toggles if a sound is relative to the listener.", GROUPNAME));

        // This property manipulates the minimum distance of a sound. It uses
        // a float type to represent the minimum distance.
        // Default is 1.0f
        AddProperty(new FloatActorProperty("Min Distance", "Min Distance",
            MakeFunctor(*sound, &Sound::SetMinDistance),
            MakeFunctorRet(*sound, &Sound::GetMinDistance),
            "Sets the minimum distance of a sound.", GROUPNAME));

        // This property manipulates the maximum distance of a sound. It uses
        // a float type to represent the maximum distance.
        // Default is 3.402823466e+38F (OxFFFFFFFF).
        AddProperty(new FloatActorProperty("Max Distance", "Max Distance",
            MakeFunctor(*sound, &Sound::SetMaxDistance),
            MakeFunctorRet(*sound, &Sound::GetMaxDistance),
            "Sets the maximum distance of a sound.", GROUPNAME));

        // This property manipulates the roll off factor of a sound. It uses
        // a float type to represent the roll off factor.
        // Default is 1.0f
        AddProperty(new FloatActorProperty("Rolloff Factor", "Rolloff Factor",
            MakeFunctor(*sound, &Sound::SetRolloffFactor),
            MakeFunctorRet(*sound, &Sound::GetRolloffFactor),
            "Sets the rolloff factor of a sound.", GROUPNAME));

        // This property manipulates the minimum gain of a sound. It uses
        // a float type to represent the minimum gain.
        // Default is 0.0f
        AddProperty(new FloatActorProperty("Min Gain", "Min Gain",
            MakeFunctor(*sound, &Sound::SetMinGain),
            MakeFunctorRet(*sound, &Sound::GetMinGain),
            "Sets the minimum gain of a sound.", GROUPNAME));

        // This property manipulates the maximum gain of a sound. It uses
        // a float type to represent the maximum gain.
        // Default is 1.0f
        AddProperty(new FloatActorProperty("Max Gain", "Max Gain",
            MakeFunctor(*sound, &Sound::SetMaxGain),
            MakeFunctorRet(*sound, &Sound::GetMaxGain),
            "Sets the maximum gain of a sound.", GROUPNAME));

        // This property manipulates the direction of a sound. It uses
        // 3 values to represent the sound's direction.
        // Default is 0, 1, 0
        AddProperty(new Vec3ActorProperty("Direction", "Direction",
            MakeFunctor(*this, &SoundActorProxy::SetDirection),
            MakeFunctorRet(*this, &SoundActorProxy::GetDirection),
            "Sets the direction of a sound.", GROUPNAME));

        // This property manipulates the velocity of a sound. It uses
        // 3 values to represent the velocity.
        // Default is 0, 0, 0
        AddProperty(new Vec3ActorProperty("Velocity", "Velocity",
            MakeFunctor(*this, &SoundActorProxy::SetVelocity),
            MakeFunctorRet(*this, &SoundActorProxy::GetVelocity),
            "Sets the velocity of a sound.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::LoadFile(const std::string &fileName)
    {
        Sound* snd = dynamic_cast<Sound*>(mActor.get());
        if(!snd)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        if(!fileName.empty())
            snd->LoadFile(fileName.c_str());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::SetDirection(const osg::Vec3 &dir)
    {
        Sound* snd = dynamic_cast<Sound*>(mActor.get());
        if(!snd)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        sgVec3 Pos;
        Pos[0] = dir[0]; Pos[1] = dir[1]; Pos[2] = dir[2];
        snd->SetDirection(Pos);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 SoundActorProxy::GetDirection()
    {
        Sound* snd = dynamic_cast<Sound*>(mActor.get());
        if(!snd)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        sgVec3 Pos;
        snd->GetDirection(Pos);
        return osg::Vec3(Pos[0], Pos[1], Pos[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::SetVelocity(const osg::Vec3 &vel)
    {
        Sound* snd = dynamic_cast<Sound*>(mActor.get());
        if(!snd)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        sgVec3 Pos;
        Pos[0] = vel[0]; Pos[1] = vel[1]; Pos[2] = vel[2];
        snd->SetVelocity(Pos);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 SoundActorProxy::GetVelocity()
    {
        Sound* snd = dynamic_cast<Sound*>(mActor.get());
        if(!snd)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        sgVec3 Pos;
        snd->GetVelocity(Pos);
        return osg::Vec3(Pos[0], Pos[1], Pos[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SoundActorProxy::Play()
    {
        Sound* snd = dynamic_cast<Sound*>(mActor.get());
        if(!snd)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtAudio::Sound");

        snd->Play();
    }

    ///////////////////////////////////////////////////////////////////////////////
    dtDAL::ActorProxyIcon* SoundActorProxy::GetBillBoardIcon()
    {
        if(!mBillBoardIcon.valid()) {
            mBillBoardIcon =
                 new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::SOUND);
        }

        return mBillBoardIcon.get();
    }
}
