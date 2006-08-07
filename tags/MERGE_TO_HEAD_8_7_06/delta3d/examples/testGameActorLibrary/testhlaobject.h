/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation
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
 * @author David A. Guthrie
 */
#ifndef DELTA_TEST_HLA_OBJECT
#define DELTA_TEST_HLA_OBJECT

#include <dtUtil/enumeration.h>
#include <dtGame/gameactor.h>
#include <dtGame/deadreckoningcomponent.h>
#include "export.h"

class DT_EXAMPLE_EXPORT TestHLAObject : public dtGame::GameActor
{
public:
   // Declare the classes used to represent the various enumerations
   // located in the distypes.h file.
   class DT_EXAMPLE_EXPORT DamageStateEnum : public dtUtil::Enumeration
   {
      DECLARE_ENUM(DamageStateEnum);
   public:
      static DamageStateEnum NO_DAMAGE;
      static DamageStateEnum DAMAGED;
      static DamageStateEnum DESTROYED;
   private:
      DamageStateEnum(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
   };

   /// Constructor
   TestHLAObject(dtGame::GameActorProxy& proxy);
      
   /// Destructor
   virtual ~TestHLAObject();

   /**
    * Sets this entity's DIS/RPR-FOM damage state.
    *
    * @param damageState the damage state
    */
   void SetDamageState(TestHLAObject::DamageStateEnum &damageState);
   
   /**
    * Returns this entity's DIS/RPR-FOM damage state.
    *
    * @return the damage state
    */
   TestHLAObject::DamageStateEnum& GetDamageState() const;

   /**
    * Sets this entity's last known translation.  This should
    * only be set for remote actors.
    *
    * @param vec the new last position.
    */
   void SetLastKnownTranslation(const osg::Vec3 &vec);
   
   /**
    * @return the last known position for this if it's a remote entity.
    */
   osg::Vec3 GetLastKnownTranslation() const { return mDeadReckoningHelper->GetLastKnownTranslation(); }
   
   /**
    * Sets this entity's last known rotation.  This should
    * only be set for remote actors.
    *
    * @param vec the new last rotation as yaw, pitch, roll.
    */
   void SetLastKnownRotation(const osg::Vec3 &vec);
   
   /**
    * @return the last known rotation for this if it's a remote entity as yaw, pitch, roll.
    */
   osg::Vec3 GetLastKnownRotation() const { return mDeadReckoningHelper->GetLastKnownRotation(); }
   
private:
   dtCore::RefPtr<dtGame::DeadReckoningHelper> mDeadReckoningHelper;
   DamageStateEnum* mDamageState;
};

class DT_EXAMPLE_EXPORT TestHLAObjectProxy : public dtGame::GameActorProxy
{
   public:

      /// Constructor
      TestHLAObjectProxy();

      /// Destructor
      virtual ~TestHLAObjectProxy();

      /**
       * Builds the properties associated with this proxy's actor
       */
      virtual void BuildPropertyMap();

      /**
       * Builds the invokable associated with this proxy.
       */
      virtual void BuildInvokables();
             
      /**
       * Sets this entity's last known rotation.  This should
       * only be set for remote actors.  This method sets it on the actor and
       * reorders the values since actor properties store the values differently than actors themselves.
       *
       * @param vec the new last rotation as x, y, z.
       */
      void SetLastKnownRotation(const osg::Vec3 &vec);
      
      /**
       * @return the last know rotation for this if it's a remote entity as x, y, z.
       */
      osg::Vec3 GetLastKnownRotation() const;
   protected:
      virtual void CreateActor();
   private:
};

#endif 

