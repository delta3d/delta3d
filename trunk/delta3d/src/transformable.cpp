
#include "transformable.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Transformable)

Transformable::Transformable():
mParent(NULL)
{
   RegisterInstance(this);
   mRelTransform = new Transform();
}

Transformable::~Transformable()
{
   DeregisterInstance(this);
   delete(mRelTransform);
}


/*!
 * Set position/attitude of this Transformable using the supplied Transform.
 * An optional coordinate system parameter may be supplied to specify whether
 * the Transform is in relation to this Transformable's parent.  
 *
 * If the CoordSysEnum is ABS_CS,
 * then the Transformable is positioned assuming 
 * absolute world coordinates and the Transformable parent/child relative
 * position is recalculated.
 * If the CoordSysEnum is REL_CS, then the Transformable is positioned relative
 * to it's parent's Transform. (Note - if REL_CS is supplied and the Transformable
 * does not have a parent, the Transform is assumed to be an absolute world 
 * coordinate.
 *
 * @param *xform : The new Transform to position this instance
 * @param cs : Optional parameter describing the coordinate system of xform
 *             Defaults to ABS_CS.
 */
void Transformable::SetTransform(Transform *xform, CoordSysEnum cs )
{
   sgMat4 newMat;
   xform->Get( newMat );

   if (cs == ABS_CS) //absolute Transform
   {  
      if (mParent.valid())
      {
         //calculate and save our new relative Transform using the parent's 
         // absolute Transform and the new Transform supplied
         Transform parentXform;
         sgMat4 parentMat;
         mParent->GetTransform(&parentXform);
         parentXform.Get(parentMat);

         sgInvertMat4(parentMat);

         sgMat4 relMat;
         sgMultMat4(relMat, parentMat, newMat);

         mRelTransform->Set(relMat); //store new rel xform
      }
      else
      {
         //no parent - just save the new Transform as our new relative Transform
         mRelTransform->Set(newMat);
      }
   }
   else if (cs == REL_CS) //relative to our parent
   {
      mRelTransform->Set( newMat );           
   } 
}

/** Calculate this Transformable's absolute Transform and store it in xform.
  *
  * @param xform : the Transform to save the calculated absolute Transform
  */
void Transformable::CalcAbsTransform( Transform *xform )
{
   if (mParent.valid())
   {
      mParent.get()->CalcAbsTransform(xform);
      sgMat4 parentMat;
      xform->Get(parentMat);

      sgMat4 relMat;
      mRelTransform->Get(relMat);

      sgMat4 newMat;
      sgMultMat4(newMat, parentMat, relMat);

      xform->Set(newMat);  
   }
   else
   { 
      //if no parent, just use the relative xform as the absolute position
      sgMat4 absMat;
      mRelTransform->Get(absMat);
      xform->Set(absMat);
   }
}

/*!
 * Get the current Transform of this Transformable.
 *
 * @param *xform : The Transform to be filled in
 * @param cs : Optional parameter to select either the absolute world coordinate
 *             or the parent relative coordinate (default == ABS_CS)
 */
void Transformable::GetTransform( Transform *xform, CoordSysEnum cs )
{
   sgMat4 mat;

   if (cs == ABS_CS) 
   {     
      if (mParent.valid())
      {
         Transform absXform;
         CalcAbsTransform( &absXform );
         absXform.Get(mat);
      }
      else
      {
         mRelTransform->Get(mat);
      }
   }
   else if (cs == REL_CS)
   {     
      mRelTransform->Get(mat);
   } 

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
 * @return bool  : True if it can be a child, false otherwise
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
