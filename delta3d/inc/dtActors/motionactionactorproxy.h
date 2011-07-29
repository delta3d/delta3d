#ifndef __MOTION_ACTION_ACTOR_PROXY_H__
#define __MOTION_ACTION_ACTOR_PROXY_H__

#include <dtCore/plugin_export.h>
#include <dtUtil/enumeration.h>
#include <dtActors/actionactorproxy.h>
#include <osg/Vec3>

/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
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
* Bradley Anderegg
*/

namespace dtCore
{
   class Transformable;
}

namespace dtActors
{


class DT_PLUGIN_EXPORT MotionActionActorProxy: public ActionActorProxy
{

public:

   class DT_PLUGIN_EXPORT ParentRelationEnum: public dtUtil::Enumeration
   {
      DECLARE_ENUM(ParentRelationEnum);
   public:
      static ParentRelationEnum TRACK_PARENT;
      static ParentRelationEnum FOLLOW_PARENT;
      static ParentRelationEnum TRACK_AND_FOLLOW;
      static ParentRelationEnum NO_RELATION;

   private:
      ParentRelationEnum(const std::string& name): dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
   };

public:

   /*virtual*/ void BuildPropertyMap();
   /*virtual*/ bool IsPlaceable();

   void SetActorTargetObject(dtCore::BaseActorObject* node);
   dtCore::DeltaDrawable* GetActorTargetObject();

   void SetActorParentObject(dtCore::BaseActorObject* actor);
   dtCore::DeltaDrawable* GetActorParentObject();

   void SetActorParentRelation(ParentRelationEnum& e);
   ParentRelationEnum& GetActorParentRelation();

   void SetRotation(const osg::Vec3 &rotation);
   osg::Vec3 GetRotation();

   void SetTranslation(const osg::Vec3 &translation);
   osg::Vec3 GetTranslation();

protected:

   virtual ~MotionActionActorProxy() {}
};

}//namespace dtActors

#endif //__MOTION_ACTION_ACTOR_PROXY_H__
