#include <dtCore/odegeomwrap.h>
#include <dtCore/transform.h>
#include <dtUtil/boundingshapeutils.h>
#include <dtUtil/log.h>
#include <ode/collision.h>
#include <osg/TriangleFunctor>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/PolygonOffset>

using namespace dtCore;

IMPLEMENT_ENUM(CollisionGeomType)
CollisionGeomType CollisionGeomType::NONE("NONE");
CollisionGeomType CollisionGeomType::SPHERE("SPHERE");
CollisionGeomType CollisionGeomType::CYLINDER("CYLINDER");
CollisionGeomType CollisionGeomType::CCYLINDER("CCYLINDER");
CollisionGeomType CollisionGeomType::CUBE("CUBE");
CollisionGeomType CollisionGeomType::RAY("RAY");
CollisionGeomType CollisionGeomType::MESH("MESH");

//////////////////////////////////////////////////////////////////////////
dtCore::ODEGeomWrap::ODEGeomWrap()
   : mGeomID(NULL)
   , mOriginalGeomID(NULL)
   , mTriMeshDataID(NULL)
   , mMeshVertices(NULL)
   , mMeshIndices(NULL)
{
   // Setup default collision geometry stuff.
   mGeomID = dCreateGeomTransform(0); //Add support for more spaces.
   dGeomTransformSetCleanup(mGeomID, 1);
   dGeomTransformSetInfo(mGeomID, 1);
   dGeomDisable(mGeomID);
}

//////////////////////////////////////////////////////////////////////////
dtCore::ODEGeomWrap::~ODEGeomWrap()
{
   dGeomDestroy(mGeomID);

   if (mTriMeshDataID != NULL)
   {
      dGeomTriMeshDataDestroy(mTriMeshDataID);
   }

   if (mMeshVertices != NULL)
   {
      delete[] mMeshVertices;
      delete[] mMeshIndices;
   }
}

//////////////////////////////////////////////////////////////////////////
dGeomID dtCore::ODEGeomWrap::GetGeomID() const
{
   return mGeomID;
}

//////////////////////////////////////////////////////////////////////////
dGeomID dtCore::ODEGeomWrap::GetOriginalGeomID() const
{
   return mOriginalGeomID;
}

