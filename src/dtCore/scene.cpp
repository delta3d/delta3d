// scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix-src.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h> //due to include of scene.h

#include <osg/FrameStamp>
#include <osg/PolygonMode>
#include <osgDB/DatabasePager>
#include <osgParticle/ParticleSystemUpdater>
#include <osgUtil/IntersectVisitor>
#include <osg/Group>
#include <osgViewer/View>

#include <dtCore/camera.h>
#include <dtCore/infinitelight.h>
#include <dtCore/light.h>
#include <dtCore/system.h>
#include <dtCore/odecontroller.h>

#include <dtUtil/configproperties.h>

#include <cassert>

using namespace dtUtil;

namespace dtCore
{



IMPLEMENT_MANAGEMENT_LAYER(Scene)
/////////////////////////////////////////////
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
/////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
Scene::Scene( const std::string& name) : Base(name),
   mPhysicsController(NULL/*new ODEController()*/),
   mSceneNode(new osg::Group),
   mLights(MAX_LIGHTS),
   mRenderMode(POINT),
   mRenderFace(FRONT)
{
   mPhysicsController = new ODEController(this);
   Ctor();
}

//////////////////////////////////////////////////////////////////////////
Scene::Scene(dtCore::ODEController *physicsController, const std::string &name) : Base(name),
   mPhysicsController(physicsController),
   mSceneNode(new osg::Group),
   mLights(MAX_LIGHTS),
   mRenderMode(POINT),
   mRenderFace(FRONT)
{
   Ctor();
}

/////////////////////////////////////////////
Scene::~Scene()
{
   // Remove the remaining DeltaDrawables from the Scene. This is redundant to help prevent
   // crash-on-exits. The "exit" message should have the same effect. This must be called
   // after the above code that destroys the ODE world.
   RemoveAllDrawables();

   DeregisterInstance(this);

   RemoveSender( &System::GetInstance() );
}


//////////////////////////////////////////////////////////////////////////
void Scene::Ctor()
{
   RegisterInstance(this);
   mSceneNode->setName(GetName());

   InfiniteLight* skyLight = new InfiniteLight( 0, "SkyLight" );

   AddDrawable( skyLight );
   skyLight->SetEnabled( true );

   AddSender(&System::GetInstance());

   //TODO set default render face, mode
}

/////////////////////////////////////////////
void Scene::SetSceneNode(osg::Group* newSceneNode)
{
   //remove all children from our current scene node
   //and add them to the new scene node

//   osg::Group * sceneData = mOsgViewerScene->getSceneData()->asGroup();
   unsigned numChildren = mSceneNode->getNumChildren();

   for(unsigned i = 0; i < numChildren; ++i)
   {
      osg::Node* child = mSceneNode->getChild(i);
      newSceneNode->addChild(child);
   }
   mSceneNode->removeChildren(0, numChildren);

   mSceneNode = newSceneNode;
   UpdateViewSet();

   //now we need to remove and re-add all the drawables
   DrawableList dl = mAddedDrawables;
   RemoveAllDrawables();

   DrawableList::iterator iterEnd = dl.end();
   for(DrawableList::iterator iter = dl.begin(); iter != iterEnd; ++iter)
   {
      AddDrawable((*iter).get());
   }

}
/////////////////////////////////////////////
void Scene::AddDrawable( DeltaDrawable *drawable )
{
   // This is modified to put a RefPtr in the scene
   // They are required or when you change the stateset
   // Everything is killed and you get a blank screen.
   // I still pushback the original *drawable
   // so remove will still work
   RefPtr<DeltaDrawable> drawme = drawable;
   mSceneNode->addChild(drawme->GetOSGNode());
   drawable->AddedToScene(this);

   mAddedDrawables.push_back(drawable);

   if (mPager.valid())
   {
      mPager->RegisterDrawable( *drawable );
   }
}
/////////////////////////////////////////////
void Scene::RemoveDrawable(DeltaDrawable *drawable)
{
   mSceneNode->removeChild( drawable->GetOSGNode() );
   drawable->AddedToScene(NULL);

   unsigned int pos = GetDrawableIndex(drawable);
   if (pos<mAddedDrawables.size())
   {
      mAddedDrawables.erase( mAddedDrawables.begin()+pos );
   }
}
/////////////////////////////////////////////
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
/////////////////////////////////////////////
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

