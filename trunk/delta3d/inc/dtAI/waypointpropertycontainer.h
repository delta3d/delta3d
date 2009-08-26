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

#ifndef __DELTA_WAYPOINTPROPERTYCONTAINER_H__
#define __DELTA_WAYPOINTPROPERTYCONTAINER_H__

#include <dtDAL/propertycontainer.h>
#include <dtUtil/log.h>
#include <osg/Referenced>

namespace dtAI
{
   class WaypointPropertyBase: public dtDAL::PropertyContainer
   {
      public:

         class CommandBase : public osg::Referenced
         {
         public:
            CommandBase(WaypointPropertyBase& wp) 
               : osg::Referenced() 
               , mOwnership(&wp)
            {

            }

            WaypointInterface* Get()
            {
               return mOwnership->Get();
            };

            const WaypointInterface* Get() const
            {
               return mOwnership->Get();
            };

         protected:
            virtual ~CommandBase() {}

         private:
            WaypointPropertyBase* mOwnership;
         };

         WaypointPropertyBase(){};

         virtual ~WaypointPropertyBase(){}


         operator WaypointInterface*()
         {
            return Get();
         }

         operator const WaypointInterface*() const
         {
            return Get();
         }

         WaypointInterface* operator->()
         {
            return Get();
         }

         const WaypointInterface* operator->() const
         {
            return Get();
         }

         WaypointInterface& operator*()
         {
            return *Get();
         }

         const WaypointInterface& operator*() const
         {
            return *Get();
         }

         virtual void AddCommand(CommandBase* cmd)
         {
            mCommands.push_back(cmd);
         }

         virtual WaypointInterface* Get() = 0;
         virtual const WaypointInterface* Get() const = 0;
         virtual void Set(WaypointInterface*) = 0;

      protected:

         std::vector<dtCore::RefPtr<CommandBase> > mCommands;


   };

   //T must be derived from WaypointInterface
   template <class T>
   class WaypointPropertyContainer: public WaypointPropertyBase
   {
   public:
      typedef T WaypointType;

      template<typename RetT, typename PointerToMemFn>
      class Command0 : public CommandBase
      {
      public:

         Command0(WaypointPropertyBase& wp, PointerToMemFn ptr)
            : CommandBase(wp) 
            , pMemFn_(ptr)            
         {
         }

         RetT Invoke()
         {
            return ((*Get()).*pMemFn_)();
         }

      protected:
         /*virtual*/ ~Command0()
         {
         }

      private:
         PointerToMemFn pMemFn_;
      };

      template<typename RetT, typename PointerToMemFn, typename ArgT>
      class Command1 : public CommandBase
      {
      public:

         Command1(WaypointPropertyBase& wp, PointerToMemFn ptr) 
            : CommandBase(wp)
            , pMemFn_(ptr)
         {
         }

         RetT Invoke(ArgT t)
         {
            return ((*Get()).*pMemFn_)(t);
         }


      protected:
         /*virtual*/ ~Command1()
         {
         }

      private:
         PointerToMemFn pMemFn_;
      };


   public:
      WaypointPropertyContainer() 
      {

      }
      virtual ~WaypointPropertyContainer(){}

      /*virtual*/ WaypointInterface* Get()
      {
         return mWaypoint;
      }

      /*virtual*/ const WaypointInterface* Get() const
      {
         return mWaypoint;
      }

      /*virtual*/ void Set(WaypointInterface* wi)
      {
         WaypointType* wt = dynamic_cast<WaypointType*>(wi);
         if(wt == NULL)
         {
            LOG_ERROR("Attempting to set incompatible waypoint type on WaypointPropertyContainer.");
         }
         else
         {
            mWaypoint = wt;
         }
      }

      /*virtual*/ dtDAL::PropertyContainer& GetProperties()
      {
         return *mPropertyContainer;
      }

      /*virtual*/ const dtDAL::PropertyContainer& GetProperties() const
      {
         return *mPropertyContainer;
      }

      /*virtual*/ void AddCommand(CommandBase* cmd)
      {
         mCommands.push_back(cmd);
      }

   private:
      WaypointType* mWaypoint;
   };

} // namespace dtAI

#endif // __DELTA_WAYPOINTPROPERTYCONTAINER_H__
