// effectmanager.h: Declaration of the EffectManager class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DELTA_EFFECT_MANAGER
#define DELTA_EFFECT_MANAGER

#include <map>
#include <set>
#include <string>
#include <vector>

#include "Producer/Timer"

#include "osg/ref_ptr"
#include "osg/Node"
#include "osg/Group"

#include "osgParticle/ParticleSystemUpdater"

#include "sg.h"

#include "base.h"
#include "scene.h"


namespace dtCore
{
   class EffectListener;
   class Effect;
   class Detonation;
   
   
   /**
    * Detonation types (values correspond to DIS warhead field values).
    */
   enum DetonationType
   {
      HighExplosiveDetonation = 1000,
      SmokeDetonation = 2000
   };


   /**
    * An effect manager.
    */
   class DT_EXPORT EffectManager : public Base, public Drawable
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
            sgVec3 position,
            DetonationType type = HighExplosiveDetonation,
            double timeToLive = 5.0,
            Transformable* parent = NULL
         );

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
         osg::ref_ptr<osg::Group> mGroup;

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
         osg::ref_ptr<osg::Node> mNode;

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
                    sgVec3 position,
                    DetonationType type,
                    Transformable* parent);
         
         /**
          * Retrieves the position of this detonation.
          *
          * @param result a vector to hold the result
          */
         void GetPosition(sgVec3 result);

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
         sgVec3 mPosition;

         /**
          * The type of the detonation.
          */
         DetonationType mType;
         
         /**
          * The optional parent of the transformation.
          */
         osg::ref_ptr<Transformable> mParent;
   };
};

#endif // DELTA_EFFECT_MANAGER
