// physical.cpp: Implementation of the Physical class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/deltadrawable.h"
#include "dtCore/scene.h"
#include "dtCore/physical.h"
#include "dtCore/transformable.h"
#include "dtCore/object.h"
#include "dtCore/notify.h"

#include <vector>

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

using namespace dtCore;
using namespace std;

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
   Notify(DEBUG_INFO, "Physical: Deleting '%s'", GetName().c_str());

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
   dGeomTransformSetGeom(mGeomID, dCreateSphere(0, radius));
}

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
      dGeomID subTransformID = dCreateGeomTransform(0);

      dGeomTransformSetCleanup(subTransformID, 1);

      dGeomTransformSetGeom(
         subTransformID,
         dCreateSphere(0, node->getBound().radius())
      );

      /*
      dGeomSetPosition(
      subTransformID,
      absCenter[0],
      absCenter[1],
      absCenter[2]
      );
      */

      dGeomTransformSetGeom(mGeomID, subTransformID);
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
   dGeomTransformSetGeom(mGeomID, dCreateBox(0, lx, ly, lz));
}

class BoundingBoxVisitor : public osg::NodeVisitor
{
   public:
   
      /**
       * Constructor.
       */
      BoundingBoxVisitor()
         : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      {}
      
      /**
       * Visits the specified geode.
       *
       * @param node the geode to visit
       */
      virtual void apply(osg::Geode& node)
      {
         osg::Matrix matrix = 
            osg::computeLocalToWorld(getNodePath());
         
         for(unsigned int i=0;i<node.getNumDrawables();i++)
         {
            for(unsigned int j=0;j<8;j++)
            {
               mBoundingBox.expandBy(
                  node.getDrawable(i)->getBound().corner(j) * matrix
               );
            }
         }
      }
      
      /**
       * The aggregate bounding box.
       */
      osg::BoundingBox mBoundingBox;
};

/**
 * Sets this object's collision geometry to a box with parameters
 * derived from the specified OpenSceneGraph node.
 *
 * @param node the node from which to obtain the box parameters
 * (if NULL, attempt to use own node)
 */
void Physical::SetCollisionBox(osg::Node* node)
{
   if(node == NULL)
      node = GetOSGNode();
   
   if(node != NULL)
   {
      BoundingBoxVisitor bbv;
      
      node->accept(bbv);
      
      dGeomID subTransformID = dCreateGeomTransform(0);
      
      dGeomTransformSetCleanup(subTransformID, 1);
      
      dGeomTransformSetGeom(
         subTransformID,
         dCreateBox(
            0, 
            bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin(),
            bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin(),
            bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin()
         )
      );
      
      /*
      dGeomSetPosition(
         subTransformID,
         bbv.mBoundingBox.center()[0],
         bbv.mBoundingBox.center()[1],
         bbv.mBoundingBox.center()[2]
      );
      */
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
   dGeomTransformSetGeom(mGeomID, dCreateCCylinder(0, radius, length));
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
 * A visitor that determines the cylinder parameters associated
 * with a node.
 */
class CylinderVisitor : public osg::NodeVisitor
{
   public:
   
      osg::TriangleFunctor<CylinderFunctor> mFunctor;
      
      /**
       * Constructor.
       */
      CylinderVisitor()
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
               mFunctor.mMatrix = 
                  osg::computeLocalToWorld(getNodePath());
               
               d->accept(mFunctor);
            }
         }
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
      CylinderVisitor cv;
      
      node->accept(cv);
      
      dGeomID subTransformID = dCreateGeomTransform(0);
      
      dGeomTransformSetCleanup(subTransformID, 1);
      
      dGeomTransformSetGeom(
         subTransformID,
         dCreateCCylinder(
            0, 
            cv.mFunctor.mRadius,
            cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ
         )
      );
      
      /*
      dGeomSetPosition(
         subTransformID,
         0.0,
         0.0,
         (cv.mFunctor.mMaxZ + cv.mFunctor.mMinZ) * 0.5
      );
      */
      
      dGeomTransformSetGeom(mGeomID, subTransformID);
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
   dGeomTransformSetGeom(mGeomID, dCreateRay(0, length));
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
 * A visitor that collects all OSG geometry into a single mesh.
 */
class MeshVisitor : public osg::NodeVisitor
{
   public:
   
      osg::TriangleFunctor<TriangleRecorder> mRecorder;
      
      /**
       * Constructor.
       */
      MeshVisitor()
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

            if(d->supports(mRecorder))
            {
               mRecorder.mMatrix = 
                  osg::computeLocalToWorld(getNodePath());
               
               d->accept(mRecorder);
            }
         }
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
      MeshVisitor mv;
      
      node->accept(mv);
   
      if(mMeshVertices != NULL)
      {
         delete[] mMeshVertices;
         delete[] mMeshIndices;
      }
      
      mMeshVertices = new dVector3[mv.mRecorder.mVertices.size()];
      mMeshIndices = new int[mv.mRecorder.mTriangles.size()*3];
      
      memcpy(
         mMeshVertices, 
         &mv.mRecorder.mVertices.front(), 
         mv.mRecorder.mVertices.size()*sizeof(StridedVertex)
      );
      
      memcpy(
         mMeshIndices,
         &mv.mRecorder.mTriangles.front(),
         mv.mRecorder.mTriangles.size()*sizeof(StridedTriangle)
      );
      
      dGeomTriMeshDataBuildSimple(
         mTriMeshDataID,
         (dReal*)mMeshVertices,
         mv.mRecorder.mVertices.size(),
         mMeshIndices,
         mv.mRecorder.mTriangles.size()*3
      );

      /*
      dGeomTransformSetGeom(
         mGeomID, 
         dCreateTriMesh(0, mTriMeshDataID, NULL, NULL, NULL)
      );
      */
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
         
         sgVec3 position;
         
         mGeomTransform.GetTranslation(position);
         
         dGeomSetPosition(mGeomID, position[0], position[1], position[2]);
         
         sgMat4 rotation;
         
         mGeomTransform.GetRotation(rotation);
         
         dMatrix3 dRot;
         
         dRot[0] = rotation[0][0];
         dRot[1] = rotation[1][0];
         dRot[2] = rotation[2][0];
         
         dRot[4] = rotation[0][1];
         dRot[5] = rotation[1][1];
         dRot[6] = rotation[2][1];
         
         dRot[8] = rotation[0][2];
         dRot[9] = rotation[1][2];
         dRot[10] = rotation[2][2];
         
         dGeomSetRotation(mGeomID, dRot);
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

      mGeomTransform.SetTranslation(
         position[0],
         position[1],
         position[2]
      );
      
      sgMat4 rot;
      
      sgMakeIdentMat4(rot);
      
      rot[0][0] = rotation[0];
      rot[1][0] = rotation[1];
      rot[2][0] = rotation[2];
         
      rot[0][1] = rotation[4];
      rot[1][1] = rotation[5];
      rot[2][1] = rotation[6];
         
      rot[0][2] = rotation[8];
      rot[1][2] = rotation[9];
      rot[2][2] = rotation[10];

      mGeomTransform.SetRotation(rot);
      
      this->SetTransform(&mGeomTransform);
   }
}

