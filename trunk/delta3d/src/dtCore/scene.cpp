// scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////

#include "osg/FrameStamp"
#include <osgUtil/IntersectVisitor>

#include "dtCore/scene.h"
#include "dtCore/system.h"
#include "dtCore/notify.h"
#include "dtCore/infinitelight.h"
#include "dtCore/positionallight.h"

using namespace dtCore;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(Scene)

// Replacement message handler for ODE 
extern "C" void ODEMessageHandler(int errnum, const char *msg, va_list ap)
{
   Notify(NOTICE, msg, ap);  
}

// Replacement debug handler for ODE  
extern "C" void ODEDebugHandler(int errnum, const char *msg, va_list ap)
{
   Notify(FATAL, msg, ap);
   
   exit(1);
}

// Replacement error handler for ODE  
extern "C" void ODEErrorHandler(int errnum, const char *msg, va_list ap)
{
   Notify(FATAL, msg, ap);
      
   exit(1);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scene::Scene( string name, bool useSceneLight )
: Base(name), mPhysicsStepSize(0.0)
{
   RegisterInstance(this);

   for( int i = 0; i < MAX_LIGHTS; i++ )
      mLights[ i ] = 0;

   SetName(name);
   mSceneHandler = new _SceneHandler(useSceneLight);

   mSceneNode = new osg::Group;
   mSceneHandler->GetSceneView()->setSceneData( mSceneNode.get() );

   mLightGroup = new osg::Group;
   mSceneNode.get()->addChild( mLightGroup );

   osg::LightSource* sceneLightSource = new osg::LightSource;	
   sceneLightSource->setLight( GetSceneHandler()->GetSceneView()->getLight() );
   mLightGroup->addChild( sceneLightSource );

   mLights[ 0 ] = new InfiniteLight( sceneLightSource, "sceneLight", Light::GLOBAL );

   mUserNearCallback = NULL;
   mUserNearCallbackData = NULL;

   mSpaceID = dHashSpaceCreate(0);
   mWorldID = dWorldCreate();
   
   dSpaceSetCleanup(mSpaceID, 0);
   
   sgSetVec3(mGravity, 0.0f, 0.0f, 0.0f);
   
   mContactJointGroupID = dJointGroupCreate(0);
   
   dSetMessageHandler(ODEMessageHandler);
   dSetDebugHandler(ODEDebugHandler);
   dSetErrorHandler(ODEErrorHandler);
   
   AddSender(System::GetSystem());
}

Scene::~Scene()
{
   DeregisterInstance(this);

   dJointGroupDestroy(mContactJointGroupID);
   dSpaceDestroy(mSpaceID);
   dWorldDestroy(mWorldID);
   Notify(DEBUG_INFO, "destroying Scene ref:%d", this->referenceCount() );
}

void Scene::AddDrawable( DeltaDrawable *drawable )
{
   if( Physical* physical = dynamic_cast<Physical*>( drawable ) )
   {       
      RegisterPhysical(physical);
   }

   mSceneNode.get()->addChild( drawable->GetOSGNode() );

   drawable->AddedToScene(this);

}

void Scene::RemoveDrawable(DeltaDrawable *drawable)
{
   if( Physical* physical = dynamic_cast<Physical*>( drawable ) )
   {
      UnRegisterPhysical(physical);
   }

   mSceneNode.get()->removeChild( drawable->GetOSGNode() );
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
   if (physical==NULL) return;

   dSpaceAdd(mSpaceID, physical->GetGeomID());

   dGeomSetData(physical->GetGeomID(), physical);

   physical->SetBodyID(dBodyCreate(mWorldID));

   mPhysicalContents.push_back(physical);
}



void Scene::UnRegisterPhysical( Physical *physical )
{
	dSpaceRemove(mSpaceID, physical->GetGeomID());

	//dBodyDestroy(physical->GetBodyID());

	for(vector<Physical*>::iterator it = mPhysicalContents.begin();
		it != mPhysicalContents.end();
		it++)
	{
		if(*it == physical)
		{
			mPhysicalContents.erase(it);
			break;
		}
	}

}



_SceneHandler::_SceneHandler(bool useSceneLight):
mSceneView(new osgUtil::SceneView()),
mFrameStamp(new osg::FrameStamp())
{
   mSceneView->init();
   mSceneView->setDefaults(); //osg::SceneView

   if(useSceneLight)
      mSceneView->setLightingMode(osgUtil::SceneView::SKY_LIGHT);
   else
      mSceneView->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);

   mSceneView->setFrameStamp(mFrameStamp.get());

   mStats = new Stats( mSceneView.get() );
   mStats->Init( mSceneView.get()->getRenderStage() );
}

