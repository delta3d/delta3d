#include "dtCore/deltadrawable.h"
#include "dtCore/scene.h"

using namespace dtCore;

DeltaDrawable::DeltaDrawable(std::string name):
Base(name)
{
}

/*!
* @param *child : The child to add to this DeltaDrawable
*
* @see RemoveChild()
*/
void DeltaDrawable::AddChild(DeltaDrawable *child)
{
   if (!CanBeChild(child)) return;

   mChildList.push_back(child);
   child->SetParent(this);

   if (mParentScene.valid())
   {
      child->AddedToScene(mParentScene.get());
   }
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
      //child->AddedToScene(NULL); ??
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
   osg::ref_ptr<DeltaDrawable> t = this->GetParent();
   while (t != NULL)
   {
      if (t==child) return false;
      t = t->GetParent();
   }
   
   return true;
}

void DeltaDrawable::AddedToScene( Scene *scene )
{
    mParentScene = scene;
}

Scene* DeltaDrawable::GetSceneParent()
{
   return mParentScene.get();
}