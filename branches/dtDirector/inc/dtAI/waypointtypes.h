/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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

#ifndef __DELTA_WAYPOINTTYPES_H__
#define __DELTA_WAYPOINTTYPES_H__

#include <string>
#include <osg/Vec3>

#include <dtUtil/refstring.h>

#include <dtAI/export.h>
#include <dtAI/primitives.h>
#include <dtAI/waypoint.h>

#include <dtDAL/objecttype.h>

#include <dtCore/refptr.h>

namespace dtDAL
{
   class PropertyContainer;
}

namespace dtAI
{
   class WaypointPropertyBase;

   class DT_AI_EXPORT WaypointTypes
   {
   public:
      static dtCore::RefPtr<dtDAL::ObjectType> DEFAULT_WAYPOINT;
      static dtCore::RefPtr<dtDAL::ObjectType> NAMED_WAYPOINT;
      static dtCore::RefPtr<dtDAL::ObjectType> TACTICAL_WAYPOINT;
      static dtCore::RefPtr<dtDAL::ObjectType> WAYPOINT_COLLECTION;
   };

   class DT_AI_EXPORT NamedWaypoint : public osg::Referenced, public WaypointInterface
   {
   public:
      typedef WaypointInterface BaseClass;

   public:
      NamedWaypoint();
      NamedWaypoint(const osg::Vec3& pos);
      NamedWaypoint(const dtUtil::RefString& name);
      NamedWaypoint(const osg::Vec3& pos, const dtUtil::RefString& name);
      /*virtual*/ ~NamedWaypoint();

      void SetName(const std::string& name);
      const std::string& GetName() const;
      std::string GetNameCopy() const;

      /*virtual*/ std::string ToString() const;

      /*virtual*/ const osg::Vec3& GetPosition() const;
      /*virtual*/ void SetPosition(const osg::Vec3& pVec);

      /*virtual*/ void CreateProperties(WaypointPropertyBase& container);

      virtual void ref() const
      {
         osg::Referenced::ref();
      }

      virtual void unref() const
      {
         osg::Referenced::unref();
      }

   protected:
      // these allow derivatives of named waypoint to pass in their derivated object type
      NamedWaypoint(const dtDAL::ObjectType* ot);
      NamedWaypoint(const dtDAL::ObjectType* ot, const osg::Vec3& pos);
      NamedWaypoint(const dtDAL::ObjectType* ot, const dtUtil::RefString& name);
      NamedWaypoint(const dtDAL::ObjectType* ot, const osg::Vec3& pos, const dtUtil::RefString& name);

   private:
      dtUtil::RefString mName;

      osg::Vec3 mPosition;
   };

   class DT_AI_EXPORT TacticalWaypoint : public NamedWaypoint
   {
   public:
      typedef NamedWaypoint BaseClass;

   public:
      TacticalWaypoint();
      TacticalWaypoint(const osg::Vec3& pos);
      TacticalWaypoint(const dtUtil::RefString& name);
      TacticalWaypoint(const osg::Vec3& pos, const dtUtil::RefString& name);
      /*virtual*/ ~TacticalWaypoint();

      void SetCover(bool b);
      bool GetCover() const;

      void SetCoverAngle(const osg::Vec3& angle);
      const osg::Vec3& GetCoverAngle() const;
      osg::Vec3 GetCoverAngleCopy() const;

      void SetSniper(bool b);
      bool GetSniper() const;

      void SetSniperAngle(const osg::Vec3& angle);
      const osg::Vec3& GetSniperAngle() const;
      osg::Vec3 GetSniperAngleCopy() const;

      /*virtual*/ void CreateProperties(WaypointPropertyBase& container);

   protected:

   private:
      bool mCover, mSniper;
      osg::Vec3 mCoverAngle;
      osg::Vec3 mSniperAngle;
   };

} // namespace dtAI

#endif // __DELTA_WAYPOINTTYPES_H__
