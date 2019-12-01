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
* @AUTHOR Bradley Anderegg
*/

#ifndef DELTA_AUTO_TRIGGER
#define DELTA_AUTO_TRIGGER

#include <dtABC/export.h>
#include <dtABC/trigger.h>

namespace dtABC
{
   /** 
   *  The AutoTrigger is a Trigger that does not need to be
   *  enabled or fired
   */ 
   class DT_ABC_EXPORT AutoTrigger : public dtCore::DeltaDrawable
   {
      DECLARE_MANAGEMENT_LAYER(AutoTrigger)

   public:
      AutoTrigger( const std::string& name = "AutoTrigger" );
   protected:
      virtual ~AutoTrigger() {}

   public:

      /**
      * Non-const getter for the internal Trigger that this AutoTrigger
      * holds. Use this to set the Action on the internal trigger.
      *
      * @return The non-const internal Trigger.
      */
      Trigger* GetTrigger() { return mTrigger.get(); }

      /**
      * Const getter for the internal Trigger that this AutoTrigger
      * holds. Use this to query the Action on the internal trigger.
      *
      * @return The const internal Trigger.
      */
      const Trigger* GetTrigger() const { return mTrigger.get(); }

      void SetTimeDelay(float delay){mTrigger->SetTimeDelay(delay);}
      float GetTimeDelay()const{return mTrigger->GetTimeDelay();}

      osg::Node* GetOSGNode(){return mNode.get();}
      const osg::Node* GetOSGNode() const{return mNode.get();}

   private:

      dtCore::RefPtr<Trigger>                   mTrigger;
      dtCore::RefPtr<osg::Node>                 mNode;      
   };
}

#endif //DELTA_AUTO_TRIGGER
