#ifndef __MOTION_ACTION_H__
#define __MOTION_ACTION_H__

#include <osg/Vec3>
#include <osg/Matrix>
#include <dtCore/transformable.h>
#include "export.h"
#include "action.h"
#include "pathpoint.h"


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
* @author Bradley Anderegg
*/

namespace dtABC
{

class DT_ABC_EXPORT MotionAction: public Action
{
public:
   /***
   * ParentRelation is used to determine how the object will move relative to its parent
   * TRACK is used to keep an orientation relative to parent
   * FOLLOW translates with the parent
   */
   enum PARENT_RELATION{TRACK_PARENT = 1, FOLLOW_PARENT, TRACK_AND_FOLLOW};

public:
   MotionAction();

   const dtCore::Transformable* GetTargetObject() const {return mTargetObject.get();}
   void SetTargetObject(dtCore::Transformable* pTarget){mTargetObject = pTarget;}

   void SetLocalTransform(const osg::Matrix& mat){mLocalTransform = mat;}
   const osg::Matrix& GetLocalTransform() const {return mLocalTransform;}

   
   const dtCore::Transformable* GetParent() const {return mParent.get();}
   void SetParent(dtCore::Transformable* pParent, PARENT_RELATION pRelation);
   void RemoveParent();


protected:

    virtual ~MotionAction();
    MotionAction(const MotionAction&); //not implemented by design

   /***
   * This function should be called by the derived class
   * which sets the transform of the object according
   * to parent and parent relation
   *
   * @note we use a PathPoint because we are only interested 
   *        in a translation and orientation, without the headache
   *        of using the transform API
   */
   void StepObject(const PathPoint& p);

  
private:

   void SetTrack(const osg::Vec3& forwardVector, osg::Matrix& mat);

protected:



   /***
   * The local transform allows us to keep the same relative transform
   * on update, if we want a scale or offset from origin, etc.
   */
   osg::Matrix                               mLocalTransform;
  
   /***
   * The TargetObject is the object we are moving
   */
   dtCore::RefPtr<dtCore::Transformable>     mTargetObject;

   /***
   * If we want to have motion relative to another transform we can 
   * set this here
   */
   dtCore::RefPtr<dtCore::Transformable>     mParent;
   osg::Vec3                                 mInitialParentPos;  
   unsigned int                              mParentRelation;

};



} //namespace dtABC




#endif //__MOTION_ACTION_H__

