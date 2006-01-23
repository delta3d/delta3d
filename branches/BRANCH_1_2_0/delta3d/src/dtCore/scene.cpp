// scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////

#include <osg/FrameStamp>
#include <osgUtil/IntersectVisitor>
#include <osg/PolygonMode>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osg/FrameStamp>

#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtCore/infinitelight.h>
#include <dtCore/physical.h>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Scene)

// Replacement message handler for ODE 
extern "C" void ODEMessageHandler(int errnum, const char *msg, va_list ap)
{  
   Log::GetInstance().LogMessage(Log::LOG_INFO, __FILE__, msg, ap);
}

// Replacement debug handler for ODE  
extern "C" void ODEDebugHandler(int errnum, const char *msg, va_list ap)
{
   Log::GetInstance().LogMessage(Log::LOG_ERROR, __FILE__, msg, ap);

   exit(1);
}

// Replacement error handler for ODE  
extern "C" void ODEErrorHandler(int errnum, const char *msg, va_list ap)
{
   Log::GetInstance().LogMessage(Log::LOG_ERROR, __FILE__, msg, ap);

   exit(1);
}

Scene::ParticleSystemFreezer::ParticleSystemFreezer()
   : osg::NodeVisitor( TRAVERSE_ALL_CHILDREN ),
     mFreezing( true )
{
   // Since we are setting all ParticleSystems to be frozen, we don't care
   // about the previous state of the last attemp to freeze.
   if( mFreezing )
   {
      mPreviousFrozenState.clear();
   }
}

