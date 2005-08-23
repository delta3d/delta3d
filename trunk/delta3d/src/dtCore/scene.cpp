// scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////

#include <osg/FrameStamp>
#include <osgUtil/IntersectVisitor>
#include <osg/PolygonMode>

#include "dtCore/scene.h"
#include "dtCore/system.h"
#include <dtUtil/log.h>
#include "dtCore/infinitelight.h"
#include "dtCore/positionallight.h"
#include "dtCore/deltadrawable.h"
#include "dtCore/physical.h"


using namespace dtCore;
using namespace dtUtil;
using namespace std;

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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Scene::Scene( const string& name, bool useSceneLight )
: Base(name), mPhysicsStepSize(0.0)
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
   while (GetNumberOfAddedDrawable()>0)
   {
      DeltaDrawable *d = GetDrawable(0);
      if (d)
      {
         RemoveDrawable(d);
      }
   }

   DeregisterInstance(this);

   dJointGroupDestroy(mContactJointGroupID);
   dSpaceDestroy(mSpaceID);
   dWorldDestroy(mWorldID);

   RemoveSender( System::Instance() );
}

void Scene::AddDrawable( DeltaDrawable *drawable )
{
   // This is modified to put a ref_ptr in the scene
   // They are required or when you change the stateset
   // Everything is killed and you get a blank screen.
   // I still pushback the original *drawable 
   // so remove will still work
   RefPtr<DeltaDrawable> drawme = drawable;
   mSceneNode->addChild( drawme->GetOSGNode() );
   drawable->AddedToScene(this);

   mAddedDrawables.push_back(drawable);
   /* 
   // this is the original code
   mSceneNode->addChild( drawable->GetOSGNode() );
   drawable->AddedToScene(this);

   mAddedDrawables.push_back(drawable);
   */
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
   if (physical==0) return;

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

   else if (data->message == "exit")
   {
      while (GetNumberOfAddedDrawable()>0)
      {
         DeltaDrawable *d = GetDrawable(0);
         if (d)
         {
            RemoveDrawable(d);
         }
      }
   }
}

// ODE collision callback     
void Scene::NearCallback(void *data, dGeomID o1, dGeomID o2)
{
   Scene* scene = (Scene*)data;
   
   Physical* p1 = (Physical*)dGeomGetData(o1);
   Physical* p2 = (Physical*)dGeomGetData(o2);
             
   dContactGeom contactGeoms[8];
   
   int numContacts = 
      dCollide(o1, o2, 8, contactGeoms, sizeof(dContactGeom));
   
   if(numContacts > 0 && p1 != 0 && p2 != 0)
   {
      CollisionData cd;
      
      cd.mBodies[0] = p1;
      cd.mBodies[1] = p2;
    
      cd.mLocation.set( 
         contactGeoms[0].pos[0], contactGeoms[0].pos[1], contactGeoms[0].pos[2]
      );
      
      cd.mNormal.set( 
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
