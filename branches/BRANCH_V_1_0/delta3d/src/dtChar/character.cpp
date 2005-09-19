// character.cpp: Implementation of the Character class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include <dtCore/scene.h>
#include <dtChar/character.h>
#include <dtUtil/log.h>
#include "osgDB/FileUtils"

using namespace dtCore;
using namespace dtChar;
using namespace dtUtil;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(Character)

/**
 * Constructor.
 *
 * @param name the instance name
 */
 Character::Character(const std::string& name)
   :  mRotation(0.0f),
      mVelocity(0.0f)
{
   SetName(name);
   mBodyNode = new rbody::OsgBodyNode(false);
   
   RegisterInstance(this);
}

/**
 * Destructor.
 */
Character::~Character()
{
   DeregisterInstance(this);
}

/**
 * Notifies this drawable object that it has been added to
 * a scene.
 *
 * @param scene the scene to which this drawable object has
 * been added
 */
void Character::AddedToScene(Scene* scene)
{
   if(mBodyNode.valid() && scene)
   {
      mCollisionRootNode = scene->GetSceneNode();
      mBodyNode->setCollisionRootNode(mCollisionRootNode.get());    
   }
}

/**
 * Loads a ReplicantBody data file for this character.
 *
 * @param filename the name of the file to load
 */
osg::Node* Character::LoadFile(const std::string& filename, bool useCache)
{
   mFilename = filename;
   
   string path = osgDB::findDataFile(mFilename);
   
   if(path.empty())
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__,
         "Character: Can't find %s", mFilename.c_str());
      return false;
   }
   else
   {
      CalLoader::setLoadingMode(LOADER_INVERT_V_COORD);

      // Prevents ReplicantBody from complaining
      putenv("REPLICANTBODY_FILE_PATH=."); 
      
      // Find a unique name for the instance
      string name = GetName();
      
      for(int i = 2;
          rbody::ReplicantBodyMgr::instance()->findCharacter(name) != NULL;
          i++)
      {
         std::ostringstream buf;

         buf << GetName() << " " << i;
         
         name = buf.str();
      }
      
      osg::Matrix mat;
      if (mBodyNode.valid())
      {
         mat = mBodyNode->getMatrix();
        mBodyNode = NULL;
      }

      mBodyNode = rbody::ReplicantBodyMgr::instance()->createCharacter(
         path,
         name,
         mCollisionRootNode.get()
      );

      //HACK: Somehow fixes bug where multi-textures models drop their textures when
      //      a character is in the scene. Weird. Please someone fix this fo' real!
      for( unsigned int i = 0; i < mBodyNode->getNumChildren(); i++ )
      {
         osg::MatrixTransform* scale = dynamic_cast<osg::MatrixTransform*>( mBodyNode->getChild(i) );

         for( unsigned int j = 0; j < scale->getNumChildren(); j++ )
         {
            osg::Geode* geode = dynamic_cast<osg::Geode*>( scale->getChild(j) );

            for( unsigned int k = 0; k < geode->getNumDrawables(); k++ )
            {
               osg::Drawable* drawable = dynamic_cast<osg::Drawable*>( geode->getDrawable(k) );

               osg::StateSet* stateSet = drawable->getOrCreateStateSet();
               stateSet->setMode(GL_TEXTURE_COORD_ARRAY, osg::StateAttribute::OFF);
            }
         }
      }

      mBodyNode->setMatrix(mat);

      mBodyNode->setUpdateMode(  rbody::OsgBodyNode::UPDATE_ANIMATION | 
                                 rbody::OsgBodyNode::UPDATE_CONTACT_TRANSLATION | 
                                 rbody::OsgBodyNode::UPDATE_GROUND_FOLLOWING );
      
      if(mBodyNode.get() == NULL)
      {
         Log::GetInstance().LogMessage(Log::LOG_WARNING, __FUNCTION__, 
             "Character: Can't load %s", mFilename.c_str());
      }
      else
      {
         mBodyNode->setRotation(osg::DegreesToRadians(mRotation));
         
         SetVelocity(mVelocity);
         
         dynamic_cast<osg::Group*>(mNode.get())->addChild(mBodyNode.get());
      }
   }
   return mBodyNode.get();
}