void Physical::RenderCollisionGeometry( const bool enable)
{
   osg::MatrixTransform *xform = this->GetMatrixNode();
   if (!xform) return;

   if (enable)
   {
      mGeomGeod = new osg::Geode();
      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio(0.5f);

      int geomclass = dGeomGetClass(mGeomID) ;
      dGeomID id = mGeomID;

      sgMat4 absMat; ///<The cumulative position from parent to child
      sgMakeIdentMat4(absMat);

      while (geomclass == dGeomTransformClass) 
      {
         id = dGeomTransformGetGeom(id);
         if (id == 0) return; //in case we haven't assigned a collision shape yet
         geomclass = dGeomGetClass(id);
         const dReal *pos = dGeomGetPosition(id);
         const dReal *rot = dGeomGetRotation(id);
         sgMat4 tmp;
         sgMakeIdentMat4(tmp);
         sgSetVec3(tmp[0], rot[0], rot[1], rot[2] );
         sgSetVec3(tmp[1], rot[4], rot[5], rot[6] );
         sgSetVec3(tmp[2], rot[8], rot[9], rot[10] );
         sgSetVec3(tmp[3], pos[0], pos[1], pos[2] );

         sgPostMultMat4(absMat, tmp);
      }

      switch(dGeomGetClass(id))
      {
      case dBoxClass:
         {
            dVector3 side;
            dGeomBoxGetLengths (id, side);
            const dReal *pos = dGeomGetPosition(id);
            const dReal *rot = dGeomGetRotation(id);
            mGeomGeod.get()->addDrawable(
               new osg::ShapeDrawable(
               new osg::Box(osg::Vec3(absMat[3][0], absMat[3][1], absMat[3][2]),
                                       side[0], side[1], side[2]), hints) );
         }
         break;
      case dSphereClass:
         {
            dReal rad = dGeomSphereGetRadius(id);
            mGeomGeod.get()->addDrawable(
               new osg::ShapeDrawable(
               new osg::Sphere(osg::Vec3(absMat[3][0], absMat[3][1], absMat[3][2]),
                              rad), hints ) );
         }
         break;
      case dCCylinderClass:
         {
            dReal radius, length;
            dGeomCCylinderGetParams (id, &radius, &length);
            mGeomGeod.get()->addDrawable(
               new osg::ShapeDrawable(
               new osg::Cylinder(osg::Vec3(absMat[3][0], absMat[3][1], absMat[3][2]),
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
      mParentScene->UnRegisterPhysical(this);
      DeltaDrawable::AddedToScene( scene );
   }

   
} 
