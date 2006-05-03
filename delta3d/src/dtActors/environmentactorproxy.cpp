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

#include <dtActors/environmentactorproxy.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtCore/enveffect.h>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors 
{
    ///////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(EnvironmentActorProxy::FogModeEnum);
    EnvironmentActorProxy::FogModeEnum
        EnvironmentActorProxy::FogModeEnum::LINEAR("LINEAR");
    EnvironmentActorProxy::FogModeEnum
        EnvironmentActorProxy::FogModeEnum::EXP("EXP");
    EnvironmentActorProxy::FogModeEnum
        EnvironmentActorProxy::FogModeEnum::EXP2("EXP2");
    EnvironmentActorProxy::FogModeEnum
        EnvironmentActorProxy::FogModeEnum::ADV("ADV");
    ///////////////////////////////////////////////////////////////////////////////
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
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment.");

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
        AddProperty(new EnumActorProperty<EnvironmentActorProxy::FogModeEnum>("Fog Mode", "Fog Mode",
            MakeFunctor(*this, &EnvironmentActorProxy::SetFogMode),
            MakeFunctorRet(*this, &EnvironmentActorProxy::GetFogMode),
            "Sets the mode of the fog associated with the environment.", GROUPNAME));

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
    void EnvironmentActorProxy::AddEffect(dtCore::RefPtr<EnvEffectActorProxy> effect)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment.");

        env->AddEffect(static_cast<EnvEffect*>(effect->GetActor()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::RemoveEffect(dtCore::RefPtr<EnvEffectActorProxy> effect)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment.");

        env->RemEffect(static_cast<EnvEffect*>(effect->GetActor()));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetSkyColor(const osg::Vec3 &color)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        env->SetSkyColor(color);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 EnvironmentActorProxy::GetSkyColor()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        osg::Vec3 color;
        env->GetSkyColor(color);
        return color;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetFogColor(const osg::Vec3 &color)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        env->SetFogColor(color);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 EnvironmentActorProxy::GetFogColor()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        osg::Vec3 color;
        env->GetFogColor(color);
        return color;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetFogMode(EnvironmentActorProxy::FogModeEnum &mode)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(env == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        if(mode == FogModeEnum::LINEAR)
            env->SetFogMode(Environment::LINEAR);
        else if(mode == FogModeEnum::EXP)
            env->SetFogMode(Environment::EXP);
        else if(mode == FogModeEnum::EXP2)
            env->SetFogMode(Environment::EXP2);
        else
            env->SetFogMode(Environment::ADV);
    }

    ///////////////////////////////////////////////////////////////////////////////
    EnvironmentActorProxy::FogModeEnum& EnvironmentActorProxy::GetFogMode()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(env == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        if(env->GetFogMode() == Environment::LINEAR)
            return FogModeEnum::LINEAR;
        else if(env->GetFogMode() == Environment::EXP)
            return FogModeEnum::EXP;
        else if(env->GetFogMode() == Environment::EXP2)
            return FogModeEnum::EXP2;
        else
            return FogModeEnum::ADV;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void EnvironmentActorProxy::SetRefLatLong(const osg::Vec2 &latlong)
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(env == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        env->SetRefLatLong(latlong);
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec2 EnvironmentActorProxy::GetRefLatLong()
    {
        Environment *env = dynamic_cast<Environment*>(mActor.get());
        if(!env)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Environment");

        osg::Vec2 l;
        env->GetRefLatLong(l);
        return l;
    }
}
