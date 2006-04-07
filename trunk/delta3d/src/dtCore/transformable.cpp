#include <dtCore/pointaxis.h>
#include <dtCore/scene.h>
#include <dtCore/transformable.h>
#include <dtUtil/matrixutil.h>

#include <osg/BoundingBox>
#include <osg/BoundingSphere>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osg/Transform>
#include <osg/TriangleFunctor>

#include <cassert>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Transformable)

const std::string Transformable::COLLISION_GEODE_ID("__DELTA3D_COLLISION_GEOMETRY__");

///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(Transformable::CollisionGeomType);
Transformable::CollisionGeomType
Transformable::CollisionGeomType::NONE("NONE");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::SPHERE("SPHERE");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::CYLINDER("CYLINDER");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::CUBE("CUBE");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::RAY("RAY");
Transformable::CollisionGeomType
Transformable::CollisionGeomType::MESH("MESH");

Transformable::Transformable( const std::string& name )
   :  DeltaDrawable(name),
      mGeomID(0),
      mOriginalGeomID(0),
      mTriMeshDataID(0),
      mMeshVertices(0),
      mMeshIndices(0),
      mGeomGeod(0),
      mRenderingGeometry(false)
{
   // Haxor! In Transformable we virutally derive from DeltaDrawable 
   // in order to avoid collisions in PositionalLight (diamond o' death
   // baby!). Unfortunately, the "name" paramter in the constructor
   // is not passed up the chain in this situation. Bummer. Instead
   // we have to overwrite it aftewards when we get to this constructor.
   SetName(name);

   RegisterInstance(this);
   mNode = new osg::MatrixTransform;

   SetNormalRescaling( true );

   // Setup default collision geometry stuff.
   mGeomID = dCreateGeomTransform(0); //Add support for more spaces.
   dGeomTransformSetCleanup(mGeomID, 1);
   dGeomTransformSetInfo(mGeomID, 1);
   dGeomDisable(mGeomID);
   mTriMeshDataID = dGeomTriMeshDataCreate();

   // Default collision category = 11
   SetCollisionCategoryBits( UNSIGNED_BIT(11) );

   // By default, collide with all categories.
   SetCollisionCollideBits( 0xFFFFFFFF );
}

Transformable::~Transformable()
{
   dGeomDestroy(mGeomID);

   dGeomTriMeshDataDestroy(mTriMeshDataID);

   if(mMeshVertices != 0)
   {
      delete[] mMeshVertices;
      delete[] mMeshIndices;
   }

   DeregisterInstance(this);
}

/** Calculates the world coordinate system matrix using the supplied node.
 * @param node : the node to calculate the world coordinate matrix from
 * @param wcMat : The supplied matrix to return with world coordinates
 * @return successfully or not
 */
