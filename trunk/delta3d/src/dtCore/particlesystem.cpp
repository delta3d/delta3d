// particlesystem.cpp: Implementation of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/particlesystem.h"
#include "dtCore/scene.h"
#include "dtCore/notify.h"

#include <osg/Group>
#include <osg/NodeVisitor>
#include <osgParticle/ModularEmitter>

using namespace dtCore;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(ParticleSystem)




/**
 * A visitor class that applies a set of particle system parameters.
 */
class ParticleSystemParameterVisitor : public osg::NodeVisitor
{
   public:

      ParticleSystemParameterVisitor(bool enabled)
         : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
           mEnabled(enabled)
      {}

      virtual void apply(osg::Node& node)
      {
         osg::Node* nodePtr = &node;

         if(osgParticle::Emitter* emitter = 
            dynamic_cast<osgParticle::Emitter*>(nodePtr))
         {          
            emitter->setEnabled(mEnabled);
         }
         
         traverse(node);
      }


   private:

      bool mEnabled;
};


/**
 * Constructor.
 *
 * @param name the instance name
 */
ParticleSystem::ParticleSystem(string name)
   : mEnabled(true),
     mParentRelative(false)
{
   SetName(name);
   
   RegisterInstance(this);
}

/**
 * Destructor.
 */
ParticleSystem::~ParticleSystem()
{
   Notify(DEBUG_INFO, "ParticleSystem: Destroying %s", GetName().c_str());
   DeregisterInstance(this);
}


///find the VariableRateCounter and RadialShooter and store the pointers
class ParticleVisitor : public osg::NodeVisitor
{
public:

   ParticleVisitor():
      osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
      {
      }

      virtual void apply(osg::Node& node)
      {
         osg::Node* nodePtr = &node;

         if(IS_A(nodePtr, osgParticle::ModularEmitter*))
         {
            osgParticle::ModularEmitter* me =
               (osgParticle::ModularEmitter*)nodePtr;
            
            emitter = me;

            if(IS_A(me->getCounter(), osgParticle::VariableRateCounter*))
            {
               vrc = (osgParticle::VariableRateCounter*)me->getCounter();             
            }

            if(IS_A(me->getShooter(), osgParticle::RadialShooter*))
            {
               rs = (osgParticle::RadialShooter*)me->getShooter();
            }            
         }

         traverse(node);
      }

      osgParticle::VariableRateCounter *vrc;
      osgParticle::RadialShooter* rs;
      osgParticle::ModularEmitter *emitter;
};


///Load a file from disk
osg::Node* ParticleSystem::LoadFile( std::string filename, bool useCache)
{
   osg::Node *node = NULL;
   node = Loadable::LoadFile(filename, useCache);

   if(node != NULL)
   {
      mLoadedFile = node;

      if(GetMatrixNode()->getNumChildren() > 0)
      {
         GetMatrixNode()->removeChild(0, GetMatrixNode()->getNumChildren());
      }

      ParticleVisitor pv;
      node->accept(pv);

      //Note: the Emitter is removed from the Particle System group
      //and added to the mNode (Transform) for repositioning.
      //The rest of the Particle System gets added to the Scene with *no*
      //transform nodes above it.

      //get the emitter
      osg::ref_ptr<osgParticle::ModularEmitter> em = pv.emitter;

      //remove it from it's current parent
      em.get()->getParent(0)->removeChild( em.get() );

      //add it as a child to mNode
      GetMatrixNode()->addChild( em.get() );
         
      //add the rest of the PS to the Scene
      if (mParentScene.valid())
      {
         mParentScene.get()->GetSceneNode()->addChild(mLoadedFile.get());
      }

      ParticleSystemParameterVisitor pspv = ParticleSystemParameterVisitor( mEnabled );
      mLoadedFile->accept(pspv);
   }
   else
   {
      Notify(WARN, "ParticleSystem: Can't load %s", filename.c_str());
      return NULL;
   }

   return node;
}



/**
 * Enables or disables this particle system.
 *
 * @param enable true to enable the particle system, false
 * to disable it
 */
void ParticleSystem::SetEnabled(bool enable)
{
   mEnabled = enable;

   ParticleSystemParameterVisitor pspv = ParticleSystemParameterVisitor( mEnabled );
   mNode->accept( pspv );
}

/**
 * Checks whether this particle system is enabled.
 *
 * @return true if the particle system is enabled, false
 * otherwise
 */
bool ParticleSystem::IsEnabled()
{
   return mEnabled;
}

/**
 * Sets the parent-relative state of this particle system.  If
 * parent-relative mode is enabled, the entire particle system
 * will be positioned relative to the parent.  If disabled, only
 * the emitter will be positioned relative to the parent.  By
 * default, particle systems are not parent-relative.
 *
 * @param parentRelative true to enable parent-relative mode,
 * false to disable it
 */
void ParticleSystem::SetParentRelative(bool parentRelative)
{
   mParentRelative = parentRelative;
}

/**
 * Returns the parent-relative state of this particle system.
 *
 * @return true if the particle system is in parent-relative mode,
 * false if not
 */
bool ParticleSystem::IsParentRelative()
{
   return mParentRelative;
}
         
void ParticleSystem::AddedToScene( Scene* scene )
{
   if (mParentScene.get() == scene) return;

   if (scene != NULL)
   {
      DeltaDrawable::AddedToScene(scene);
      mParentScene.get()->GetSceneNode()->addChild(mLoadedFile.get());
   }
   else
   {
      mParentScene.get()->GetSceneNode()->removeChild( mLoadedFile.get() );
      DeltaDrawable::AddedToScene(scene);
   }

}
