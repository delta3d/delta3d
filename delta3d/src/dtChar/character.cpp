// character.cpp: Implementation of the Character class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "dtChar/character.h"
#include "dtCore/notify.h"
#include "dtCore/scene.h"

#include "osgDB/FileUtils"
#include "rbody/osg/ReplicantBodyMgr.h"

#include "vrutils/os/FilePathContainer.h"

using namespace dtCore;
using namespace dtChar;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(Character)


/**
 * Updates the state of a character based on its transform.
 */
class CharacterCallback : public osg::NodeCallback
{
   public:
      
      /**
       * Constructor.
       *
       * @param character the dtCore Character object
       */
      CharacterCallback(Character* character)
         : mCharacter(character)
      {}
      
      /**
       * Update action.
       *
       * @param node the node object
       * @param nv the visitor object
       */
      virtual void operator()(osg::Node* node, osg::NodeVisitor *nv)
      {
         osg::Group* group = (osg::Group*)node;
         
         if(group->getNumChildren() > 0)
         {
            rbody::OsgBodyNode* child = 
               (rbody::OsgBodyNode*)group->getChild(0);
            
            osg::Matrix mat = child->getMatrix();
            
            Transform transform;
         
            mCharacter->GetTransform(&transform);
            
            if(mTransform.EpsilonEquals(&transform))
            {
               sgMat4 matrix = {
                  { mat(0,0), mat(0,1), mat(0,2), mat(0,3) },
                  { mat(1,0), mat(1,1), mat(1,2), mat(1,3) },
                  { mat(2,0), mat(2,1), mat(2,2), mat(2,3) },
                  { mat(3,0), mat(3,1), mat(3,2), mat(3,3) }
               };
               
               transform.Set(matrix);
               
               mCharacter->SetTransform(&transform);
            }
            else
            {
               sgMat4 matrix;
               
               transform.Get(matrix);
               
               child->setMatrix(
                  osg::Matrix(
                     matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
                     matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
                     matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3],
                     matrix[3][0], matrix[3][1], matrix[3][2], matrix[3][3]
                  )
               );
            }
         
            mTransform = transform;
         }
         
         traverse(node, nv);
      }
      
      
   private:
   
      /**
       * The dtCore Character object.
       */
      Character* mCharacter;
      
      /**
       * The transform applied on the last update.
       */
      Transform mTransform;
};


/**
 * Constructor.
 *
 * @param name the instance name
 */
Character::Character(string name)
   : mRotation(0.0f),
     mVelocity(0.0f)
{
   SetName(name);
   
   mNode = new osg::Group;

   mNode->setUpdateCallback(
      new CharacterCallback(this)
   );
   
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
 * Returns this object's OpenSceneGraph node.
 *
 * @return the OpenSceneGraph node
 */
osg::Node* Character::GetOSGNode()
{
   return mNode.get();
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
   mCollisionRootNode = scene->GetSceneNode();
   
   if(mBodyNode.get() != NULL)
   {
      mBodyNode->setCollisionRootNode(mCollisionRootNode.get());
   }
}

/**
 * Loads a ReplicantBody data file for this character.
 *
 * @param filename the name of the file to load
 */
bool Character::LoadFile(string filename)
{
   mFilename = filename;
   
   string path = osgDB::findDataFile(mFilename);
   
   if(path.empty())
   {
      Notify(WARN, "Character: Can't find %s", mFilename.c_str());
      return false;
   }
   else
   {
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
      
      mBodyNode = rbody::ReplicantBodyMgr::instance()->createCharacter(
         path,
         name,
         mCollisionRootNode.get()
      );
      
      if(mBodyNode.get() == NULL)
      {
         Notify(WARN, "Character: Can't load %s", mFilename.c_str());
      }
      else
      {
         mBodyNode->setRotation(mRotation*SG_DEGREES_TO_RADIANS);
         
         SetVelocity(mVelocity);
         
         mNode->addChild(mBodyNode.get());
      }
   }
   return true;
}

/**
 * Returns the name of the last file loaded.
 *
 * @return the name of the last file loaded
 */
string Character::GetFilename() const
{
   return mFilename;
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
         (rotation - mRotation) * SG_DEGREES_TO_RADIANS
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
   
      double walkSpeed;
      
      rbody::ActionRequest* walk = 
         mBodyNode->getBody()->getActionPrototype("ACT_WALK");
         
      walk->getPropertyValueReturn("speed", walkSpeed);
      
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
void Character::ExecuteAction(string name,
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
void Character::ExecuteActionWithSpeed(string name, 
                                       double speed,
                                       bool priority,
                                       bool force)
{
   rbody::ActionRequest* action = mBodyNode->getBody()->getActionPrototype(name);
   
   if(action != NULL)
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyValueReturn("speed", speed);
      
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
void Character::ExecuteActionWithAngle(string name, 
                                       double angle,
                                       bool priority, 
                                       bool force)
{
   rbody::ActionRequest* action = mBodyNode->getBody()->getActionPrototype(name);
   
   if(action != NULL)
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyValueReturn("angle", angle);
   
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
void Character::ExecuteActionWithSpeedAndAngle(string name, 
                                               double speed, 
                                               double angle,
                                               bool priority,
                                               bool force)
{
   rbody::ActionRequest* action = mBodyNode->getBody()->getActionPrototype(name);
   
   if(action != NULL)
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyValueReturn("speed", speed);
      action->setPropertyValueReturn("angle", angle);
   
      mBodyNode->getBody()->executeAction(action, force);
   }
}
                                             
/**
 * Stops a character action.
 *
 * @param action the name of the action to stop
 */
void Character::StopAction(string name)
{
   mBodyNode->getBody()->stopAction(name);
}