bool Transformable::GetAbsoluteMatrix( osg::Node* node, osg::Matrix& wcMatrix )
{
   if( node != 0 )
   {
      osg::NodePathList nodePathList = node->getParentalNodePaths();

      if( !nodePathList.empty() )
      {
         osg::NodePath nodePath = nodePathList[0];

         // \TODO: This makes me feel nauseous... It would probably
         // be better to drop in a pointer to the CameraNode. This is the
         // only way I know how to get it.
         //
         // dtCore::Camera::GetSceneHandler()->GetSceneView()->getRenderStage()->getCameraNode()
         //
         //-osb
         if( std::string( nodePath[0]->className() ) == std::string("CameraNode") )
         {
            nodePath = osg::NodePath( nodePath.begin()+1, nodePath.end() );
         }

         wcMatrix.set( osg::computeLocalToWorld(nodePath) );
         return true;
      }
   }
   
   return false;
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
void Transformable::SetTransform( const Transform* xform, CoordSysEnum cs )
{
   osg::Matrix newMat;
   xform->Get(newMat);

   if( cs == ABS_CS )
   {
      //convert the xform into a Relative CS as the MatrixNode is always
      //in relative coords

      //if this has a parent
      if(mParent)
      {
         //get the parent's world position
         osg::Matrix parentMat;
         GetAbsoluteMatrix( mParent->GetOSGNode(), parentMat );

         //calc the difference between xform and the parent's world position
         //child * parent^-1
         osg::Matrix relMat = newMat * osg::Matrix::inverse(parentMat);

         //pass the rel matrix to this node
         GetMatrixNode()->setMatrix(relMat);
      }
      else 
      {
         //pass the xform to the this node
         GetMatrixNode()->setMatrix(newMat);
      }
   }
   else if( cs == REL_CS )
   {
     GetMatrixNode()->setMatrix(newMat);
   }

   PrePhysicsStepUpdate();
}

/*!
 * Get the current Transform of this Transformable.
 *
 * @param *xform : The Transform to be filled in
 * @param cs : Optional parameter to select either the absolute world coordinate
 *             or the parent relative coordinate (default == ABS_CS)
 */
void Transformable::GetTransform( Transform *xform, CoordSysEnum cs ) const
{
   osg::Matrix newMat;

   //yes, we know this sucks, but we can't have a const visitor :(
   //osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>( const_cast<osg::Node*>(mNode.get()) );
   osg::MatrixTransform* mt = const_cast<osg::MatrixTransform*>( GetMatrixNode() );

   if( cs == ABS_CS )
   { 
      GetAbsoluteMatrix( mt, newMat );     
   }
   else if( cs == REL_CS )
   {
     newMat = mt->getMatrix();
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
   // Add the child's node to our's
   if( DeltaDrawable::AddChild(child) ) 
   {
      GetMatrixNode()->addChild( child->GetOSGNode() );
      return true;
   }
   else
   {
      return false;
   }

}

/*!
 * Remove a child from this Transformable.  This will detach the child from its
 * parent so that its free to be repositioned on its own.
 *
 * @param *child : The child Transformable to be removed
 */
void Transformable::RemoveChild(DeltaDrawable *child)
{
   GetMatrixNode()->removeChild( child->GetOSGNode() );
   DeltaDrawable::RemoveChild(child);
}

void Transformable::RenderProxyNode( const bool enable )
{
   if( enable )
   {
      // Make sphere
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
      mProxyNode = 0;
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
   {
      return true;
   }
   else
   {
      return false;
   }
}

/**
* Returns the type of collision geometry associated with this
* object.
*
* @return the object's collision geometry type
*/
Transformable::CollisionGeomType* Transformable::GetCollisionGeomType() const
{
   int geomClass = dGeomGetClass(mGeomID);
   dGeomID id = mGeomID;
   while( geomClass == dGeomTransformClass && id != 0 )
   {
      id = dGeomTransformGetGeom(id);

      if( id != 0 )
      {
         geomClass = dGeomGetClass(id);
      }
   }

   switch( geomClass )
   {
   case dSphereClass :	
      return &CollisionGeomType::SPHERE;
   case dBoxClass	:
      return &CollisionGeomType::CUBE;
   case dCCylinderClass :	
      return &CollisionGeomType::CYLINDER;
   case dRayClass	:
      return &CollisionGeomType::RAY;
   case dTriMeshClass :
      return &CollisionGeomType::MESH;
   default:
      break;
   }

   return &CollisionGeomType::NONE;
}

/**
* Returns the dimensions of collision geometry associated with this
* object.
*
* @param dimensions The dimenstions of the object's collision geometry.
* What is filled into the vector is dependent on Collision type.
* CUBE     : ( lx, ly, lz )
* SPHERE   : ( radius )
* CYLINDER : ( radius, length )
* RAY      : ( length, start_x, start_y, start_z, dir_x, dir_y, dir_z )
*/
void Transformable::GetCollisionGeomDimensions( std::vector<float>& dimensions )
{  
   dimensions.clear();
   
   // Sync up ODE with our OSG transforms.
   PrePhysicsStepUpdate();

   // The while loop is repeated here because we need both the type
   // and actual dGeomID of the geometry. GetCollisionGeomType only
   // return the type.
   int geomClass = dGeomGetClass(mGeomID);
   dGeomID id = mGeomID;
   while( geomClass == dGeomTransformClass && id != 0 )
   {
      id = dGeomTransformGetGeom(id);
      if( id != 0 )
      {
         geomClass = dGeomGetClass(id);
      }
   }
   
   switch( geomClass )
   {
      case dSphereClass :
      {
         dReal rad = dGeomSphereGetRadius(id);
         dimensions.push_back( float(rad) );

         break;
      }
      case dBoxClass :
      {
         dVector3 sides;
         dGeomBoxGetLengths(id, sides);
         for( int i = 0; i < 3; i++ )
         {
            dimensions.push_back( float( sides[i] ) );
         }

         break;
      }
      case dCCylinderClass :	
      {
         dReal radius, length;
         dGeomCCylinderGetParams(id, &radius, &length);

         dimensions.push_back( float(radius) );
         dimensions.push_back( float(length) );
         break;
      }
      case dRayClass	:
      {
         dVector3 start, dir;
         dReal length = dGeomRayGetLength(id);
         dGeomRayGet(id, start, dir);

         dimensions.push_back( length );
         for( int i = 0; i < 3; i++ )
         {
            dimensions.push_back( float( start[i] ) );
         }
         for( int i = 0; i < 3; i++ )
         {
            dimensions.push_back( float( dir[i] ) );
         }
         break;
      }
      case dTriMeshClass :
      default:
         break;
   }
}

/**
* Sets whether or not collisions detection will be performed.
* Note: This does not handle collisions in any way, the user
* is still responsible for implementing collision response
* (or just use Physical).
*
* @param solid true if the Transformable is solid
*/
void Transformable::SetCollisionDetection( bool enabled )
{ 
   if( mGeomID != 0 )
   {
      
      int geomClass = dGeomGetClass(mGeomID) ;
      dGeomID id = mGeomID;
      while( geomClass == dGeomTransformClass && id != 0 )
      {
         id = dGeomTransformGetGeom(id);
         if( id != 0 )
         {
            geomClass = dGeomGetClass(id);
         }
      }
      
      //If we get here then a collision shape has been found.
      //Now we can enable/disable the GeomTransform at the top of the hierarchy.
      //Disabling the collision geometry itself will not prevent the transform
      //from colliding and still moving the collision hierarchy with it.
      if(enabled)
      {
         dGeomEnable(mGeomID);
      }
      else
      {
         dGeomDisable(mGeomID);
      }
   }
}

/**
* Gets whether or not collisions with other Transformables
* will be detected.
*
* @return true if the Transformable is solid
*/
bool Transformable::GetCollisionDetection() const 
{ 
   if( mGeomID != 0 )
   {
      return dGeomIsEnabled(mGeomID) == 1;
   }

   return false;
}

/**
* Sets this object's collision geometry to the specified ODE
* geom.
*
* @param geom the new collision geom
*/
void Transformable::SetCollisionGeom(dGeomID geom)
{
   dGeomTransformSetGeom(mGeomID, geom);

   RenderCollisionGeometry(mRenderingGeometry);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

/**
* Sets this object's collision geometry to a sphere with the
* specified radius.
*
* @param radius the radius of the collision sphere
*/
void Transformable::SetCollisionSphere(float radius)
{
   mOriginalGeomID = dCreateSphere(0, radius);
   dGeomDisable( mOriginalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateSphere(0, radius) );

   RenderCollisionGeometry(mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

/**
* A visitor that determines the parameters associated
* with a node.
*/
template< class T >
class DrawableVisitor : public osg::NodeVisitor
{
public:

   osg::TriangleFunctor<T> mFunctor;

   /**
   * Constructor.
   */
   DrawableVisitor()
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
   {}

   /**
   * Applies this visitor to a geode.
   *
   * @param node the geode to visit
   */
   virtual void apply(osg::Geode& node)
   {
      for(unsigned int i=0;i<node.getNumDrawables();i++)
      {
         osg::Drawable* d = node.getDrawable(i);

         if(d->supports(mFunctor))
         {
            osg::NodePath nodePath = getNodePath();
            // \TODO: This makes me feel nauseous... It would probably
            // be better to drop in a pointer to the CameraNode. This is the
            // only way I know how to get it.
            //
            // dtCore::Camera::GetSceneHandler()->GetSceneView()->getRenderStage()->getCameraNode()
            //
            //-osb
            if( std::string( nodePath[0]->className() ) == std::string("CameraNode") )
            {
               nodePath = osg::NodePath( nodePath.begin()+1, nodePath.end() );
            }
            mFunctor.mMatrix = osg::computeLocalToWorld(nodePath);

            d->accept(mFunctor);
         }
      }
   }
};

/**
* Determines the cylinder parameters
*/
class SphereFunctor
{
public:

   float mRadius;

   osg::Matrix mMatrix;

   /**
   * Constructor.
   */
   SphereFunctor()
      : mRadius(0.0f)
   {}

   /**
   * Called once for each visited triangle.
   *
   * @param v1 the triangle's first vertex
   * @param v2 the triangle's second vertex
   * @param v3 the triangle's third vertex
   * @param treatVertexDataAsTemporary whether or not to treat the vertex data
   * as temporary
   */
   void operator()(const osg::Vec3& v1,
      const osg::Vec3& v2,
      const osg::Vec3& v3,
      bool treatVertexDataAsTemporary)
   {
      osg::Vec3 tv1 = v1*mMatrix,
         tv2 = v2*mMatrix,
         tv3 = v3*mMatrix;

      tv1[2] = 0;
      if(tv1.length() > mRadius) mRadius = tv1.length();

      tv2[2] = 0;
      if(tv2.length() > mRadius) mRadius = tv2.length();

      tv3[2] = 0;
      if(tv2.length() > mRadius) mRadius = tv3.length();
   }
};

/**
* Sets this object's collision geometry to a sphere with
* radius derived from the specified OpenSceneGraph node.
*
* @param node the node from which to obtain the sphere radius
* (if NULL, attempt to use own node)
*/
void Transformable::SetCollisionSphere( osg::Node* node )
{
   if( node == 0 )
   {
      node = GetOSGNode();
   }

   if( node )
   {
      // Hmm, do we even need this here? I think this is a hack
      // to overcome a bug in DrawableVisitor... -osb
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      DrawableVisitor<SphereFunctor> sv;
      node->accept(sv);

      GetMatrixNode()->setMatrix( oldMatrix );

      if( sv.mFunctor.mRadius > 0 )
      {
         //dGeomID subTransformID = dCreateGeomTransform(0);

         //dGeomTransformSetCleanup(subTransformID, 1);

         mOriginalGeomID = dCreateSphere( 0, sv.mFunctor.mRadius );
         dGeomDisable( mOriginalGeomID );

         //dGeomTransformSetGeom( subTransformID, dCreateSphere( 0, sv.mFunctor.mRadius ) );
         dGeomTransformSetGeom( mGeomID, dCreateSphere( 0, sv.mFunctor.mRadius ) );

         //dGeomTransformSetGeom(mGeomID, subTransformID);

         //GetMatrixNode()->setMatrix( oldMatrix );

         RenderCollisionGeometry(mRenderingGeometry);
         SetCollisionDetection(true);

         // Sync-up the transforms on mGeomID
         PrePhysicsStepUpdate();
      }
      else
      {
         // This case happens in STAGE when the collision shape property is
         // parsed _before_ the mesh filename. After the mesh has been
         // loaded then the box will be recalculated.
         LOG_WARNING( "Calculated values for collision sphere are invalid. If you are reading this from STAGE, don't worry, it just means the collision shape ActorProperty was parsed from XML before the actual mesh. The collision shape will be recalculated when the mesh is loaded." )
      }
   }
}

/**
* Sets this object's collision geometry to a box with the
* specified dimensions.
*
* @param lx the length of the box in the x direction
* @param ly the length of the box in the y direction
* @param lz the length of the box in the z direction
*/
void Transformable::SetCollisionBox(float lx, float ly, float lz)
{
   mOriginalGeomID = dCreateBox(0, lx, ly, lz);
   dGeomDisable( mOriginalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateBox(0, lx, ly, lz) );

   RenderCollisionGeometry(mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

/**
* Sets this object's collision geometry to a box with parameters
* derived from the specified OpenSceneGraph node.
*
* @param node the node from which to obtain the box parameters
* (if 0, attempt to use own node)
*/
void Transformable::SetCollisionBox( osg::Node* node )
{
   if( node == 0 )
   {
      node = GetOSGNode();
   }

   if( node != 0 )
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      BoundingBoxVisitor bbv;
      node->accept(bbv);

      GetMatrixNode()->setMatrix( oldMatrix );

      float lx = bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin();
      float ly = bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin();
      float lz = bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin();

      if( lx > 0.0f && ly > 0.0f && lz > 0.0f )
      {
         dGeomID subTransformID = dCreateGeomTransform(0);

         dGeomTransformSetCleanup(subTransformID, 1);

         mOriginalGeomID =  dCreateBox( 0, lx, ly, lz );

         dGeomDisable( mOriginalGeomID );

         dGeomTransformSetGeom( subTransformID, dCreateBox( 0,
            bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin(),
            bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin(),
            bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin()
            )
            );

         dGeomSetPosition(
            subTransformID,
            bbv.mBoundingBox.center()[0],
            bbv.mBoundingBox.center()[1],
            bbv.mBoundingBox.center()[2]
            );

         dGeomTransformSetGeom(mGeomID, subTransformID);

         RenderCollisionGeometry(mRenderingGeometry);
         SetCollisionDetection(true);

         PrePhysicsStepUpdate();
      }
      else
      {
         // This case happens in STAGE when the collision shape property is
         // parsed _before_ the mesh filename. After the mesh has been
         // loaded then the box will be recalculated.
         LOG_WARNING( "Calculated values for collision box are invalid. If you are reading this from STAGE, don't worry, it just means the collision shape ActorProperty was parsed from XML before the actual mesh. The collision shape will be recalculated when the mesh is loaded." )
      }
   }
}

/**
* Sets this object's collision geometry to a capped cylinder
* (oriented along the z axis) with the specified radius and length.
*
* @param radius the radius of the cylinder
* @param length the length of the cylinder
*/
void Transformable::SetCollisionCappedCylinder(float radius, float length)
{
   mOriginalGeomID = dCreateCCylinder(0, radius, length);
   dGeomDisable( mOriginalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateCCylinder(0, radius, length) );

   RenderCollisionGeometry(mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

/**
* Determines the cylinder parameters
*/
class CylinderFunctor
{
public:

   float mMinZ, mMaxZ, mRadius;

   osg::Matrix mMatrix;

   /**
   * Constructor.
   */
   CylinderFunctor()
      : mMinZ(FLT_MAX), mMaxZ(-FLT_MAX), mRadius(0.0f)
   {}

   /**
   * Called once for each visited triangle.
   *
   * @param v1 the triangle's first vertex
   * @param v2 the triangle's second vertex
   * @param v3 the triangle's third vertex
   * @param treatVertexDataAsTemporary whether or not to treat the vertex data
   * as temporary
   */
   void operator()(const osg::Vec3& v1,
      const osg::Vec3& v2,
      const osg::Vec3& v3,
      bool treatVertexDataAsTemporary)
   {
      osg::Vec3 tv1 = v1*mMatrix,
         tv2 = v2*mMatrix,
         tv3 = v3*mMatrix;

      if(tv1[2] < mMinZ) mMinZ = tv1[2];
      else if(tv1[2] > mMaxZ) mMaxZ = tv1[2];

      if(tv2[2] < mMinZ) mMinZ = tv2[2];
      else if(tv2[2] > mMaxZ) mMaxZ = tv2[2];

      if(tv3[2] < mMinZ) mMinZ = tv3[2];
      else if(tv3[2] > mMaxZ) mMaxZ = tv3[2];

      tv1[2] = 0;
      if(tv1.length() > mRadius) mRadius = tv1.length();

      tv2[2] = 0;
      if(tv2.length() > mRadius) mRadius = tv2.length();

      tv3[2] = 0;
      if(tv2.length() > mRadius) mRadius = tv3.length();
   }
};

/**
* Sets this object's collision geometry to a capped cylinder with
* parameters derived from the given OpenSceneGraph node.
*
* @param node the node from which to obtain the cylinder parameters
* (if NULL, attempt to use own node)
*/
void Transformable::SetCollisionCappedCylinder(osg::Node* node)
{
   if( node == 0 )
   {
      node = this->GetOSGNode();
   }

   if( node )
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      DrawableVisitor<CylinderFunctor> cv;
      node->accept(cv);

      float radius = cv.mFunctor.mRadius;
      float length = cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ;

      if( radius > 0 && length > 0 )
      {
         dGeomID subTransformID = dCreateGeomTransform(0);

         dGeomTransformSetCleanup(subTransformID, 1);

         mOriginalGeomID = dCreateCCylinder( 0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ );
         dGeomDisable( mOriginalGeomID );

         dGeomTransformSetGeom( subTransformID, dCreateCCylinder( 0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ ) );

         dGeomTransformSetGeom(mGeomID, subTransformID);

         GetMatrixNode()->setMatrix( oldMatrix );

         RenderCollisionGeometry(mRenderingGeometry);
         SetCollisionDetection(true);

         // Sync-up the transforms on mGeomID
         PrePhysicsStepUpdate();
      }
      else
      {
         // This case happens in STAGE when the collision shape property is
         // parsed _before_ the mesh filename. After the mesh has been
         // loaded then the box will be recalculated.
         LOG_WARNING( "Calculated values for collision cylinder are invalid. If you are reading this from STAGE, don't worry, it just means the collision shape ActorProperty was parsed from XML before the actual mesh. The collision shape will be recalculated when the mesh is loaded." )
      }
   }
}

/**
* Sets this object's collision geometry to a ray (along the z axis)
* with the specified length.
*
* @param length the length of the ray
*/
void Transformable::SetCollisionRay(float length)
{
   mOriginalGeomID = dCreateRay(0, length);
   dGeomDisable( mOriginalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateRay(0, length) );

   RenderCollisionGeometry(mRenderingGeometry);
   SetCollisionDetection(true);

   // Sync-up the transforms on mGeomID
   PrePhysicsStepUpdate();
}

/**
* A strided vertex for the ODE triangle mesh collision geometry.
*/
struct StridedVertex
{
   dVector3 Vertex;
};

/**
* A strided triangle for the ODE triangle mesh collision geometry.
*/
struct StridedTriangle
{
   int Indices[3];
};

/**
* Records visited triangles into an array suitable for passing
* into ODE's triangle mesh collision detection class.
*/
class TriangleRecorder
{
public:

   std::vector<StridedVertex> mVertices;

   std::vector<StridedTriangle> mTriangles;

   osg::Matrix mMatrix;

   /**
   * Called once for each visited triangle.
   *
   * @param v1 the triangle's first vertex
   * @param v2 the triangle's second vertex
   * @param v3 the triangle's third vertex
   * @param treatVertexDataAsTemporary whether or not to treat the vertex data
   * as temporary
   */
   void operator()(const osg::Vec3& v1,
      const osg::Vec3& v2,
      const osg::Vec3& v3,
      bool treatVertexDataAsTemporary)
   {
      osg::Vec3 tv1 = v1*mMatrix,
         tv2 = v2*mMatrix,
         tv3 = v3*mMatrix;

      StridedVertex sv1, sv2, sv3;
      StridedTriangle t;

      t.Indices[0] = mVertices.size();
      t.Indices[1] = mVertices.size() + 1;
      t.Indices[2] = mVertices.size() + 2;

      mTriangles.push_back(t);

      sv1.Vertex[0] = tv1[0];
      sv1.Vertex[1] = tv1[1];
      sv1.Vertex[2] = tv1[2];

      sv2.Vertex[0] = tv2[0];
      sv2.Vertex[1] = tv2[1];
      sv2.Vertex[2] = tv2[2];

      sv3.Vertex[0] = tv3[0];
      sv3.Vertex[1] = tv3[1];
      sv3.Vertex[2] = tv3[2];

      mVertices.push_back(sv1);
      mVertices.push_back(sv2);
      mVertices.push_back(sv3);
   }
};

/**
* Sets this object's collision geometry to a triangle mesh derived
* from the given OpenSceneGraph node.
*
* @param node the node from which to obtain the mesh data
* (if 0, attempt to use own node)
*/
void Transformable::SetCollisionMesh(osg::Node* node)
{
   if( node == 0 )
   {
      node = GetOSGNode();
   }

   if( node )
   {
      //the following is a workaround to temporarily bypass this Physical's Transform
      //At this point, we'll set it temporarily to the Identity so it doesn't affect
      //our creation of the collision mesh.  This Transform will be accounted
      //for later in PrePhysicsUpdate().
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      DrawableVisitor<TriangleRecorder> mv;

      node->accept(mv);

      if(mMeshVertices != 0)
      {
         delete[] mMeshVertices;
         delete[] mMeshIndices;
      }

      mMeshVertices = new dVector3[mv.mFunctor.mVertices.size()];
      mMeshIndices = new int[mv.mFunctor.mTriangles.size()*3];

      memcpy(
         mMeshVertices,
         &mv.mFunctor.mVertices.front(),
         mv.mFunctor.mVertices.size()*sizeof(StridedVertex)
         );

      memcpy(
         mMeshIndices,
         &mv.mFunctor.mTriangles.front(),
         mv.mFunctor.mTriangles.size()*sizeof(StridedTriangle)
         );

      dGeomTriMeshDataBuildSimple(
         mTriMeshDataID,
         (dReal*)mMeshVertices,
         mv.mFunctor.mVertices.size(),
         mMeshIndices,
         mv.mFunctor.mTriangles.size()*3
         );

      dGeomTransformSetGeom(
         mGeomID,
         dCreateTriMesh(0, mTriMeshDataID, 0, 0, 0)
         );

      GetMatrixNode()->setMatrix( oldMatrix );

      RenderCollisionGeometry(mRenderingGeometry);
      SetCollisionDetection(true);
   }
}

/**
* Removes any collision geometry specified for this object.
*/
void Transformable::ClearCollisionGeometry()
{
   SetCollisionDetection(false);
   dGeomTransformSetGeom(mGeomID, 0);
   
   //If the collision geometry is valid, this implies the user has
   //enabled render collision geometry.  Therefore, we just remove
   //the drawables from the geode.  When the user turns off render
   //collision geometry, that will remove the geode from this node.
   if( mGeomGeod.valid() )
   {
      mGeomGeod->removeDrawable(0,mGeomGeod->getNumDrawables());
   }
}

/**
* Updates the state of this object just before a physical
* simulation step.  Should only be called by dtCore::Scene.
* The default implementation updates the state of the body
* to reflect any user-applied transformation.
*/
void Transformable::PrePhysicsStepUpdate()
{
   Transform transform;

   this->GetTransform(&transform, Transformable::ABS_CS);

   if( !transform.EpsilonEquals(&mGeomTransform) )
   {
      mGeomTransform = transform;

      osg::Matrix rotation;
      osg::Vec3 position, scale;

      mGeomTransform.GetTranslation( position );
      mGeomTransform.GetRotation( rotation );
      mGeomTransform.GetScale( scale );

      // Set translation
      dGeomSetPosition(mGeomID, position[0], position[1], position[2]);

      // Set rotation
      dMatrix3 dRot;

      dRot[0] = rotation(0,0);
      dRot[1] = rotation(1,0);
      dRot[2] = rotation(2,0);

      dRot[4] = rotation(0,1);
      dRot[5] = rotation(1,1);
      dRot[6] = rotation(2,1);

      dRot[8] = rotation(0,2);
      dRot[9] = rotation(1,2);
      dRot[10] = rotation(2,2);

      dGeomSetRotation(mGeomID, dRot);
      
      int geomClass = dGeomGetClass( mGeomID ) ;
      dGeomID id = mGeomID;

      while ( geomClass == dGeomTransformClass )
      {
         id = dGeomTransformGetGeom(id);
         if( id == 0 )
         {
            return; // In case we haven't assigned a collision shape yet
         }
         geomClass = dGeomGetClass(id);
      }

      if( id )
      {
         switch( dGeomGetClass( id ) )
         {
         case dBoxClass:
            {
               dVector3 originalSide;
               dGeomBoxGetLengths( mOriginalGeomID, originalSide );
               dGeomBoxSetLengths( id, originalSide[0]*scale[0], originalSide[1]*scale[1], originalSide[2]*scale[2] );
            }
            break;
         case dSphereClass:
            {
               dReal originalRadius = dGeomSphereGetRadius( mOriginalGeomID );
               float maxScale = std::max( std::max( scale[0], scale[1] ), scale[2] );
               dGeomSphereSetRadius( id, originalRadius * maxScale );
            }
            break;
         case dCCylinderClass:
            {
               dReal originalRadius, originalLength;
               dGeomCCylinderGetParams( mOriginalGeomID, &originalRadius, &originalLength );

               // Find max radius based on x/y scaling
               float maxRadiusScale = std::max( scale[0], scale[1] );

               dGeomCCylinderSetParams( id, originalRadius * maxRadiusScale, originalLength * scale[2] );
            }
            break;
         case dRayClass:
            {
               dVector3 start, dir;
               dReal originalLength = dGeomRayGetLength( mOriginalGeomID );

               dGeomRayGet(mOriginalGeomID, start, dir);

               // Ignore x/y scaling, use z to scale ray
               dGeomRaySetLength( id, originalLength * scale[2] );
            }
            break;
         case dTriMeshClass:
            {
               SetCollisionMesh();
            }
            break;
         default:
            break;
         }
      }
   }
}

void Transformable::RenderCollisionGeometry( bool enable )
{
   osg::MatrixTransform *xform = this->GetMatrixNode();

   if(!xform)
   {
      return;
   }

   mRenderingGeometry = enable;

   if(enable)
   {
      mGeomGeod = new osg::Geode();
      mGeomGeod->setName(Transformable::COLLISION_GEODE_ID);
      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio(0.5f);

      int geomClass = dGeomGetClass( mGeomID ) ;
      dGeomID id = mGeomID;

      osg::Matrix absMatrix;

      while( geomClass == dGeomTransformClass )
      {
         id = dGeomTransformGetGeom(id);

         if (id == 0)
         {
            return; //in case we haven't assigned a collision shape yet
         }

         geomClass = dGeomGetClass(id);
         const dReal *pos = dGeomGetPosition(id);
         const dReal *rot = dGeomGetRotation(id);

         osg::Matrix tempMatrix;

         tempMatrix(0,0) = rot[0];
         tempMatrix(0,1) = rot[1];
         tempMatrix(0,2) = rot[2];

         tempMatrix(1,0) = rot[4];
         tempMatrix(1,1) = rot[5];
         tempMatrix(1,2) = rot[6];

         tempMatrix(2,0) = rot[8];
         tempMatrix(2,1) = rot[9];
         tempMatrix(2,2) = rot[10];

         tempMatrix(3,0) = pos[0];
         tempMatrix(3,1) = pos[1];
         tempMatrix(3,2) = pos[2];

         absMatrix.postMult( tempMatrix );
      }

      switch(dGeomGetClass(id))
      {
      case dBoxClass:
         {
            dVector3 side;
            dGeomBoxGetLengths(mOriginalGeomID, side);
            mGeomGeod.get()->addDrawable(
               new osg::ShapeDrawable(
               new osg::Box(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
               side[0], side[1], side[2]), hints) );
         }
         break;
      case dSphereClass:
         {
            dReal rad = dGeomSphereGetRadius(mOriginalGeomID);
            mGeomGeod.get()->addDrawable(
               new osg::ShapeDrawable(
               new osg::Sphere(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
               rad), hints ) );
         }
         break;
      case dCCylinderClass:
         {
            dReal radius, length;
            dGeomCCylinderGetParams(mOriginalGeomID, &radius, &length);
            mGeomGeod.get()->addDrawable(
               new osg::ShapeDrawable(
               new osg::Cylinder(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
               radius, length), hints ) );
         }
         break;

      case dTriMeshClass:
      case dCylinderClass:
      case dPlaneClass:
         {
            //dVector4 result; //a*x+b*y+c*z = d
            //dGeomPlaneGetParams(id, result);
         }
      case dRayClass:
         {
            //dVector3 start, dir;
            //dReal length = dGeomRayGetLength(id);
            //dGeomRayGet(id, start, dir);
         }

      default:
         Log::GetInstance().LogMessage( Log::LOG_WARNING, __FILE__, 
            "Transformable:Can't render unhandled geometry class:%d",
            dGeomGetClass(id) );
         break;
      }

      osg::Material *mat = new osg::Material();
      mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.f,0.f,1.f, 0.5f));
      mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.f,0.f,1.f, 1.f));
      mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(0.f,0.f,0.f, 1.f));

      osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
      polyoffset->setFactor(-1.0f);
      polyoffset->setUnits(-1.0f);

      osg::StateSet *ss = mGeomGeod.get()->getOrCreateStateSet();
      ss->setAttributeAndModes(mat, osg::StateAttribute::OVERRIDE |
         osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
      ss->setMode(GL_BLEND,osg::StateAttribute::OVERRIDE |
         osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      ss->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE |
         osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

      xform->addChild(mGeomGeod.get());

   } //end if enabled==true
   else
   {
      if( mGeomGeod.valid() )
      {
         xform->removeChild(mGeomGeod.get());
         mGeomGeod = 0;
      }
   }
}

/** This typically gets called from Scene::AddDrawable().
*
* This method perform the standard DeltaDrawable::AddedToScene() functionality
* then registers this Transformable object with the supplied Scene to create the
* internal physical properties.
*
* If the param scene is 0, this will unregister this Transformable from the
* previous parent Scene.
* If this Transformable already has a parent Scene, it will
* first remove itself from the old Scene (Scene::RemoveDrawable()), then
* re-register with the new Scene.
*
* @param scene The Scene this Transformable has been added to
*/
void Transformable::AddedToScene( Scene* scene )
{
   if( scene )
   {
      //remove us from our existing parent scene, if we already have one.
      //TODO This ends up calling AddedToScene again with a 0.  Is this bad?
      if( mParentScene )
      {
         mParentScene->RemoveDrawable( this );
      }

      DeltaDrawable::AddedToScene( scene );
      scene->RegisterCollidable( this );
   }
   else
   {
      if( mParentScene )
      {
         mParentScene->UnRegisterCollidable( this );
      }
      DeltaDrawable::AddedToScene( scene );
   }
}
