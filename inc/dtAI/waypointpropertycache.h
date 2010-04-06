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

#ifndef __DELTA_WAYPOINTPROPERTYCACHE_H__
#define __DELTA_WAYPOINTPROPERTYCACHE_H__

#include <dtUtil/log.h>
#include <osg/Referenced>

#include <dtDAL/objecttype.h>
#include <dtUtil/objectfactory.h>
#include <dtCore/refptr.h>

#include <dtAI/waypointpropertycontainer.h>

namespace dtAI
{
   class WaypointPropertyCache: public osg::Referenced
   {
   public:
      WaypointPropertyCache()
         : mPropertyFactory(new WaypointPropertyFactory())
      {

      }

      WaypointPropertyBase* GetPropertyContainer(const dtDAL::ObjectType& waypointType, WaypointInterface* wt)
      {
         WaypointPropertyBase* wpb = mPropertyFactory->CreateObject(&waypointType);
         wpb->Set(wt);
         wt->CreateProperties(*wpb);
         return wpb;
      }

      template <class WaypointDerivative>
      void RegisterType(dtCore::RefPtr<const dtDAL::ObjectType> type)
      {
         mPropertyFactory->RegisterType<WaypointPropertyContainer<WaypointDerivative> >(type);
      }

   private:
      typedef dtUtil::ObjectFactory< dtCore::RefPtr<const dtDAL::ObjectType>, WaypointPropertyBase> WaypointPropertyFactory;

      dtCore::RefPtr<WaypointPropertyFactory> mPropertyFactory;
   };

} // namespace dtAI

#endif // __DELTA_WAYPOINTPROPERTYCACHE_H__
