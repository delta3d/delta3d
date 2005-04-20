#include "dtCore/deltadrawable.h"
#include "dtCore/scene.h"
#include "dtCore/notify.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(DeltaDrawable)

DeltaDrawable::DeltaDrawable(std::string name)
:  Base(name), 
   mProxyNode(0), 
   mRenderingProxy(false)
{
   RegisterInstance(this);
}

DeltaDrawable::~DeltaDrawable()
{
   DeregisterInstance(this);
}

/** This virtual method can be overwritten
*  to perform specific functionality.  The default method will 
*  store the child in a list and set the child's parent.
* @param child : The child to add to this Drawable
* @return : Successfully added this child or not
*/
bool DeltaDrawable::AddChild(DeltaDrawable *child)
{
   if (!CanBeChild(child))
   {
      Notify(WARN, "DeltaDrawable: '%s' cannot be added as a child to '%s'",
             child->GetName().c_str(), this->GetName().c_str() );
      return (false);
   }

   mChildList.push_back(child);
   child->SetParent(this);

   if (mParentScene.valid())
   {
      child->AddedToScene(mParentScene.get());
   }
   return (true);
}

/*!
* Remove a child from this DeltaDrawable.  This will detach the child from its
* parent so that its free to be repositioned on its own.
*
* @param *child : The child DeltaDrawable to be removed
*/
void DeltaDrawable::RemoveChild(DeltaDrawable *child)
{
   if (!child || child->GetParent()!=this) return;

   unsigned int pos = GetChildIndex( child );
   if (pos < mChildList.size())
   {
      child->SetParent(NULL);
      child->AddedToScene(NULL);
      mChildList.erase( mChildList.begin()+pos );
   }
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
bool DeltaDrawable::CanBeChild(DeltaDrawable *child)
{
   if (child->GetParent()!=NULL) return false;
   if (this == child) return false;
   
   //loop through parent's parents and make sure they're not == child
   RefPtr<DeltaDrawable> t = this->GetParent();
   while (t != NULL)
   {
      if (t==child) return false;
      t = t->GetParent();
   }
   
   return true;
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
void DeltaDrawable::AddedToScene( Scene *scene )
{
   //TODO Should DeltaDrawable remove itself from it's existing parent Scene?
   mParentScene = scene;

   for (ChildList::iterator itr = mChildList.begin();
      itr != mChildList.end();
      ++itr)
   {
      (*itr)->AddedToScene(scene);
   }
}

Scene* DeltaDrawable::GetSceneParent()
{
   return mParentScene.get();
}

/** Remove this DeltaDrawable from it's parent DeltaDrawable if it has one.
  * Each DeltaDrawable may have only one parent and it must be removed from
  * it's parent before adding it as a child to another.
  * @see RemoveChild()
  */
void DeltaDrawable::Emancipate() 
{
   if (mParent.valid())
   {
      mParent->RemoveChild(this);
   }
}
