// physical.cpp: Implementation of the Physical class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/deltadrawable.h"
#include "dtCore/scene.h"
#include "dtCore/physical.h"
#include "dtCore/transformable.h"
#include "dtCore/object.h"
#include "dtCore/notify.h"
#include "dtUtil/polardecomp.h"
#include "dtUtil/matrixutil.h"

#include <osg/BoundingBox>
#include <osg/BoundingSphere>
#include <osg/Geode>
#include <osg/NodeVisitor>
#include <osg/Transform>
#include <osg/TriangleFunctor>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/LineWidth>

#undef min
#undef max
#include <algorithm>
#include <vector>


using namespace dtCore;
using namespace std;

IMPLEMENT_MANAGEMENT_LAYER(Physical)

/**
 * Constructor.
 */
Physical::Physical()
   : mBodyID(0),
     mDynamicsEnabled(false),
     mTriMeshDataID(0),
     mMeshVertices(NULL),
     mMeshIndices(NULL),
     mRenderingGeometry(false)
{
   RegisterInstance(this);

   mGeomID = dCreateGeomTransform(0);
   
   dGeomTransformSetCleanup(mGeomID, 1);
   
   dGeomTransformSetInfo(mGeomID, 1);
   
   dMassSetSphere(&mMass, 1.0f, 1.0f);
   
   mTriMeshDataID = dGeomTriMeshDataCreate();


}

/**
 * Destructor.
 */
Physical::~Physical()
{
   dGeomDestroy(mGeomID);
 
   if(mBodyID != 0)
   {
      dBodyDestroy(mBodyID);
   }
   
   dGeomTriMeshDataDestroy(mTriMeshDataID);
   
   if(mMeshVertices != NULL)
   {
      delete[] mMeshVertices;
      delete[] mMeshIndices;
   }

   DeregisterInstance(this);
}


/**
 * Returns the ODE geometry identifier associated with this
 * object.
 *
 * @return the object's geometry identifier
 */
dGeomID Physical::GetGeomID() const
{
   return mGeomID;
}

/**
 * Sets the ODE body identifier associated with this object.  Should
 * only be called by Scene.
 *
 * @param bodyID the new body identifier
 */
void Physical::SetBodyID(dBodyID bodyID)
{
   if(mBodyID != 0)
   {
      dBodyDestroy(mBodyID);
   }
   
   mBodyID = bodyID;
   
   EnableDynamics(mDynamicsEnabled);
   
   SetMass(&mMass);
   
   dGeomSetBody(mGeomID, mBodyID);
}

/**
 * Returns the ODE body identifier associated with this
 * object.
 *
 * @return the object's body identifier
 */
dBodyID Physical::GetBodyID() const
{
   return mBodyID;
}

/**
 * Sets this object's collision geometry to the specified ODE
 * geom.
 *
 * @param geom the new collision geom
 */
void Physical::SetCollisionGeom(dGeomID geom)
{
   //mOrignalGeomID = dGeomID(geom);
   //dGeomDisable( mOrignalGeomID );

   dGeomTransformSetGeom(mGeomID, geom);
}

/**
 * Sets this object's collision geometry to a sphere with the
 * specified radius.
 *
 * @param radius the radius of the collision sphere
 */
