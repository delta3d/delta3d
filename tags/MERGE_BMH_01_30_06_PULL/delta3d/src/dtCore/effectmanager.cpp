// effectmanager.cpp: Implementation of the EffectManager class.
//
//////////////////////////////////////////////////////////////////////
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Group>
#include <osg/NodeVisitor>

#include <osgDB/ReadFile>

#include <osgParticle/Emitter>
#include <osgParticle/Particle>

#include <dtCore/effectmanager.h>
#include <dtCore/transformable.h>
#include <dtCore/system.h>
#include <dtCore/scene.h>
#include <dtUtil/matrixutil.h>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(EffectManager)

/**
 * A visitor class that finds matrix transforms and sets their
 * positions to the given value.
 */
class PositionVisitor : public osg::NodeVisitor
{
   public:

      PositionVisitor(osg::Vec3 position)
         : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
           mPosition(position)
      {}

      virtual void apply(osg::MatrixTransform& node)
      {
         osg::Matrix matrix;

         matrix.makeTranslate(mPosition);

         node.setMatrix(matrix);
      }

   private:

      osg::Vec3 mPosition;
};


/**
 * A callback class that updates the state of a detonation.
 */
class DetonationUpdateCallback : public osg::NodeCallback
{
   public:
   
      DetonationUpdateCallback(Detonation* detonation)
         : mDetonation(detonation)
      {}
      
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         osg::Vec3 position;
         
         mDetonation->GetPosition(position);
         
         if(mDetonation->GetParent() != NULL)
         {
            Transform transform;
            
            mDetonation->GetParent()->GetTransform(&transform);
            
            osg::Matrix mat;
            transform.Get(mat);
            dtUtil::MatrixUtil::TransformVec3(position, mat);
         }

         PositionVisitor pv = PositionVisitor( osg::Vec3(position[0], position[1], position[2] ) );
         node->accept( pv );
         
         traverse(node, nv);
      }
      
   private:
   
      Detonation* mDetonation;
};


/**
 * Constructor.
 *
 * @param name the instance name
 */
EffectManager::EffectManager(const std::string& name) :
DeltaDrawable(name),
   mLastTime(0)
{
   RegisterInstance(this);
   
   mGroup = new osg::Group;

   AddSender(System::Instance());
}

/**
 * Destructor.
 */
EffectManager::~EffectManager()
{
   DeregisterInstance(this);
   RemoveSender( System::Instance() );
}

/**
 * Maps the specified detonation type to the given filename.
 *
 * @param detonationType the detonation type to map
 * @param filename the filename corresponding to the detonation type
 */
void EffectManager::AddDetonationTypeMapping(
   DetonationType detonationType,
   const std::string& filename)
{
   mDetonationTypeFilenameMap.insert( DetonationStringMap::value_type( detonationType, filename ) );
}

/**
 * Removes the mapping for the given detonation type.
 *
 * @param detonationType the detonation type to unmap
 */
void EffectManager::RemoveDetonationTypeMapping(
   DetonationType detonationType)
{
   mDetonationTypeFilenameMap.erase(detonationType);
}

/**
 * Returns the number of active effects.
 *
 * @return the number of active effects
 */
int EffectManager::GetEffectCount() const
{
   return mEffects.size();
}

/**
 * Returns the effect at the specified index.
 *
 * @param index the index of the effect to retrieve
 * @return the effect at the specified index
 */
Effect* EffectManager::GetEffect(int index) const
{
   return mEffects[index];
}

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
Detonation* EffectManager::AddDetonation(const osg::Vec3& position,
                                         DetonationType type,
                                         double timeToLive,
                                         Transformable* parent)
{
   if(mDetonationTypeFilenameMap.count(type) > 0)
   {
      RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
      options->setObjectCacheHint( osgDB::ReaderWriter::Options::CACHE_IMAGES );
      
      osg::Node* node = osgDB::readNodeFile( mDetonationTypeFilenameMap[type], options.get() );
      
      Detonation* detonation = new Detonation(node, timeToLive, position, type, parent);

      if(node != 0) 
      {
         if(parent != 0)
         {
            node->setUpdateCallback(
               new DetonationUpdateCallback(detonation)
            );
         }
         else
         {
            PositionVisitor pv = PositionVisitor( osg::Vec3( position[0], position[1], position[2] ) );
            node->accept( pv );
          }
      }
      
      AddEffect(detonation);

      return detonation;
   }

   return 0;
}