_SceneHandler::~_SceneHandler()
{
   dtCore::Notify(dtCore::DEBUG_INFO, "Destroying _SceneHandler");
}

void _SceneHandler::clear(Producer::Camera& cam)
{
   ClearImplementation( cam );
}

void _SceneHandler::ClearImplementation( Producer::Camera &cam )
{
   //Override the Producer::Camera::clear() because the 
   //  OSGUtil::SceneView::draw() does it for us.

   //So lets not do anything clearing here, ok?
}

void _SceneHandler::cull( Producer::Camera &cam) 
{
   //call osg cull here         
   CullImplementation( cam );
}

void _SceneHandler::CullImplementation(Producer::Camera &cam)
{
   mStats->SetTime(Stats::TIME_BEFORE_CULL);

   mFrameStamp->setFrameNumber(mFrameStamp->getFrameNumber()+1);

   mClock.update();

   mFrameStamp->setReferenceTime(mClock.getAbsTime() );      

   //copy the Producer Camera's position to osg::SceneView  
   mSceneView->getProjectionMatrix().set(cam.getProjectionMatrix());
   mSceneView->getViewMatrix().set(cam.getPositionAndAttitudeMatrix());

   //Copy the Producer Camera's viewport info to osg::SceneView
   int x, y;
   unsigned int w, h;
   cam.getProjectionRectangle( x, y, w, h );

   mSceneView->setViewport( x, y, w, h );

   //Now tell SceneView to cull
   mSceneView->cull();

   mStats->SetTime(Stats::TIME_AFTER_CULL);
}

void _SceneHandler::draw( Producer::Camera &cam) 
{
   //call osg draw here
   DrawImplementation( cam );
};


void _SceneHandler::DrawImplementation( Producer::Camera &cam )
{
   mStats->SetTime(Stats::TIME_BEFORE_DRAW);

   mSceneView->draw();
   mStats->SetTime(Stats::TIME_AFTER_DRAW);
   mStats->Draw();
}




/*!
 * Get the height of terrain at the specified (X,Y).  This essentially 
 * does an intersection check of the whole scene from (X,Y,10k) to (X,Y,-10k).
 * Any geometry that intersects is considered the "terrain".
 *
 * @param *x : The X location to check for HOT
 * @param *y : The Y location to check for HOT
 *
 * @return float  : The found Height of Terrain (or 0 if no intersection)
 */