//////////////////////////////////////////////////////////////////////////
dtCore::CollisionGeomType* dtCore::ODEGeomWrap::GetCollisionGeomType() const
{
   int geomClass = dGeomGetClass(mGeomID);
   dGeomID id = mGeomID;
   while (geomClass == dGeomTransformClass && id != 0)
   {
      id = dGeomTransformGetGeom(id);

      if (id != 0)
      {
         geomClass = dGeomGetClass(id);
      }
   }

   switch (geomClass)
   {
   case dSphereClass:
      return &CollisionGeomType::SPHERE;
   case dBoxClass:
      return &CollisionGeomType::CUBE;
   case dCylinderClass:
      return &CollisionGeomType::CYLINDER;
   case dCCylinderClass:
      return &CollisionGeomType::CCYLINDER;
   case dRayClass:
      return &CollisionGeomType::RAY;
   case dTriMeshClass:
      return &CollisionGeomType::MESH;
   default:
      break;
   }

   return &CollisionGeomType::NONE;

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::GetCollisionGeomDimensions(std::vector<float>& dimensions)
{
   dimensions.clear();

   // The while loop is repeated here because we need both the type
   // and actual dGeomID of the geometry. GetCollisionGeomType only
   // return the type.
   int geomClass = dGeomGetClass(mGeomID);
   dGeomID id = mGeomID;
   while (geomClass == dGeomTransformClass && id != 0)
   {
      id = dGeomTransformGetGeom(id);
      if (id != 0)
      {
         geomClass = dGeomGetClass(id);
      }
   }

   switch (geomClass)
   {
   case dSphereClass:
      {
         dReal rad = dGeomSphereGetRadius(id);
         dimensions.push_back(float(rad));

         break;
      }
   case dBoxClass:
      {
         dVector3 sides;
         dGeomBoxGetLengths(id, sides);
         for (int i = 0; i < 3; i++)
         {
            dimensions.push_back(float(sides[i]));
         }

         break;
      }
   case dCylinderClass:
      {
         dReal radius, length;
         dGeomCylinderGetParams(id, &radius, &length);

         dimensions.push_back(float(radius));
         dimensions.push_back(float(length));
         break;
      }
   case dCCylinderClass:
      {
         dReal radius, length;
         dGeomCCylinderGetParams(id, &radius, &length);

         dimensions.push_back(float(radius));
         dimensions.push_back(float(length));
         break;
      }
   case dRayClass:
      {
         dVector3 start, dir;
         dReal length = dGeomRayGetLength(id);
         dGeomRayGet(id, start, dir);

         dimensions.push_back( length );
         for (int i = 0; i < 3; i++)
         {
            dimensions.push_back(float(start[i]));
         }
         for (int i = 0; i < 3; i++)
         {
            dimensions.push_back(float(dir[i]));
         }
         break;
      }
   case dTriMeshClass:
   default:
      break;
   }

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionDetection(bool enabled)
{
   if (mGeomID != 0)
   {
      int geomClass = dGeomGetClass(mGeomID);
      dGeomID id = mGeomID;
      while (geomClass == dGeomTransformClass && id != 0)
      {
         id = dGeomTransformGetGeom(id);
         if (id != 0)
         {
            geomClass = dGeomGetClass(id);
         }
      }

      // If we get here then a collision shape has been found.
      // Now we can enable/disable the GeomTransform at the top of the hierarchy.
      // Disabling the collision geometry itself will not prevent the transform
      // from colliding and still moving the collision hierarchy with it.
      if (enabled)
      {
         dGeomEnable(mGeomID);
      }
      else
      {
         dGeomDisable(mGeomID);
      }
   }

}

//////////////////////////////////////////////////////////////////////////
bool dtCore::ODEGeomWrap::GetCollisionDetection() const
{
   if (mGeomID != 0)
   {
      return dGeomIsEnabled(mGeomID) == 1;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionGeom(dGeomID geom)
{
   dGeomTransformSetGeom(mGeomID, geom);

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionSphere(float radius)
{
   mOriginalGeomID = dCreateSphere(0, radius);
   dGeomDisable(mOriginalGeomID);

   dGeomTransformSetGeom(mGeomID, dCreateSphere(0, radius));

   SetCollisionDetection(true);
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
      for (unsigned int i=0;i < node.getNumDrawables(); i++)
      {
         osg::Drawable* d = node.getDrawable(i);

         if (d->supports(mFunctor))
         {
            osg::NodePath nodePath = getNodePath();

#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR == 1 && OSG_VERSION_MINOR == 0
            // Luckily, this behavior is redundant with OSG 1.1
            if (dynamic_cast<osg::CameraNode*>(nodePath[0]) != NULL)
            {
               nodePath = osg::NodePath( nodePath.begin()+1, nodePath.end() );
            }
#endif // OSG 1.1

            mFunctor.mMatrix = osg::computeLocalToWorld(nodePath);

            d->accept(mFunctor);
         }
      }
   }
};


/**
 * Determines the sphere parameters
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
         tv2 = v2 * mMatrix,
         tv3 = v3 * mMatrix;

      tv1[2] = 0;
      if (tv1.length() > mRadius)
      {
         mRadius = tv1.length();
      }

      tv2[2] = 0;
      if (tv2.length() > mRadius)
      {
         mRadius = tv2.length();
      }

      tv3[2] = 0;
      if (tv2.length() > mRadius)
      {
         mRadius = tv3.length();
      }
   }
};

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionSphere(osg::Node* node)
{

   if (node)
   {
      DrawableVisitor<SphereFunctor> sv;
      node->accept(sv);

      //GetMatrixNode()->setMatrix(oldMatrix);

      if (sv.mFunctor.mRadius > 0)
      {
         mOriginalGeomID = dCreateSphere(0, sv.mFunctor.mRadius);
         dGeomDisable( mOriginalGeomID );

         dGeomTransformSetGeom(mGeomID, dCreateSphere(0, sv.mFunctor.mRadius));
      }
      else
      {
         // This case happens in STAGE when the collision shape property is
         // parsed _before_ the mesh filename. After the mesh has been
         // loaded then the box will be recalculated.
         LOG_INFO("Calculated values for collision sphere are invalid. If you are reading this from STAGE, don't worry, it just means the collision shape ActorProperty was parsed from XML before the actual mesh. The collision shape will be recalculated when the mesh is loaded.")
      }
   }

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionBox(float lx, float ly, float lz)
{
   mOriginalGeomID = dCreateBox(0, lx, ly, lz);
   dGeomDisable( mOriginalGeomID );

   dGeomTransformSetGeom(mGeomID, dCreateBox(0, lx, ly, lz) );
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionBox(osg::Node* node)
{
   if (node != 0)
   {

      dtUtil::BoundingBoxVisitor bbv;
      node->accept(bbv);

      float lx = bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin();
      float ly = bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin();
      float lz = bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin();

      if (lx > 0.0f && ly > 0.0f && lz > 0.0f)
      {
         dGeomID subTransformID = dCreateGeomTransform(0);

         dGeomTransformSetCleanup(subTransformID, 1);

         mOriginalGeomID =  dCreateBox(0, lx, ly, lz);

         dGeomDisable(mOriginalGeomID);

         dGeomTransformSetGeom(subTransformID, dCreateBox(0,
            bbv.mBoundingBox.xMax() - bbv.mBoundingBox.xMin(),
            bbv.mBoundingBox.yMax() - bbv.mBoundingBox.yMin(),
            bbv.mBoundingBox.zMax() - bbv.mBoundingBox.zMin()));

         dGeomSetPosition(subTransformID,
            bbv.mBoundingBox.center()[0],
            bbv.mBoundingBox.center()[1],
            bbv.mBoundingBox.center()[2]);

         dGeomTransformSetGeom(mGeomID, subTransformID);

      }
      else
      {
         // This case happens in STAGE when the collision shape property is
         // parsed _before_ the mesh filename. After the mesh has been
         // loaded then the box will be recalculated.
         LOG_INFO("Calculated values for collision box are invalid. If you are reading this from STAGE, don't worry, it just means the collision shape ActorProperty was parsed from XML before the actual mesh. The collision shape will be recalculated when the mesh is loaded.")
      }
   }
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

      if (tv1[2] < mMinZ) mMinZ = tv1[2];
      else if (tv1[2] > mMaxZ) mMaxZ = tv1[2];

      if (tv2[2] < mMinZ) mMinZ = tv2[2];
      else if (tv2[2] > mMaxZ) mMaxZ = tv2[2];

      if (tv3[2] < mMinZ) mMinZ = tv3[2];
      else if (tv3[2] > mMaxZ) mMaxZ = tv3[2];

      tv1[2] = 0;
      if (tv1.length() > mRadius) { mRadius = tv1.length(); }

      tv2[2] = 0;
      if (tv2.length() > mRadius) { mRadius = tv2.length(); }

      tv3[2] = 0;
      if (tv2.length() > mRadius) { mRadius = tv3.length(); }
   }
};

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionCylinder(float radius, float length)
{
   mOriginalGeomID = dCreateCylinder(0, radius, length);
   dGeomDisable(mOriginalGeomID);

   dGeomTransformSetGeom(mGeomID, dCreateCylinder(0, radius, length));
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionCylinder(osg::Node* node)
{

   if (node)
   {
      DrawableVisitor<CylinderFunctor> cv;
      node->accept(cv);

      float radius = cv.mFunctor.mRadius;
      float length = cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ;

      if (radius > 0 && length > 0)
      {
         dGeomID subTransformID = dCreateGeomTransform(0);

         dGeomTransformSetCleanup(subTransformID, 1);

         mOriginalGeomID = dCreateCylinder(0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ);
         dGeomDisable(mOriginalGeomID);

         dGeomTransformSetGeom(subTransformID, dCreateCylinder(0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ));

         dGeomTransformSetGeom(mGeomID, subTransformID);

      }
      else
      {
         // This case happens in STAGE when the collision shape property is
         // parsed _before_ the mesh filename. After the mesh has been
         // loaded then the box will be recalculated.
         LOG_INFO("Calculated values for collision cylinder are invalid. If you are reading this from STAGE, don't worry, it just means the collision shape ActorProperty was parsed from XML before the actual mesh. The collision shape will be recalculated when the mesh is loaded.")
      }
   }

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionCappedCylinder(float radius, float length)
{
   mOriginalGeomID = dCreateCCylinder(0, radius, length);
   dGeomDisable(mOriginalGeomID);

   dGeomTransformSetGeom(mGeomID, dCreateCCylinder(0, radius, length));
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionCappedCylinder(osg::Node* node)
{

   if (node)
   {
      DrawableVisitor<CylinderFunctor> cv;
      node->accept(cv);

      float radius = cv.mFunctor.mRadius;
      float length = cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ;

      if (radius > 0 && length > 0)
      {
         dGeomID subTransformID = dCreateGeomTransform(0);

         dGeomTransformSetCleanup(subTransformID, 1);

         mOriginalGeomID = dCreateCCylinder(0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ);
         dGeomDisable(mOriginalGeomID);

         dGeomTransformSetGeom(subTransformID, dCreateCCylinder(0, cv.mFunctor.mRadius, cv.mFunctor.mMaxZ - cv.mFunctor.mMinZ));

         dGeomTransformSetGeom(mGeomID, subTransformID);

      }
      else
      {
         // This case happens in STAGE when the collision shape property is
         // parsed _before_ the mesh filename. After the mesh has been
         // loaded then the box will be recalculated.
         LOG_INFO("Calculated values for collision cylinder are invalid. If you are reading this from STAGE, don't worry, it just means the collision shape ActorProperty was parsed from XML before the actual mesh. The collision shape will be recalculated when the mesh is loaded.")
      }
   }

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionRay(float length)
{
   mOriginalGeomID = dCreateRay(0, length);
   dGeomDisable(mOriginalGeomID);

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

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionMesh(osg::Node* node)
{

   if (node)
   {
      DrawableVisitor<TriangleRecorder> mv;

      node->accept(mv);

      if (mMeshVertices != 0)
      {
         delete[] mMeshVertices;
         delete[] mMeshIndices;
      }

      mMeshVertices = new dVector3[mv.mFunctor.mVertices.size()];
      mMeshIndices = new int[mv.mFunctor.mTriangles.size()*3];

      if (!mv.mFunctor.mVertices.empty())
      {
         memcpy(mMeshVertices,
            &mv.mFunctor.mVertices[0],
            mv.mFunctor.mVertices.size()*sizeof(StridedVertex));
      }

      if (!mv.mFunctor.mTriangles.empty())
      {
         memcpy(mMeshIndices,
            &mv.mFunctor.mTriangles[0],
            mv.mFunctor.mTriangles.size()*sizeof(StridedTriangle));
      }

      if (mTriMeshDataID == NULL)
      {
         mTriMeshDataID = dGeomTriMeshDataCreate();
      }

      dGeomTriMeshDataBuildSimple(mTriMeshDataID,
         (dReal*)mMeshVertices,
         mv.mFunctor.mVertices.size(),
         (dTriIndex*)mMeshIndices,
         mv.mFunctor.mTriangles.size()*3);

      dGeomTransformSetGeom(mGeomID, dCreateTriMesh(0, mTriMeshDataID, 0, 0, 0));
   }

}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::ClearCollisionGeometry()
{
   SetCollisionDetection(false);
   dGeomTransformSetGeom(mGeomID, 0);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionCategoryBits(unsigned long bits)
{
   dGeomSetCategoryBits( mGeomID, bits );
}

//////////////////////////////////////////////////////////////////////////
unsigned long dtCore::ODEGeomWrap::GetCollisionCategoryBits() const
{
   return dGeomGetCategoryBits(mGeomID);
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::SetCollisionCollideBits(unsigned long bits)
{
   dGeomSetCollideBits( mGeomID, bits );
}

//////////////////////////////////////////////////////////////////////////
unsigned long dtCore::ODEGeomWrap::GetCollisionCollideBits() const
{
   return dGeomGetCollideBits(mGeomID);
}

//////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Geode> dtCore::ODEGeomWrap::CreateRenderedCollisionGeometry() const
{
   dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

   //geode->setName(Transformable::COLLISION_GEODE_ID);
   osg::TessellationHints* hints = new osg::TessellationHints;
   hints->setDetailRatio(0.5f);

   int geomClass = dGeomGetClass(GetGeomID());
   dGeomID id = GetGeomID();

   osg::Matrix absMatrix;

   while (geomClass == dGeomTransformClass)
   {
      id = dGeomTransformGetGeom(id);

      if (id == 0)
      {
         return NULL; // in case we haven't assigned a collision shape yet
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

      absMatrix.postMult(tempMatrix);
   }

   switch (dGeomGetClass(id))
   {
   case dBoxClass:
      {
         dVector3 side;
         dGeomBoxGetLengths(GetOriginalGeomID(), side);
         geode.get()->addDrawable(
            new osg::ShapeDrawable(
            new osg::Box(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
            side[0], side[1], side[2]), hints));
      }
      break;
   case dSphereClass:
      {
         dReal rad = dGeomSphereGetRadius(GetOriginalGeomID());
         geode.get()->addDrawable(
            new osg::ShapeDrawable(
            new osg::Sphere(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
            rad), hints));
      }
      break;
   case dCylinderClass:
      {
         dReal radius, length;
         dGeomCylinderGetParams(GetOriginalGeomID(), &radius, &length);
         geode.get()->addDrawable(
            new osg::ShapeDrawable(
            new osg::Cylinder(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
            radius, length), hints));
      }
      break;
   case dCCylinderClass:
      {
         dReal radius, length;
         dGeomCCylinderGetParams(GetOriginalGeomID(), &radius, &length);
         geode.get()->addDrawable(
            new osg::ShapeDrawable(
            new osg::Capsule(osg::Vec3(absMatrix(3,0), absMatrix(3,1), absMatrix(3,2)),
            radius, length), hints));
      }
      break;

   case dTriMeshClass:
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
      dtUtil::Log::GetInstance().LogMessage( dtUtil::Log::LOG_WARNING, __FILE__,
         "Transformable:Can't render unhandled geometry class:%d",
         dGeomGetClass(id) );
      break;
   }

   osg::Material* mat = new osg::Material();
   mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.f,0.f,1.f, 0.5f));
   mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.f,0.f,1.f, 1.f));
   mat->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(0.f,0.f,0.f, 1.f));

   osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
   polyoffset->setFactor(-1.0f);
   polyoffset->setUnits(-1.0f);

   osg::StateSet* ss = geode.get()->getOrCreateStateSet();
   ss->setAttributeAndModes(mat, osg::StateAttribute::OVERRIDE |
      osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
   ss->setMode(GL_BLEND,osg::StateAttribute::OVERRIDE |
      osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
   ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   ss->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE |
      osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

   return geode;
}

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::UpdateGeomTransform(const dtCore::Transform& newTransform)
{
   // If the supplied Transform is different than the current position
   // and rotation of the Geom, then set the Geom to match.

   if (GetCollisionDetection() == false) { return; }

   dtCore::Transform odeTransform;
   GetGeomTransform(odeTransform);

   if (!newTransform.EpsilonEquals(odeTransform))
   {
      osg::Matrix rotation;
      osg::Vec3 position;

      newTransform.GetTranslation(position);
      newTransform.GetRotation(rotation);

      // Set translation
      dGeomSetPosition(GetGeomID(), position[0], position[1], position[2]);

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

      dGeomSetRotation(GetGeomID(), dRot);

      int geomClass = dGeomGetClass(GetGeomID());
      dGeomID id = GetGeomID();

      while (geomClass == dGeomTransformClass)
      {
         id = dGeomTransformGetGeom(id);
         if (id == 0)
         {
            return; // In case we haven't assigned a collision shape yet
         }
         geomClass = dGeomGetClass(id);
      }


      if (id)
      {
         switch (dGeomGetClass(id))
         {
         case dBoxClass:
            {
               dVector3 originalSide;
               dGeomBoxGetLengths(GetOriginalGeomID(), originalSide);
               dGeomBoxSetLengths(id, originalSide[0], originalSide[1], originalSide[2]);
            }
            break;
         case dSphereClass:
            {
               dReal originalRadius = dGeomSphereGetRadius(GetOriginalGeomID());
               dGeomSphereSetRadius(id, originalRadius);
            }
            break;
         case dCCylinderClass:
            {
               dReal originalRadius, originalLength;
               dGeomCCylinderGetParams(GetOriginalGeomID(), &originalRadius, &originalLength);

               dGeomCCylinderSetParams(id, originalRadius, originalLength);
            }
            break;
         case dRayClass:
            {
               dVector3 start, dir;
               dReal originalLength = dGeomRayGetLength(GetOriginalGeomID());

               dGeomRayGet(GetOriginalGeomID(), start, dir);

               // Ignore x/y scaling, use z to scale ray
               dGeomRaySetLength(id, originalLength);
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

//////////////////////////////////////////////////////////////////////////
void dtCore::ODEGeomWrap::GetGeomTransform(dtCore::Transform& xform) const
{
   const dReal* position = dGeomGetPosition(GetGeomID());
   const dReal* rotation = dGeomGetRotation(GetGeomID());

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

   xform.SetTranslation( position[0], position[1], position[2] );
   xform.SetRotation(newRotation);
}
