// scene.cpp: implementation of the Scene class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/scene.h>

#include <osg/PolygonMode>
#include <osgParticle/ParticleSystemUpdater>

#include <dtCore/infinitelight.h>
#include <dtCore/system.h>
#include <dtCore/odecontroller.h>
#include <dtCore/databasepager.h>
#include <dtCore/view.h>
#include <dtCore/batchisector.h>
#include <dtUtil/cullmask.h>
#include <dtUtil/log.h>

using namespace dtUtil;

namespace dtCore
{

class SceneImpl
{
public:
   SceneImpl(dtCore::ODEController* physicsController)
      : mPhysicsController(physicsController)
      , mSceneNode(new osg::Group)
      , mLights(MAX_LIGHTS)
      , mRenderMode(Scene::POINT)
      , mRenderFace(Scene::FRONT)
   {
   }

   ///The physics controller to use for physics integration (can be NULL)
   dtCore::RefPtr<ODEController> mPhysicsController;

   RefPtr<osg::Group> mSceneNode; ///<This will be our Scene

   typedef std::vector< RefPtr<Light> > LightVector;
   LightVector mLights; ///<Contains all light associated with this scene

   typedef std::vector< RefPtr<DeltaDrawable> > DrawableList;
   DrawableList mAddedDrawables; ///<The list of Drawable directly added

   typedef std::list< osg::observer_ptr<View> > ViewSet;
   ViewSet mViewSet;

   Scene::Mode mRenderMode;
   Scene::Face mRenderFace;

