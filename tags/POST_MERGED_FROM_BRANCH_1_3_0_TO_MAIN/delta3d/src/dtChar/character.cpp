// character.cpp: Implementation of the Character class.
//
//////////////////////////////////////////////////////////////////////

#include <dtChar/character.h>

#include <dtCore/camera.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>

#include <osgDB/FileUtils>

#include <sstream>

using namespace dtCore;
using namespace dtChar;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Character)

/**
 * Constructor.
 *
 * @param name the instance name
 */
 Character::Character(const std::string& name)
   :  Transformable(name),
      mRotation(0.0f),
      mVelocity(0.0f),
      mPreviousUpdateMode( rbody::OsgBodyNode::UPDATE_NONE ),
      mUpdateVisitor(NULL),
      mPauseFrameNumber(0)
{
   RegisterInstance(this);

   AddSender( System::Instance() );

   // Default collision category = 12
   SetCollisionCategoryBits( UNSIGNED_BIT(12) );
}

/**
 * Destructor.
 */
Character::~Character()
{
   DeregisterInstance(this);
}

/**
 * Add any DeltaDrawable as a child of this Character. In the scene
 * graph, the osg::Node inside the the DeltaDrawable child which is
 * returned by GetOSGNode() will be attached to the rbody::OsgNodeNode
 * that is contained in the Character class.
 *
 * @param child The DeltaDrawable which to add as a child.
 * @return Whether or not the child was added successfully.
 *
 * @pre child != NULL
 */
bool Character::AddChild( DeltaDrawable* child )
{
   if( DeltaDrawable::AddChild(child) && GetMatrixNode() != NULL ) 
   {
      GetMatrixNode()->addChild( child->GetOSGNode() );
      return true;
   }
   else
   {
      return false;
   }
}
   
/**
 * Removes any DeltaDrawable from being a child of this Character. 
 * In the scene graph, the osg::Node inside the the DeltaDrawable child
 * which is returned by GetOSGNode() will be removed from the 
 * rbody::OsgNodeNode that is contained in the Character class if is
 * inded a child already.
 *
 * @param child The DeltaDrawable which to remove as a child.
 * @return Whether or not the child was removed successfully.
 *
 * @pre child != NULL
 */
void Character::RemoveChild( DeltaDrawable* child )
{
   if( GetMatrixNode() != NULL )
   {
      GetMatrixNode()->removeChild( child->GetOSGNode() );
      DeltaDrawable::RemoveChild(child);
   }
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
   Transformable::AddedToScene(scene);

   if( GetBodyNode() != NULL && scene != NULL )
   {
      mCollisionRootNode = scene->GetSceneNode();
      GetBodyNode()->setCollisionRootNode(mCollisionRootNode.get());    
   }
}

