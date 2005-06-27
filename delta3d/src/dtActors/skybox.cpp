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
#include "dtActors/skybox.h"

#include <osg/Depth>
#include <osg/Image>
#include <osg/Geode>
#include <osg/Vec3>
#include <osg/Geometry>
#include <osg/ClearNode>
#include <osgDB/ReadFile>
#include <osgUtil/CullVisitor>
#include "dtDAL/log.h"

#include <iostream>

namespace dtActors 
{
    bool SkyBox::SkyBoxEyePointTransform::computeLocalToWorldMatrix(
        osg::Matrix& matrix,osg::NodeVisitor* nv) const
    {
        osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv) 
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    bool SkyBox::SkyBoxEyePointTransform::computeWorldToLocalMatrix(
        osg::Matrix& matrix,osg::NodeVisitor* nv) const
    {
        osgUtil::CullVisitor *cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv) 
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    SkyBox::SkyBox(const std::string &name)
    {
        SetName(name);

        SkyBoxEyePointTransform *transForm = new SkyBoxEyePointTransform;
        osg::Geode *geode = CreateSkyBoxGeom();

        transForm->setCullingActive(false);
        transForm->addChild(geode);

        //Drawing a skybox eliminates the need for clearing the color and depth buffers.
        osg::ClearNode* clearNode = new osg::ClearNode;
        clearNode->setRequiresClear(false); //Sky eliminates need for clearing
        clearNode->addChild(transForm);
        mNode = clearNode;
    }

    ///////////////////////////////////////////////////////////////////////////////
    void SkyBox::SetTexture(SkyBoxSide side, const std::string &fileName)
    {
        osg::Image *newImage = osgDB::readImageFile(fileName);
        mTextureList[side]->setImage(newImage);
        mTextureList[side]->dirtyTextureObject();
    }

    ///////////////////////////////////////////////////////////////////////////////
    osg::Geode *SkyBox::CreateSkyBoxGeom()
    {
        osg::Geode *geode = new osg::Geode();
        float x = 1.0f;
        float y = 1.0f;
        float z = 1.0f;

        osg::Vec3 coords0[] = //front
        {
            osg::Vec3(-x,y,-z),
            osg::Vec3(x,y,-z),
            osg::Vec3(x,y,z),
            osg::Vec3(-x,y,z),
        };

        osg::Vec3 coords1[] = //right
        {
            osg::Vec3(x,y,-z),
            osg::Vec3(x,-y,-z),
            osg::Vec3(x,-y,z),
            osg::Vec3(x,y,z)
        };

        osg::Vec3 coords2[] = //back
        {
            osg::Vec3(x,-y,-z),
            osg::Vec3(-x,-y,-z),
            osg::Vec3(-x,-y,z),
            osg::Vec3(x,-y,z)
        };

        osg::Vec3 coords3[] = //left
        {
            osg::Vec3(-x,-y,-z),
            osg::Vec3(-x,y,-z),
            osg::Vec3(-x,y,z),
            osg::Vec3(-x,-y,z)
        };

        osg::Vec3 coords4[] = //top
        {
            osg::Vec3(-x,y,z),
            osg::Vec3(x,y,z),
            osg::Vec3(x,-y,z),
            osg::Vec3(-x,-y,z)
        };

        osg::Vec3 coords5[] = //bottom
        {
            osg::Vec3(-x,y,-z),
            osg::Vec3(-x,-y,-z),
            osg::Vec3(x,-y,-z),
            osg::Vec3(x,y,-z)
        };

        osg::Vec2 tCoords[] =
        {
            osg::Vec2(0,0),
            osg::Vec2(1,0),
            osg::Vec2(1,1),
            osg::Vec2(0,1)
        };

        osg::Vec3Array *vArray[6];
        vArray[0] = new osg::Vec3Array(4, coords0);
        vArray[1] = new osg::Vec3Array(4, coords1);
        vArray[2] = new osg::Vec3Array(4, coords2);
        vArray[3] = new osg::Vec3Array(4, coords3);
        vArray[4] = new osg::Vec3Array(4, coords4);
        vArray[5] = new osg::Vec3Array(4, coords5);

        osg::Geometry *polyGeom[6];
        for (int side=0; side<6; side++) 
        {
            polyGeom[side] = new osg::Geometry();

            polyGeom[side]->setVertexArray(vArray[side]);
            polyGeom[side]->setTexCoordArray(0, new osg::Vec2Array(4, tCoords));
            polyGeom[side]->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

            osg::StateSet *dstate = new osg::StateSet;
            dstate->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
            dstate->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

            // clear the depth to the far plane.
            osg::Depth* depth = new osg::Depth;
            depth->setFunction(osg::Depth::ALWAYS);
            depth->setRange(1.0,1.0);
            dstate->setAttributeAndModes(depth,osg::StateAttribute::ON);
            dstate->setMode(GL_FOG, osg::StateAttribute::OFF);
            dstate->setMode(GL_LIGHTING,osg::StateAttribute::PROTECTED |
                osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
            dstate->setRenderBinDetails(-2,"RenderBin");

            mTextureList[side] = new osg::Texture2D();
            mTextureList[side]->setUnRefImageDataAfterApply(true);
            mTextureList[side]->setWrap(osg::Texture::WRAP_S,osg::Texture::CLAMP_TO_EDGE);
            mTextureList[side]->setWrap(osg::Texture::WRAP_T,osg::Texture::CLAMP_TO_EDGE);
            mTextureList[side]->setFilter(osg::Texture::MIN_FILTER,
                osg::Texture::LINEAR_MIPMAP_LINEAR);
            mTextureList[side]->setFilter(osg::Texture::MAG_FILTER,osg::Texture::LINEAR);
            dstate->setTextureAttributeAndModes(0,mTextureList[side].get(),
                osg::StateAttribute::ON);

            polyGeom[side]->setStateSet(dstate);
            geode->addDrawable(polyGeom[side]);
        }

        return geode;
    }
}