   dtCore::RefPtr<dtCore::DatabasePager> mPager;
};


IMPLEMENT_MANAGEMENT_LAYER(Scene)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
Scene::Scene(const std::string& name)
   : DeltaDrawable(name)
   , mImpl(NULL)
{
   mImpl = new SceneImpl(new ODEController(this));
   Ctor();
}

//////////////////////////////////////////////////////////////////////////
Scene::Scene(dtCore::ODEController* physicsController, const std::string& name)
   : DeltaDrawable(name)
   , mImpl(NULL)
{
   mImpl = new SceneImpl(physicsController);
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

   RemoveSender(&System::GetInstance());
   delete mImpl;
   mImpl = NULL;
}

//////////////////////////////////////////////////////////////////////////
osg::Node* Scene::GetOSGNode()
{
   return GetSceneNode();
}

//////////////////////////////////////////////////////////////////////////
const osg::Node* Scene::GetOSGNode() const
{
   return mImpl->mSceneNode.get();
}

//////////////////////////////////////////////////////////////////////////
osg::Group* Scene::GetSceneNode()
{
   return (mImpl->mSceneNode.get());
}

//////////////////////////////////////////////////////////////////////////
const std::pair<Scene::Face, Scene::Mode> Scene::GetRenderState() const
{
   return std::make_pair(mImpl->mRenderFace, mImpl->mRenderMode);
}

//////////////////////////////////////////////////////////////////////////
void Scene::Ctor()
{
   RegisterInstance(this);
   mImpl->mSceneNode->setName(GetName());

   InfiniteLight* skyLight = new InfiniteLight(0, "SkyLight");

   AddChild(skyLight);
   skyLight->SetEnabled(true);

   AddSender(&System::GetInstance());

   // TODO set default render face, mode
}

/////////////////////////////////////////////
void Scene::SetSceneNode(osg::Group* newSceneNode)
{
   // remove all children from our current scene node
   // and add them to the new scene node

//   osg::Group* sceneData = mOsgViewerScene->getSceneData()->asGroup();
   unsigned numChildren = mImpl->mSceneNode->getNumChildren();

   for (unsigned i = 0; i < numChildren; ++i)
   {
      osg::Node* child = mImpl->mSceneNode->getChild(i);
      newSceneNode->addChild(child);
   }
   mImpl->mSceneNode->removeChildren(0, numChildren);

   mImpl->mSceneNode = newSceneNode;
   UpdateViewSet();

   //now we need to remove and re-add all the drawables
   SceneImpl::DrawableList dl = mImpl->mAddedDrawables;
   RemoveAllDrawables();

   SceneImpl::DrawableList::iterator iterEnd = dl.end();
   for (SceneImpl::DrawableList::iterator iter = dl.begin(); iter != iterEnd; ++iter)
   {
      AddChild((*iter).get());
   }
}

////////////////////////////////////////////////////////////////////////////////
bool Scene::AddChild(DeltaDrawable* child)
{
   if (child == NULL)
   {
      LOG_WARNING("A NULL DeltaDrawable was attempted to be added to the Scene");
      return false;
   }

   if (dynamic_cast<dtCore::Scene*>(child) != NULL)
   {
      LOG_WARNING("A Scene is being added as a child of a Scene!");
   }

   if (! DeltaDrawable::AddChild(child))
   {
      return false;
   }

   // This is modified to put a RefPtr in the scene
   // They are required or when you change the stateset
   // Everything is killed and you get a blank screen.
   // I still pushback the original *drawable
   // so remove will still work
   RefPtr<DeltaDrawable> drawme = child;
   mImpl->mSceneNode->addChild(drawme->GetOSGNode());
   child->AddedToScene(this);

   mImpl->mAddedDrawables.push_back(child);

   if (mImpl->mPager.valid())
   {
      mImpl->mPager->RegisterDrawable(*child);
   }

   return true;
}

/////////////////////////////////////////////
void Scene::AddDrawable(DeltaDrawable* drawable)
{
   // Deprecated Nov 19, 2010
   DEPRECATE("void dtCore::Scene::AddDrawable(DeltaDrawable* drawable)",
             "bool dtCore::Scene::AddChild(DeltaDrawable* drawable)");

   AddChild(drawable);
}

////////////////////////////////////////////////////////////////////////////////
void Scene::RemoveChild(DeltaDrawable* child)
{
   if (child == NULL)
   {
      LOG_WARNING("A NULL DeltaDrawable was attempted to be removed from the Scene");
      return;
   }

   // Activate the child to ensure that no switch node is between child and the scene-group-node.
   // With that the child can be removed properly.  This is a fix to get
   // around a problem with the DeltaDrawable's active/inactive internal structure.
   bool active = child->GetActive();
   if (!active)
   {
      child->SetActive(true);
   }

   mImpl->mSceneNode->removeChild(child->GetOSGNode());
   child->AddedToScene(NULL);
   child->RemovedFromScene(this);

   unsigned int pos = GetChildIndex(child);
   if (pos < mImpl->mAddedDrawables.size())
   {
      mImpl->mAddedDrawables.erase(mImpl->mAddedDrawables.begin() + pos);
   }

   // Restore active flag. 
   if (!active)
   {
      child->SetActive(false);
   }

   DeltaDrawable::RemoveChild(child);
}


/////////////////////////////////////////////
void Scene::RemoveDrawable(DeltaDrawable* drawable)
{
   // Deprecated Nov 19, 2010
   DEPRECATE("void dtCore::Scene::RemoveDrawable(DeltaDrawable* drawable)",
             "void dtCore::Scene::RemoveChild(DeltaDrawable* drawable)");

   RemoveChild(drawable);   
}

/////////////////////////////////////////////
void Scene::RemoveAllDrawables()
{
   while (!mImpl->mAddedDrawables.empty())
   {
      if (DeltaDrawable* d = GetChild(0))
      {
         RemoveChild(d);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
DeltaDrawable* Scene::GetDrawable(unsigned int i) const
{
   // Deprecated Nov 19, 2010
   DEPRECATE("DeltaDrawable* dtCore::Scene::GetDrawable(unsigned int i) const",
             "DeltaDrawable* dtCore::DeltaDrawable::GetChild(DeltaDrawable* drawable)");

   //return mImpl->mAddedDrawables[i].get();
   
   return (DeltaDrawable*) GetChild(i);
}

/////////////////////////////////////////////
///Get the index number of the supplied drawable
unsigned int Scene::GetDrawableIndex(const DeltaDrawable* drawable) const
{
   // Deprecated Nov 19, 2010
   DEPRECATE("unsigned int dtCore::Scene::GetDrawableIndex(const DeltaDrawable* drawable) const",
             "unsigned int GetChildIndex(const DeltaDrawable* child) const;");

   
   //for (unsigned int childNum = 0; childNum < mImpl->mAddedDrawables.size(); ++childNum)
   //{
   //   if (mImpl->mAddedDrawables[childNum] == drawable)
   //   {
   //      return childNum;
   //   }
   //}
   //return mImpl->mAddedDrawables.size(); // node not found.   

   return GetChildIndex(drawable);   
}

//////////////////////////////////////////////////////////////////////////
unsigned int Scene::GetNumberOfAddedDrawable() const
{
   return mImpl->mAddedDrawables.size();
}

//////////////////////////////////////////////////////////////////////////
void Scene::GetDrawableChildren(std::vector<dtCore::DeltaDrawable*>& children,
                                dtCore::DeltaDrawable& parent) const
{
   for (unsigned int childIdx = 0; childIdx < parent.GetNumChildren(); ++childIdx)
   {
      DeltaDrawable* child = parent.GetChild(childIdx);
      if (child != NULL)
      {
         children.push_back(child);

         GetDrawableChildren(children, *child);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
std::vector<dtCore::DeltaDrawable*> Scene::GetAllDrawablesInTheScene() const
{
   std::vector<dtCore::DeltaDrawable*> drawables;

   for (unsigned int drawableIdx = 0; drawableIdx < GetNumberOfAddedDrawable(); drawableIdx++)
   {
      DeltaDrawable* drawable = (DeltaDrawable*) GetChild(drawableIdx);
      if (drawable != NULL)
      {
         drawables.push_back(drawable);
         GetDrawableChildren(drawables, *drawable);
      }
   }
   return drawables;
}

/////////////////////////////////////////////
void Scene::SetRenderState(Face face, Mode mode)
{
   mImpl->mRenderFace = face;
   mImpl->mRenderMode = mode;

   // We need a face and a mode from osg to set the face and mode in delta
   // I used the same names to create enums in scene
   osg::PolygonMode::Face myface;
   osg::PolygonMode::Mode mymode;

   // this switch statement will take the osg face and make it the corresponding
   // scene face
   switch (mImpl->mRenderFace)
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
   switch (mImpl->mRenderMode)
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

   osg::StateSet* stateSet = mImpl->mSceneNode->getOrCreateStateSet();
   stateSet->setAttributeAndModes(polymode.get(),osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
}


/////////////////////////////////////////////
void Scene::RegisterCollidable(Transformable* collidable) const
{
   if (mImpl->mPhysicsController.valid())
   {
      mImpl->mPhysicsController->RegisterCollidable(collidable);
   }
}

/////////////////////////////////////////////
void Scene::UnRegisterCollidable(Transformable* collidable) const
{
   if (mImpl->mPhysicsController.valid())
   {
      mImpl->mPhysicsController->UnRegisterCollidable(collidable);
   }
}

/////////////////////////////////////////////
bool Scene::GetHeightOfTerrain(float& heightOfTerrain, float x, float y, float maxZ, float minZ)
{
   bool heightFound = false;

   // use an isector to calculate the height of the terrain
   {
      const osg::Vec3 start(x, y, maxZ);
      const osg::Vec3 end(x, y, minZ);
      dtCore::RefPtr<dtCore::BatchIsector> isector = new dtCore::BatchIsector(this);
      isector->EnableAndGetISector(0).SetSectorAsLineSegment(start, end);

      // set the traversal mask so we don't collide with the skybox
      isector->SetTraversalMask(dtUtil::CullMask::SCENE_INTERSECT_MASK);

      heightFound = isector->Update();

      if (heightFound)
      {
         osg::Vec3 hitPoint;
         isector->GetSingleISector(0).GetHitPoint(hitPoint);
         heightOfTerrain = hitPoint.z();
      }
   }

   return heightFound;
}

/////////////////////////////////////////////
void Scene::SetGravity(const osg::Vec3& gravity) const
{
   if (mImpl->mPhysicsController.valid())
   {
      mImpl->mPhysicsController->SetGravity(gravity);
   }
}

//////////////////////////////////////////////////////////////////////////
void Scene::GetGravity(osg::Vec3& vec) const
{
   if (mImpl->mPhysicsController.valid())
   {
      vec = mImpl->mPhysicsController->GetGravity();
   }
}

//////////////////////////////////////////////////////////////////////////
osg::Vec3 Scene::GetGravity() const
{
   if (mImpl->mPhysicsController.valid())
   {
      return mImpl->mPhysicsController->GetGravity();
   }
   else
   {
      return osg::Vec3(0.0f, 0.0f, 0.0f);
   }
}

//////////////////////////////////////////////////////////////////////////
void Scene::GetGravity(float& x, float& y, float& z) const
{
   if (mImpl->mPhysicsController.valid())
   {
      osg::Vec3 grav = mImpl->mPhysicsController->GetGravity();
      x = grav[0]; y = grav[1]; z = grav[2];
   }
}

/////////////////////////////////////////////
// Get the ODE space ID
dSpaceID Scene::GetSpaceID() const
{
   if (mImpl->mPhysicsController.valid())
   {
      return mImpl->mPhysicsController->GetSpaceID();
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
   if (mImpl->mPhysicsController.valid())
   {
      return mImpl->mPhysicsController->GetWorldID();
   }
   else
   {
      return 0;
   }
}


/////////////////////////////////////////////
// Get the ODE contact joint group ID
dJointGroupID Scene::GetContactJointGroupID() const
{
   if (mImpl->mPhysicsController.valid())
   {
      return mImpl->mPhysicsController->GetContactJointGroupID();
   }
   else
   {
      return 0;
   }
}
/////////////////////////////////////////////
// Performs collision detection and updates physics
void Scene::OnMessage(MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_POST_FRAME)
   {
   }
   else if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      double dt = *static_cast<double*>(data->userData);
      if (mImpl->mPhysicsController.valid())
      {
         mImpl->mPhysicsController->Iterate(dt);
      }
   }
   else if (data->message == dtCore::System::MESSAGE_EXIT)
   {
      RemoveAllDrawables();
   }
}

/////////////////////////////////////////////
/**
 * The supplied function will be used instead of the built-in collision
 *  callback.
 * @param func : The function to handle collision detection
 * @param data : A void pointer to user data.  This gets passed directly to func.
 */
void Scene::SetUserCollisionCallback(dNearCallback* func, void* data) const
{
   if (mImpl->mPhysicsController.valid())
   {
      mImpl->mPhysicsController->SetUserCollisionCallback(func, data);
   }
}

/////////////////////////////////////////////
dNearCallback* Scene::GetUserCollisionCallback() const
{
   return mImpl->mPhysicsController.valid() ? mImpl->mPhysicsController->GetUserCollisionCallback() : NULL;
}

/////////////////////////////////////////////
void* Scene::GetUserCollisionData()
{
   return mImpl->mPhysicsController.valid() ? mImpl->mPhysicsController->GetUserCollisionData() : NULL;
}

/////////////////////////////////////////////
const void* Scene::GetUserCollisionData() const
{
   return mImpl->mPhysicsController.valid() ? mImpl->mPhysicsController->GetUserCollisionData() : NULL;
}

/////////////////////////////////////////////
template<typename T>
struct HasName : public std::binary_function<dtCore::RefPtr<T>, std::string, bool>
{
   bool operator()(const dtCore::RefPtr<T>& obj, const std::string& name) const
   {
      if (obj.valid())
      {
         return obj->GetName() == name;
      }
      else
      {
         return false;
      }
   }
};

//////////////////////////////////////////////////////////////////////////
Light* Scene::GetLight(const int number)
{
   return mImpl->mLights[number].get();
}

//////////////////////////////////////////////////////////////////////////
const Light* Scene::GetLight(const int number) const
{
   return mImpl->mLights[number].get();
}

/////////////////////////////////////////////
Light* Scene::GetLight(const std::string& name)
{
   SceneImpl::LightVector::iterator found = std::find_if(mImpl->mLights.begin(),
                                              mImpl->mLights.end(),
                                              std::bind2nd(HasName<Light>(), name));

   if (found != mImpl->mLights.end())
   {
      return found->get();
   }
   else
   {
      return NULL;
   }
}
/////////////////////////////////////////////
const Light* Scene::GetLight(const std::string& name) const
{
   SceneImpl::LightVector::const_iterator found = std::find_if(mImpl->mLights.begin(),
                                                    mImpl->mLights.end(),
                                                    std::bind2nd(HasName<Light>(), name));

   if (found != mImpl->mLights.end())
   {
      return found->get();
   }
   else
   {
      return NULL;
   }
}


/////////////////////////////////////////////
///registers a light using the light number
void Scene::RegisterLight(Light* light)
{
   mImpl->mLights[light->GetNumber()] = light; //add to internal array of lights
}
/////////////////////////////////////////////
///unreferences the current light, by number, Note: does not erase
void Scene::UnRegisterLight(Light* light)
{
   mImpl->mLights[light->GetNumber()] = NULL;
}

/////////////////////////////////////////////
void Scene::UseSceneLight(bool lightState)
{
   if (mImpl->mLights[0] == NULL && lightState)
   {
      InfiniteLight* skyLight = new InfiniteLight(0, "SkyLight");
      AddChild(skyLight);
      skyLight->SetEnabled(true);
   }
   else
   {
      mImpl->mLights[0]->SetEnabled(lightState);

      // If the light is not already in the scene, add it
      if (GetChildIndex(mImpl->mLights[0].get()) == GetNumberOfAddedDrawable())
      {
         AddChild(mImpl->mLights[0].get());
      }
   }
}


/////////////////////////////////////////////////////////////////////////////
void Scene::RemoveView(dtCore::View& view)
{
   mImpl->mViewSet.remove(&view);
}

/////////////////////////////////////////////////////////////////////////////
void Scene::AddView(dtCore::View& view)
{
   mImpl->mViewSet.push_back(&view);
}

/////////////////////////////////////////////////////////////////////////////
void Scene::UpdateViewSet()
{
   if (mImpl->mViewSet.empty() == false)
   {
      SceneImpl::ViewSet::iterator it, end = mImpl->mViewSet.end();
      for (it = mImpl->mViewSet.begin(); it != end; ++it)
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
   SceneImpl::ViewSet::const_iterator it, end = mImpl->mViewSet.end();
   for (it = mImpl->mViewSet.begin(); it != end; ++it)
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
   return mImpl->mPager.get();
}

/////////////////////////////////////////////////////////////////////////////
void Scene::SetDatabasePager(dtCore::DatabasePager* pager)
{
   mImpl->mPager = pager;
   if (mImpl->mPager.valid())
   {
      mImpl->mPager->GetOsgDatabasePager()->registerPagedLODs(GetSceneNode());
   }
}

//////////////////////////////////////////////////////////////////////////
double Scene::GetPhysicsStepSize() const
{
   if (mImpl->mPhysicsController.valid())
   {
      return mImpl->mPhysicsController->GetPhysicsStepSize();
   }
   else
   {
      return 0.0;
   }
}

//////////////////////////////////////////////////////////////////////////
void Scene::SetPhysicsStepSize(double stepSize) const
{
   if (mImpl->mPhysicsController.valid())
   {
      mImpl->mPhysicsController->SetPhysicsStepSize(stepSize);
   }
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEController* Scene::GetPhysicsController() const
{
   return mImpl->mPhysicsController.get();
}

//////////////////////////////////////////////////////////////////////////
void Scene::ResetDatabasePager()
{
   if (mImpl->mPager.valid())
   {
      mImpl->mPager->GetOsgDatabasePager()->clear();
      mImpl->mPager->GetOsgDatabasePager()->registerPagedLODs(GetSceneNode());
   }
} 

} // namespace dtCore
