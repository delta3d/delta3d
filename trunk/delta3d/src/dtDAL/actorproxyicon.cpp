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
 * @author Matthew W. Campbell
*/

#include "dtDAL/actorproxyicon.h"
#include "dtCore/scene.h"
#include <dtCore/deltadrawable.h>
#include <osg/Geometry>
#include <osg/Billboard>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/StateSet>
#include <osg/PolygonMode>
#include <osg/CullFace>
#include <osgDB/ReadFile>

namespace dtDAL
{

    //////////////////////////////////////////////////////////////////////////
    IMPLEMENT_ENUM(ActorProxyIcon::IconType);
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::CHARACTER("CHARACTER_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::GENERIC("GENERIC_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::STATICMESH("STATICMESH_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::SOUND("SOUND_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::LIGHT("LIGHT_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::PARTICLESYSTEM("PARTICLESYSTEM_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::MESHTERRAIN("MESHTERRAIN_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::PLAYERSTART("PLAYERSTART_ICON");
    const ActorProxyIcon::IconType ActorProxyIcon::IconType::TRIGGER("TRIGGER_ICON");
    //////////////////////////////////////////////////////////////////////////

    //absolute paths
    //const std::string deltaRoot = "DELTA_ROOT";
    //char* ptr = getenv( deltaRoot.c_str() );
    //const std::string ActorProxyIcon::mPrefix(std::string(ptr) + "/utilities/editor/billboards/");
   
    //relative paths
    const std::string ActorProxyIcon::mPrefix("../utilities/editor/billboards/");
   
    //////////////////////////////////////////////////////////////////////////
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_GENERIC("");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_CHARACTER(mPrefix+"animcharacter.png");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH(mPrefix+"staticmesh.png");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_LIGHT(mPrefix+"light.png");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_SOUND(mPrefix+"sound.png");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_PARTICLESYSTEM(mPrefix+"particlesystem.png");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_MESHTERRAIN(mPrefix+"terrain.png");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_PLAYERSTART(mPrefix+"playerstart.png");
    const std::string ActorProxyIcon::IMAGE_BILLBOARD_TRIGGER(mPrefix+"trigger.png");
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    ActorProxyIcon::ActorProxyIcon(const IconType &type)
    {
        mIconType = &type;
        mIconNode = NULL;
        CreateBillBoard();
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProxyIcon &ActorProxyIcon::operator=(const ActorProxyIcon &rhs)
    {
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProxyIcon::ActorProxyIcon(const ActorProxyIcon &rhs)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    ActorProxyIcon::~ActorProxyIcon()
    {

    }

    //////////////////////////////////////////////////////////////////////////
    void ActorProxyIcon::CreateBillBoard()
    {
        osg::Image *image = GetBillBoardImage();

        //Create the texture object and quad geometry for our billboard.
        osg::StateSet *ss = new osg::StateSet();
        osg::PolygonMode *pm = new osg::PolygonMode();
        osg::Texture2D *texture = new osg::Texture2D();
        osg::CullFace *cf = new osg::CullFace();
        osg::Geometry *geom = CreateGeom(osg::Vec3(-1.0f,0.0f,-1.0f),
            osg::Vec3(2.0f,0.0f,0.0f),osg::Vec3(0.0f,0.0f,2.0f));

        pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
        texture->setImage(image);
        texture->setUnRefImageDataAfterApply(true);

        ss->setTextureAttributeAndModes(0,texture,
            osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
        ss->setMode(GL_LIGHTING,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
        ss->setAttributeAndModes(pm,osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
        ss->setAttributeAndModes(cf,osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED);
        geom->setStateSet(ss);

        //Create the Delta3D object and attach a billboard object to it.
        osg::Billboard *billBoard = new osg::Billboard();
        billBoard->addDrawable(geom);

        if(!mIconNode.valid())
           mIconNode = new dtCore::Transformable();

        mIconNode->GetMatrixNode()->removeChild(0,mIconNode->GetMatrixNode()->getNumChildren());
        mIconNode->GetMatrixNode()->addChild(billBoard);
    }

    //////////////////////////////////////////////////////////////////////////
    osg::Image *ActorProxyIcon::GetBillBoardImage()
    {
        osg::Image *image = NULL;

        //Based on the icon type, load the correct image.
        if (mIconType == &IconType::GENERIC)
            return NULL;
        else if (mIconType == &IconType::CHARACTER)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_CHARACTER);
        else if (mIconType == &IconType::STATICMESH)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH);
        else if (mIconType == &IconType::SOUND)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_SOUND);
        else if (mIconType == &IconType::LIGHT)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_LIGHT);
        else if (mIconType == &IconType::PARTICLESYSTEM)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_PARTICLESYSTEM);
        else if (mIconType == &IconType::MESHTERRAIN)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_MESHTERRAIN);
        else if (mIconType == &IconType::PLAYERSTART)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_PLAYERSTART);
        else if (mIconType == &IconType::TRIGGER)
            image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_TRIGGER);

        return image;
    }


    //////////////////////////////////////////////////////////////////////////
    osg::Geometry *ActorProxyIcon::CreateGeom(const osg::Vec3 &corner,
                                              const osg::Vec3 &width,
                                              const osg::Vec3 &height)
    {
        osg::Geometry *geom = new osg::Geometry;

        osg::Vec3Array *coords = new osg::Vec3Array(4);
        (*coords)[0]=corner;
        (*coords)[1]=corner+width;
        (*coords)[2]=corner+width+height;
        (*coords)[3]=corner+height;
        geom->setVertexArray(coords);

        osg::Vec3Array* norms = new osg::Vec3Array(1);
        (*norms)[0] = width^height;
        (*norms)[0].normalize();
        geom->setNormalArray(norms);
        geom->setNormalBinding(osg::Geometry::BIND_OVERALL);

        osg::Vec2Array* tcoords = new osg::Vec2Array(4);
        (*tcoords)[0].set(0.0f,0.0f);
        (*tcoords)[1].set(1.0f,0.0f);
        (*tcoords)[2].set(1.0f,1.0f);
        (*tcoords)[3].set(0.0f,1.0f);
        geom->setTexCoordArray(0,tcoords);

        geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
        return geom;
    }

}
