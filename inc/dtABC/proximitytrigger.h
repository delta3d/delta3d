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
*/

#ifndef DELTA_PROXIMITY_TRIGGER
#define DELTA_PROXIMITY_TRIGGER

#include <dtCore/transformable.h>
#include <dtABC/export.h>
#include <dtABC/trigger.h>
#include <osg/NodeVisitor>

namespace dtABC
{
   /** The ProximityTrigger class contains a Trigger which it fires 
    * whenever a Transformable enters it's bounding shape. By default,
    * it has a collision sphere set with a radius of 5 units. To filter
    * what can collide with this ProximityTrigger use SetCollisionCollideBits
    * as the OR of all the CollisionCategories you want. A ProximityTrigger
    * is fired only once per touch of a Transformable.
    *
    * @note  This class currently does not work!  It may be replaced by other functionality or fixed before the next release.
    */
   class DT_ABC_EXPORT ProximityTrigger : public dtCore::Transformable
   {
      DECLARE_MANAGEMENT_LAYER(ProximityTrigger)

      ///Node callback to update traversal numbers inside ProximityTrigger.
      class NodeCallback : public osg::NodeCallback
      {
         friend class ProximityTrigger;

      public:

         NodeCallback(ProximityTrigger* trigger)
            : mTrigger(trigger)
         {}

         /**
         * Callback function.
         *
         * @param node the node to operate on
         * @param nv the active node visitor
         */
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
         {            
            mTrigger->SetTraversalNumber(nv->getFrameStamp()->getFrameNumber());
         
            traverse(node, nv);
         }

      private:

         /**
         * The owning ProximityTrigger object.
         */
         ProximityTrigger* mTrigger;
      };

   public:
      ProximityTrigger(const std::string& name = "ProximityTrigger");
   protected:
      virtual ~ProximityTrigger() {}
   public:

      /**
      * Non-const getter for the internal Trigger that this ProximityTrigger
      * holds. Use this to set the Action on the internal trigger.
      *
      * @return The non-const internal Trigger.
      */
      Trigger* GetTrigger() { return mTrigger.get(); }

      /**
      * Const getter for the internal Trigger that this ProximityTrigger
      * holds. Use this to query the Action on the internal trigger.
      *
      * @return The const internal Trigger.
      */
      const Trigger* GetTrigger() const { return mTrigger.get(); }
   
      bool IsPointInVolume(float x, float y, float z);


      void SetTimeDelay(float delay){mTrigger->SetTimeDelay(delay);}
      float GetTimeDelay()const{return mTrigger->GetTimeDelay();}

   protected:

      void SetTraversalNumber(int number) { mLastTraversalNumber = number; }

   private:

      dtCore::RefPtr<Trigger>                   mTrigger;
      int                                       mLastTraversalNumber;

      typedef std::map<Transformable*, int>     TransformableIntMap;
      TransformableIntMap                       mTraversalNumberMap;

   };
}

#endif //DELTA_PROXIMITY_TRIGGER
