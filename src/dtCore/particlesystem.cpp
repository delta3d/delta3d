// particlesystem.cpp: Implementation of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <prefix/dtcoreprefix.h>
#include <dtCore/observerptr.h>
#include <dtCore/particlesystem.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtUtil/nodemask.h>

#include <osg/Group>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/MatrixTransform>

#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/FluidProgram>

#include <osgParticle/ParticleProcessor>
#include <osgParticle/ParticleSystemUpdater>

#include <osg/Version> // For #ifdef

using namespace dtCore;
using namespace dtUtil;

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

      if (osgParticle::Emitter* emitter =
         dynamic_cast<osgParticle::Emitter*>(nodePtr))
      {
         emitter->setEnabled(mEnabled);
      }

      traverse(node);
   }


private:

   bool mEnabled;
};


////////////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem(std::string name)
   : BaseClass(name)
   , mEnabled(true)
   , mParentRelative(false)
{
   SetName(name);

   RegisterInstance(this);

   dtCore::System::GetInstance().TickSignal.connect_slot(this, &ParticleSystem::OnSystem);
}

////////////////////////////////////////////////////////////////////////////////
ParticleSystem::~ParticleSystem()
{
   mLayers.clear();

   DeregisterInstance(this);
}


class psGeodeTransform : public osg::MatrixTransform
{
public:
   class psGeodeTransformCallback : public osg::NodeCallback
   {
      //\NOTE: This is triggered once per camera which may
      // be inefficient, we may want to look into this later
      // to ensure this happens once per frame,
      // instead of once per camera per frame
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         if (psGeodeTransform* ps = dynamic_cast<psGeodeTransform*>(node))
         {
            osg::NodePath fullNodePath = nv->getNodePath();

            if (!fullNodePath.empty())
            {
               fullNodePath.pop_back();

               osg::Matrix localCoordMat = osg::computeLocalToWorld( fullNodePath );
               osg::Matrix inverseOfAccum = osg::Matrix::inverse( localCoordMat );

               ps->setMatrix( inverseOfAccum );
            }
         }
         traverse(node, nv);
      }
   };

   psGeodeTransform() { setUpdateCallback(new psGeodeTransformCallback()); }

};

class findGeodeVisitor : public osg::NodeVisitor
{
public:
   findGeodeVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
   {
   }
   virtual void apply(osg::Geode& searchNode)
   {
       foundGeodeVector.push_back(&searchNode);

       traverse(searchNode);
   }

   const std::vector<osg::Geode*> getGeodeVector() { return foundGeodeVector; }

protected:
   std::vector<osg::Geode*> foundGeodeVector;

};


class particleSystemHelper : public osg::Group
{
public:
   particleSystemHelper(osg::Group* psGroup) : osg::Group(*psGroup)
   {
      osg::ref_ptr<findGeodeVisitor> geodeFinder = new findGeodeVisitor();
      accept(*geodeFinder);
      std::vector<osg::Geode*> psGeodeVector = geodeFinder->getGeodeVector();

      // Use a transform that will allow a particle to be
      // translated to world space from the group's local space.
      mGeodeTransform = new psGeodeTransform();

      this->addChild(mGeodeTransform.get());

      for (std::vector<osg::Geode*>::iterator itr = psGeodeVector.begin();
         itr != psGeodeVector.end();
         ++itr)
      {
         // Pull particle out of group and place it into world space.
         this->removeChild(*itr);
         mGeodeTransform->addChild(*itr);
      }
   }

protected:
   osg::observer_ptr<psGeodeTransform> mGeodeTransform;
};

////////////////////////////////////////////////////////////////////////////////
ParticleLayer::ParticleLayer() : mProgTypeIsModular(false) {}

////////////////////////////////////////////////////////////////////////////////
ParticleLayer::~ParticleLayer(){}

