/* -*-c++-*-
* testGameActorLibrary - testhlaobject (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* David A. Guthrie
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
   TestHLAObject(dtGame::GameActorProxy& parent);
      
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

   ///This is a placeholder for the mesh property since we don't want to actually load anything.
   void TestLoadTheMesh(const std::string& value);
   
private:
   dtCore::RefPtr<dtGame::DeadReckoningActorComponent> mDeadReckoningActorComponent;
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


      /// Builds the actor components needed to make this work.
      virtual void BuildActorComponents();
             
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
      virtual void CreateDrawable();
   private:
};

#endif 

