#include "dtCore/scene.h"
#include "dtCore/transformable.h"
#include "dtCore/notify.h"

#include "dtUtil/dtUtil.h"

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Transformable)




Transformable::Transformable()
{
   RegisterInstance(this);
   mNode = new osg::MatrixTransform();
   mNode->setName("Transformable");
}

Transformable::~Transformable()
{
   Notify(DEBUG_INFO, "Transformable: Deleting '%s'", GetName().c_str());
   DeregisterInstance(this);
}


/** Calculates the world coordinate system matrix using the supplied node.
 * @param node : the node to calculate the world coordinate matrix from
 * @param wcMat : The supplied matrix to return with world coordinates
 * @return successfully or not
 */
bool Transformable::GetAbsoluteMatrix( osg::Node *node, osg::Matrix *wcMat)
{
   if (!node || !wcMat) return false;

   osg::Node *topParent = node;

   for (;topParent->getNumParents()!=0; topParent=topParent->getParent(0) ) {}

   osg::ref_ptr<Transformable::getWCofNodeVisitor> vis = new Transformable::getWCofNodeVisitor( node );

   topParent->accept( *vis.get() );

   wcMat->set( vis->wcMatrix );

   return( vis->success );
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

   if (cs == ABS_CS)
   {
      //convert the xform into a Relative CS as the MatrixNode is always
      //in relative coords

      //if this has a parent
      if (mParent.valid())
      {
         //get the parent's world position
         osg::Matrix mat;
         GetAbsoluteMatrix( mParent->GetOSGNode(), &mat );

         //calc the difference between xform and the parent's world position
         //child * parent^-1
         sgMat4 relMat;
         sgMat4 parentMat;
         for (int i=0; i<4;i++)
         {
            for (int j=0; j<4; j++)
            {
               parentMat[i][j] = mat(i,j);
            }
         }

         sgInvertMat4(parentMat);
         sgMultMat4(relMat, newMat, parentMat);

         //pass the rel matrix to this node
         GetMatrixNode()->setMatrix( osg::Matrix((float*)relMat) );
      }
      else 
      {
         //pass the xform to the this node
         GetMatrixNode()->setMatrix( osg::Matrix((float*)newMat) );
      }
   }
   else if (cs == REL_CS)
   {
     GetMatrixNode()->setMatrix( osg::Matrix((float*)newMat) );
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
   osg::Matrix newMat;

   if (cs ==ABS_CS)
   { 
     GetAbsoluteMatrix( GetMatrixNode(), &newMat);     
   }
   else if (cs == REL_CS)
   {
     newMat = GetMatrixNode()->getMatrix();
   }

   for (int i=0; i<4;i++)
   {
      for (int j=0; j<4; j++)
      {
         mat[i][j] = newMat(i,j);
      }
   }

   xform->Set( mat );
}


/*!
 * Add a child to this Transformable.  This will allow the child to be 
 * repositioned whenever the parent moves.  An optional offset may be applied to
 * the child.  Any number of children may be added to a parent.
 * The child's position in relation to the parent's will not change (ie: the 
 * child will *not* snap to the parent's position) unless the offset is 
 * overwritten using SetTransform() on the child.
 *
 * @param *child : The child to add to this Transformable
 *
 * @see SetTransform()
 * @see RemoveChild()
 */
void Transformable::AddChild(DeltaDrawable *child)
{
   DeltaDrawable::AddChild(child);

   //add the child's node to our's
   GetMatrixNode()->addChild( child->GetOSGNode() );
}

/*!
 * Remove a child from this Transformable.  This will detach the child from its
 * parent so that its free to be repositioned on its own.
 *
 * @param *child : The child Transformable to be removed
 */
void Transformable::RemoveChild(DeltaDrawable *child)
{
   osg::Matrix absMat;
   bool success = GetAbsoluteMatrix( child->GetOSGNode(), &absMat );
   GetMatrixNode()->removeChild( child->GetOSGNode() );
   DeltaDrawable::RemoveChild(child);
}
