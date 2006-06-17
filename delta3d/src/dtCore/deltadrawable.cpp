#include <dtCore/deltadrawable.h>
#include <dtCore/scene.h>
#include <dtUtil/log.h>
#include <osg/Node>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(DeltaDrawable)

DeltaDrawable::DeltaDrawable(const std::string& name)
:  Base(name),
   mParent(0), 
   mParentScene(0),
   mProxyNode(0)
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
      Log::GetInstance().LogMessage( Log::LOG_WARNING, __FILE__, 
         "DeltaDrawable: '%s' cannot be added as a child to '%s'",
         child->GetName().c_str(), this->GetName().c_str() );
      return (false);
   }

   mChildList.push_back(child);
   child->SetParent(this);

   if( mParentScene )
   {
      child->AddedToScene(mParentScene);
   }
   return true;
}

/*!
* Remove a child from this DeltaDrawable.  This will detach the child from its
* parent so that its free to be repositioned on its own.
*
* @param *child : The child DeltaDrawable to be removed
*/
void DeltaDrawable::RemoveChild(DeltaDrawable *child)
{
   if (!child) return;
   
   if ( child->GetParent()!=this && child->GetParent()!=NULL ) return;

   unsigned int pos = GetChildIndex( child );
   if (pos < mChildList.size())
   {
      child->SetParent(NULL);
      child->AddedToScene(NULL);
      mChildList.erase( mChildList.begin()+pos );
   }
}

/** 
 * Return a value between
 * 0 and the number of children-1 if found, if not found then
 * return the number of children.
 */
unsigned int DeltaDrawable::GetChildIndex( const DeltaDrawable* child ) const
{
   for( unsigned int childNum = 0; childNum < mChildList.size(); ++childNum )
   {
      if( mChildList[childNum] == child )
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

void DeltaDrawable::RenderProxyNode( bool enable )
{  
   if( mProxyNode == 0 )
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
void DeltaDrawable::AddedToScene( Scene *scene )
{
   //TODO Should DeltaDrawable remove itself from it's existing parent Scene?
   mParentScene = scene;

   for(  ChildList::iterator itr = mChildList.begin();
         itr != mChildList.end();
         ++itr)
   {
      (*itr)->AddedToScene(scene);
   }
}

/** Remove this DeltaDrawable from it's parent DeltaDrawable if it has one.
  * Each DeltaDrawable may have only one parent and it must be removed from
  * it's parent before adding it as a child to another.
  * @see RemoveChild()
  */
void DeltaDrawable::Emancipate() 
{
   if( mParent )
   {
      mParent->RemoveChild(this);
   }
}


void DeltaDrawable::SetProxyNode( osg::Node* proxyNode )
{
   mProxyNode = proxyNode;
}
