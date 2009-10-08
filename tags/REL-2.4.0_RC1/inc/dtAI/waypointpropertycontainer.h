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

#include <dtDAL/typetoactorproperty.h>
#include <dtDAL/propertycontainer.h>
#include <dtUtil/log.h>
#include <osg/Referenced>

namespace dtAI
{

   //////////////////////////////////////////////////////////////////////////
   // A custom command class that can be used with a PropertyContainer
   //////////////////////////////////////////////////////////////////////////
   class WaypointPropertyBase;
   class CommandBase : public osg::Referenced
   {
   public:
      CommandBase(WaypointPropertyBase& wp)
         : osg::Referenced()
         , mOwnership(&wp)
      {
      }

      WaypointInterface* Get();
      //{
      //   return mOwnership->Get();
      //};

      const WaypointInterface* Get() const;
      //{
      //   return mOwnership->Get();
      //};

   protected:
      virtual ~CommandBase() {}

   private:
      WaypointPropertyBase* mOwnership;
   };

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
         return ((*static_cast<typename dtUtil::FunTraits<PointerToMemFn>::ObjType*>(Get())).*pMemFn_)();
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
         return ((*static_cast<typename dtUtil::FunTraits<PointerToMemFn>::ObjType*>(Get())).*pMemFn_)(t);
      }

   protected:
      /*virtual*/ ~Command1()
      {
      }

   private:
      PointerToMemFn pMemFn_;
   };

   //////////////////////////////////////////////////////////////////////////
   // WaypointPropertyBase
   //////////////////////////////////////////////////////////////////////////
   class WaypointPropertyBase: public dtDAL::PropertyContainer
   {
   public:
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

      template<class PropertyType_, typename CallTypeGet, typename CallTypeSet>
      void CreateProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
                           CallTypeGet getFunc,
                           CallTypeSet setFunc,
                           const dtUtil::RefString& desc, const dtUtil::RefString& group)
      {
         typedef typename dtUtil::FunTraits<CallTypeGet>::ObjType ClassType_;
         typedef typename dtDAL::TypeToActorProperty<PropertyType_>::value_type ActorPropertyType;

         typedef typename dtDAL::TypeToActorProperty<PropertyType_>::SetValueType SetParamType;
         typedef typename dtDAL::TypeToActorProperty<PropertyType_>::GetValueType GetParamType;

         typedef typename ActorPropertyType::SetFuncType SetFuncType;
         typedef typename ActorPropertyType::GetFuncType GetFuncType;

         typedef Command0<GetParamType, PropertyType_(ClassType_::*)() const> GetCmd;
         typedef Command1<void, void (ClassType_::*)(SetParamType), SetParamType> SetCmd;

         GetCmd* getter = new GetCmd(*this, getFunc);
         SetCmd* setter = new SetCmd(*this, setFunc);

         AddCommand(setter);
         AddCommand(getter);

         ActorPropertyType* prop = new ActorPropertyType(name, label,
            SetFuncType(setter, &SetCmd::Invoke),
            GetFuncType(getter, &GetCmd::Invoke),
            desc, group);

         PropertyContainer::AddProperty(prop);
      }

      template<class PropertyType_, typename CallTypeGet>
      void CreateReadOnlyProperty(const dtUtil::RefString& name, const dtUtil::RefString& label,
         CallTypeGet getFunc, const dtUtil::RefString& desc, const dtUtil::RefString& group)
      {
         typedef typename dtUtil::FunTraits<CallTypeGet>::ObjType ClassType_;
         typedef typename dtDAL::TypeToActorProperty<PropertyType_>::value_type ActorPropertyType;
         typedef typename ActorPropertyType::SetFuncType SetFuncType;
         typedef typename ActorPropertyType::GetFuncType GetFuncType;

         typedef typename dtDAL::TypeToActorProperty<PropertyType_>::GetValueType GetParamType;

         typedef Command0<GetParamType, PropertyType_(ClassType_::*)() const> GetCmd;

         GetCmd* getter = new GetCmd(*this, getFunc);

         AddCommand(getter);

         ActorPropertyType* prop = new ActorPropertyType(name, label,
            SetFuncType(),
            GetFuncType(getter, &GetCmd::Invoke),
            desc, group);

         prop->SetReadOnly(true);
         PropertyContainer::AddProperty(prop);
      }

   protected:
      std::vector<dtCore::RefPtr<CommandBase> > mCommands;
   };

   //////////////////////////////////////////////////////////////////////////
   // WaypointPropertyContainer
   //////////////////////////////////////////////////////////////////////////

   // T must be derived from WaypointInterface, could add a compile time assert here if we had loki
   template <class T>
   class WaypointPropertyContainer: public WaypointPropertyBase
   {
   public:
      typedef T WaypointType;

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

   private:
      WaypointType* mWaypoint;
   };

   // inline implementations

   inline WaypointInterface* CommandBase::Get()
   {
      return mOwnership->Get();
   };

   inline const WaypointInterface* CommandBase::Get() const
   {
      return mOwnership->Get();
   };

} // namespace dtAI

#endif // __DELTA_WAYPOINTPROPERTYCONTAINER_H__