////////////////////////////////////////////////////////////////////////////////
ParticleLayer::ParticleLayer(const ParticleLayer &copyLayer)
{
   mGeode             = copyLayer.mGeode;
   mParticleSystem    = copyLayer.mParticleSystem;
   mEmitterTransform  = copyLayer.mEmitterTransform;
   mModularEmitter    = copyLayer.mModularEmitter;
   mProgram           = copyLayer.mProgram;
   mLayerName         = copyLayer.mLayerName;
   mProgTypeIsModular = copyLayer.mProgTypeIsModular;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleLayer::SetGeode(osg::Geode& geode)
{
   mGeode = &geode;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleLayer::SetParticleSystem(osgParticle::ParticleSystem& particleSystem)
{
   mParticleSystem = &particleSystem;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleLayer::SetEmitterTransform(osg::MatrixTransform& matrixtransform)
{
   mEmitterTransform = &matrixtransform;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleLayer::SetModularEmitter(osgParticle::ModularEmitter& modularEmitter)
{
   mModularEmitter = &modularEmitter;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleLayer::SetProgram(osgParticle::Program& program)
{
   mProgram = &program;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleLayer::SetLayerName(const std::string& name)
{
   mLayerName = name;
}

////////////////////////////////////////////////////////////////////////////////
osg::Geode& ParticleLayer::GetGeode()
{
   return *mGeode;
}

////////////////////////////////////////////////////////////////////////////////
const osg::Geode& ParticleLayer::GetGeode() const
{
   return *mGeode;
}

////////////////////////////////////////////////////////////////////////////////
osgParticle::ParticleSystem& ParticleLayer::GetParticleSystem()
{
   return *mParticleSystem;
}

////////////////////////////////////////////////////////////////////////////////
const osgParticle::ParticleSystem& ParticleLayer::GetParticleSystem() const
{
   return *mParticleSystem;
}

////////////////////////////////////////////////////////////////////////////////
osg::MatrixTransform& ParticleLayer::GetEmitterTransform()
{
   return *mEmitterTransform;
}

////////////////////////////////////////////////////////////////////////////////
const osg::MatrixTransform& ParticleLayer::GetEmitterTransform() const
{
   return *mEmitterTransform;
}

////////////////////////////////////////////////////////////////////////////////
osgParticle::ModularEmitter& ParticleLayer::GetModularEmitter()
{
   return *mModularEmitter;
}

////////////////////////////////////////////////////////////////////////////////
const osgParticle::ModularEmitter& ParticleLayer::GetModularEmitter() const
{
   return *mModularEmitter;
}

////////////////////////////////////////////////////////////////////////////////
osgParticle::Program& ParticleLayer::GetProgram()
{
   return *mProgram;
}

////////////////////////////////////////////////////////////////////////////////
const osgParticle::Program& ParticleLayer::GetProgram() const
{
   return *mProgram;
}

////////////////////////////////////////////////////////////////////////////////
bool ParticleLayer::operator==(const ParticleLayer& testLayer) const
{
   return testLayer.mLayerName == mLayerName;
}

///////////////////////////////////////////////////////////////////////////////
void ParticleSystem::OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)

{
   if (str == dtCore::System::MESSAGE_PAUSE_START)
   {
      OnPause();
   }
   else if (str == dtCore::System::MESSAGE_PAUSE_END)
   {
      OnUnpause();
   }
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* ParticleSystem::LoadFile( const std::string& filename, bool useCache)
{
   //First, cleanup
   if (GetMatrixNode()->getNumChildren() > 0)
   {
      GetMatrixNode()->removeChild(0, GetMatrixNode()->getNumChildren());
   }

   mLoadedFile = NULL;
   mOriginalLoadedParticleSystem = NULL;
   mParticleSystemUpdater = NULL;

   //set the default node mask for particles
   GetMatrixNode()->setNodeMask(dtUtil::NodeMask::TRANSPARENT_EFFECTS);

   dtCore::RefPtr<osg::Node> node = Loadable::LoadFile(filename, useCache); //force it to use cache

   if (node.valid())
   {
      if (useCache)
      {
         //This is weird, but DON'T copy the drawables yet.  It must be don't later...
         static const unsigned int COPY_OPS_SHARED_GEOMETRY =
              osg::CopyOp::DEEP_COPY_OBJECTS
            | osg::CopyOp::DEEP_COPY_NODES
            | osg::CopyOp::DEEP_COPY_STATESETS
            | osg::CopyOp::DEEP_COPY_STATEATTRIBUTES
            | osg::CopyOp::DEEP_COPY_UNIFORMS;

         dtCore::RefPtr<osg::Node> copy = static_cast<osg::Node*>(node->clone(COPY_OPS_SHARED_GEOMETRY));

         mLoadedFile = copy;
         mOriginalLoadedParticleSystem = node;
      }
      else
      {
         mLoadedFile = node;
      }

      // Set up all the particle layers
      ParseParticleLayers(*mLoadedFile, mLayers, mParticleSystemUpdater);

      if (useCache)
      {
         // Now clone the drawables that weren't cloned in the clone before and fix
         // all the crosslinks.
         CloneParticleSystemDrawables();
      }

      if (mParentRelative)
      {
         // Attach the particle system tree directly to the transform node.
         GetMatrixNode()->addChild(mLoadedFile.get());

         //Presumably, osg 2.8.0 and greater doesn't need to do this.
         #if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR == 2 && OSG_VERSION_MINOR < 8
         // Modify the reference frame of both the emitter and program
         // for each particle layer.
         for (LayerList::iterator pLayerIter = mLayers.begin();
            pLayerIter != mLayers.end(); ++pLayerIter)
         {
            // Set the emitter and program to run from the origin
            pLayerIter->GetProgram().setReferenceFrame(osgParticle::ParticleProcessor::ABSOLUTE_RF);
            pLayerIter->GetModularEmitter().setReferenceFrame(osgParticle::ParticleProcessor::ABSOLUTE_RF);
         }
         #endif

      }
      else // Emit particles into world space
      {
         particleSystemHelper* psh = new particleSystemHelper(static_cast<osg::Group*>(mLoadedFile.get()));

         GetMatrixNode()->addChild(psh);
      }

      // Enable/disable particle system
      ParticleSystemParameterVisitor pspv = ParticleSystemParameterVisitor(mEnabled);
      mLoadedFile->accept(pspv);

      ResetTime();

      // Re-apply a shader if one had been specified.
      if ( ! BaseClass::GetShaderGroup().empty())
      {
         BaseClass::ApplyShaderGroup();
      }
   }
   else
   {
      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FILE__,
               "ParticleSystem: Can't load %s", filename.c_str());
      return NULL;
   }

   return node.get();
}


////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::SetEnabled(bool enable)
{
   mEnabled = enable;

   ParticleSystemParameterVisitor pspv = ParticleSystemParameterVisitor(mEnabled);
   GetOSGNode()->accept(pspv);
}

////////////////////////////////////////////////////////////////////////////////
bool ParticleSystem::IsEnabled() const
{
   return mEnabled;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::SetParentRelative(bool parentRelative)
{
   mParentRelative = parentRelative;
}

////////////////////////////////////////////////////////////////////////////////
bool ParticleSystem::IsParentRelative() const
{
   return mParentRelative;
}

//////////////////////////////////////////////////////////////////////////
// Particle Layer Code Below
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
osg::Node* ParticleSystem::GetLoadedParticleSystemRoot()
{
   return mLoadedFile.get();
}

////////////////////////////////////////////////////////////////////////////////
const osg::Node* ParticleSystem::GetLoadedParticleSystemRoot() const
{
   return mLoadedFile.get();
}

////////////////////////////////////////////////////////////////////////////////
osg::Node* ParticleSystem::GetCachedOriginalParticleSystemRoot()
{
   return mOriginalLoadedParticleSystem.get();
}

////////////////////////////////////////////////////////////////////////////////
const osg::Node* ParticleSystem::GetCachedOriginalParticleSystemRoot() const
{
   return mOriginalLoadedParticleSystem.get();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::ParseParticleLayers(osg::Node& ps, LayerList& layers, dtCore::RefPtr<osgParticle::ParticleSystemUpdater>& particleSystemUpdater)
{
   // particle system group of the root note from the loaded particle system file.
   // will only be valid after you call load file on an object, thus its protected
   osg::Group* newParticleSystemGroup = static_cast<osg::Group*>(&ps);

   // node we are going to reuse over and over again to search through all the children of
   // the osg root node
   osg::Node* searchingNode = NULL;

   // iterating through children var
   unsigned int i = 0;

   // clear the list in case it is loaded twice.
   layers.clear();

   // This should already be NULL, but just in case
   particleSystemUpdater = NULL;

   //    Not everything has a name.... which sucks. usually only the geode
   //    we will bind the geode name to the whole struct with newly created var
   //    layer.mstrLayername
   //    Osg checks for the same particle system, instead of names
   for (i = 0; i < newParticleSystemGroup->getNumChildren(); i++)
   {
      searchingNode = newParticleSystemGroup->getChild(i);
      ParticleLayer layer;

      if (dynamic_cast<osgParticle::ParticleSystemUpdater*>(searchingNode)!= NULL)
      {
         if (particleSystemUpdater.valid())
         {
            LOG_ERROR("Found a second particle system updater.  This is not supported.")
         }
         particleSystemUpdater = static_cast<osgParticle::ParticleSystemUpdater*>(searchingNode);
      }

      // See if this is the particle system of the geode
      osg::Geode* geode = dynamic_cast<osg::Geode*>(searchingNode);
      if (geode != NULL)
      {
         // well its a geometry node.
         layer.SetGeode(*geode);

         // see if the geometry node has drawables that are the particle system
         // we are looking for
         for (unsigned int j = 0; j < layer.GetGeode().getNumDrawables(); j++)
         {
            osg::Drawable* drawable    = layer.GetGeode().getDrawable(j);

            // seems like we found the particle system, continue on!
            osgParticle::ParticleSystem* psDrawable = dynamic_cast<osgParticle::ParticleSystem*>(drawable);
            if (psDrawable != NULL)
            {
               layer.SetParticleSystem(*psDrawable);
               layer.SetLayerName(layer.GetGeode().getName());

               // We're done setting values up, push it onto the list
               layers.push_back(layer);
            }
         }
      }
   }

   // we do this in two separate processes since the top particle system nodes and the cousins
   // could be in any order.
   for (i = 0; i<newParticleSystemGroup->getNumChildren(); i++)
   {
      searchingNode = newParticleSystemGroup->getChild(i);
      // Node can't be a matrix and a program
      // reason for if / else if
      if (dynamic_cast<osg::MatrixTransform*>(searchingNode)!= NULL)
      {
         // the transform in space
         osg::MatrixTransform* newEmitterTransform = static_cast<osg::MatrixTransform*>(searchingNode);

         for (unsigned int j=0;j<newEmitterTransform->getNumChildren();j++)
         {
            osg::Node* childNode = newEmitterTransform->getChild(j);

            if (dynamic_cast<osgParticle::ModularEmitter*>(childNode) != NULL)
            {
               osgParticle::ModularEmitter* newModularEmitter = static_cast<osgParticle::ModularEmitter*>(childNode);

               // Go through the populated particle system list and see where this
               // belongs too.
               for (LayerList::iterator layerIter = layers.begin();
                     layerIter != layers.end(); ++layerIter)
               {
                  // check for pointers, osg comparison
                  if (&layerIter->GetParticleSystem() == newModularEmitter->getParticleSystem())
                  {
                     // set the data in our layer
                     layerIter->SetEmitterTransform(*newEmitterTransform);
                     layerIter->SetModularEmitter(*newModularEmitter);
                  }
               }
            }
         }
      }
      // particle cant be a fluid and modular program
      // reason for else if/ else if
      else if (dynamic_cast<osgParticle::ModularProgram*>(searchingNode) != NULL)
      {
            osgParticle::ModularProgram* newModularProgram = static_cast<osgParticle::ModularProgram*>(searchingNode);
            // Go through the populated particle system list and see where this
            // belongs too.
            for (LayerList::iterator layerIter = layers.begin();
                 layerIter != layers.end();
                 ++layerIter)
            {
               // check for pointers, osg comparison
               if (&layerIter->GetParticleSystem() == newModularProgram->getParticleSystem())
               {
                  // set the data in our layer
                  layerIter->SetIsModularProgram(true);
                  layerIter->SetProgram(*newModularProgram);

                  break;
               }
            }
      }
      // check and see if this is a fluid program
      else if (dynamic_cast<osgParticle::FluidProgram*>(searchingNode)!= NULL)
      {
         osgParticle::FluidProgram* newFluidProgram = static_cast<osgParticle::FluidProgram*>(searchingNode);
         // Go through the populated particle system list and see where this
         // belongs too.
         for (LayerList::iterator layerIter = layers.begin();
            layerIter != layers.end(); ++layerIter)
         {
            // check for pointers, osg comparison
            if (&layerIter->GetParticleSystem() == newFluidProgram->getParticleSystem())
            {
               // set the data in our layer
               layerIter->SetIsFluidProgram(true);
               layerIter->SetProgram(*newFluidProgram);
               break;
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::CloneParticleSystemDrawables()
{
   LayerList::iterator nextLayerIt;

   static const unsigned int COPY_OPS_DRAWABLE = (osg::CopyOp::DEEP_COPY_OBJECTS
      | osg::CopyOp::DEEP_COPY_NODES
      | osg::CopyOp::DEEP_COPY_STATESETS
      | osg::CopyOp::DEEP_COPY_STATEATTRIBUTES
      | osg::CopyOp::DEEP_COPY_UNIFORMS
      | osg::CopyOp::DEEP_COPY_DRAWABLES);

   if (!mParticleSystemUpdater.valid())
   {
      LOG_ERROR("No instance of osgParticle::ParticleSystemUpdater was found.  "
               "The particle system cloning will not work.  Current file :\"" + GetFilename() + "\".");
      return;
   }

   dtCore::RefPtr<osgParticle::ParticleSystemUpdater> newParticleSystemUpdater = new osgParticle::ParticleSystemUpdater;
   newParticleSystemUpdater->setNodeMask(mParticleSystemUpdater->getNodeMask());
   newParticleSystemUpdater->setName(mParticleSystemUpdater->getName());
   newParticleSystemUpdater->setDescriptions(mParticleSystemUpdater->getDescriptions());
   newParticleSystemUpdater->setUserData(mParticleSystemUpdater->getUserData());
   newParticleSystemUpdater->setUpdateCallback(mParticleSystemUpdater->getUpdateCallback());
   newParticleSystemUpdater->setCullCallback(mParticleSystemUpdater->getCullCallback());
   newParticleSystemUpdater->setCullingActive(mParticleSystemUpdater->getCullingActive());
   newParticleSystemUpdater->setDataVariance(mParticleSystemUpdater->getDataVariance());
   newParticleSystemUpdater->setEventCallback(mParticleSystemUpdater->getEventCallback());

   for(nextLayerIt = mLayers.begin(); nextLayerIt != mLayers.end(); ++nextLayerIt)
   {
      ParticleLayer& layer = *nextLayerIt;

      dtCore::RefPtr<osgParticle::ParticleSystem> newPSDrawable =
         static_cast<osgParticle::ParticleSystem*>(layer.GetParticleSystem().clone(COPY_OPS_DRAWABLE));

      mParticleSystemUpdater->removeParticleSystem(&layer.GetParticleSystem());
      newParticleSystemUpdater->addParticleSystem(newPSDrawable.get());

      layer.GetGeode().removeDrawable(&layer.GetParticleSystem());
      layer.GetGeode().addDrawable(newPSDrawable.get());
      layer.SetParticleSystem(*newPSDrawable);
      layer.GetModularEmitter().setParticleSystem(newPSDrawable.get());
      layer.GetProgram().setParticleSystem(newPSDrawable.get());
   }

   mParticleSystemUpdater->getParent(0)->addChild(newParticleSystemUpdater.get());
   mParticleSystemUpdater->getParent(0)->removeChild(mParticleSystemUpdater.get());

   mParticleSystemUpdater = newParticleSystemUpdater;
}

///////////////////////////////////////////////////////////////////////////////
void ParticleSystem::OnPause()
{
   if (osgParticle::ParticleSystemUpdater* psu = GetParticleSystemUpdater())
   {
      for (unsigned int i = 0; i < psu->getNumParticleSystems(); ++i)
      {
         if (osgParticle::ParticleSystem* ps = psu->getParticleSystem(i))
         {
            // Save the previous frozen state of the ParticleSystem, so subsequent attempts
            // to unfreeze it will bring it back to the way it was.
            mWasFrozenBeforePauseMap.insert(ParticleSystemBoolMap::value_type(ps, ps->isFrozen()));

            // Allow me to break the ice. My name is Freeze. Learn it well.
            // For it's the chilling sound of your doom. -Mr. Freeze
            ps->setFrozen(true);
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
void ParticleSystem::OnUnpause()
{
   if (osgParticle::ParticleSystemUpdater* psu = GetParticleSystemUpdater())
   {
      for (unsigned int i = 0; i < psu->getNumParticleSystems(); ++i)
      {
         if (osgParticle::ParticleSystem* ps = psu->getParticleSystem(i))
         {
            // If there was a previous state restore it
            if (mWasFrozenBeforePauseMap.find(ps) != mWasFrozenBeforePauseMap.end())
            {
               ps->setFrozen(mWasFrozenBeforePauseMap[ps]);
            }
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::ResetTime()
{
   LayerList::iterator i, iend;
   i = mLayers.begin();
   iend = mLayers.end();
   for (; i != iend; ++i)
   {
      ParticleLayer& layer = *i;
      layer.GetProgram().setCurrentTime(0.0);
      layer.GetModularEmitter().setCurrentTime(0.0);
   }
}

////////////////////////////////////////////////////////////////////////////////
ParticleLayer* ParticleSystem::GetSingleLayer(const std::string& layerName)
{
   for (LayerList::iterator pLayerIter = mLayers.begin();
       pLayerIter != mLayers.end(); ++pLayerIter)
   {
      // check if the name is what they want, send it back
      if (layerName == pLayerIter->GetLayerName())
         return &(*pLayerIter);
   }
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
const ParticleLayer* ParticleSystem::GetSingleLayer(const std::string &layerName) const
{
   for (LayerList::const_iterator pLayerIter = mLayers.begin();
      pLayerIter != mLayers.end();
      ++pLayerIter)
   {
      // check if the name is what they want, send it back
      if (layerName == pLayerIter->GetLayerName())
      {
         return &(*pLayerIter);
      }
   }
   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
osgParticle::ParticleSystemUpdater* ParticleSystem::GetParticleSystemUpdater()
{
   return mParticleSystemUpdater.get();
}

////////////////////////////////////////////////////////////////////////////////
const osgParticle::ParticleSystemUpdater* ParticleSystem::GetParticleSystemUpdater() const
{
   return mParticleSystemUpdater.get();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::SetAllLayers(const LayerList& layersToSet)
{
   mLayers = layersToSet;
}

////////////////////////////////////////////////////////////////////////////////
void ParticleSystem::SetSingleLayer(ParticleLayer& layerToSet)
{
   for (LayerList::iterator pLayerIter = mLayers.begin();
       pLayerIter != mLayers.end(); ++pLayerIter)
   {
      if (layerToSet.GetLayerName() == pLayerIter->GetLayerName())
      {
         //set the layer to what they want.
         *pLayerIter = layerToSet;
         break;
      }
   }
}
