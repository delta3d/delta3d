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

#ifndef DELTA_EFFECTMANAGER
#define DELTA_EFFECTMANAGER

// effectmanager.h: Declaration of the EffectManager class.
//
//////////////////////////////////////////////////////////////////////


#include <map>
#include <set>
#include <string>
#include <vector>

#include <Producer/Timer>
#include <osg/Node>
#include <osg/Group>
#include <osgParticle/ParticleSystemUpdater>

#include <osg/Vec3>
#include "sg.h"

#include "dtCore/deltadrawable.h"
#include "dtUtil/deprecationmgr.h"

namespace dtCore
{
   //forward declaration
   class EffectListener;
   class Effect;
   class Detonation;
   class Transformable;
   
   /**
    * Detonation types 
    */
   enum DetonationType
   {
      HighExplosiveDetonation = 1000,
      SmokeDetonation         = 2000,
      WP                      = 3000,
      VT                      = 4000,
      ICM                     = 5000,
      M825                    = 6000
   };

   /**
    * An effect manager.
    */
   class DT_EXPORT EffectManager : public DeltaDrawable
   {
      DECLARE_MANAGEMENT_LAYER(EffectManager)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         EffectManager(std::string name = "effectManager");

         /**
          * Destructor.
          */
         virtual ~EffectManager();

         /**
          * Maps the specified detonation type to the given filename.
          *
          * @param detonationType the detonation type to map
          * @param filename the filename corresponding to the detonation type
          */
         void AddDetonationTypeMapping(DetonationType detonationType, std::string filename);

         /**
          * Removes the mapping for the given detonation type.
          *
          * @param detonationType the detonation type to unmap
          */
         void RemoveDetonationTypeMapping(DetonationType detonationType);

         /**
          * Returns the number of active effects.
          *
          * @return the number of active effects
          */
         int GetEffectCount() const;

         /**
          * Returns the effect at the specified index.
          *
          * @param index the index of the effect to retrieve
          * @return the effect at the specified index
          */
         Effect* GetEffect(int index) const;

         /**
          * Adds a new detonation effect.
          *
          * @param position the position of the detonation
          * @param type the type of the detonation
          * @param timeToLive the lifespan of the detonation, in seconds,
          * or 0.0 for unlimited
          * @param parent the parent of the detonation, or NULL for
          * none
          * @return a pointer to the detonation object
          */
         Detonation* AddDetonation(
            const osg::Vec3& position,
            DetonationType type = HighExplosiveDetonation,
            double timeToLive = 5.0,
            Transformable* parent = NULL
         );

         //deprecated conversion
         Detonation* AddDetonation(
            sgVec3 position,
            DetonationType type = HighExplosiveDetonation,
            double timeToLive = 5.0,
            Transformable* parent = NULL
            )
         {
            DEPRECATE("Detonation* AddDetonation(\
               sgVec3 position,\
               DetonationType type = HighExplosiveDetonation,\
               double timeToLive = 5.0,\
               Transformable* parent = NULL",

               "Detonation* AddDetonation(\
               const osg::Vec3& position,\
               DetonationType type = HighExplosiveDetonation,\
               double timeToLive = 5.0,\
               Transformable* parent = NULL"
               )
                
               return AddDetonation(osg::Vec3(position[0], position[1], position[2]), type, timeToLive, parent);
         }

         /**
          * Removes an effect from this manager.
          *
          * @param effect the effect to remove
          */
         void RemoveEffect(Effect* effect);

         /**
          * Adds a listener for effect events.
          *
          * @param effectListener the listener to add
          */
         void AddEffectListener(EffectListener* effectListener);

         /**
          * Removes a listener for effect events.
          *
          * @param effectListener the listener to remove
          */
         void RemoveEffectListener(EffectListener* effectListener);

         /**
          * Returns this object's OpenSceneGraph node.
          *
          * @return the OpenSceneGraph node
          */
         virtual osg::Node* GetOSGNode();

         /**
          * Processes a received message.
          *
          * @param data the message structure
          */
         virtual void OnMessage(MessageData *data);


      protected:

         /**
          * Adds an effect to this manager.
          *
          * @param effect the effect to add
          */
         void AddEffect(Effect* effect);


      private:

         /**
          * The group that contains all effect nodes.
          */
         RefPtr<osg::Group> mGroup;

         /**
          * Maps detonation types to filenames.
          */
         std::map< DetonationType, std::string > mDetonationTypeFilenameMap;

         /**
          * The vector of active effects.
          */
         std::vector<Effect*> mEffects;

         /**
          * The set of effect listeners.
          */
         std::set<EffectListener*> mEffectListeners;

         /**
          * A Producer timer object.
          */
         Producer::Timer mTimer;

         /**
          * The last recorded time.
          */
         Producer::Timer_t mLastTime;
   };


