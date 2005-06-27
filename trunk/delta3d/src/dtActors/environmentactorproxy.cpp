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

#include "dtActors/environmentactorproxy.h"
#include "dtDAL/enginepropertytypes.h"

using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
    void EnvironmentActorProxy::CreateActor()
    {
        mActor = new dtCore::Environment;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::BuildPropertyMap()
    {
        const std::string &GROUPNAME = "Global Environment";
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment.");

        // This property manipulates the sky color of an Environment.
        // A SkyDome/SkyBox must be added as an effect to the environment
        // in order for this property to have an effect. Uses an
        // osg::Vec3 in RGB format. All values are clamped between 0 - 1.
        // Default is  0.39f, 0.50f, 0.74f
        AddProperty(new Vec3ActorProperty("Sky Color", "Sky Color",
            MakeFunctor(*this, &EnvironmentActorProxy::SetSkyColor),
            MakeFunctorRet(*this, &EnvironmentActorProxy::GetSkyColor),
            "Sets the color of the sky associated with the environment.", GROUPNAME));

        // This property manipulates the fog color of an Environment.
        // Uses 3 values in RGB format. All values are clamped
        // between 0 - 1.
        // Default is 0.84f, 0.87f, 1.0f
        AddProperty(new Vec3ActorProperty("Fog Color", "Fog Color",
            MakeFunctor(*this, &EnvironmentActorProxy::SetFogColor),
            MakeFunctorRet(*this, &EnvironmentActorProxy::GetFogColor),
            "Sets the color of the fog associated with the environment.", GROUPNAME));

        // This property manipulates the fog mode of an Environment.
        // Uses a dtCore::Environment::FogMode enumeration to represent
        // the fog mode.
        // Default is EXP2.
        AddProperty(new IntActorProperty("Fog Mode", "Fog Mode",
            MakeFunctor(*this, &EnvironmentActorProxy::SetFogMode),
            MakeFunctorRet(*this, &EnvironmentActorProxy::GetFogMode),
            "Set the mode of the fog associated with the environment.", GROUPNAME));

        // This property manipulates the latitude and longitude of an
        // Environment. Uses 2 values to represent the latitude and
        // longitude, with the format being osg::Vec2(latitude, longitude)
        // Default is 36.586944f, -121.842778f.
        AddProperty(new Vec2ActorProperty("Lat/Long", "Lat/Long",
            MakeFunctor(*this, &EnvironmentActorProxy::SetRefLatLong),
            MakeFunctorRet(*this, &EnvironmentActorProxy::GetRefLatLong),
            "Sets the latitude and longitude of an enviroment", GROUPNAME));

        // This property toggles the enabling of fog in an Environment.
        // Uses a bool type to represent the state of fog. true = enabled,
        // false = disabled.
        // Default is true.
        AddProperty(new BooleanActorProperty("Enable Fog", "Enable Fog",
            MakeFunctor(*env, &Environment::SetFogEnable),
            MakeFunctorRet(*env, &Environment::GetFogEnable),
            "Toggles fog on and off", GROUPNAME));

        // This property manipulates the visibility of an Environment.
        // Uses a float type to represent the visibility. Represents
        // distance in meters.
        // Default is 0.0f
        AddProperty(new FloatActorProperty("Visibility", "Visibility",
            MakeFunctor(*env, &Environment::SetVisibility),
            MakeFunctorRet(*env, &Environment::GetVisibility),
            "Sets how close the camera fog is rendered.", GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::AddEffect(osg::ref_ptr<EnvEffectActorProxy> effect)
    {
        mEffects.push_back(effect);

        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment.");

        //env->AddEffect(effect.get()->GetActor());
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::RemoveEffect(osg::ref_ptr<EnvEffectActorProxy> effect)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment.");

        for(std::vector< osg::ref_ptr<EnvEffectActorProxy> >::iterator it = mEffects.begin();
            it != mEffects.end();
            it++)
        {
            if((*it) == effect)
            {
                mEffects.erase(it);
                return;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    std::vector< osg::ref_ptr<EnvEffectActorProxy> >& EnvironmentActorProxy::GetEffects()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment.");

        return mEffects;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetSkyColor(const osg::Vec3 &color)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        sgVec3 col;
        col[0] = color[0]; col[1] = color[1]; col[2] = color[2];
        env->SetSkyColor(col);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 EnvironmentActorProxy::GetSkyColor()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        sgVec3 color;
        env->GetSkyColor(color);
        return osg::Vec3(color[0], color[1], color[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetFogColor(const osg::Vec3 &color)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        sgVec3 col;
        col[0] = color[0]; col[1] = color[1]; col[2] = color[2];
        env->SetFogColor(col);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 EnvironmentActorProxy::GetFogColor()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        sgVec3 color;
        env->GetFogColor(color);
        return osg::Vec3(color[0], color[1], color[2]);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetFogMode(int mode)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        env->SetFogMode((dtCore::Environment::FogMode)mode);
    }

    ///////////////////////////////////////////////////////////////////////////////
    int EnvironmentActorProxy::GetFogMode()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        return env->GetFogMode();
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetRefLatLong(const osg::Vec2 &latlong)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        sgVec2 l;
        l[0] = latlong[0]; l[1] = latlong[1]; l[2] = latlong[2];
        env->SetRefLatLong(l);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec2 EnvironmentActorProxy::GetRefLatLong()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        sgVec2 l;
        env->GetRefLatLong(l);
        return osg::Vec2(l[0], l[1]);
    }
}
