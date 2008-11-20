#include <prefix/dtcoreprefix-src.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h> //due to including scene.h
#include <dtCore/keyboardmousehandler.h> //due to including scene.h
#include <dtUtil/log.h>
#include <osg/Node>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(DeltaDrawable)

DeltaDrawable::DeltaDrawable(const std::string& name)
   : Base(name)
   , mParent(NULL)
   , mParentScene(NULL)
   , mIsActive(true)
{
   RegisterInstance(this);
}

DeltaDrawable::~DeltaDrawable()
{
   DeregisterInstance(this);

   // Alert all children of this parent's demise
   for(ChildList::iterator itr = mChildList.begin(); itr != mChildList.end(); ++itr)
   {
      (*itr)->OnOrphaned();
   }
}

/** This virtual method can be overwritten
 *  to perform specific functionality.  The default method will
 *  store the child in a list and set the child's parent.
 * @param child : The child to add to this Drawable
 * @return : Successfully added this child or not
 */
bool DeltaDrawable::AddChild(DeltaDrawable* child)
{
   if (!CanBeChild(child))
   {
      Log::GetInstance().LogMessage(Log::LOG_WARNING, __FILE__,
         "DeltaDrawable: '%s' cannot be added as a child to '%s'",
         child->GetName().c_str(), this->GetName().c_str());
      return false;
   }

   mChildList.push_back(child);
   child->SetParent(this);

   if (mParentScene)
   {
      child->AddedToScene(mParentScene);
   }
   return true;
}

//////////////////////////////////////////////////////////////////////////
DeltaDrawable* DeltaDrawable::GetChild(unsigned int idx)
{
   if (idx >= GetNumChildren())
   {
      return NULL;
   }

   return mChildList[idx].get();
}


//////////////////////////////////////////////////////////////////////////
const DeltaDrawable* DeltaDrawable::GetChild(unsigned int idx) const
{
   if (idx >= GetNumChildren())
   {
      return NULL;
   }

   return mChildList[idx].get();
}

/*!
* Remove a child from this DeltaDrawable.  This will detach the child from its
* parent so that its free to be repositioned on its own.
*
* @param *child : The child DeltaDrawable to be removed
*/
void DeltaDrawable::RemoveChild(DeltaDrawable* child)
{
   if (!child) return;

   if (child->GetParent() != this && child->GetParent() != NULL) return;

   unsigned int pos = GetChildIndex(child);
   if (pos < mChildList.size())
   {
      child->SetParent(NULL);
      child->AddedToScene(NULL);
      mChildList.erase(mChildList.begin() + pos);
   }
}

/**
 * Return a value between
 * 0 and the number of children-1 if found, if not found then
 * return the number of children.
 */
unsigned int DeltaDrawable::GetChildIndex(const DeltaDrawable* child) const
{
   for (unsigned int childNum = 0; childNum < mChildList.size(); ++childNum)
   {
      if (mChildList[childNum] == child)
      {
         return childNum;
      }
   }

   return mChildList.size(); // node not found.
}

/*!
 * Check to see if the supplied DeltaDrawable can be a child to this instance.
 * To be valid, it can't already have a parent, can't be this instance, and
 * can't be the parent of this instance.
 *
 * @param *child : The candidate child to be tested
 *
 * @return bool  : True if it can be a child, false otherwise
 */
bool DeltaDrawable::CanBeChild(DeltaDrawable* child) const
{
   if (child->GetParent() != NULL)
   {
      return false;
   }
   if (this == child)
   {
      return false;
   }

   //loop through parent's parents and make sure they're not == child
   RefPtr<const DeltaDrawable> t = GetParent();
   while (t != NULL)
   {
      if (t == child)
      {
         return false;
      }
      t = t->GetParent();
   }

   return true;
}

void DeltaDrawable::RenderProxyNode(bool enable)
{
   if (!mProxyNode.valid())
   {
      LOG_WARNING("Proxy node is not implemented, overwrite RenderProxyNode." );
   }
}

/**
 * Notifies this drawable object that it has been added to
 * a scene.  This is typically called from Scene::AddDrawable().
 *
 * This method will iterate through the list of children DeltaDrawable's (if any)
 * and call AddedToScene() with the supplied Scene.
 *
 * @param scene the scene to which this drawable object has
 * been added.  Note: Can be NULL.
 */