/**
 * Adds an effect to this manager.
 *
 * @param effect the effect to add
 */
void EffectManager::AddEffect(Effect* effect)
{
   mEffects.push_back(effect);

   if(effect->GetNode() != NULL)
   {
      mGroup->addChild(effect->GetNode());
   }
   
   for(std::set<EffectListener*>::iterator it = mEffectListeners.begin();
       it != mEffectListeners.end();
       it++)
   {
      (*it)->EffectAdded(this, effect);
   }
}

/**
 * Removes an effect from this manager.
 *
 * @param effect the effect to remove
 */
void EffectManager::RemoveEffect(Effect* effect)
{
   for(std::vector<Effect*>::iterator it = mEffects.begin();
       it != mEffects.end();
       it++)
   {
      if((*it) == effect)
      {
         mGroup->removeChild(effect->GetNode());

         mEffects.erase(it);

         for(std::set<EffectListener*>::iterator it2 = 
               mEffectListeners.begin();
             it2 != mEffectListeners.end();
             it2++)
         {
            (*it2)->EffectRemoved(this, effect);
         }

         delete effect;

         return;
      }
   }
}

/**
 * Adds a listener for effect events.
 *
 * @param effectListener the listener to add
 */
void EffectManager::AddEffectListener(EffectListener* effectListener)
{
   mEffectListeners.insert(effectListener);
}

/**
 * Removes a listener for effect events.
 *
 * @param effectListener the listener to remove
 */
void EffectManager::RemoveEffectListener(EffectListener* effectListener)
{
   mEffectListeners.erase(effectListener);
}

/**
 * Returns this object's OpenSceneGraph node.
 *
 * @return the OpenSceneGraph node
 */
osg::Node* EffectManager::GetOSGNode()
{
   return mGroup.get();
}

/**
 * Finds and returns the maximimum lifetime of the particles
 * whose systems lie under the specified node.
 *
 * @param effectNode the effect node to search
 * @return the maximum particle lifetime, or 0.0 if no
 * particle systems lie under the node
 */
static double FindMaximumParticleLifeTime(osg::Node* effectNode)
{
   double maximumLifeTime = 0.0;

   if(osg::Group* group = dynamic_cast<osg::Group*>(effectNode))
   {
      for(unsigned int i=0;i<group->getNumChildren();i++)
      {
         double lifeTime = FindMaximumParticleLifeTime(group->getChild(i));

         if(lifeTime > maximumLifeTime)
         {
            maximumLifeTime = lifeTime;
         }
      }
   }
   else if(osg::Geode* geode = dynamic_cast<osg::Geode*>(effectNode))
   {
      for(unsigned int i=0;i<geode->getNumDrawables();i++)
      {
         osg::Drawable* drawable = geode->getDrawable(i);

         if(osgParticle::ParticleSystem* particleSystem =
            dynamic_cast<osgParticle::ParticleSystem*>(drawable))
         {
            maximumLifeTime = particleSystem->getDefaultParticleTemplate().getLifeTime();
         }
      }
   }

   return maximumLifeTime;
}

/**
 * Deletes all particle emitters under the specified node.
 *
 * @param effectNode the effect node to modify
 */
static void DeleteParticleEmitters(osg::Node* effectNode)
{
   if(osg::Group* group = dynamic_cast<osg::Group*>(effectNode))
   {
      std::vector<osg::Node*> nodesToRemove;

      for(unsigned int i=0;i<group->getNumChildren();i++)
      {
         osg::Node* node = group->getChild(i);
         
         if(IS_A(node,osgParticle::Emitter*))  
         {
            nodesToRemove.push_back(node);
         }
         else if(IS_A(node, osg::Group*)) 
         {
            DeleteParticleEmitters(node);
         }
      }

      for(std::vector<osg::Node*>::iterator it = nodesToRemove.begin();
          it != nodesToRemove.end();
          it++)
      {
         group->removeChild(*it);
      }
   }
}

