// particlesystem.cpp: Implementation of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/particlesystem.h"
#include "dtCore/scene.h"
#include "dtCore/notify.h"

#include <osg/Group>

using namespace dtCore;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(ParticleSystem)


/**
 * A visitor class that finds matrix transforms and sets their
 * transforms to the given value.
 */
class TransformVisitor : public osg::NodeVisitor
{
   public:

      TransformVisitor(osg::Matrix matrix)
         : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN),
           mMatrix(matrix)
      {}

      virtual void apply(osg::MatrixTransform& node)
      {
         node.setMatrix(mMatrix);
      }


   private:

      osg::Matrix mMatrix;
};


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
 * A transformation callback.
 */
class TransformCallback : public osg::NodeCallback
{
   public:

      TransformCallback(ParticleSystem* particleSystem)
      {
         mParticleSystem = particleSystem;
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      { 
         Transform transform;

         mParticleSystem->GetTransform(&transform);
         
         sgMat4 psMatrix, eMatrix;

         if(mParticleSystem->IsParentRelative())
         {
            transform.Get(psMatrix);
            
            sgInvertMat4(eMatrix, psMatrix);
         }
         else
         {
            sgMakeIdentMat4(psMatrix);
            
            transform.Get(eMatrix);
         }
         
         osg::MatrixTransform* mt = mParticleSystem->GetMatrixNode();
            //(osg::MatrixTransform*)mParticleSystem->GetOSGNode();
         
         mt->setMatrix(
            osg::Matrix(
               psMatrix[0][0], psMatrix[0][1], psMatrix[0][2], psMatrix[0][3],
               psMatrix[1][0], psMatrix[1][1], psMatrix[1][2], psMatrix[1][3],
               psMatrix[2][0], psMatrix[2][1], psMatrix[2][2], psMatrix[2][3],
               psMatrix[3][0], psMatrix[3][1], psMatrix[3][2], psMatrix[3][3]
            )
         );
         
         for(unsigned int i=0;i<mt->getNumChildren();i++)
         {
            TransformVisitor tv = TransformVisitor(
               osg::Matrix(
                     eMatrix[0][0], eMatrix[0][1], eMatrix[0][2], eMatrix[0][3],
                     eMatrix[1][0], eMatrix[1][1], eMatrix[1][2], eMatrix[1][3],
                     eMatrix[2][0], eMatrix[2][1], eMatrix[2][2], eMatrix[2][3],
                     eMatrix[3][0], eMatrix[3][1], eMatrix[3][2], eMatrix[3][3]
               )
            );
            mt->getChild(i)->accept( tv );

            /*
            mt->getChild(i)->accept( 
               TransformVisitor( 
                  osg::Matrix( 
                     eMatrix[0][0], eMatrix[0][1], eMatrix[0][2], eMatrix[0][3],
                     eMatrix[1][0], eMatrix[1][1], eMatrix[1][2], eMatrix[1][3],
                     eMatrix[2][0], eMatrix[2][1], eMatrix[2][2], eMatrix[2][3],
                     eMatrix[3][0], eMatrix[3][1], eMatrix[3][2], eMatrix[3][3]
                  )
               )
            );
            */
         }
         
         traverse(node, nv);
      }
      
   private:
      
      ParticleSystem* mParticleSystem;
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
   
   //mNode = new osg::MatrixTransform;
   
   GetMatrixNode()->setUpdateCallback(
      new TransformCallback(this)
   );
}

/**
 * Destructor.
 */
ParticleSystem::~ParticleSystem()
{
   DeregisterInstance(this);
}


///Load a file from disk
osg::Node* ParticleSystem::LoadFile( std::string filename, bool useCache)
{
   osg::Node *node = NULL;
   node = Loadable::LoadFile(filename, useCache);

   if(node != NULL)
   {
      if(GetMatrixNode()->getNumChildren() > 0)
      {
         GetMatrixNode()->removeChild(0, GetMatrixNode()->getNumChildren());
      }

      GetMatrixNode()->addChild(node);

      ParticleSystemParameterVisitor pspv = ParticleSystemParameterVisitor( mEnabled );
      GetMatrixNode()->accept( pspv );
   }
   else
   {
      Notify(WARN, "ParticleSystem: Can't load %s", filename.c_str());
      return NULL;
   }
   return node;
}


/**
 * Loads a particle system from a file.
 *
 * @param filename the name of the file to load
 */
//bool ParticleSystem::LoadFile(std::string filename)
//{
//   mFilename = filename;
//   
//   osg::Node* node = osgDB::readNodeFile(filename);
//   
//   if(node != NULL)
//   {
//      if(GetMatrixNode()->getNumChildren() > 0)
//      {
//         GetMatrixNode()->removeChild(0, GetMatrixNode()->getNumChildren());
//      }
//      
//      GetMatrixNode()->addChild(node);
//
//      ParticleSystemParameterVisitor pspv = ParticleSystemParameterVisitor( mEnabled );
//      GetMatrixNode()->accept( pspv );
//   }
//   else
//   {
//      Notify(WARN, "ParticleSystem: Can't load %s", mFilename.c_str());
//      return false;
//   }
//   return true;
//}

/**
 * Returns the name of the last loaded file.
 *
 * @return the filename
 */
//std::string ParticleSystem::GetFilename()
//{
//   return mFilename;
//}

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
         
/**
 * Returns this object's OpenSceneGraph node.
 *
 * @return the OpenSceneGraph node
 */
//osg::Node* ParticleSystem::GetOSGNode()
//{
//   return mNode.get();
//}
