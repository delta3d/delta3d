// particlesystem.cpp: Implementation of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/particlesystem.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>

#include <osg/Group>
#include <osg/NodeVisitor>
#include <osg/Geode>
using namespace dtCore;
using namespace dtUtil;

#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/FluidProgram>

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
ParticleSystem::ParticleSystem(std::string name)
   : mEnabled(true),
     mParentRelative(false)
{
   SetName(name);
   
   RegisterInstance(this);

   // Default collision category = 6
   SetCollisionCategoryBits( UNSIGNED_BIT(6) );
}

/**
 * Destructor.
 */
ParticleSystem::~ParticleSystem()
{
   mLayers.empty();
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
         if ( psGeodeTransform* ps = dynamic_cast<psGeodeTransform*>( node ) )
         {
            osg::NodePath fullNodePath = nv->getNodePath();
  
            if( !fullNodePath.empty() )
            {
               fullNodePath.pop_back();

               // \TODO: This makes me feel nauseous... It would probably
               // be better to drop in a pointer to the CameraNode. This is the
               // only way I know how to get it.
               //
               // dtCore::Camera::GetSceneHandler()->GetSceneView()->getRenderStage()->getCameraNode()
               //
               //-osb
               if( std::string( fullNodePath[0]->className() ) == std::string("CameraNode") )
               {
                  fullNodePath = osg::NodePath( fullNodePath.begin()+1, fullNodePath.end() );
               }

               osg::Matrix localCoordMat = osg::computeLocalToWorld( fullNodePath );
               osg::Matrix inverseOfAccum = osg::Matrix::inverse( localCoordMat );
            
               ps->setMatrix( inverseOfAccum );
            }
         }
         traverse(node, nv); 
      }
   };

   psGeodeTransform() {setUpdateCallback( new psGeodeTransformCallback() );}

};

class findGeodeVisitor : public osg::NodeVisitor
{
public:
   findGeodeVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
   {
   }
   virtual void apply(osg::Geode &searchNode)
   {
       foundGeodeVector.push_back(&searchNode);

       traverse(searchNode);
   }

   const std::vector<osg::Geode*> getGeodeVector() {return foundGeodeVector;}

protected:
   std::vector<osg::Geode*> foundGeodeVector;

};


class particleSystemHelper : public osg::Group 
{ 
public: 
   particleSystemHelper(osg::Group* psGroup) : osg::Group(*psGroup) 
   { 
      findGeodeVisitor* fg = new findGeodeVisitor(); 
      accept(*fg); 
      std::vector<osg::Geode*> psGeodeVector = fg->getGeodeVector(); 

      psGeodeXForm = new psGeodeTransform();

      this->addChild(psGeodeXForm);

      for (std::vector<osg::Geode*>::iterator itr = psGeodeVector.begin();
         itr != psGeodeVector.end(); 
         ++itr)
      {
         this->removeChild( *itr );         
         psGeodeXForm->addChild( *itr ); 
      }

   } 

protected: 
   psGeodeTransform* psGeodeXForm; 
}; 


/** This method will load the particle effect from a file.  The loaded particle 
  * system will be broken apart, with the Emitter added to the parent 
  * MatrixTransform node and the geometry added directly to the Scene.
  *
  * @param filename : The file to load.  This will use the search paths to 
  *                   locate the file.
  * @param useCache : This param gets ignored and is forced to false
  */
osg::Node* ParticleSystem::LoadFile( const std::string& filename, bool useCache)
{
   osg::Node *node = NULL;
   node = Loadable::LoadFile(filename, false); //force it not to use cache

   if(node != NULL)
   {
      mLoadedFile = node;

      if(GetMatrixNode()->getNumChildren() > 0)
      {
         GetMatrixNode()->removeChild(0, GetMatrixNode()->getNumChildren());
      }

      particleSystemHelper *psh = new particleSystemHelper((osg::Group*)mLoadedFile.get());

      GetMatrixNode()->addChild(psh);

      ParticleSystemParameterVisitor pspv = ParticleSystemParameterVisitor( mEnabled );
      mLoadedFile->accept(pspv);
   }
   else
   {
      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FILE__, 
               "ParticleSystem: Can't load %s", filename.c_str());
      return NULL;
   }

   SetupParticleLayers();
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
   GetOSGNode()->accept( pspv );
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

//////////////////////////////////////////////////////////////////////////
// Particle Layer Code Below
//////////////////////////////////////////////////////////////////////////