osg::Node* Character::LoadFile(const std::string& filename, bool useCache)
{
   //clear out the children.
   if( GetMatrixNode()->getNumChildren() != 0 )
   {
      GetMatrixNode()->removeChild( 0, GetMatrixNode()->getNumChildren() );
   }

   std::string path = osgDB::findDataFile(filename);
   
   if( path.empty() )
   {
      Log::GetInstance("character.cpp").
         LogMessage(Log::LOG_WARNING, __FUNCTION__, __LINE__, 
                     "Character: Can't find file \"%s\".", 
                     filename.c_str());
   }
   else
   {
      CalLoader::setLoadingMode(LOADER_INVERT_V_COORD);

      // Prevents ReplicantBody from complaining
      putenv("REPLICANTBODY_FILE_PATH=."); 
      
      // Find a unique name for the instance
      std::string name = GetName();      
      for(  int i = 2;
            rbody::ReplicantBodyMgr::instance()->findCharacter(name) != NULL;
            ++i )
      {
         std::ostringstream buf;
         buf << GetName() << " " << i;         
         name = buf.str();
      }
      
      osg::Matrix mat;
      if( GetMatrixNode() )
      {
         mat = GetMatrixNode()->getMatrix();
      }

      ReplaceMatrixNode( rbody::ReplicantBodyMgr::instance()->createCharacter(   path,
                                                                                 name,
                                                                                 mCollisionRootNode.get() ) );

      //HACK: Somehow fixes bug where multi-textures models drop their textures when
      //      a character is in the scene. Weird. Please someone fix this fo' real!
      for( unsigned int i = 0; i < GetMatrixNode()->getNumChildren(); ++i )
      {
         if( osg::MatrixTransform* scale = dynamic_cast<osg::MatrixTransform*>( GetMatrixNode()->getChild(i) ) )
         {
            for( unsigned int j = 0; j < scale->getNumChildren(); ++j )
            {
               if( osg::Geode* geode = dynamic_cast<osg::Geode*>( scale->getChild(j) ) )
               {
                  for( unsigned int k = 0; k < geode->getNumDrawables(); ++k )
                  {
                     osg::Drawable* drawable = dynamic_cast<osg::Drawable*>( geode->getDrawable(k) );

                     osg::StateSet* stateSet = drawable->getOrCreateStateSet();
                     stateSet->setMode(GL_TEXTURE_COORD_ARRAY, osg::StateAttribute::OFF);
                  }
               }
            }
         }
      }
 
      GetMatrixNode()->setMatrix(mat);

      GetBodyNode()->setUpdateMode( rbody::OsgBodyNode::UPDATE_ANIMATION | 
                                    rbody::OsgBodyNode::UPDATE_CONTACT_TRANSLATION | 
                                    rbody::OsgBodyNode::UPDATE_GROUND_FOLLOWING );     
      GetBodyNode()->setRotation(osg::DegreesToRadians(mRotation));     

      SetVelocity(mVelocity);            
   }

   return GetMatrixNode();
}

void Character::OnMessage( Base::MessageData* data )
{
   // Oh no!!! Here come the rbody haxors!
   // rbody needs two conditions in order to "pause".
   // First, it needs to have it's update modes set to UPDATE_NONE.
   // Second, there needs to be an update traversal performed on the
   // rbody::OsgBodyNode. Yes, this is silly. So since the update
   // traversal is shutoff in "pause" mode (all the way over in the
   // SceneView, we need to make our own little UpdateVisitor with
   // an incrementing traversal number.
   
   if( data->message == "pause" )
   {
      if( GetMatrixNode() != NULL )
      {
         if( osg::Node* n1 = GetMatrixNode()->getChild(0) )
         {
            if( osg::Group* g2 = n1->asGroup() )
            {
               if( osg::Node* n2 = g2->getChild(0) )
               {
                  if( osg::NodeCallback* callback = n2->getUpdateCallback() )
                  {
                     if( !mUpdateVisitor.valid() )
                     {
                        mUpdateVisitor = new osgUtil::UpdateVisitor;
                     }
                     
                     mUpdateVisitor->reset();
                     mUpdateVisitor->setTraversalNumber( mPauseFrameNumber++ );
                     
                     (*callback)( n2, mUpdateVisitor.get() );
                  }
               }
            }
         }
      }
   }
   else if( data->message == "pause_start" )
   {
      mPauseFrameNumber = 0;

      if( GetBodyNode() != NULL )
      {
         mPreviousUpdateMode = GetBodyNode()->getUpdateMode();
         GetBodyNode()->setUpdateMode( rbody::OsgBodyNode::UPDATE_NONE );
         
         mPreviousInternalUpdateMode = GetBodyNode()->getInternalUpdateMode();
         GetBodyNode()->setInternalUpdateMode( rbody::OsgBodyNode::UPDATE_NONE );
      }
   }
   else if( data->message == "pause_end" )
   {
      if( GetBodyNode() != NULL )
      {
         GetBodyNode()->setUpdateMode( mPreviousUpdateMode );
         GetBodyNode()->setInternalUpdateMode( mPreviousInternalUpdateMode );
      }
   }
}

/**
 * Sets the rotation of this character.
 *
 * @param rotation the new rotation, in degrees about the Z
 * axis
 */