float Scene::GetHeightOfTerrain(const float *x, const float *y)
{
   float HOT = 0.f;
   osgUtil::IntersectVisitor iv;
   osg::ref_ptr<osg::LineSegment> segDown = new osg::LineSegment;
   
   segDown->set(osg::Vec3(*x, *y, 10000.f),osg::Vec3(*x,*y, -10000.f));
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

void Scene::SetGravity(sgVec3 gravity)
{
   sgCopyVec3(mGravity, gravity);
   
   dWorldSetGravity(mWorldID, mGravity[0], mGravity[1], mGravity[2]);
}

void Scene::SetGravity(float x, float y, float z)
{
   sgSetVec3(mGravity, x, y, z);
   
   dWorldSetGravity(mWorldID, mGravity[0], mGravity[1], mGravity[2]);
}
         
void Scene::GetGravity(sgVec3 vec)
{
   sgCopyVec3(vec, mGravity);
}

void Scene::GetGravity(float* x, float* y, float* z)
{
   *x = mGravity[0];
   *y = mGravity[1];
   *z = mGravity[2];
}

///Get the ODE space ID
dSpaceID Scene::GetSpaceID() const
{
   return mSpaceID;
}

///Get the ODE world ID
dWorldID Scene::GetWorldID() const
{
   return mWorldID;
}

///Performs collision detection and updates physics
void Scene::OnMessage(MessageData *data)
{

   if(data->message == "preframe")
   {
      double dt = *(double *)data->userData;

      bool usingDeltaStep = false;

      // if step size is 0.0, use the deltaFrameTime instead
      if( mPhysicsStepSize == 0.0 ) 
      {
         SetPhysicsStepSize( dt );
         usingDeltaStep = true;
      }

      const int numSteps = (int)(dt/mPhysicsStepSize);

      vector<Physical*>::iterator it;
      
      for(it = mPhysicalContents.begin();
          it != mPhysicalContents.end();
          it++)
      {
         (*it)->PrePhysicsStepUpdate();
      }

      for (int i=0; i<numSteps; i++)
      {
         if (mUserNearCallback)
            dSpaceCollide(mSpaceID, mUserNearCallbackData, mUserNearCallback);
         else
            dSpaceCollide(mSpaceID, this, NearCallback);

         dWorldQuickStep(mWorldID, mPhysicsStepSize);
         
         dJointGroupEmpty(mContactJointGroupID);
      }

      double leftOver = dt - (numSteps * mPhysicsStepSize);
      
      if(leftOver > 0.0)
      {   
         if (mUserNearCallback)
            dSpaceCollide(mSpaceID, mUserNearCallbackData, mUserNearCallback);
         else
            dSpaceCollide(mSpaceID, this, NearCallback);

         dWorldStep(mWorldID, leftOver);
         
         dJointGroupEmpty(mContactJointGroupID);
      }
      
      for(it = mPhysicalContents.begin();
          it != mPhysicalContents.end();
          it++)
      {
         (*it)->PostPhysicsStepUpdate();
      }

      if( usingDeltaStep ) //reset physics step size to 0.0 (i.e. use System step size)
         SetPhysicsStepSize( 0.0 );
   }
}

///ODE collision callback     
void Scene::NearCallback(void *data, dGeomID o1, dGeomID o2)
{
   Scene* scene = (Scene*)data;
   
   Physical* p1 = (Physical*)dGeomGetData(o1);
   Physical* p2 = (Physical*)dGeomGetData(o2);
             
   dContactGeom contactGeoms[8];
   
   int numContacts = 
      dCollide(o1, o2, 8, contactGeoms, sizeof(dContactGeom));
   
   if(numContacts > 0 && p1 != NULL && p2 != NULL)
   {
      CollisionData cd;
      
      cd.mBodies[0] = p1;
      cd.mBodies[1] = p2;
      
      sgSetVec3(
         cd.mLocation, 
         contactGeoms[0].pos[0], contactGeoms[0].pos[1], contactGeoms[0].pos[2]
      );
      
      sgSetVec3(
         cd.mNormal, 
         contactGeoms[0].normal[0], contactGeoms[0].normal[1], contactGeoms[0].normal[2]
      );
      
      cd.mDepth = contactGeoms[0].depth;
      
      scene->SendMessage("collision", &cd);
      
      if(p1->DynamicsEnabled() || p2->DynamicsEnabled())
      {
         dContact contact;
      
         for(int i=0;i<numContacts;i++)
         {
            contact.surface.mode = dContactMu2 | dContactBounce;
            contact.surface.mu = 1000.0;
            contact.surface.mu2 = 1000.0;
            contact.surface.bounce = 0.75;
            contact.surface.bounce_vel = 0.001;
            
            contact.geom = contactGeoms[i];
       
            if(p1->FilterContact(&contact, p2) && p2->FilterContact(&contact, p1))
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

void Scene::AddLight( Light* light )
{
   light->SetSceneParent( this );
   light->SetEnabled( true );
 
   mLightGroup->addChild( light->GetOSGLightSource() ); //add to a group that is alraedy a child of the scene
   mLights[ light->GetNumber() ] = light; //add to internal array of lights
  
}

void Scene::RemoveLight( Light* light )
{
   for( int i = 0; i < MAX_LIGHTS; i++ )
   {
      if( mLights[ i ] == light )
      {
         mLightGroup->removeChild( mLights[ i ]->GetOSGLightSource() );

         light->SetEnabled( false );

         mLights[ i ] = NULL;
      }
   }
}

/*
Light* Scene::GetLight( const std::string name ) const
{
   for( int i = 0; i < MAX_LIGHTS; i++ )
   {
      if( mLights[ i ]->GetName() == name )
      {
         return mLights[ i ];
      }
   }

   return NULL;
}
*/

void Scene::UseSceneLight( bool lightState )
{
   osg::Light* osgLight = mLights[ 0 ]->GetOSGLightSource()->getLight();

   if(lightState)
   {
      GetSceneHandler()->GetSceneView()->setLightingMode( osgUtil::SceneView::SKY_LIGHT );
      GetSceneHandler()->GetSceneView()->getGlobalStateSet()->setAssociatedModes( osgLight, osg::StateAttribute::ON );
   }
   else
   {
      GetSceneHandler()->GetSceneView()->setLightingMode( osgUtil::SceneView::NO_SCENEVIEW_LIGHT );
      GetSceneHandler()->GetSceneView()->getGlobalStateSet()->setAssociatedModes( osgLight, osg::StateAttribute::OFF );
   }
}
