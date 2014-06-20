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
#include <prefix/dtcoreprefix.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/transform.h>
#include <dtUtil/log.h>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/StateSet>
#include <osg/PolygonMode>
#include <osg/ShapeDrawable>
#include <osg/TexMat>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>

namespace dtCore
{

   /////////////////////////////////////////////////////////////////////////////
   std::string ActorProxyIcon::IMAGE_BILLBOARD_GENERIC("billboards/question.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_CHARACTER("billboards/animcharacter.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH("billboards/staticmesh.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_LIGHT("billboards/light.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_SOUND("billboards/sound.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_PARTICLESYSTEM("billboards/particlesystem.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_MESHTERRAIN("billboards/terrain.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_PLAYERSTART("billboards/playerstart.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_TRIGGER("billboards/trigger.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_CAMERA("billboards/camera.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_WAYPOINT("billboards/Foot.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_DIRECTOR("billboards/director.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_POSITION("billboards/staticmesh.png");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_PATHNODE("billboards/beziernode.png");

   std::string ActorProxyIcon::IMAGE_ARROW_HEAD("billboards/arrowhead.png");
   std::string ActorProxyIcon::IMAGE_ARROW_BODY("billboards/arrowbody.png");
   /////////////////////////////////////////////////////////////////////////////



   /////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon::ActorProxyIcon(const std::string& iconImageFilename)
      : mIconImageFile(iconImageFilename)
      , mIconNode(NULL)
      , mIconStateSet(NULL)
      , mConeStateSet(NULL)
      , mCylinderStateSet(NULL)
   {
      CreateBillBoard();
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon::ActorProxyIcon(const std::string& iconImageFilename,
                                  const ActorProxyIconConfig& pConfig)
      : mIconImageFile(iconImageFilename)
      , mConfig(pConfig)
      , mIconNode(NULL)
      , mIconStateSet(0)
      , mConeStateSet(0)
      , mCylinderStateSet(0)
   {
      CreateBillBoard();
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon& ActorProxyIcon::operator=(const ActorProxyIcon&)
   {
      return *this;
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon::ActorProxyIcon(const ActorProxyIcon&)
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   ActorProxyIcon::~ActorProxyIcon()
   {

   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::CreateBillBoard()
   {
      if (!mBillBoard.valid())
      {
         osg::StateAttribute::GLModeValue turnOn = osg::StateAttribute::ON |
            osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE;

         osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::OFF |
            osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE;

         //Create the quad geometry for our billboard.
         mIconStateSet = new osg::StateSet();
         osg::PolygonMode* pm = new osg::PolygonMode();
         osg::Geometry* geom = CreateGeom(osg::Vec3(-1.0f, 0.0f, -1.0f),
            osg::Vec3(2.0f, 0.0f, 0.0f),osg::Vec3(0.0f, 0.0f, 2.0f));

         pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);

         mIconStateSet->setMode(GL_LIGHTING,turnOff);
         mIconStateSet->setAttributeAndModes(pm, turnOn);
         geom->setStateSet(mIconStateSet.get());

         if (mConfig.mForwardVector)
         {
            osg::Group* arrow = CreateOrientationArrow();
            mArrowNode = new dtCore::Transformable();
            mArrowNode->GetMatrixNode()->addChild(arrow);
         }

         if (mConfig.mUpVector)
         {
            osg::Group* arrowUp = CreateOrientationArrow();
            mArrowNodeUp = new dtCore::Transformable();
            mArrowNodeUp->GetMatrixNode()->addChild(arrowUp);
         }

         mIconNode = new dtCore::Transformable();
         osg::MatrixTransform* scaleMatrix = new osg::MatrixTransform();
         scaleMatrix->setMatrix(osg::Matrix::scale(osg::Vec3(mConfig.mScale, mConfig.mScale, mConfig.mScale)));

         osg::Geode* geode = new osg::Geode();
         geode->addDrawable(geom);

         dtCore::Transform transform;

         transform.SetRotation(0.0f, -90.0f, 0.0f);
         mTopTransform = new dtCore::Transformable();
         mTopTransform->GetOSGNode()->asGroup()->addChild(geode);
         mTopTransform->GetOSGNode()->setNodeMask(0x00001000);
         mTopTransform->SetTransform(transform);
         scaleMatrix->addChild(mTopTransform->GetOSGNode());

         transform.SetRotation(90.0f, 0.0f, 0.0f);
         mSideTransform = new dtCore::Transformable();
         mSideTransform->GetOSGNode()->asGroup()->addChild(geode);
         mSideTransform->GetOSGNode()->setNodeMask(0x00002000);
         mSideTransform->SetTransform(transform);
         scaleMatrix->addChild(mSideTransform->GetOSGNode());

         transform.SetRotation(0.0f, 0.0f, 0.0f);
         mFrontTransform = new dtCore::Transformable();
         mFrontTransform->GetOSGNode()->asGroup()->addChild(geode);
         mFrontTransform->GetOSGNode()->setNodeMask(0x00004000);
         mFrontTransform->SetTransform(transform);
         scaleMatrix->addChild(mFrontTransform->GetOSGNode());

         mPerspTransform = new dtCore::Transformable();
         mPerspTransform->GetOSGNode()->asGroup()->addChild(geode);
         mPerspTransform->GetOSGNode()->setNodeMask(0x00008000);
         scaleMatrix->addChild(mPerspTransform->GetOSGNode());

         mIconNode->GetMatrixNode()->addChild(scaleMatrix);

         mBillBoard = new BillBoardDrawable();
         mBillBoard->AddChild(mIconNode.get());

         if (mConfig.mForwardVector)
         {
            mBillBoard->AddChild(mArrowNode.get());
         }

         if (mConfig.mUpVector)
         {
            mBillBoard->AddChild(mArrowNodeUp.get());
         }

         SetActorRotation(osg::Vec3(0.0f, 0.0f, 0.0f));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::LoadImages()
   {
      osg::StateAttribute::GLModeValue turnOn = osg::StateAttribute::ON |
         osg::StateAttribute::PROTECTED | osg::StateAttribute::OVERRIDE;
      osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::OVERRIDE |
         osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF;

      osg::Image* image = GetBillBoardImage();

      // Create the texture object for our billboard
      osg::Texture2D* texture = new osg::Texture2D();
      texture->setImage(image);
      texture->setUnRefImageDataAfterApply(true);
      mIconStateSet->setTextureAttributeAndModes(0, texture, turnOn);

      // Orientation Arrow
      osg::TexMat* texMat = new osg::TexMat();
      osg::Texture2D* tex;

      osg::PolygonMode* pm = new osg::PolygonMode();
      pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);

      if (mConfig.mForwardVector)
      {
         image = osgDB::readImageFile(ActorProxyIcon::IMAGE_ARROW_HEAD);
         tex = new osg::Texture2D(image);
         tex->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP);
         tex->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP);
         tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_NEAREST);
         tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
         mConeStateSet->setTextureAttributeAndModes(0, tex, turnOn);
         mConeStateSet->setAttributeAndModes(pm, turnOn);
         mConeStateSet->setMode(GL_LIGHTING, turnOff);
      }

      if (mConfig.mUpVector)
      {
         image = osgDB::readImageFile(ActorProxyIcon::IMAGE_ARROW_BODY);
         tex = new osg::Texture2D(image);
         texMat->setMatrix(osg::Matrix::scale(5.0f, 7.0f, 0.0f));
         tex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
         tex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP);
         tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_NEAREST);
         tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
         mCylinderStateSet->setTextureAttributeAndModes(0, tex, turnOn);
         mCylinderStateSet->setAttributeAndModes(pm, turnOn);
         mCylinderStateSet->setMode(GL_LIGHTING, turnOff);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* ActorProxyIcon::GetDrawable()
   {
      return mBillBoard.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* ActorProxyIcon::GetDrawable() const
   {
      return mBillBoard.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtCore::Transformable* ActorProxyIcon::GetPerspectiveTransform()
   {
      return mPerspTransform;
   }

   /////////////////////////////////////////////////////////////////////////////
   const dtCore::Transformable* ActorProxyIcon::GetPerspectiveTransform() const
   {
      return mPerspTransform;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorProxyIcon::OwnsDrawable(dtCore::DeltaDrawable* drawable) const
   {
      return  mBillBoard->GetUniqueId() == drawable->GetUniqueId() ||
         mIconNode->GetUniqueId() == drawable->GetUniqueId();// || //is this really necessary
         //mArrowNode->GetUniqueId() == drawable->GetUniqueId() || mArrowNodeUp->GetUniqueId() == drawable->GetUniqueId();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetImage(const std::string& iconImageFilename)
   {
      mIconImageFile = iconImageFilename;
      osg::Image* image = GetBillBoardImage();

      if (image)
      {
         // Create the texture object for our billboard
         osg::Texture2D* texture = new osg::Texture2D();
         texture->setImage(image);
         texture->setUnRefImageDataAfterApply(true);

         mIconStateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
      }
      else
      {
         LOG_ERROR("Unable to load file: " + iconImageFilename);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetPosition(const osg::Vec3& newPos)
   {
      dtCore::Transform trans;

      mIconNode->GetTransform(trans);
      trans.SetTranslation(newPos);
      mIconNode->SetTransform(trans);

      if (mConfig.mForwardVector)
      {
         mArrowNode->GetTransform(trans);
         trans.SetTranslation(newPos);
         mArrowNode->SetTransform(trans);
      }

      if (mConfig.mUpVector)
      {
         mArrowNodeUp->GetTransform(trans);
         trans.SetTranslation(newPos);
         mArrowNodeUp->SetTransform(trans);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetRotation(const osg::Matrix& mat)
   {
      dtCore::Transform tx;
      mPerspTransform->GetTransform(tx);
      tx.SetRotation(mat);
      mPerspTransform->SetTransform(tx);
   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetActorRotation(const osg::Vec3& hpr)
   {
      dtCore::Transform tx;
      if (mConfig.mForwardVector)
      {
         mArrowNode->GetTransform(tx);
         tx.SetRotation(hpr);
         mArrowNode->SetTransform(tx);
      }

      if (mConfig.mUpVector)
      {
         tx.SetRotation(osg::Vec3(hpr[0], hpr[1] + 90.0f, hpr[2]));
         mArrowNodeUp->SetTransform(tx);
      }

   }

   /////////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetActorRotation(const osg::Matrix& mat)
   {
      dtCore::Transform tx;

      if (mConfig.mForwardVector)
      {
         mArrowNode->GetTransform(tx);
         tx.SetRotation(mat);
         mArrowNode->SetTransform(tx);
      }

      if (mConfig.mUpVector)
      {
         osg::Vec3 hpr;
         tx.GetRotation(hpr);
         hpr[1] += 90.0f;
         tx.SetRotation(hpr);
         mArrowNodeUp->SetTransform(tx);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Matrix ActorProxyIcon::GetActorRotation()
   {
      osg::Matrix mat;
      dtCore::Transform tx;

      if (mConfig.mForwardVector)
      {
         mArrowNode->GetTransform(tx);
         tx.Get(mat);
      }
      return mat;
   }


   //////////////////////////////////////////////////////////////////////////
   osg::Image* ActorProxyIcon::GetBillBoardImage()
   {
      if (mIconImageFile.empty())
      {
         return NULL;
      }

      return osgDB::readImageFile(mIconImageFile);
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Group* ActorProxyIcon::CreateOrientationArrow()
   {
      if (!mConeStateSet.valid())
      {
         mConeStateSet = new osg::StateSet();
      }

      if (!mCylinderStateSet.valid())
      {
         mCylinderStateSet = new osg::StateSet();
      }

      osg::TessellationHints* tessHints = new osg::TessellationHints();
      tessHints->setDetailRatio(0.1f);
      tessHints->setCreateNormals(false);
      tessHints->setCreateTextureCoords(true);

      osg::Geode* cylinderGeode = new osg::Geode();
      osg::MatrixTransform* cylinderTx = new osg::MatrixTransform();
      osg::ShapeDrawable* cylinder = new osg::ShapeDrawable(new osg::Cylinder(),tessHints);

      cylinderGeode->addDrawable(cylinder);
      cylinderGeode->setStateSet(mCylinderStateSet.get());
      cylinderTx->setMatrix(osg::Matrix::scale(osg::Vec3(0.1f,0.1f,2)) *
                            osg::Matrix::rotate(osg::DegreesToRadians(90.0f),osg::Vec3(1,0,0)) *
                            osg::Matrix::translate(osg::Vec3(0,1.1f,0)));
      cylinderTx->addChild(cylinderGeode);

      osg::Geode* coneGeode = new osg::Geode();
      osg::MatrixTransform* coneTx = new osg::MatrixTransform();
      osg::ShapeDrawable* cone = new osg::ShapeDrawable(new osg::Cone(),tessHints);

      coneGeode->addDrawable(cone);
      coneGeode->setStateSet(mConeStateSet.get());
      coneTx->setMatrix(osg::Matrix::scale(osg::Vec3(0.35f,0.35f,0.85f)) *
                        osg::Matrix::rotate(osg::DegreesToRadians(-90.0f),osg::Vec3(1,0,0)) *
                        osg::Matrix::translate(osg::Vec3(0,2.1f,0)));
      coneTx->addChild(coneGeode);

      osg::Group* group = new osg::Group();
      group->addChild(cylinderTx);
      group->addChild(coneTx);
      return group;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Geometry *ActorProxyIcon::CreateGeom(const osg::Vec3& corner,
                                             const osg::Vec3& width,
                                             const osg::Vec3& height)
   {
      osg::Geometry* geom = new osg::Geometry;

      osg::Vec3Array* coords = new osg::Vec3Array(4);
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