void Physical::SetCollisionSphere(float radius)
{
   mOrignalGeomID = dCreateSphere(0, radius);
   dGeomDisable( mOrignalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateSphere(0, radius) );
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
void Physical::SetCollisionSphere( osg::Node* node )
{
   if( node == NULL )
      node = GetOSGNode();

   if( node )
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      DrawableVisitor<SphereFunctor> sv;
      node->accept(sv);

      dGeomID subTransformID = dCreateGeomTransform(0);

      dGeomTransformSetCleanup(subTransformID, 1);

      mOrignalGeomID = dCreateSphere( 0, sv.mFunctor.mRadius );
      dGeomDisable( mOrignalGeomID );

      dGeomTransformSetGeom( subTransformID, dCreateSphere( 0, sv.mFunctor.mRadius ) );

      dGeomTransformSetGeom(mGeomID, subTransformID);

      GetMatrixNode()->setMatrix( oldMatrix );
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
void Physical::SetCollisionBox(float lx, float ly, float lz)
{
   mOrignalGeomID = dCreateBox(0, lx, ly, lz);
   dGeomDisable( mOrignalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateBox(0, lx, ly, lz) );
}

/**
 * Sets this object's collision geometry to a box with parameters
 * derived from the specified OpenSceneGraph node.
 *
 * @param node the node from which to obtain the box parameters
 * (if NULL, attempt to use own node)
 */
void Physical::SetCollisionBox( osg::Node* node )
{
   if(node == NULL)
      node = GetOSGNode();
   
   if(node != NULL)
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      BoundingBoxVisitor bbv;
      node->accept(bbv);

      GetMatrixNode()->setMatrix( oldMatrix );
      
      dGeomID subTransformID = dCreateGeomTransform(0);
      
      dGeomTransformSetCleanup(subTransformID, 1);

      mOrignalGeomID =  dCreateBox( 0, 
         bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin(),
         bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin(),
         bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin()
      );

      dGeomDisable( mOrignalGeomID );

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
      
   }
}

/**
 * Sets this object's collision geometry to a capped cylinder
 * (oriented along the z axis) with the specified radius and length.
 *
 * @param radius the radius of the cylinder
 * @param length the length of the cylinder
 */
void Physical::SetCollisionCappedCylinder(float radius, float length)
{
   mOrignalGeomID = dCreateCCylinder(0, radius, length);
   dGeomDisable( mOrignalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateCCylinder(0, radius, length) );
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
         : mMinZ(SG_MAX), mMaxZ(-SG_MAX), mRadius(0.0f)
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
void Physical::SetCollisionCappedCylinder(osg::Node* node)
{
   if( node == NULL )
      node = this->GetOSGNode();
   
   if( node )
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      DrawableVisitor<CylinderFunctor> cv;
      node->accept(cv);
      
      dGeomID subTransformID = dCreateGeomTransform(0);
      
      dGeomTransformSetCleanup(subTransformID, 1);

      mOrignalGeomID = dCreateCCylinder( 0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ );
      dGeomDisable( mOrignalGeomID );

      dGeomTransformSetGeom( subTransformID, dCreateCCylinder( 0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ ) );
      
      dGeomTransformSetGeom(mGeomID, subTransformID);

      GetMatrixNode()->setMatrix( oldMatrix );
   }
}

/**
 * Sets this object's collision geometry to a ray (along the z axis)
 * with the specified length.
 *
 * @param length the length of the ray
 */
void Physical::SetCollisionRay(float length)
{
   mOrignalGeomID = dCreateRay(0, length);
   dGeomDisable( mOrignalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateRay(0, length) );
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

      vector<StridedVertex> mVertices;

      vector<StridedTriangle> mTriangles;
      
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
 * (if NULL, attempt to use own node)
 */
void Physical::SetCollisionMesh(osg::Node* node)
{
   if( node == NULL )
      node = GetOSGNode();
   
   if( node )
   {
      osg::Matrix oldMatrix = GetMatrixNode()->getMatrix();
      GetMatrixNode()->setMatrix( osg::Matrix::identity() );

      DrawableVisitor<TriangleRecorder> mv;
      
      node->accept(mv);
   
      if(mMeshVertices != NULL)
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
         dCreateTriMesh(0, mTriMeshDataID, NULL, NULL, NULL)
      );

      GetMatrixNode()->setMatrix( oldMatrix );
      
   }
}

/**
 * Removes any collision geometry specified for this object.
 */
void Physical::ClearCollisionGeometry()
{
   dGeomTransformSetGeom(mGeomID, 0);
}

/**
 * Enables or disables dynamics for this object.
 *
 * @param enable true to enable dynamics, false to disable
 */
void Physical::EnableDynamics(bool enable)
{
   mDynamicsEnabled = enable;
   
   if(mBodyID != 0)
   {
      if(enable)
      {
         dBodyEnable(mBodyID);
      }
      else
      {
         dBodyDisable(mBodyID);
      }
   }
}

/**
 * Checks whether or not dynamics are enabled for
 * this object.
 *
 * @return true if dynamics are enabled, false otherwise
 */
bool Physical::DynamicsEnabled() const
{
   return mDynamicsEnabled;
}

/**
 * Sets the ODE mass parameters of this object.
 *
 * @param mass a pointer to the mass structure to copy
 */
void Physical::SetMass(const dMass* mass)
{
   mMass = *mass;

   if(mBodyID != 0)
   {   
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Retrieves the ODE mass parameters of this object.
 *
 * @param mass a pointer to the mass structure to fill
 */
void Physical::GetMass(dMass* mass) const
{
   *mass = mMass;
}

/**
 * Sets the mass of this object.
 *
 * @param mass the new mass, in kilograms
 */
void Physical::SetMass(float mass)
{
   mMass.mass = mass;
   
   if(mBodyID != 0)
   {
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Returns the mass of this object.
 *
 * @return the current mass
 */
float Physical::GetMass() const
{
   return mMass.mass;
}

/**
 * Sets this object's center of gravity.
 *
 * @param centerOfGravity the new center of gravity
 */
void Physical::SetCenterOfGravity(const sgVec3 centerOfGravity)
{
   mMass.c[0] = centerOfGravity[0];
   mMass.c[1] = centerOfGravity[1];
   mMass.c[2] = centerOfGravity[2];
   
   if(mBodyID != 0)
   {
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Retrieves this object's center of gravity.
 *
 * @param dest the vector in which to place the center
 * of gravity
 */
void Physical::GetCenterOfGravity(sgVec3 dest) const
{
   dest[0] = mMass.c[0];
   dest[1] = mMass.c[1];
   dest[2] = mMass.c[2];
}

/**
 * Sets this object's inertia tensor.
 *
 * @param inertiaTensor the new inertia tensor
 */
void Physical::SetInertiaTensor(const sgMat3 inertiaTensor)
{
   mMass.I[0] = inertiaTensor[0][0];
   mMass.I[1] = inertiaTensor[1][0];
   mMass.I[2] = inertiaTensor[2][0];
   
   mMass.I[4] = inertiaTensor[0][1];
   mMass.I[5] = inertiaTensor[1][1];
   mMass.I[6] = inertiaTensor[2][1];
   
   mMass.I[8] = inertiaTensor[0][2];
   mMass.I[9] = inertiaTensor[1][2];
   mMass.I[10] = inertiaTensor[2][2];
   
   if(mBodyID != 0)
   {
      dBodySetMass(mBodyID, &mMass);
   }
}

/**
 * Retrieves this object's inertia tensor.
 *
 * @param dest the matrix in which to place the inertia
 * tensor
 */
void Physical::GetInertiaTensor(sgMat3 dest) const
{
   dest[0][0] = mMass.I[0];
   dest[1][0] = mMass.I[1];
   dest[2][0] = mMass.I[2];
   
   dest[0][1] = mMass.I[4];
   dest[1][1] = mMass.I[5];
   dest[2][1] = mMass.I[6];
   
   dest[0][2] = mMass.I[8];
   dest[1][2] = mMass.I[9];
   dest[2][2] = mMass.I[10];
}

/**
 * Updates the state of this object just before a physical
 * simulation step.  Should only be called by dtCore::Scene.
 * The default implementation updates the state of the body
 * to reflect any user-applied transformation.
 */
void Physical::PrePhysicsStepUpdate()
{
   if (DynamicsEnabled())   
   {
      Transform transform;
      
      this->GetTransform(&transform);
      
      if(!transform.EpsilonEquals(&mGeomTransform))
      {
         mGeomTransform = transform;
         
         osg::Matrix rotation;
         osg::Vec3 position, scale;

         mGeomTransform.GetTranslation( position );
         mGeomTransform.GetRotation( rotation );
         mGeomTransform.GetScale( scale );
         
         //set translation
         dGeomSetPosition(mGeomID, position[0], position[1], position[2]);

         //set rotation
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

         //set scale
         int geomClass = dGeomGetClass( mGeomID ) ;
         dGeomID id = mGeomID;

         while ( geomClass == dGeomTransformClass ) 
         {
            id = dGeomTransformGetGeom(id);
            if (id == 0) return; //in case we haven't assigned a collision shape yet
            geomClass = dGeomGetClass(id);

         }

         if( id )
         {
            switch( dGeomGetClass( id ) )
            {
            case dBoxClass:
               {
                  dVector3 currentSide, originalSide;

                  dGeomBoxGetLengths( id, currentSide );
                  dGeomBoxGetLengths( mOrignalGeomID, originalSide );
       
                  dGeomBoxSetLengths( id, originalSide[0]*scale[0], originalSide[1]*scale[1], originalSide[2]*scale[2] );
                     
               }
               break;
            case dSphereClass:
               {
                  dReal currentRadius = dGeomSphereGetRadius( id );
                  dReal originalRadius = dGeomSphereGetRadius( mOrignalGeomID );

                  float maxScale = std::max( std::max( scale[0], scale[1] ), scale[2] );

                  dGeomSphereSetRadius( id, originalRadius * maxScale );
               }
               break;
            case dCCylinderClass:
               {
                  dReal currentRadius, currentLength, originalRadius, originalLength;

                  dGeomCCylinderGetParams( id, &currentRadius, &currentLength );
                  dGeomCCylinderGetParams( mOrignalGeomID, &originalRadius, &originalLength );

                  //find max radius based on x/y scaling
                  float maxRadiusScale = std::max( scale[0], scale[1] );

                  dGeomCCylinderSetParams( id, originalRadius * maxRadiusScale, originalLength * scale[2] );
               }
               break;
            case dRayClass:
               {
                  dVector3 start, dir;
                  dReal currentLength = dGeomRayGetLength( id );
                  dReal originalLength = dGeomRayGetLength( mOrignalGeomID );

                  dGeomRayGet(mOrignalGeomID, start, dir);

                  //ignore x/y scaling, use z to scale ray
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
}

/**
 * Modifies or cancels the specified contact joint definition
 * according to the relationship between this object and the
 * specified collider.  Should only be called by dtCore::Scene.
 *
 * @param contact the joint definition to modify
 * @param collider the object with which this is colliding
 * @return true to keep the contact joint as modified,
 * false to cancel it
 */
bool Physical::FilterContact(dContact* contact, Physical* collider)
{
   return true;
}

/**
 * Updates the state of this object just after a physical
 * simulation step.  Should only be called by dtCore::Scene.
 * The default implementation copies the new object
 * position into the user-accessible transformation.
 */
void Physical::PostPhysicsStepUpdate()
{
   if (DynamicsEnabled())
   {
      const dReal* position = dGeomGetPosition(mGeomID);
      const dReal* rotation = dGeomGetRotation(mGeomID);

      osg::Matrix newRotation;
      
      newRotation(0,0) = rotation[0];
      newRotation(1,0) = rotation[1];
      newRotation(2,0) = rotation[2];
         
      newRotation(0,1) = rotation[4];
      newRotation(1,1) = rotation[5];
      newRotation(2,1) = rotation[6];
         
      newRotation(0,2) = rotation[8];
      newRotation(1,2) = rotation[9];
      newRotation(2,2) = rotation[10];

      mGeomTransform.SetTranslation( position[0], position[1], position[2] );
      mGeomTransform.SetRotation(newRotation);

      this->SetTransform(&mGeomTransform);

   }
}

void Physical::RenderCollisionGeometry( const bool enable )
{
   osg::MatrixTransform *xform = this->GetMatrixNode();
   if (!xform) return;

   if (enable)
   {
      mGeomGeod = new osg::Geode();
      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio(0.5f);

      int geomClass = dGeomGetClass( mGeomID ) ;
      dGeomID id = mGeomID;

      osg::Matrix absMatrix;

      while ( geomClass == dGeomTransformClass ) 
      {
         id = dGeomTransformGetGeom(id);
         if (id == 0) return; //in case we haven't assigned a collision shape yet
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
               dGeomBoxGetLengths(mOrignalGeomID, side);
               const dReal *pos = dGeomGetPosition(id);
               const dReal *rot = dGeomGetRotation(id);
               mGeomGeod.get()->addDrawable(
                  new osg::ShapeDrawable(
                  new osg::Box(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
                  side[0], side[1], side[2]), hints) );
            }
            break;
         case dSphereClass:
            {
               dReal rad = dGeomSphereGetRadius(mOrignalGeomID);
               mGeomGeod.get()->addDrawable(
                  new osg::ShapeDrawable(
                  new osg::Sphere(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
                  rad), hints ) );
            }
            break;
         case dCCylinderClass:
            {
               dReal radius, length;
               dGeomCCylinderGetParams(mOrignalGeomID, &radius, &length);
               mGeomGeod.get()->addDrawable(
                  new osg::ShapeDrawable(
                  new osg::Cylinder(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
                  radius, length), hints ) );
            }
            break;

         case dCylinderClass:
         case dPlaneClass:
            {
               dVector4 result; //a*x+b*y+c*z = d
               dGeomPlaneGetParams(id, result);
            }
         case dRayClass:
            {
               dVector3 start, dir;
               dReal length = dGeomRayGetLength(id);
               dGeomRayGet(id, start, dir);
            }
         case dTriMeshClass:
         default:
            Notify(WARN, "Physical:Can't render unhandled geometry class:%d",
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
      ss->setAttributeAndModes(mat, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
      ss->setMode(GL_BLEND,osg::StateAttribute::ON);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
      ss->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

      xform->addChild(mGeomGeod.get());
   } //end if enabled==true
   else
   {
      if (mGeomGeod.get() == NULL) return;

      xform->removeChild(mGeomGeod.get());
      mGeomGeod = NULL;
   }

   mRenderingGeometry = enable;
}

void Physical::AddedToScene( Scene *scene )
{
   if( scene )
   {
      DeltaDrawable::AddedToScene( scene );
      scene->RegisterPhysical(this); 
   }
   else
   {
      if (mParentScene.valid()) mParentScene->UnRegisterPhysical(this);
      DeltaDrawable::AddedToScene( scene );
   } 
} 