/**
 * Sets the rotation of this character.
 *
 * @param rotation the new rotation, in degrees about the Z
 * axis
 */
void Character::SetRotation(float rotation)
{
   if(mBodyNode.get() != NULL)
   {
      mBodyNode->setRotation(
         osg::DegreesToRadians(rotation - mRotation)
         );
   }
   
   // Normalize
   while(rotation < 0.0f) rotation += 360.0f;
   while(rotation > 360.0f) rotation -= 360.0f;
   
   mRotation = rotation;
}

/**
 * Returns the current rotation of this character.
 *
 * @return the current rotation, in degrees about the Z
 * axis
 */
float Character::GetRotation() const
{
   return mRotation;
}

/**
 * Sets the walk/run velocity of this character.
 *
 * @param velocity the new walk/run velocity
 */
void Character::SetVelocity(float velocity)
{
   if(mVelocity != velocity)
   {
      mVelocity = velocity;
   
      float walkSpeed;
      
      rbody::ActionRequest* walk = 
         mBodyNode->getBody()->getActionPrototype("ACT_WALK");
         
      walkSpeed = walk->getPropertyFloat("speed");
      
      rbody::ActionRequest* action;
      
      if(mVelocity > walkSpeed)
      {
         action = mBodyNode->getBody()->getActionPrototype("ACT_RUN");
      }
      else if(mVelocity > 0.0f)
      {
         action = walk;
      }
      else
      {
         action = mBodyNode->getBody()->getActionPrototype("ACT_STAND");
      }

      action->setPrioritized(false);

      mBodyNode->getBody()->executeAction(action);
   }
}

/**
 * Returns the current walk/run velocity of this character.
 *
 * @return the current walk/run velocity
 */
float Character::GetVelocity() const
{
   return mVelocity;
}
         
/**
 * Executes an animation action.
 *
 * @param action the name of the action to execute
 * @param priority whether or not the action is high-priority
 * @param force whether or not to force the action
 */
void Character::ExecuteAction(std::string name,
                              bool priority,
                              bool force)
{
   rbody::ActionRequest* action = mBodyNode->getBody()->getActionPrototype(name);
   
   if(action != NULL)
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      mBodyNode->getBody()->executeAction(action, force);
   }
}

/**
 * Executes a character action with a speed parameter.
 *
 * @param name the name of the action to execute
 * @param speed the speed at which to execute the action
 * @param priority whether or not the action is high-priority
 * @param force whether or not to force the action
 */
void Character::ExecuteActionWithSpeed(std::string name, 
                                       float speed,
                                       bool priority,
                                       bool force)
{
   rbody::ActionRequest* action = mBodyNode->getBody()->getActionPrototype(name);
   
   if(action != NULL)
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyFloat("speed", speed);
      
      mBodyNode->getBody()->executeAction(action, force);
   }
}

/**
 * Executes a character action with an angle parameter.
 *
 * @param name the name of the action to execute
 * @param angle the angle at which to execute the action
 * @param priority whether or not the action is high-priority
 * @param force whether or not to force the action
 */
void Character::ExecuteActionWithAngle(std::string name, 
                                       float angle,
                                       bool priority, 
                                       bool force)
{
   rbody::ActionRequest* action = mBodyNode->getBody()->getActionPrototype(name);
   
   if(action != NULL)
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyFloat("angle", angle);
   
      mBodyNode->getBody()->executeAction(action, force);
   }
}

/**
 * Executes a character action with speed and angle parameters.
 *
 * @param name the name of the action to execute
 * @param speed the speed at which to execute the action
 * @param angle the angle at which to execute the action
 * @param priority whether or not the action is high-priority
 * @param force whether or not to force the action
 */
void Character::ExecuteActionWithSpeedAndAngle(std::string name, 
                                               float speed, 
                                               float angle,
                                               bool priority,
                                               bool force)
{
   rbody::ActionRequest* action = mBodyNode->getBody()->getActionPrototype(name);
   
   if(action != NULL)
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyFloat("speed", speed);
      action->setPropertyFloat("angle", angle);
   
      mBodyNode->getBody()->executeAction(action, force);
   }
}
                                             
/**
 * Stops a character action.
 *
 * @param action the name of the action to stop
 */
void Character::StopAction(std::string name)
{
   mBodyNode->getBody()->stopAction(name);
}
