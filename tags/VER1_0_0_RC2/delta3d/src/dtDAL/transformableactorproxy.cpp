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

#include "dtDAL/transformableactorproxy.h"
#include "dtDAL/enginepropertytypes.h"
#include <dtDAL/exceptionenum.h>
#include "dtDAL/actorproxyicon.h"
#include <dtCore/transformable.h>
#include <dtCore/scene.h>
#include <dtUtil/matrixutil.h>

namespace dtDAL
{
    void TransformableActorProxy::BuildPropertyMap()
    {
        const std::string GROUPNAME = "Transformable";

        dtCore::Transformable *trans = dynamic_cast<dtCore::Transformable*>(mActor.get());
        if(trans == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type "
                    "dtCore::Transformable\n");

        AddProperty(new Vec3ActorProperty("Rotation", "Rotation",
            MakeFunctor(*this, &TransformableActorProxy::SetRotation),
            MakeFunctorRet(*this, &TransformableActorProxy::GetRotation),
            "Sets the amount of rotation on a transformable. Represented with pitch, yaw, and roll.",
            GROUPNAME));

        AddProperty(new Vec3ActorProperty("Translation", "Translation",
            MakeFunctor(*this, &TransformableActorProxy::SetTranslation),
            MakeFunctorRet(*this, &TransformableActorProxy::GetTranslation),
            "Sets the location of a transformable in 3D space.",
            GROUPNAME));

        AddProperty(new Vec3ActorProperty("Scale", "Scale",
            MakeFunctor(*this, &TransformableActorProxy::SetScale),
            MakeFunctorRet(*this, &TransformableActorProxy::GetScale),
            "Sets the scale of a transformable.",GROUPNAME));

        AddProperty(new BooleanActorProperty("Normal Rescaling", "Normal Rescaling",
            MakeFunctor(*trans, &dtCore::Transformable::SetNormalRescaling),
            MakeFunctorRet(*trans, &dtCore::Transformable::GetNormalRescaling),
            "Enables the automatic scaling of normals when a Transformable is scaled",
            GROUPNAME));
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TransformableActorProxy::SetRotation(const osg::Vec3 &rotation)
    {
        dtCore::Transformable *t = dynamic_cast<dtCore::Transformable*>(mActor.get());
        if (t == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type "
                    "dtCore::Transformable\n");

        mHPR = rotation;

        //Normalize the rotation.
        if (mHPR.x() < 0.0f)
            mHPR.x() += 360.0f;
        if (mHPR.x() > 360.0f)
            mHPR.x() -= 360.0f;

        if (mHPR.y() < 0.0f)
            mHPR.y() += 360.0f;
        if (mHPR.y() > 360.0f)
            mHPR.y() -= 360.0f;

        if (mHPR.z() < 0.0f)
            mHPR.z() += 360.0f;
        if (mHPR.z() > 360.0f)
            mHPR.z() -= 360.0f;

        dtCore::Transform trans;
        t->GetTransform(&trans);
        trans.SetRotation(osg::Vec3(mHPR[2],mHPR[0],mHPR[1]));
        t->SetTransform(&trans);

        //If we have a billboard update its rotation as well.
        if (GetRenderMode() == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
            GetRenderMode() == ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
        {
            ActorProxyIcon *billBoard = GetBillBoardIcon();
            if (billBoard != NULL)
                billBoard->SetActorRotation(osg::Vec3(mHPR[2],mHPR[0],mHPR[1]));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TransformableActorProxy::SetRotationFromMatrix(const osg::Matrix &rotation)
    {
        osg::Vec3 hpr;
        dtUtil::MatrixUtil::MatrixToHpr(hpr,rotation);        
        SetRotation(osg::Vec3(hpr[1],hpr[2],hpr[0]));
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 TransformableActorProxy::GetRotation()
    {
        dtCore::Transformable *t = dynamic_cast<dtCore::Transformable*>(mActor.get());
        if (t == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type "
                    "dtCore::Transformable\n");

        return mHPR;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TransformableActorProxy::SetTranslation(const osg::Vec3 &translation)
    {
        dtCore::Transformable *t = dynamic_cast<dtCore::Transformable*>(mActor.get());
        if (t == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type "
                    "dtCore::Transformable\n");

        dtCore::Transform trans;
        t->GetTransform(&trans);
        trans.SetTranslation(translation[0], translation[1], translation[2]);
        t->SetTransform(&trans);

        //If we have a billboard update its position as well.
        if (GetRenderMode() == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
            GetRenderMode() == ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
        {
            ActorProxyIcon *billBoard = GetBillBoardIcon();
            if (billBoard != NULL)
                billBoard->SetPosition(translation);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 TransformableActorProxy::GetTranslation()
    {
        dtCore::Transformable *t = dynamic_cast<dtCore::Transformable*>(mActor.get());
        if (t == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type "
                    " dtCore::Transformable\n");

        dtCore::Transform trans;
        t->GetTransform(&trans);
        float x, y, z;
        trans.GetTranslation(x, y, z);
        return osg::Vec3(x, y, z);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void TransformableActorProxy::SetScale(const osg::Vec3 &scale)
    {
        dtCore::Transformable *t = dynamic_cast<dtCore::Transformable*>(mActor.get());
        if (t == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type "
                    "dtCore::Transformable\n");

        dtCore::Transform trans;
        t->GetTransform(&trans);
        trans.SetScale(scale[0], scale[1], scale[2]);
        t->SetTransform(&trans);

        //If we have a billboard update its scale as well.
        if (GetRenderMode() == ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON ||
            GetRenderMode() == ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
        {
            ActorProxyIcon *billBoard = GetBillBoardIcon();
            if (billBoard != NULL)
                billBoard->SetScale(scale);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Vec3 TransformableActorProxy::GetScale()
    {
        dtCore::Transformable *t = dynamic_cast<dtCore::Transformable*>(mActor.get());
        if (t == NULL)
            EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Transformable\n");

        dtCore::Transform trans;
        t->GetTransform(&trans);
        float x, y, z;
        trans.GetScale(x, y, z);
        return osg::Vec3(x, y, z);
    }
}
