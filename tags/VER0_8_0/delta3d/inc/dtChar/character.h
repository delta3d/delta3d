/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_CHARACTER
#define DELTA_CHARACTER

// character.h: Declaration of the Character class.
//
//////////////////////////////////////////////////////////////////////


#include "dtCore/transformable.h"
#include "dtCore/loadable.h"

#undef Status
#include "rbody/osg/OsgBody.h"

namespace dtChar
{
   /**
    * An animated character.
    */
   class DT_EXPORT Character : public dtCore::Transformable, public dtCore::Loadable
   {
      DECLARE_MANAGEMENT_LAYER(Character)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         Character(std::string name = "character");

         /**
          * Destructor.
          */
         virtual ~Character();

         virtual osg::Node* GetOSGNode() { return dynamic_cast<osg::Node*>(mBodyNode.get()); }
         virtual osg::MatrixTransform* GetMatrixNode(void) { return dynamic_cast<osg::MatrixTransform*>(mBodyNode.get()); }
      
         /**
          * Notifies this drawable object that it has been added to
          * a scene.
          *
          * @param scene the scene to which this drawable object has
          * been added
          */
         virtual void AddedToScene(dtCore::Scene* scene);
         
         /**
          * Loads a ReplicantBody data file for this character.
          *
          * @param filename the name of the file to load
          * @return true if successful, false if not
          */
         virtual osg::Node* LoadFile( std::string filename, bool useCache = true );

         /**
          * Sets the rotation of this character.
          *
          * @param rotation the new rotation, in degrees about the Z
          * axis
          */
         void SetRotation(float rotation);
         
         /**
          * Returns the current rotation of this character.
          *
          * @return the current rotation, in degrees about the Z
          * axis
          */
         float GetRotation() const;
         
         /**
          * Sets the walk/run velocity of this character.
          *
          * @param velocity the new walk/run velocity
          */
         void SetVelocity(float velocity);
         
         /**
          * Returns the current walk/run velocity of this character.
          *
          * @return the current walk/run velocity
          */
         float GetVelocity() const;
         
         /**
          * Executes a character action.
          *
          * @param action the name of the action to execute
          * @param priority whether or not the action is high-priority
          * @param force whether or not to force the action
          */
         void ExecuteAction(std::string name,
                            bool priority = true, 
                            bool force = false);
         
         /**
          * Executes a character action with a speed parameter.
          *
          * @param name the name of the action to execute
          * @param speed the speed at which to execute the action
          * @param priority whether or not the action is high-priority
          * @param force whether or not to force the action
          */
         void ExecuteActionWithSpeed(std::string name, 
                                     double speed,
                                     bool priority = true,
                                     bool force = false);
         
         /**
          * Executes a character action with an angle parameter.
          *
          * @param name the name of the action to execute
          * @param angle the angle at which to execute the action
          * @param priority whether or not the action is high-priority
          * @param force whether or not to force the action
          */
         void ExecuteActionWithAngle(std::string name, 
                                     double angle,
                                     bool priority = true,
                                     bool force = false);
         
         /**
          * Executes a character action with speed and angle parameters.
          *
          * @param name the name of the action to execute
          * @param speed the speed at which to execute the action
          * @param angle the angle at which to execute the action
          * @param priority whether or not the action is high-priority
          * @param force whether or not to force the action
          */
         void ExecuteActionWithSpeedAndAngle(std::string name, 
                                             double speed, 
                                             double angle,
                                             bool priority = true,
                                             bool force = false);
         
         /**
          * Stops a character action.
          *
          * @param name the name of the action to stop
          */
         void StopAction(std::string name);
         

      private:
         
         /**
          * The ReplicantBody OSG node.
          */
         osg::ref_ptr<rbody::OsgBodyNode> mBodyNode;
         
         /**
          * The collision root node.
          */
         osg::ref_ptr<osg::Node> mCollisionRootNode;
         
         /**
          * The name of the last file loaded.
          */
         std::string mFilename;
         
         /**
          * The character's current rotation.
          */
         float mRotation;
         
         /**
          * The character's current walk/run velocity.
          */
         float mVelocity;
   };
};


#endif // DELTA_CHARACTER
