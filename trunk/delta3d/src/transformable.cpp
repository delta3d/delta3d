
#include "transformable.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Transformable)

Transformable::Transformable():
mParent(NULL)
{
   RegisterInstance(this);
   mAbsTransform = new Transform();
   mRelTransform = new Transform();
}

Transformable::~Transformable()
{
   DeregisterInstance(this);
   delete(mAbsTransform);
   delete(mRelTransform);
}


/*!
 * Set position/attitude of this Transformable using the supplied Transform.
 * An optional coordinate system parameter may be supplied to diferentiate 
 * between parent relative and absolute coordinates.
 * If this Transformable has any children added, they will be updated as well.
 *
 * @param *xform : The new Transform to position this instance
 * @param cs : Optional parameter describing the coord sys of xform
 *             Defaults to ABS_CS.
 */
void Transformable::SetTransform(Transform *xform, CoordSysEnum cs )
{
   sgMat4 absMat;
   sgMat4 relMat;
   
   if (cs == ABS_CS) //absolute Transform
   {
      if (mParent.valid())
      {
         //if we have a parent, then assume xform is an offset from the parent
         // in absulte coordinates
         Transform parentXform;
         mParent->GetTransform(&parentXform, ABS_CS);
         parentXform.Get(absMat);
         
         xform->Get(relMat);
         sgInvertMat4( relMat, absMat ); //calc the relative based on the abs
         
         mRelTransform->Set( relMat );
         mAbsTransform->Set( absMat );
      }
      else
      { 
         //otherwise, just set our absolute position
         xform->Get( absMat );
         mAbsTransform->Set( absMat );
      }
   }
   else if (cs == REL_CS) //relative to our parent (if any)
   {
      xform->Get( relMat );
      mRelTransform->Set( relMat ); //set our new relative xform
      
      //If we have a parent, use its abs xform and add in our relative xform
      // to calc our abs xform.  Otherwise, just use our current abs xform.
      if (mParent.valid())
      {
         Transform parentXform;
         mParent->GetTransform(&parentXform);
         parentXform.Get(absMat);
         mAbsTransform->Set(absMat);
      }
      else mAbsTransform->Get(absMat);
      
      sgPreMultMat4( absMat, relMat);
      mAbsTransform->Set( absMat ); //set our new abs xform
   }
   
   //If we have children, tell them they need to recalc their positions since
   //we were repositioned.
   UpdateChildrenTransforms(mAbsTransform);
}


/*!
 * Get the current position/attitude of this Transformable.
 *
 * @param *xform : The Transform to be filled in
 * @param cs : Optional parameter to select either the absolute coordinate
 *             or the parent relative coordinate (default == ABS_CS)
 */
void Transformable::GetTransform( Transform *xform, CoordSysEnum cs )
{
   sgMat4 mat;

   if (cs == ABS_CS)      mAbsTransform->Get( mat );
   else if (cs == REL_CS) mRelTransform->Get( mat );

   xform->Set( mat );
}


/*!
 * Add a child to this Transformable.  This will allow the child to be 
 * repositioned whenever the parent moves.  An optional offset may be applied to
 * the child.  Any number of children may be added to a parent.
 * The child's position in relation to the parent's will not change (ie: the 
 * child will *not* snap to the parent's position) unless the offset is 
 * overriden using SetTransform() on the child.
 *
 * @param *child : The child to add to this Transformable
 *
 * @see SetTransform()
 * @see RemoveChild()
 */
void Transformable::AddChild(Transformable *child)
{
   if (!CanBeChild(child)) return;

   mChildList.push_back(child);
   child->mParent = this;

   //This ends up keeping the child positioned where it currently is located by
   //using the child's current position in relation to the parent's as an offset.
   //child->SetTransform( mAbsTransform );

   //This keeps doesn't recalc the child's parent-relative offset effectively
   //snapping the child to the parent's position.
   child->UpdateTransform(mAbsTransform);
}


/*!
 * Remove a child from this Transformable.  This will detach the child from its
 * parent so that its free to be repositioned on its own.
 *
 * @param *child : The child Transformable to be removed
 */
void Transformable::RemoveChild(Transformable *child)
{
   if (!child || child->mParent!=this) return;

   unsigned int pos = GetChildIndex( child );
   if (pos < mChildList.size())
   {
      mChildList.erase( mChildList.begin()+pos );
   }
   child->mParent = NULL;
}



/*!
 * Check to see if the supplied Transformable can be a child to this instance.
 * To be valid, it can't already have a parent, can't be this instance, and
 * can't be the parent of this instance.
 *
 * @param *child : The candidate child to be tested
 *
 * @return bool  : True if it can be a child, false otherise
 */
bool Transformable::CanBeChild(Transformable *child)
{
   if (child->GetParent()!=NULL) return false;
   if (this == child) return false;
   
   //loop through parent's parents and make sure they're not == child
   osg::ref_ptr<Transformable> t = this->GetParent();
   while (t != NULL)
   {
      if (t==child) return false;
      t = t->GetParent();
   }
   
   return true;
}



/*!
 * This method is used to make sure the relative and absolute Transforms
 * are up-to-date.  Typically, this is called from a parent Transform, indicating
 * that it moved.  The default behavior is to calculate our absolute
 * Transform based on the parent's absolute Transform and our relative
 * Transform.  After the Transforms are recalculated, any children are 
 * told to update as well.
 * Override this method to implement your own functionality.
 */
void Transformable::UpdateTransform(Transform *parentAbsXform)
{
   if (parentAbsXform != NULL)
   {
      sgMat4 absMat, relMat;
      parentAbsXform->Get(absMat);
      mRelTransform->Get( relMat );
            
      sgPreMultMat4( absMat, relMat);
      mAbsTransform->Set( absMat );

      UpdateChildrenTransforms(mAbsTransform);
   }
}

/** Update any children Transformables that might be attached.
 * @param parentAbsXform : The absolute coordinate Transform of the parent
 */
void Transformable::UpdateChildrenTransforms(Transform *parentAbsXform)
{
   if (mChildList.size() > 0)
   {
      //pass this new abs position to all the children
      ChildList::iterator itr;
      for (itr=mChildList.begin(); itr!=mChildList.end(); ++itr)
      {
         (*itr)->UpdateTransform(parentAbsXform);
      }
   }

}
