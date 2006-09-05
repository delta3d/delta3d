#ifndef __PATH_POINT_H__
#define __PATH_POINT_H__

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Quat>

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

namespace dtABC
{

/***
* PathPoint defines a single point on a BezierPath,
* it contains an orientation and position
*/
class PathPoint
{
  public: 
     PathPoint(){}
     PathPoint(const osg::Vec3& point, const osg::Quat& rot){mPosition = point; mOrientation = rot;}
     PathPoint(const osg::Vec3& point, const osg::Matrix& rot){mPosition = point; mOrientation.set(rot);}
     ~PathPoint(){}

     osg::Vec3 GetPosition() const { return mPosition; }
     //const osg::Vec3& GetPosition() const {return mPosition;}

     osg::Quat GetOrientation() const {return mOrientation;}
     void GetOrientation(osg::Matrix& mat) const {mat.set(mOrientation);}
     //const osg::Quat& GetOrientation() const {return mOrientation;}


     void SetPosition(const osg::Vec3& pPos){mPosition = pPos;}
     void SetOrientation(const osg::Quat& pOrientation){mOrientation = pOrientation;}

private:
   osg::Vec3 mPosition;
   osg::Quat mOrientation;

};

}//namespace dtABC


#endif //__PATH_POINT_H__