void DeltaDrawable::AddedToScene(Scene* scene)
{
   if (mParentScene == scene) { return; } //nothing to do here.

   mParentScene = scene;

   for (ChildList::iterator itr = mChildList.begin();
        itr != mChildList.end();
        ++itr)
   {
      (*itr)->AddedToScene(scene);
   }

   // If we've been set to inactive before being added to a Scene,
   // then we need to do add in our Switch node.  If we've just been
   // removed from a Scene and we're inactive, then we should remove our Switch
   if (mParentScene != NULL)
   {
      if (GetActive() == false)
      {
         InsertSwitchNode();
      }
   }
   else
   {
      //we've just been removed from a scene
      if (GetActive() == false)
      {
         RemoveSwitchNode();
      }
   }
}

/** Remove this DeltaDrawable from it's parent DeltaDrawable if it has one.
  * Each DeltaDrawable may have only one parent and it must be removed from
  * it's parent before adding it as a child to another.
  * @see RemoveChild()
  */
void DeltaDrawable::Emancipate()
{
   if (mParent != NULL)
   {
      mParent->RemoveChild(this);
   }
}


void DeltaDrawable::SetProxyNode(osg::Node* proxyNode)
{
   mProxyNode = proxyNode;
}

void DeltaDrawable::OnOrphaned()
{
   mParent = NULL;
}

void DeltaDrawable::GetBoundingSphere(osg::Vec3* center, float* radius)
{
   osg::Node* node = GetOSGNode();
   if (node != NULL)
   {
      osg::BoundingSphere bs = node->getBound();
      center->set(bs.center());
      *radius = bs.radius();
   }
   else
   {
      LOG_WARNING("Can't calculate Bounding Sphere, there is no geometry associated with this DeltaDrawable");
   }
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawable::SetActive(bool enable)
{
   if (mIsActive == enable)
   {
      return; //nothing to do here
   }

   mIsActive = enable;

   if (mParentScene == NULL)
   {
      // if we haven't been added to a Scene yet, then we are already effectively
      // inactive.  Once we get added to a Scene, we'll make sure we remain inactive.
      return;
   }

   if (mIsActive == false)
   {
      InsertSwitchNode();
   }
   else
   {
      RemoveSwitchNode();
   }
}

//////////////////////////////////////////////////////////////////////////
bool DeltaDrawable::GetActive() const
{
   return mIsActive;
}

//////////////////////////////////////////////////////////////////////////
void DeltaDrawable::InsertSwitchNode()
{
   // save off all parents of the Node
   osg::Node::ParentList parents = GetOSGNode()->getParents();

   // remove the Node from all its parents
   osg::Node::ParentList::iterator parentItr = parents.begin();
   while (parentItr != parents.end())
   {
      (*parentItr)->removeChild(GetOSGNode());
      ++parentItr;
   }

   osg::ref_ptr<osg::Switch> parentSwitch = new osg::Switch();
   parentSwitch->setAllChildrenOff();

   // add the Node as a child of parentSwitch
   parentSwitch->addChild(GetOSGNode());

   // add parentSwitch to all of the Node's parents
   parentItr = parents.begin();
   while (parentItr != parents.end())
   {
      (*parentItr)->addChild(parentSwitch.get());
      ++parentItr;
   }
}

//////////////////////////////////////////////////////////////////////////
void dtCore::DeltaDrawable::RemoveSwitchNode()
{
   osg::Node* node = GetOSGNode();
   if (node == NULL) { return; }  // no geometry?

   if (node->getNumParents() == 0) { return; }  // no parents?

   osg::Switch* parentSwitch = dynamic_cast<osg::Switch*>(node->getParent(0));
   if (parentSwitch == NULL) { return; }

   // save off all parents of the Switch Node
   osg::Node::ParentList parents = parentSwitch->getParents();

   // remove the Switch node from all its parents
   osg::Node::ParentList::iterator parentItr = parents.begin();
   while (parentItr != parents.end())
   {
      (*parentItr)->removeChild(parentSwitch);
      ++parentItr;
   }

   // Add the Node as a child to what was the Switch node's parents
   parentItr = parents.begin();
   while (parentItr != parents.end())
   {
      (*parentItr)->addChild(GetOSGNode());
      ++parentItr;
   }
}
