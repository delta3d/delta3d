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

#ifndef DELTA_EFFECTMANAGER
#define DELTA_EFFECTMANAGER

// effectmanager.h: Declaration of the EffectManager class.
//
//////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Change Advisory (13 January 2006 submitted by LT Ryan Ernst)
 * 
 * Discussion:
 * The enumerated DetonationType parameter restricts extensibility of the
 * EffectManager.
 *
 * Solution:
 * A string value replaces DetonationType.
 *
 * Backward Compatability:
 * DetonationType remains supported but is converted internally to a string
 * representing the numeric value of the DetonationType.
 * For example; SmokeDetonation is converted to the string value "2000".
 *
 * Developer Recommendations:
 * Utilize string functionality to avoid future deprecation.
 *****************************************************************************/

#include <dtCore/deltadrawable.h>
#include <dtCore/refptr.h>
#include <osg/Vec3>

#include <map>
#include <set>
#include <string>
#include <vector>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
   class Node;
}
/// @endcond

namespace dtCore
{
   //forward declaration
   class EffectListener;
   class Effect;
   class Detonation;
   class Transformable;
   
   /**
    * Detonation types 
    *
    * Note: This may be deprecated in the future, use string functionality
    * instead.
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
   class DT_CORE_EXPORT EffectManager : public DeltaDrawable
   {
      DECLARE_MANAGEMENT_LAYER(EffectManager)


      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         EffectManager(const std::string& name = "effectManager");
      
      protected:
         /**
          * Destructor.
          */
         virtual ~EffectManager();

      public:
         /**
          * Maps the specified detonation name to the given filename. It will
          * replace any existing filename bound to the detonationName string.
          *
          * @param detonationName the detonation name to map
          * @param filename the filename corresponding to the detonation type
          */
         void AddDetonationTypeMapping(const std::string& detonationName, 
                                       const std::string& filename);

         /**
          * Removes the mapping for the given detonation name.
          *
          * @param detonationName the detonation name to unmap
          */
         void RemoveDetonationTypeMapping(const std::string& detonationName);


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
         const Effect* GetEffect(int index) const;

         /**
          * Adds a new detonation effect.
          *
          * @param position the position of the detonation
          * @param type the name of the detonation
          * @param timeToLive the lifespan of the detonation, in seconds,
          * or 0.0 for unlimited
          * @param parent the parent of the detonation, or 0 for
          * none
          * @return a pointer to the detonation object
          */
         Detonation* AddDetonation(const osg::Vec3& position,
                                   const std::string& detonationName = "HighExplosiveDetonation",
                                   double timeToLive = 5.0,
                                   Transformable* parent = 0);

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
         virtual const osg::Node* GetOSGNode() const;

         /**
          * Processes a received message.
          *
          * @param data the message structure
          */
         virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;


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
          * Maps detonation names to filenames.
          */
         typedef std::map<std::string, std::string> StringMap;
         StringMap mDetonationTypeFilenameMap;

         /**
          * The vector of active effects.
          */
         typedef std::vector<dtCore::RefPtr<Effect> > EffectVector;
         EffectVector mEffects;

         /**
          * The set of effect listeners.
          */         
         typedef std::vector<dtCore::RefPtr<EffectListener> > EffectListenerVector;
         EffectListenerVector mEffectListeners;

         /**
          * The last recorded time.
          */
         double mLastTime;
   };

   DetonationType StringToDetonationType(const std::string& stringType);
   std::string DetonationTypeToString(DetonationType detonationType);
     
   /**
    * An interface for objects interested in the addition and removal
    * of effects from the manager.
    */
   class DT_CORE_EXPORT EffectListener : public osg::Referenced
   {
      public:
      
         /**
          * Called when an effect is added to the manager.
          *
          * @param effectManager the effect manager that generated
          * the event
          * @param effect the effect object
          */
         virtual void EffectAdded(EffectManager* effectManager, Effect* effect) = 0;

         /**
          * Called when an effect is removed from the manager.
          *
          * @param effectManager the effect manager that generated
          * the event
          * @param effect the effect object
          */
         virtual void EffectRemoved(EffectManager* effectManager, Effect* effect) = 0;
   };

   /**
    * The base class of all effects.
    */
   class DT_CORE_EXPORT Effect : public osg::Referenced
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

      protected:
      
         /**
          * Destructor.
          */
         virtual ~Effect();

      public:

         /**
          * Returns the effect's OpenSceneGraph node.
          *
          * @return the effect's OpenSceneGraph node
          */
         osg::Node* GetNode();
         const osg::Node* GetNode() const;

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
   class DT_CORE_EXPORT Detonation : public Effect
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
                    const std::string& detonationName,
                    Transformable* parent);

         /**
          * Retrieves the position of this detonation.
          *
          * @param result a vector to hold the result
          */
         void GetPosition(osg::Vec3& res) const;
         const osg::Vec3& GetPosition() const;

         /**
          * Returns the type of this detonation.
          *
          * @return the type of this detonation
          */
         const std::string& GetType();
         void GetType(DetonationType& detonationType);

         /**
          * Returns the Transformable parent of the detonation, or
          * 0 if the detonation is unparented.
          *
          * @return the parent of the detonation
          */
         Transformable* GetParent();
         const Transformable* GetParent() const;
         
      private:

         /**
          * The position of the detonation.
          */
         osg::Vec3 mPosition;

         /**
          * The name of the detonation.
          */
         std::string mDetonationName;
         
         /**
          * The optional parent of the transformation.
          */
         RefPtr<Transformable> mParent;

   };
}


#endif // DELTA_EFFECTMANAGER