/**
*  Called from LoadFile() function, should never be called
*  from user
*/
void ParticleSystem::SetupParticleLayers()
{
   // particle system group of the root note from the loaded particle system file.
   // will only be valid after you call load file on an object, thus its protected
   osg::Group*     newParticleSystemGroup  = static_cast<osg::Group*>(mLoadedFile.get());

   // node we are going to reuse over and over again to search through all the children of 
   // the osg root node
   osg::Node*      searchingNode           = NULL;
   
   // iterating through children var
   unsigned int    i                       = 0;

   //    Not everything has a name.... which sucks. usually only the geode
   //    we will bind the geode name to the whole struct with newly created var
   //    layer.mstrLayername
   //    Osg checks for the same particle system, instead of names
   for(i=0;i<newParticleSystemGroup->getNumChildren();i++)
   {
      searchingNode = newParticleSystemGroup->getChild(i);
      ParticleLayer layer;
      
      if(dynamic_cast<osgParticle::ParticleSystemUpdater*>(searchingNode)!= NULL)
      {
         // This is when you import multiple osg files in one system. This wont be done in
         // delta3d since it was set up where you can only load in 1 per system. Which
         // makes sense. 
         // Auto loads one for each file, to tell the system everything else is in here.

         // printf without a formatting string causes a warning on gcc. Why is this here
         // anyways??? -osb
         //printf("");
      }

      // See if this is the particle system of the geode
      osg::Geode *geode = dynamic_cast<osg::Geode*>(searchingNode);
      if(geode != NULL)
      {
         // well its a geometry node.
         layer.SetGeode(*geode);

         // see if the geometry node has drawables that are the particle system
         // we are looking for
         for(unsigned int j=0;j<layer.GetGeode().getNumDrawables();j++)
         {
            osg::Drawable* drawable    = layer.GetGeode().getDrawable(j);

            // seems like we found the particle system, continue on!
            osgParticle::ParticleSystem *psDrawable = dynamic_cast<osgParticle::ParticleSystem*>(drawable);
            if(psDrawable != NULL)
            {
               layer.SetParticleSystem(*psDrawable);
               layer.SetLayerName(layer.GetGeode().getName());
                  
               // We're done setting values up, push it onto the list
               mLayers.push_back(layer);
            }
         }
      }                       
   }
      
   // we do this in two seperate processes since the top particle system nodes and the cousins
   // could be in any order.
   for(i=0;i<newParticleSystemGroup->getNumChildren();i++)
   {
      searchingNode = newParticleSystemGroup->getChild(i);
      // Node can't be a matrix and a program
      // reason for if / else if
      if(dynamic_cast<osg::MatrixTransform*>(searchingNode)!= NULL)
      {
         // the transform in space
         osg::MatrixTransform* newEmitterTransform = static_cast<osg::MatrixTransform*>(searchingNode);

         for(unsigned int j=0;j<newEmitterTransform->getNumChildren();j++)
         {
            osg::Node* childNode = newEmitterTransform->getChild(j);

            if(dynamic_cast<osgParticle::ModularEmitter*>(childNode) != NULL)
            {
               osgParticle::ModularEmitter* newModularEmitter = static_cast<osgParticle::ModularEmitter*>(childNode);
               
               // Go through the populated particle system list and see where this
               // belongs too.
               for(std::list<ParticleLayer>::iterator layerIter = mLayers.begin(); 
                     layerIter != mLayers.end(); ++layerIter)
               {
                  // check for pointers, osg comparison
                  if(&layerIter->GetParticleSystem() == newModularEmitter->getParticleSystem())
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
      else if(dynamic_cast<osgParticle::ModularProgram*>(searchingNode)!= NULL)
      {
            osgParticle::ModularProgram* newModularProgram = static_cast<osgParticle::ModularProgram*>(searchingNode);
            // Go through the populated particle system list and see where this
            // belongs too.
            for(std::list<ParticleLayer>::iterator layerIter = mLayers.begin(); 
                     layerIter != mLayers.end(); ++layerIter)
            {
               // check for pointers, osg comparison
               if(&layerIter->GetParticleSystem() == newModularProgram->getParticleSystem())
               {
                  // set the data in our layer
                  layerIter->SetIsModularProgram(true);
                  layerIter->SetProgram(*newModularProgram);
                  break;
               }
            }
      }
      // check and see if this is a fluid program
      else if(dynamic_cast<osgParticle::FluidProgram*>(searchingNode)!= NULL)
      {
         osgParticle::FluidProgram* newFluidProgram = static_cast<osgParticle::FluidProgram*>(searchingNode);
         // Go through the populated particle system list and see where this
         // belongs too.
         for(std::list<ParticleLayer>::iterator layerIter = mLayers.begin(); 
            layerIter != mLayers.end(); ++layerIter)
         {
            // check for pointers, osg comparison
            if(&layerIter->GetParticleSystem() == newFluidProgram->getParticleSystem())
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

/**
* GetSingleLayer will return the Layer of said name,
* Null will return if bad name sent in.
*
* @return Will return the link list you requested by name
*/
ParticleLayer* ParticleSystem::GetSingleLayer(const std::string &layerName)
{
   for(std::list<ParticleLayer>::iterator pLayerIter = mLayers.begin();
       pLayerIter != mLayers.end(); ++pLayerIter)
   {
      // check if the name is what they want, send it back
      if(layerName == pLayerIter->GetLayerName())
         return &(*pLayerIter); 
   }
   return NULL;
}

/**
* SetAllLayers Will take in the new list of layers
* and set all the current layers to those of that 
* sent in
*/
void ParticleSystem::SetAllLayers(const std::list<ParticleLayer> &layersToSet)
{
   mLayers = layersToSet;
}

/**
* SetSingleLayer will take in the layerToSet 
* and set the layer in mlLayers to that sent in
*/
void ParticleSystem::SetSingleLayer(ParticleLayer& layerToSet)
{
   for(std::list<ParticleLayer>::iterator pLayerIter = mLayers.begin();
       pLayerIter != mLayers.end(); ++pLayerIter)
   {
      if(layerToSet.GetLayerName() == pLayerIter->GetLayerName())
      {
         //set the layer to what they want.
         *pLayerIter = layerToSet; 
         break;
      }
   }
}