   osg::StateSet *stateSet = mSceneNode->getOrCreateStateSet();
   stateSet->setAttributeAndModes(polymode.get(),osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
}


/////////////////////////////////////////////
void Scene::RegisterCollidable(Transformable* collidable) const
{
   if (mPhysicsController.valid())
   {
      mPhysicsController->RegisterCollidable(collidable);
   }
}

/////////////////////////////////////////////
void Scene::UnRegisterCollidable(Transformable* collidable) const
{
   if (mPhysicsController.valid())
   {
      mPhysicsController->UnRegisterCollidable(collidable);
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
/////////////////////////////////////////////
void Scene::SetGravity(const osg::Vec3& gravity) const
{
   if (mPhysicsController.valid()) 
   { 
      mPhysicsController->SetGravity(gravity);
   }
}

//////////////////////////////////////////////////////////////////////////
void Scene::GetGravity(osg::Vec3& vec) const
{
   if (mPhysicsController.valid()) 
   { 
      vec = mPhysicsController->GetGravity();
   }
}

//////////////////////////////////////////////////////////////////////////
osg::Vec3 Scene::GetGravity() const
{
   if (mPhysicsController.valid()) 
   { 
      return mPhysicsController->GetGravity();
   }
   else
   {
      return osg::Vec3(0.f, 0.f, 0.f);
   }
}

//////////////////////////////////////////////////////////////////////////
void Scene::GetGravity(float &x, float &y, float &z) const
{
   if (mPhysicsController.valid()) 
   { 
      osg::Vec3 grav = mPhysicsController->GetGravity();
      x = grav[0]; y = grav[1]; z = grav[2];
   }
}

/////////////////////////////////////////////
// Get the ODE space ID
dSpaceID Scene::GetSpaceID() const
{
   if (mPhysicsController.valid()) 
   {
      return mPhysicsController->GetSpaceID();
   }
   else
   {
      return 0;
   }
}

/////////////////////////////////////////////
// Get the ODE world ID
dWorldID Scene::GetWorldID() const
{
   if (mPhysicsController.valid()) 
   { 
      return mPhysicsController->GetWorldID();
   }
   else
   {
      return 0;
   }
}

/////////////////////////////////////////////
// Get the ODE contact join group ID
dJointGroupID Scene::GetContactJoinGroupID() const
{
   if (mPhysicsController.valid()) 
   { 
      return mPhysicsController->GetContactJoinGroupID();
   }
   else
   {
      return 0;
   }
}
/////////////////////////////////////////////
// Performs collision detection and updates physics
void Scene::OnMessage(MessageData *data)
{
   if(data->message == "postframe")
   {
   }
   else if(data->message == "preframe")
   {
      double dt = *static_cast<double*>(data->userData);
      if (mPhysicsController.valid())
      {
         mPhysicsController->Iterate(dt);
      }
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

/////////////////////////////////////////////
/** The supplied function will be used instead of the built-in collision
 *  callback.
 * @param func : The function to handle collision detection
 * @param data : A void pointer to user data.  This gets passed directly to func.
 */
void Scene::SetUserCollisionCallback(dNearCallback* func, void* data) const
{
   if (mPhysicsController.valid())
   {
      mPhysicsController->SetUserCollisionCallback(func, data);
   }
}

/////////////////////////////////////////////
template< typename T >
struct HasName : public std::binary_function< dtCore::RefPtr<T>, std::string, bool >
{
   bool operator()( const dtCore::RefPtr<T>& obj, const std::string& name ) const
   {
      if( obj.valid() )
      {
         return obj->GetName() == name;
      }
      else
      {
         return false;
      }
   }
};
/////////////////////////////////////////////
Light* Scene::GetLight( const std::string& name )
{
   LightVector::iterator found = std::find_if(  mLights.begin(),
                                                mLights.end(),
                                                std::bind2nd( HasName<Light>(), name ) );

   if( found != mLights.end() )
   {
      return found->get();
   }
   else
   {
      return NULL;
   }
}
/////////////////////////////////////////////
const Light* Scene::GetLight( const std::string& name ) const
{
   LightVector::const_iterator found = std::find_if(  mLights.begin(),
                                                      mLights.end(),
                                                      std::bind2nd( HasName<Light>(), name ) );

   if( found != mLights.end() )
   {
      return found->get();
   }
   else
   {
      return NULL;
   }
}

/////////////////////////////////////////////
///Get the index number of the supplied drawable
unsigned int Scene::GetDrawableIndex( const DeltaDrawable* drawable ) const
{
   for( unsigned int childNum = 0; childNum < mAddedDrawables.size(); ++childNum )
   {
      if( mAddedDrawables[childNum] == drawable )
      {
         return childNum;
      }
   }

   return mAddedDrawables.size(); // node not found.
}
/////////////////////////////////////////////
///registers a light using the light number
void Scene::RegisterLight( Light* light )
{
   mLights[ light->GetNumber() ] = light; //add to internal array of lights
}
/////////////////////////////////////////////
///unreferences the current light, by number, Note: does not erase
void Scene::UnRegisterLight( Light* light )
{
   mLights[ light->GetNumber() ] = NULL;
}

/////////////////////////////////////////////
void Scene::UseSceneLight( bool lightState )
{
   if (mLights[0] == NULL && lightState)
   {
      InfiniteLight *skyLight = new InfiniteLight( 0, "SkyLight" );
      AddDrawable(skyLight);
      skyLight->SetEnabled(true);
   }
   else
   {
      mLights[0]->SetEnabled(lightState);
      if (GetDrawableIndex(mLights[0].get()) == GetNumberOfAddedDrawable())
         AddDrawable(mLights[0].get());
   }
}


/////////////////////////////////////////////////////////////////////////////
void Scene::RemoveView(dtCore::View& view)
{
   mViewSet.remove(&view);
}

/////////////////////////////////////////////////////////////////////////////
void Scene::AddView(dtCore::View& view)
{
   mViewSet.push_back(&view);
}

/////////////////////////////////////////////////////////////////////////////
void Scene::UpdateViewSet()
{
   if (mViewSet.empty() == false)
   {
      ViewSet::iterator it, end = mViewSet.end();
      for (it = mViewSet.begin(); it != end; ++it)
      {
         if (it->valid())
         {
            (*it)->UpdateFromScene();
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
bool Scene::IsAssignedToView(dtCore::View& view) const
{
   ViewSet::const_iterator it, end = mViewSet.end();
   for (it = mViewSet.begin(); it != end; ++it)
   {
      if (it->get() == &view)
      {
         return true;
      }
   }
   return false;
}

/////////////////////////////////////////////////////////////////////////////
dtCore::DatabasePager* Scene::GetDatabasePager() const
{
   return mPager.get();
}

/////////////////////////////////////////////////////////////////////////////
void Scene::SetDatabasePager( dtCore::DatabasePager *pager )
{
   mPager = pager;
   if (mPager.valid())
   {
      mPager->GetOsgDatabasePager()->registerPagedLODs(GetSceneNode());
   }
}

double Scene::GetPhysicsStepSize() const
{
   if (mPhysicsController.valid())
   {
      return mPhysicsController->GetPhysicsStepSize();
   }
   else
   {
      return 0.0;
   }
}

void Scene::SetPhysicsStepSize(double stepSize) const
{
   if (mPhysicsController.valid())
   {
      mPhysicsController->SetPhysicsStepSize(stepSize);
   }
}


}

