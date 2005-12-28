// particlesystem.cpp: Implementation of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/particlesystem.h"
#include "dtCore/scene.h"
#include <dtUtil/log.h>

#include <osg/Group>
#include <osg/NodeVisitor>
#include <osgParticle/ModularEmitter>
#include <osg/Geode>

using namespace dtCore;
using namespace dtUtil;
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
   DeregisterInstance(this);
}


class psGeodeTransform : public osg::MatrixTransform
{
public:
   class psGeodeTransformCallback : public osg::NodeCallback
   {
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
         itr++)
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
