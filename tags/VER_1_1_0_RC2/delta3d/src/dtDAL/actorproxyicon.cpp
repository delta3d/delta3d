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
#include <dtCore/transformable.h>
#include <osg/Geometry>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/StateSet>
#include <osg/PolygonMode>
#include <osg/CullFace>
#include <osg/ShapeDrawable>
#include <osg/TexMat>
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
   const ActorProxyIcon::IconType ActorProxyIcon::IconType::CAMERA("CAMERA");
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   std::string ActorProxyIcon::IMAGE_BILLBOARD_GENERIC("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_CHARACTER("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_LIGHT("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_SOUND("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_PARTICLESYSTEM("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_MESHTERRAIN("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_PLAYERSTART("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_TRIGGER("");
   std::string ActorProxyIcon::IMAGE_BILLBOARD_CAMERA("");

   std::string ActorProxyIcon::IMAGE_ARROW_HEAD("");
   std::string ActorProxyIcon::IMAGE_ARROW_BODY("");
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::staticInitialize()
   {
      static char* ptr = getenv("DELTA_ROOT");
      static std::string prefix;

      prefix = std::string(ptr);                
      if (prefix[prefix.length()-1] == '/' || prefix[prefix.length()-1] == '\\') {
         prefix = prefix.substr(0,prefix.length()-1);
      }

      prefix += "/utilities/editor/billboards/";
      IMAGE_BILLBOARD_GENERIC = ("");
      IMAGE_BILLBOARD_CHARACTER = (prefix+"animcharacter.png");
      IMAGE_BILLBOARD_STATICMESH = (prefix+"staticmesh.png");
      IMAGE_BILLBOARD_LIGHT = (prefix+"light.png");
      IMAGE_BILLBOARD_SOUND = (prefix+"sound.png");
      IMAGE_BILLBOARD_PARTICLESYSTEM = (prefix+"particlesystem.png");
      IMAGE_BILLBOARD_MESHTERRAIN = (prefix+"terrain.png");
      IMAGE_BILLBOARD_PLAYERSTART = (prefix+"playerstart.png");
      IMAGE_BILLBOARD_TRIGGER = (prefix+"trigger.png");
      IMAGE_BILLBOARD_CAMERA = (prefix+"camera.png");

      IMAGE_ARROW_HEAD = (prefix+"arrowhead.png");
      IMAGE_ARROW_BODY = (prefix+"arrowbody.png");
   }

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
      if (mBillBoard.valid())
         return;

      osg::StateAttribute::GLModeValue turnOn = osg::StateAttribute::ON |
         osg::StateAttribute::PROTECTED |
         osg::StateAttribute::OVERRIDE;
      osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::OFF |
         osg::StateAttribute::PROTECTED |
         osg::StateAttribute::OVERRIDE;
      osg::Image *image = GetBillBoardImage();

      //Create the texture object and quad geometry for our billboard.
      osg::StateSet *ss = new osg::StateSet();
      osg::PolygonMode *pm = new osg::PolygonMode();
      osg::Texture2D *texture = new osg::Texture2D();
      osg::Geometry *geom = CreateGeom(osg::Vec3(-1.0f,0.0f,-1.0f),
         osg::Vec3(2.0f,0.0f,0.0f),osg::Vec3(0.0f,0.0f,2.0f));

      pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
      texture->setImage(image);
      texture->setUnRefImageDataAfterApply(true);

      ss->setTextureAttributeAndModes(0,texture,turnOn);
      ss->setMode(GL_LIGHTING,turnOff);
      ss->setAttributeAndModes(pm,turnOn);
      geom->setStateSet(ss);

      osg::Group *arrow = CreateOrientationArrow();
      mArrowNode = new dtCore::Transformable();
      mArrowNode->GetMatrixNode()->addChild(arrow);

      osg::Group *arrowUp = CreateOrientationArrow();
      mArrowNodeUp = new dtCore::Transformable();
      mArrowNodeUp->GetMatrixNode()->addChild(arrowUp);


      mIconNode = new dtCore::Transformable();
      osg::Geode *billBoard = new osg::Geode();
      billBoard->addDrawable(geom);
      mIconNode->GetMatrixNode()->addChild(billBoard);

      mBillBoard = new BillBoardDrawable();
      mBillBoard->AddChild(mIconNode.get());
      mBillBoard->AddChild(mArrowNode.get());
      mBillBoard->AddChild(mArrowNodeUp.get());

      SetActorRotation(osg::Vec3(0.0f, 0.0f, 0.0f));
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::DeltaDrawable* ActorProxyIcon::GetDrawable()
   {
      return mBillBoard.get();
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::DeltaDrawable* ActorProxyIcon::GetDrawable() const
   {
      return mBillBoard.get();
   }

   //////////////////////////////////////////////////////////////////////////
   bool ActorProxyIcon::OwnsDrawable(dtCore::DeltaDrawable *drawable) const
   {
      return  mBillBoard->GetUniqueId() == drawable->GetUniqueId() ||
         mIconNode->GetUniqueId() == drawable->GetUniqueId() ||
         mArrowNode->GetUniqueId() == drawable->GetUniqueId() || mArrowNodeUp->GetUniqueId() == drawable->GetUniqueId();
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetPosition(const osg::Vec3 &newPos)
   {
      dtCore::Transform trans;

      mIconNode->GetTransform(&trans);
      trans.SetTranslation(newPos);
      mIconNode->SetTransform(&trans);

      mArrowNode->GetTransform(&trans);
      trans.SetTranslation(newPos);
      mArrowNode->SetTransform(&trans);

      mArrowNodeUp->GetTransform(&trans);
      trans.SetTranslation(newPos);
      mArrowNodeUp->SetTransform(&trans);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetRotation(const osg::Matrix &mat)
   {
      dtCore::Transform tx;
      mIconNode->GetTransform(&tx);
      tx.SetRotation(mat);
      mIconNode->SetTransform(&tx);
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetActorRotation(const osg::Vec3 &hpr)
   {
      dtCore::Transform tx;
      mArrowNode->GetTransform(&tx);
      tx.SetRotation(hpr);
      mArrowNode->SetTransform(&tx);
     
      tx.SetRotation(osg::Vec3(hpr[0], hpr[1] + 90.0f, hpr[2]));

      mArrowNodeUp->SetTransform(&tx);
      
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetActorRotation(const osg::Matrix &mat)
   {
      dtCore::Transform tx;
      mArrowNode->GetTransform(&tx);
      tx.SetRotation(mat);
      mArrowNode->SetTransform(&tx);
      osg::Vec3 hpr;
      tx.GetRotation(hpr);
      hpr[1] += 90.0f;
      tx.SetRotation(hpr);

      mArrowNodeUp->SetTransform(&tx);
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Matrix ActorProxyIcon::GetActorRotation()
   {
      osg::Matrix mat;
      dtCore::Transform tx;

      mArrowNode->GetTransform(&tx);
      tx.Get(mat);
      return mat;
   }

   //////////////////////////////////////////////////////////////////////////
   void ActorProxyIcon::SetScale(const osg::Vec3 &newScale)
   {
      dtCore::Transform tx;
      mIconNode->GetTransform(&tx);
      tx.SetScale(newScale);
      mIconNode->SetTransform(&tx);
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
      else if (mIconType == &IconType::CAMERA)
         image = osgDB::readImageFile(ActorProxyIcon::IMAGE_BILLBOARD_CAMERA);

      return image;
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Group *ActorProxyIcon::CreateOrientationArrow()
   {
      osg::StateSet *coneSS = new osg::StateSet();
      osg::StateSet *cylinderSS = new osg::StateSet();

      osg::StateAttribute::GLModeValue turnOn = osg::StateAttribute::OVERRIDE |
         osg::StateAttribute::PROTECTED |
         osg::StateAttribute::ON;
      osg::StateAttribute::GLModeValue turnOff = osg::StateAttribute::OVERRIDE |
         osg::StateAttribute::PROTECTED |
         osg::StateAttribute::OFF;

      osg::TexMat *texMat = new osg::TexMat();
      osg::Image *image;
      osg::Texture2D *tex;

      osg::PolygonMode *pm = new osg::PolygonMode();
      pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);

      image = osgDB::readImageFile(ActorProxyIcon::IMAGE_ARROW_HEAD);
      tex = new osg::Texture2D(image);
      tex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
      tex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP);
      tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_NEAREST);
      tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      coneSS->setTextureAttributeAndModes(0,tex,turnOn);
      coneSS->setAttributeAndModes(pm,turnOn);
      coneSS->setMode(GL_LIGHTING,turnOff);

      image = osgDB::readImageFile(ActorProxyIcon::IMAGE_ARROW_BODY);
      tex = new osg::Texture2D(image);
      texMat->setMatrix(osg::Matrix::scale(5,7,0.0f));
      tex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP);
      tex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP);
      tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR_MIPMAP_NEAREST);
      tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR_MIPMAP_LINEAR);
      cylinderSS->setTextureAttributeAndModes(0,tex,turnOn);
      cylinderSS->setAttributeAndModes(pm,turnOn);
      cylinderSS->setMode(GL_LIGHTING,turnOff);

      osg::TessellationHints *tessHints = new osg::TessellationHints();
      tessHints->setTessellationMode(osg::TessellationHints::USE_TARGET_NUM_FACES);
      tessHints->setTargetNumFaces(50);
      tessHints->setCreateNormals(false);
      tessHints->setCreateTextureCoords(true);

      osg::Geode *cylinderGeode = new osg::Geode();
      osg::MatrixTransform *cylinderTx = new osg::MatrixTransform();
      osg::ShapeDrawable *cylinder = new osg::ShapeDrawable(new osg::Cylinder(),tessHints);

      cylinderGeode->addDrawable(cylinder);
      cylinderGeode->setStateSet(cylinderSS);
      cylinderTx->setMatrix(osg::Matrix::scale(osg::Vec3(0.1f,0.1f,2)) *
         osg::Matrix::rotate(osg::DegreesToRadians(90.0f),osg::Vec3(1,0,0)) *
         osg::Matrix::translate(osg::Vec3(0,1.1f,0)));
      cylinderTx->addChild(cylinderGeode);

      osg::Geode *coneGeode = new osg::Geode();
      osg::MatrixTransform *coneTx = new osg::MatrixTransform();
      osg::ShapeDrawable *cone = new osg::ShapeDrawable(new osg::Cone(),tessHints);

      coneGeode->addDrawable(cone);
      coneGeode->setStateSet(coneSS);
      coneTx->setMatrix(osg::Matrix::scale(osg::Vec3(0.35f,0.35f,0.85f)) *
         osg::Matrix::rotate(osg::DegreesToRadians(-90.0f),osg::Vec3(1,0,0)) *
         osg::Matrix::translate(osg::Vec3(0,2.1f,0)));
      coneTx->addChild(coneGeode);

      osg::Group *group = new osg::Group();
      group->addChild(cylinderTx);
      group->addChild(coneTx);
      return group;
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