/**
 * Processes a received message.
 *
 * @param data the message structure
 */
void EffectManager::OnMessage(MessageData *data)
{
   if(data->message == "preframe")
   {
      double delta = *static_cast<double*>(data->userData);

      if(mLastTime != 0)
      {
         std::set<Effect*> effectsToRemove;

         for(std::vector<Effect*>::iterator it = mEffects.begin();
             it != mEffects.end();
             it++)
         {
            double ttl = (*it)->GetTimeToLive();
            
            if(ttl != 0.0)
            {
               ttl -= delta;

               if(ttl <= 0.0)
               {
                  if((*it)->IsDying())
                  {
                     effectsToRemove.insert(*it);
                  }
                  else
                  {
                     double maxLifeTime = 
                        FindMaximumParticleLifeTime(
                           (*it)->GetNode()
                        );

                     if(maxLifeTime == 0.0)
                     {
                        effectsToRemove.insert(*it);
                     }
                     else
                     {
                        DeleteParticleEmitters(
                           (*it)->GetNode()
                        );

                        (*it)->SetDying(true);

                        (*it)->SetTimeToLive(maxLifeTime);
                     }
                  }
               }
               else
               {
                  (*it)->SetTimeToLive(ttl);
               }
            }
         }

         for(std::set<Effect*>::iterator it2 = effectsToRemove.begin();
             it2 != effectsToRemove.end();
             it2++)
         {
            RemoveEffect(*it2);
         }
       }

       mLastTime += delta;
   }
}

/**
 * Constructor.
 *
 * @param node the effect's OpenSceneGraph node
 * @param timeToLive the lifespan of the effect in
 * seconds, or 0.0 for unlimited
 */
Effect::Effect(osg::Node* node, double timeToLive)
   : mNode(node),
     mTimeToLive(timeToLive),
     mDying(false)
{}

/**
 * Destructor.
 */
Effect::~Effect()
{}

/**
 * Returns the effect's OpenSceneGraph node.
 *
 * @return the effect's OpenSceneGraph node
 */
osg::Node* Effect::GetNode()
{
   return mNode.get();
}

/** 
 * Sets the remaining lifespan of this effect.
 *
 * @param timeToLive the remaining lifespan, in seconds,
 * or 0.0 for unlimited
 */
void Effect::SetTimeToLive(double timeToLive)
{
   mTimeToLive = timeToLive;
}

/**
 * Returns the remaining lifespan of this effect.
 *
 * @return the remaining lifespan, in seconds, or 0.0
 * for unlimited
 */
double Effect::GetTimeToLive()
{
   return mTimeToLive;
}

/**
 * Sets the dying flag.
 *
 * @param dying the new value of the dying flag
 */
void Effect::SetDying(bool dying)
{
   mDying = dying;
}

/**
 * Checks the dying flag.
 *
 * @return the value of the dying flag
 */
bool Effect::IsDying()
{
   return mDying;
}

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
Detonation::Detonation(osg::Node* node,
                       double timeToLive,
                       const osg::Vec3& position,
                       DetonationType type,
                       Transformable* parent)
   : Effect(node, timeToLive),
     mType(type),
     mParent(parent)
{
   mPosition = position;
}

/**
 * Retrieves the position of this detonation.
 *
 * @param result a vector to hold the result
 */
void Detonation::GetPosition(osg::Vec3& result)
{
   result[0] = mPosition[0];
   result[1] = mPosition[1];
   result[2] = mPosition[2];
}

/**
 * Returns the type of this detonation.
 *
 * @return the type of this detonation
 */
DetonationType Detonation::GetType()
{
   return mType;
}

/**
 * Returns the Transformable parent of the detonation, or
 * NULL if the detonation is unparented.
 *
 * @return the parent of the detonation
 */
Transformable* Detonation::GetParent()
{
   return mParent.get();
}
