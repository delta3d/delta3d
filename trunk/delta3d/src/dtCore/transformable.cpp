#include "dtCore/scene.h"
#include "dtCore/transformable.h"
#include "dtCore/notify.h"
#include "dtUtil/matrixutil.h"

#include "dtCore/pointaxis.h"

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/PolygonOffset>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(Transformable)

Transformable::Transformable()
{
   RegisterInstance(this);
   mNode = new osg::MatrixTransform();
   mNode->setName("Transformable");

   SetNormalRescaling( true );
}

Transformable::~Transformable()
{
   DeregisterInstance(this);
}


/** Calculates the world coordinate system matrix using the supplied node.
 * @param node : the node to calculate the world coordinate matrix from
 * @param wcMat : The supplied matrix to return with world coordinates
 * @return successfully or not
 */
bool Transformable::GetAbsoluteMatrix( osg::Node *node, osg::Matrix& wcMatrix )
{
   getWCofNodeVisitor vis( wcMatrix );

   node->accept( vis );

   return true;
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
void Transformable::SetTransform( Transform *xform, CoordSysEnum cs )
{
   osg::Matrix newMat;
   xform->Get( newMat );

   if (cs == ABS_CS)
   {
      //convert the xform into a Relative CS as the MatrixNode is always
      //in relative coords

      //if this has a parent
      if (mParent.valid())
      {
         //get the parent's world position
         osg::Matrix parentMat;
         GetAbsoluteMatrix( mParent->GetOSGNode(), parentMat );

         //calc the difference between xform and the parent's world position
         //child * parent^-1

         osg::Matrix::inverse(parentMat);
         osg::Matrix relMat = newMat * parentMat;

         //pass the rel matrix to this node
         GetMatrixNode()->setMatrix( relMat );
      }
      else 
      {
         //pass the xform to the this node
         GetMatrixNode()->setMatrix( newMat );
      }
   }
   else if (cs == REL_CS)
   {
     GetMatrixNode()->setMatrix( newMat );
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
   osg::Matrix newMat;

   if (cs ==ABS_CS)
   { 
     GetAbsoluteMatrix( GetMatrixNode(), newMat );     
   }
   else if (cs == REL_CS)
   {
     newMat = GetMatrixNode()->getMatrix();
   }

   xform->Set( newMat );
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
 * @return : successfully added the child or not
 * @see SetTransform()
 * @see RemoveChild()
 */
bool Transformable::AddChild(DeltaDrawable *child)
{
   bool success = DeltaDrawable::AddChild(child);

   //add the child's node to our's
   if (success) 
   {
      GetMatrixNode()->addChild( child->GetOSGNode() );
      return (true);
   }
   else return (false);

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
   GetAbsoluteMatrix( child->GetOSGNode(), absMat );
   GetMatrixNode()->removeChild( child->GetOSGNode() );
   DeltaDrawable::RemoveChild(child);
}

void Transformable::RenderProxyNode( const bool enable )
{
   if( enable )
   {
      //make sphere
      float radius = 0.5f;
      osg::Matrix relMat = GetMatrixNode()->getMatrix();

      osg::Sphere* sphere = new osg::Sphere(  osg::Vec3( 0.0, 0.0, 0.0 ), radius );

      osg::Geode* proxyGeode = new osg::Geode();
      mProxyNode = proxyGeode;

      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio( 0.5f );

      osg::ShapeDrawable* sd = new osg::ShapeDrawable( sphere, hints );

      proxyGeode->addDrawable( sd );

      osg::Material *mat = new osg::Material();
      mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f) );
      mat->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f ) );
      mat->setEmission( osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f ) );

      osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
      polyoffset->setFactor( -1.0f );
      polyoffset->setUnits( -1.0f );

      osg::StateSet *ss = mProxyNode.get()->getOrCreateStateSet();
      ss->setAttributeAndModes( mat, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );
      ss->setMode( GL_BLEND, osg::StateAttribute::ON );
      ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
      ss->setAttributeAndModes( polyoffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON) ;

      GetMatrixNode()->addChild( mProxyNode.get() );

      PointAxis* paxis = new PointAxis();
      paxis->Enable( PointAxis::X );
      paxis->Enable( PointAxis::Y );
      paxis->Enable( PointAxis::Z );
      paxis->Enable( PointAxis::LABEL_X );
      paxis->Enable( PointAxis::LABEL_Y );
      paxis->Enable( PointAxis::LABEL_Z );

      AddChild( paxis );

   }
   else
   {
      GetMatrixNode()->removeChild( mProxyNode.get() );
      mProxyNode = NULL;
   }

   mRenderingProxy = enable;
}

void Transformable::SetNormalRescaling( const bool enable )
{
   osg::StateAttribute::GLModeValue state;

   if( enable )   state = osg::StateAttribute::ON;
   else           state = osg::StateAttribute::OFF;

   GetOSGNode()->getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, state );
}


bool Transformable::GetNormalRescaling() const
{
   osg::StateAttribute::GLModeValue state = mNode->getStateSet()->getMode( GL_RESCALE_NORMAL );

   if( state & osg::StateAttribute::ON )
      return true;
   else
      return false;
}