   /**
    * An interface for objects interested in the addition and removal
    * of effects from the manager.
    */
   class DT_EXPORT EffectListener
   {
      public:
      
         /**
          * Called when an effect is added to the manager.
          *
          * @param effectManager the effect manager that generated
          * the event
          * @param effect the effect object
          */
         virtual void EffectAdded(
            EffectManager* effectManager,
            Effect* effect
         ) {}

         /**
          * Called when an effect is removed from the manager.
          *
          * @param effectManager the effect manager that generated
          * the event
          * @param effect the effect object
          */
         virtual void EffectRemoved(
            EffectManager* effectManager,
            Effect* effect
         ) {}
   };


   /**
    * The base class of all effects.
    */
   class DT_EXPORT Effect
   {
      public:

         /**
          * Constructor.
          *
          * @param node the effect's OpenSceneGraph node
          * @param timeToLive the lifespan of the effect in
          * seconds, or 0.0 for unlimited
          */
         Effect(osg::Node* node, double timeToLive);

         /**
          * Destructor.
          */
         virtual ~Effect();

         /**
          * Returns the effect's OpenSceneGraph node.
          *
          * @return the effect's OpenSceneGraph node
          */
         osg::Node* GetNode();

         /** 
          * Sets the remaining lifespan of this effect.
          *
          * @param timeToLive the remaining lifespan, in seconds,
          * or 0.0 for unlimited
          */
         void SetTimeToLive(double timeToLive);

         /**
          * Returns the remaining lifespan of this effect.
          *
          * @return the remaining lifespan, in seconds, or
          * 0.0 for unlimited
          */
         double GetTimeToLive();

         /**
          * Sets the dying flag.
          *
          * @param dying the new value of the dying flag
          */
         void SetDying(bool dying);

         /**
          * Checks the dying flag.
          *
          * @return the value of the dying flag
          */
         bool IsDying();


      private:

         /**
          * The effect's OpenSceneGraph node.
          */
         RefPtr<osg::Node> mNode;

         /**
          * The remaining lifespan of the detonation, in seconds.
          * A value of zero indicates an unlimited lifespan.
          */
         double mTimeToLive;

         /**
          * The dying flag.  Set to true for particle system effects
          * that must be kept alive until their remaining particles
          * have disappeared.
          */
         bool mDying;
   };


   /**
    * A detonation effect.
    */
   class DT_EXPORT Detonation : public Effect
   {
      public:

         /**
          * Constructor.
          *
          * @param node the particle system node
          * @param timeToLive the lifespan of the detonation, or
          * 0.0 for unlimited
          * @param position the position of the detonation
          * @param type the type of the detonation
          * @param parent the parent of the detonation, or NULL
          * for none
          */
         Detonation(osg::Node* node,
                    double timeToLive,
                    const osg::Vec3& position,
                    DetonationType type,
                    Transformable* parent);

         //deprecated conversion
         Detonation(osg::Node* node,
                    double timeToLive,
                    sgVec3 position,
                    DetonationType type,
                    Transformable* parent
                    )
                 : Effect(node, timeToLive),
                   mType(type),
                   mParent(parent)
         {
            DEPRECATE("Detonation(\
                      osg::Node* node,\
                      double timeToLive,\
                      sgVec3 position,\
                      DetonationType type,\
                      Transformable* parent",

                      "Detonation(\
                      osg::Node* node,\
                      double timeToLive,\
                      const osg::Vec3& position,\
                      DetonationType type,\
                      Transformable* parent"
                      )

            mPosition = osg::Vec3(position[0], position[1], position[2]);          
         }

         
         /**
          * Retrieves the position of this detonation.
          *
          * @param result a vector to hold the result
          */
         void GetPosition(osg::Vec3& res);

         //deprecated version
         void GetPosition(sgVec3 res)
         {
            DEPRECATE("void GetPosition(sgVec3 result)", "void GetPosition(osg::Vec3& result)")
            osg::Vec3 tmp(res[0], res[1], res[2]);
            GetPosition(tmp);
            res[0] = tmp[0]; res[1] = tmp[1]; res[2] = tmp[2];
         }

         /**
          * Returns the type of this detonation.
          *
          * @return the type of this detonation
          */
         DetonationType GetType();

         /**
          * Returns the Transformable parent of the detonation, or
          * NULL if the detonation is unparented.
          *
          * @return the parent of the detonation
          */
         Transformable* GetParent();
         

      private:

         /**
          * The position of the detonation.
          */
         osg::Vec3 mPosition;

         /**
          * The type of the detonation.
          */
         DetonationType mType;
         
         /**
          * The optional parent of the transformation.
          */
         RefPtr<Transformable> mParent;
   };
};


#endif // DELTA_EFFECTMANAGER