void Character::SetRotation(float rotation)
{
   if( GetBodyNode() != NULL )
   {
      GetBodyNode()->setRotation( osg::DegreesToRadians(rotation - mRotation) );
   }
   
   // Normalize
   while(rotation < 0.0f) 
   {
      rotation += 360.0f;
   }

   while(rotation > 360.0f)
   {
      rotation -= 360.0f;
   }
   
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

void Character::SetRotationSpeed(float pSpeed) 
{
   mRotationSpeed = pSpeed;
}

float Character::GetRotationSpeed() const
{
   return mRotationSpeed;
}

void Character::RotateCharacterToPoint(const osg::Vec3& targetPos, float delta)
{
   dtCore::Transform trans;
   osg::Vec3 ownPos;
   GetTransform(trans);  
   trans.GetTranslation(ownPos);     

   osg::Vec3 pVector = targetPos - ownPos;

   pVector [2] = 0.0f;

   float len = pVector.length();
   float dir = osg::RadiansToDegrees(atan2(pVector[0], -pVector[1]));
   float rotation = GetRotation();
   float dR = dir - rotation;

   if( dR > 180.0f )
   {
      dR -= 360.0f;
   }
   else if( dR < -180.0f )
   {
      dR += 360.0f;
   }

   if(dR > 0.0f)
   {
      rotation += (dR > delta*90.0f ? delta*90.0f : dR);   
   }
   else if(dR < 0.0f)
   {
      rotation += (dR < -delta*90.0f ? -delta*90.0f : dR);
   }

   SetRotation(rotation);
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
      
      rbody::ActionRequest* walk = GetBodyNode()->getBody()->getActionPrototype("ACT_WALK");
         
      walkSpeed = walk->getPropertyFloat("speed");
      
      rbody::ActionRequest* action;
      
      if(mVelocity > walkSpeed)
      {
         action = GetBodyNode()->getBody()->getActionPrototype("ACT_RUN");
      }
      else if(mVelocity > 0.0f)
      {
         action = walk;
      }
      else
      {
         action = GetBodyNode()->getBody()->getActionPrototype("ACT_STAND");
      }

      action->setPrioritized(false);

      GetBodyNode()->getBody()->executeAction(action);
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
void Character::ExecuteAction(const std::string& name,
                              bool priority,
                              bool force)
{
   if( rbody::ActionRequest* action = GetBodyNode()->getBody()->getActionPrototype(name) )
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      GetBodyNode()->getBody()->executeAction(action, force);
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
void Character::ExecuteActionWithSpeed(const std::string& name, 
                                       float speed,
                                       bool priority,
                                       bool force)
{
   if( rbody::ActionRequest* action = GetBodyNode()->getBody()->getActionPrototype(name) )
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyFloat("speed", speed);
      
      GetBodyNode()->getBody()->executeAction(action, force);
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
void Character::ExecuteActionWithAngle(const std::string& name, 
                                       float angle,
                                       bool priority, 
                                       bool force)
{
   if( rbody::ActionRequest* action = GetBodyNode()->getBody()->getActionPrototype(name) )
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyFloat("angle", angle);
   
      GetBodyNode()->getBody()->executeAction(action, force);
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
void Character::ExecuteActionWithSpeedAndAngle(const std::string& name, 
                                               float speed, 
                                               float angle,
                                               bool priority,
                                               bool force)
{
   if( rbody::ActionRequest* action = GetBodyNode()->getBody()->getActionPrototype(name) )
   {
      action = action->clone();
      
      action->setPrioritized(priority);
      
      action->setPropertyFloat("speed", speed);
      action->setPropertyFloat("angle", angle);
   
      GetBodyNode()->getBody()->executeAction(action, force);
   }
}
                                             
/**
 * Stops a character action.
 *
 * @param action the name of the action to stop
 */
void Character::StopAction(const std::string& name)
{
   GetBodyNode()->getBody()->stopAction(name);
}