void Scene::ParticleSystemFreezer::apply( osg::Node& node )
{
   if( osgParticle::ParticleSystemUpdater* psu = dynamic_cast< osgParticle::ParticleSystemUpdater* >( &node ) )
   {
      for( unsigned int i = 0; i < psu->getNumParticleSystems(); i++ )
      {         
         if( osgParticle::ParticleSystem* ps = psu->getParticleSystem( i ) )
         {
            if( mFreezing )
            {
               // Save the previous frozen state of the ParticleSystem, so subsequent attempts
               // to unfreeze it will bring it back to the way it was.
               mPreviousFrozenState.insert( ParticleSystemBoolMap::value_type( ps, ps->isFrozen() ) );

               // Allow me to break the ice. My name is Freeze. Learn it well.
               // For it's the chilling sound of your doom. -Mr. Freeze
               ps->setFrozen( mFreezing );
            }
            else
            {
               // Restore the previous state.
               ps->setFrozen( mPreviousFrozenState[ ps ] );
            }
         }
      }
   }
   
   traverse(node);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scene::Scene( const std::string& name, bool useSceneLight )
   : Base(name),
     mPhysicsStepSize(0.0),
     mPagingEnabled(false),
     mStartTick(0),
     mFrameNum(0),
     mCleanupTime(0.0025),
     mTargetFrameRate(30.0)
{
   RegisterInstance(this);

   for( int i = 0; i < MAX_LIGHTS; i++ )
      mLights[ i ] = 0;

   SetName(name);

   mSceneNode = new osg::Group;

   InfiniteLight* skyLight = new InfiniteLight( 0, "SkyLight" );
   AddDrawable( skyLight );
   skyLight->SetEnabled( true );

   mUserNearCallback = 0;
   mUserNearCallbackData = 0;

   mSpaceID = dHashSpaceCreate(0);
   mWorldID = dWorldCreate();
   
   dSpaceSetCleanup(mSpaceID, 0);
   
   mGravity.set(0.0f, 0.0f, 0.0f);
   
   mContactJointGroupID = dJointGroupCreate(0);
   
   dSetMessageHandler(ODEMessageHandler);
   dSetDebugHandler(ODEDebugHandler);
   dSetErrorHandler(ODEErrorHandler);
   
   AddSender(System::Instance());

   //TODO set default render face, mode
}

Scene::~Scene()
{
   // Since we are going to destroy all the bodies in our world with dWorldDestroy,
   // we must remove the references to the bodies associated with their default collision
   // geoms. Otherwise destroying the world will leave the geoms references bad memory.
   // This prevents a crash-on-exit in STAGE.
   for(  TransformableVector::iterator iter = mCollidableContents.begin();
         iter != mCollidableContents.end();
         iter++ )
   {
      if( Physical* physical = dynamic_cast<Physical*>(*iter) )
      {
         physical->SetBodyID(0);
      }
   }

   dWorldDestroy(mWorldID);
   dJointGroupDestroy(mContactJointGroupID);

   // Remove the remaining DeltaDrawables from the Scene. This is redundant to help prevent
   // crash-on-exits. The "exit" message should have the same effect. This must be called 
   // after the above code that destroys the ODE world.
   RemoveAllDrawables();

   dSpaceDestroy(mSpaceID);

   DeregisterInstance(this);
   
   if(mPagingEnabled)
   {
      DisablePaging();
   }

   RemoveSender( System::Instance() );
}

void Scene::AddDrawable( DeltaDrawable *drawable )
{
   // This is modified to put a RefPtr in the scene
   // They are required or when you change the stateset
   // Everything is killed and you get a blank screen.
   // I still pushback the original *drawable 
   // so remove will still work
   RefPtr<DeltaDrawable> drawme = drawable;
   mSceneNode->addChild( drawme->GetOSGNode() );
   drawable->AddedToScene(this);

   mAddedDrawables.push_back(drawable);
   
   // this is the original code
   //mSceneNode->addChild( drawable->GetOSGNode() );
   //drawable->AddedToScene(this);

   //mAddedDrawables.push_back(drawable);
   
}

void Scene::RemoveDrawable(DeltaDrawable *drawable)
{
   mSceneNode.get()->removeChild( drawable->GetOSGNode() );
   drawable->AddedToScene(0);

   unsigned int pos = GetDrawableIndex(drawable);
   if (pos<mAddedDrawables.size())
   {
      mAddedDrawables.erase( mAddedDrawables.begin()+pos );                           
   }
}

void Scene::RemoveAllDrawables()
{
   while( !mAddedDrawables.empty() )
   {
      if( DeltaDrawable *d = GetDrawable(0) )
      {
         RemoveDrawable(d);
      }
   }
}

void Scene::SetRenderState( Face face, Mode mode )
{
   mRenderFace = face;
   mRenderMode = mode;

   // We need a face and a mode from osg to set the face and mode in delta
   // I used the same names to create enums in scene
   osg::PolygonMode::Face myface;
   osg::PolygonMode::Mode mymode;

   // this switch statement will take the osg face and make it the corresponding
   // scene face
   switch(mRenderFace)
   {
   case FRONT:
      myface = osg::PolygonMode::FRONT;
      break;

   case BACK:
      myface = osg::PolygonMode::BACK;
      break;

   case FRONT_AND_BACK:
      myface = osg::PolygonMode::FRONT_AND_BACK;
      break;

   default:
      myface = osg::PolygonMode::FRONT;
      break;
   }

   // this switch statement is similar to the one above for mode
   switch(mRenderMode)
   {
   case POINT:
      mymode = osg::PolygonMode::POINT;
      break;

   case LINE:
      mymode = osg::PolygonMode::LINE;
      break;

   case FILL:
      mymode = osg::PolygonMode::FILL;
      break;

   default:
      mymode = osg::PolygonMode::FILL;
      break;
   }

   // We change the scenes stateset here.
   osg::ref_ptr<osg::PolygonMode> polymode = new osg::PolygonMode;
   polymode->setMode(myface, mymode);

   osg::StateSet *stateSet = mSceneNode.get()->getOrCreateStateSet();
   stateSet->setAttributeAndModes(polymode.get(),osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON); 
}

/** Register a Physical with the Scene.  This method is automatically called 
  * when adding Drawables to the Scene.  Typically, this only needs to be 
  * called when a creating a Physical that is not added to the Scene like a
  * Drawable.
  * @param physical The Physical to register with the Scene
  * @see AddDrawable()
  */
void Scene::RegisterPhysical( Physical *physical )
{
   DEPRECATE(  "void Scene::RegisterPhysical( Physical *physical )",
               "void Scene::RegisterCollidable( Transformable* collidable )")

   RegisterCollidable( physical );
}

void Scene::RegisterCollidable( Transformable* collidable )
{
   if( collidable == 0 )
   {
      return;
   }

   dSpaceAdd( mSpaceID, collidable->GetGeomID() );

   dGeomSetData( collidable->GetGeomID(), collidable );

   // This should probably be some sort of virtual function.
   // Or perhaps RegisterPhysical can stick around and only do
   // this.
   if( Physical* physical = dynamic_cast<Physical*>(collidable) )
   {
      physical->SetBodyID( dBodyCreate( mWorldID ) );
   }

   mCollidableContents.push_back( collidable );
}

void Scene::UnRegisterPhysical( Physical *physical )
{
   DEPRECATE(  "void Scene::UnRegisterPhysical( Physical *physical )",
               "void Scene::UnRegisterCollidable( Transformable* collidable )")

   UnRegisterCollidable( physical );
}

void Scene::UnRegisterCollidable( Transformable* collidable )
{
   dSpaceRemove( mSpaceID, collidable->GetGeomID() );

   for(  TransformableVector::iterator it = mCollidableContents.begin();
         it != mCollidableContents.end();
         it++ )
   {
      if( *it == collidable )
      {
         if( Physical* physical = dynamic_cast<Physical*>(*it) )
         {
            physical->SetBodyID(0);
         }
         mCollidableContents.erase( it );
         break;
      }
   }
}

/*!
 * Get the height of terrain at the specified (X,Y).  This essentially 
 * does an intersection check of the whole scene from (X,Y,10k) to (X,Y,-10k).
 * Any geometry that intersects is considered the "terrain".
 *
 * @param x : The X location to check for HOT
 * @param y : The Y location to check for HOT
 *
 * @return float  : The found Height of Terrain (or 0 if no intersection)
 */
float Scene::GetHeightOfTerrain( float x, float y )
{
   float HOT = 0.f;
   osgUtil::IntersectVisitor iv;
   RefPtr<osg::LineSegment> segDown = new osg::LineSegment;
   
   segDown->set(osg::Vec3(x, y, 10000.f),osg::Vec3(x,y, -10000.f));
   iv.addLineSegment(segDown.get());
   iv.setTraversalMask(0x0fffffff);
   
   mSceneNode->accept(iv);
   
   if (iv.hits())
   {
      osgUtil::IntersectVisitor::HitList& hitList = iv.getHitList(segDown.get());
      if (!hitList.empty())
      {
         osg::Vec3 ip = hitList.front().getWorldIntersectPoint();
         osg::Vec3 np = hitList.front().getWorldIntersectNormal();
         HOT = ip.z();
      }
   }
   return HOT;
}

void Scene::SetGravity( const osg::Vec3& gravity )
{
   mGravity.set(gravity);
   
   dWorldSetGravity(mWorldID, mGravity[0], mGravity[1], mGravity[2]);
}

// Get the ODE space ID
dSpaceID Scene::GetSpaceID() const
{
   return mSpaceID;
}

// Get the ODE world ID
dWorldID Scene::GetWorldID() const
{
   return mWorldID;
}

// Performs collision detection and updates physics
void Scene::OnMessage(MessageData *data)
{
   if(data->message == "postframe")
   {
      double cleanup = mCleanupTime;
      if(mPagingEnabled)
      {
         if (osgDB::Registry::instance()->getDatabasePager())
         {
             osgDB::Registry::instance()->getDatabasePager()->signalEndFrame();

            for (int camNum = 0; camNum < Camera::GetInstanceCount(); ++camNum )
            {
               Camera *cam = Camera::GetInstance(camNum);

               osgDB::Registry::instance()->getDatabasePager()->compileGLObjects(*(cam->GetSceneHandler()->GetSceneView()->getState()), cleanup);
               
               cam->GetSceneHandler()->GetSceneView()->flushDeletedGLObjects(cleanup);
             }  
         }
      }
   }
   else if(data->message == "preframe")
   {
      double dt = *static_cast<double*>(data->userData);

      //if paging is enabled, update pager
      if(mPagingEnabled)
      {
         osg::FrameStamp* frameStamp = new osg::FrameStamp;
         frameStamp->setReferenceTime(osg::Timer::instance()->delta_s(mStartTick, osg::Timer::instance()->tick()));
         frameStamp->setFrameNumber(mFrameNum++);

         if (osgDB::Registry::instance()->getDatabasePager())
         {
            osgDB::Registry::instance()->getDatabasePager()->signalBeginFrame(frameStamp);
            osgDB::Registry::instance()->getDatabasePager()->updateSceneGraph(frameStamp->getReferenceTime());
         }
      }

      bool usingDeltaStep = false;

      // if step size is 0.0, use the deltaFrameTime instead
      if( mPhysicsStepSize == 0.0 ) 
      {
         SetPhysicsStepSize( dt );
         usingDeltaStep = true;
      }

      const int numSteps = int(dt/mPhysicsStepSize);

      TransformableVector::iterator it;
      
      for(  it = mCollidableContents.begin();
            it != mCollidableContents.end();
            it++ )
      {
         (*it)->PrePhysicsStepUpdate();
      }

      for (int i=0; i<numSteps; i++)
      {
         if (mUserNearCallback)
         {
            dSpaceCollide(mSpaceID, mUserNearCallbackData, mUserNearCallback);
         }
         else
         {
            dSpaceCollide(mSpaceID, this, NearCallback);
         }

         dWorldQuickStep(mWorldID, mPhysicsStepSize);
         
         dJointGroupEmpty(mContactJointGroupID);
      }

      double leftOver = dt - (numSteps * mPhysicsStepSize);
      
      if(leftOver > 0.0)
      {   
         if (mUserNearCallback)
         {
            dSpaceCollide(mSpaceID, mUserNearCallbackData, mUserNearCallback);
         }
         else
         {
            dSpaceCollide(mSpaceID, this, NearCallback);
         }

         dWorldStep(mWorldID, leftOver);
         
         dJointGroupEmpty(mContactJointGroupID);
      }
      
      for(it = mCollidableContents.begin();
          it != mCollidableContents.end();
          it++)
      {
         (*it)->PostPhysicsStepUpdate();
      }

      if( usingDeltaStep ) //reset physics step size to 0.0 (i.e. use System step size)
         SetPhysicsStepSize( 0.0 );
   }
   else if( data->message == "pause_start" )
   {
      // Freeze all particle systems.
      mFreezer.SetFreezing( true );
      GetSceneNode()->accept( mFreezer );
   }
   else if( data->message == "pause_end" )
   {
      // Unfreeze all particle systems.
      mFreezer.SetFreezing( false );
      GetSceneNode()->accept( mFreezer );
   }
   else if(data->message == "exit")
   {
      RemoveAllDrawables();
   }
}

// ODE collision callback     
void Scene::NearCallback( void* data, dGeomID o1, dGeomID o2 )
{
   if( data == 0 || o1 == 0 || o2 == 0 )
   {
      return;
   }

   Scene* scene = static_cast<Scene*>(data);
   
   Transformable* c1 = static_cast<Transformable*>( dGeomGetData(o1) );
   Transformable* c2 = static_cast<Transformable*>( dGeomGetData(o2) );
             
   dContactGeom contactGeoms[8];
   
   int numContacts = dCollide( o1, o2, 8, contactGeoms, sizeof(dContactGeom) );
   
   if( numContacts > 0 && c1 != 0 && c2 != 0 )
   {
      CollisionData cd;
      
      cd.mBodies[0] = c1;
      cd.mBodies[1] = c2;
    
      cd.mLocation.set( 
         contactGeoms[0].pos[0], contactGeoms[0].pos[1], contactGeoms[0].pos[2]
      );
      
      cd.mNormal.set( 
         contactGeoms[0].normal[0], contactGeoms[0].normal[1], contactGeoms[0].normal[2]
      );
      
      cd.mDepth = contactGeoms[0].depth;
      
      scene->SendMessage("collision", &cd);
      
      if( c1 != 0 || c2 != 0 )
      {
         dContact contact;
      
         for( int i = 0; i < numContacts; i++ )
         {
            contact.surface.mode = dContactMu2 | dContactBounce;
            contact.surface.mu = 1000.0;
            contact.surface.mu2 = 1000.0;
            contact.surface.bounce = 0.75;
            contact.surface.bounce_vel = 0.001;
            
            contact.geom = contactGeoms[i];

            // Make sure to call these both, because in the case of
            // Trigger, meaningful stuff happens even if the return
            // is false.
            bool contactResult1 = c1->FilterContact(&contact, c2);
            bool contactResult2 = c2->FilterContact(&contact, c1);
       
            if( contactResult1 && contactResult2 )
            {
               // All this also should be in a virtual function.
               Physical* p1 = dynamic_cast<Physical*>(c1);
               Physical* p2 = dynamic_cast<Physical*>(c2);

               if( p1 != 0 || p2 != 0 )
               {
                  dJointID joint = dJointCreateContact(
                     scene->mWorldID, 
                     scene->mContactJointGroupID, 
                     &contact
                     );

                  dJointAttach(
                     joint, 
                     p1->DynamicsEnabled() ? p1->GetBodyID() : 0,
                     p2->DynamicsEnabled() ? p2->GetBodyID() : 0
                     );
               }
            }
         }
      }
   }
}


/** The supplied function will be used instead of the built-in collision
 *  callback.  
 * @param func : The function to handle collision detection
 * @param data : A void pointer to user data.  This gets passed directly to func.
 */
void Scene::SetUserCollisionCallback(dNearCallback *func, void *data)
{
   mUserNearCallback = func;
   mUserNearCallbackData = data;
}

Light* Scene::GetLight( const std::string& name ) const
{
   for( int i = 0; i < MAX_LIGHTS; i++ )
   {
      if( mLights[ i ]->GetName() == name )
      {
         return mLights[ i ];
      }
   }
   
   return 0;
}


void Scene::UseSceneLight( bool lightState )
{ 
   mLights[0]->SetEnabled(lightState);
}

void Scene::EnablePaging()
{
   if(Camera::GetInstanceCount() > 0)
   {
      osgDB::DatabasePager* databasePager = osgDB::Registry::instance()->getOrCreateDatabasePager();
      databasePager->setTargetFrameRate(mTargetFrameRate);
      databasePager->registerPagedLODs( mSceneNode.get() );
      databasePager->setUseFrameBlock(false);

      for (int camNum = 0; camNum < Camera::GetInstanceCount(); ++camNum )
      {
         Camera *cam = Camera::GetInstance(camNum);

         cam->GetSceneHandler()->GetSceneView()->getCullVisitor()->setDatabaseRequestHandler(databasePager);

         databasePager->setCompileGLObjectsForContextID(cam->GetSceneHandler()->GetSceneView()->getState()->getContextID(),true);
      }    

      mStartTick = osg::Timer::instance()->tick();
      mPagingEnabled = true;
   }
}

void Scene::DisablePaging()
{
   if(mPagingEnabled && osgDB::Registry::instance()->getDatabasePager() != NULL)
   {
      osgDB::Registry::instance()->getDatabasePager()->clear();
      osgDB::Registry::instance()->setDatabasePager(NULL);
      mPagingEnabled = false;
   }
   else
   {
      LOG_ERROR("DisablePaging was called when paging wasn't enabled");
   }
}
