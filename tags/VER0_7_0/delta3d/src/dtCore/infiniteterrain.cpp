// infiniteterrain.cpp: Implementation of the InfiniteTerrain class.
//
//////////////////////////////////////////////////////////////////////

#include "dtCore/infiniteterrain.h"
#include "dtCore/notify.h"

#include "osg/CullFace"
#include "osg/Drawable"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"
#include "osg/TexGen"
#include "osg/Texture2D"
#include "osg/PrimitiveSet"

#include "osgDB/ReadFile"

using namespace dtCore;
using namespace std;


IMPLEMENT_MANAGEMENT_LAYER(InfiniteTerrain)


/**
 * The terrain callback class.  Builds terrain segments
 * around viewer.
 */
class dtCore::InfiniteTerrainCallback : public osg::NodeCallback
{
   public:
   
      /**
       * Constructor.
       *
       * @param terrain the owning InfiniteTerrain object
       */
      InfiniteTerrainCallback(InfiniteTerrain* terrain)
         : mTerrain(terrain)
      {}
      
      /**
       * Callback function.
       *
       * @param node the node to operate on
       * @param nv the active node visitor
       */
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         if(mTerrain->mClearFlag)
         {
            mTerrain->mNode->removeChild(0, mTerrain->mNode->getNumChildren());
   
            mTerrain->mBuiltSegments.clear();
            
            mTerrain->mClearFlag = false;
         }
         
         osg::Vec3 eyepoint = nv->getEyePoint();
         
         float bd = mTerrain->GetBuildDistance(),
               bd2 = bd*2,
               x = eyepoint[0] - bd,
               y = eyepoint[1] - bd;
               
         for(float i=0.0f;i<=bd2;i+=mTerrain->mSegmentSize)
         {
            for(float j=0.0f;j<=bd2;j+=mTerrain->mSegmentSize)
            {
               mTerrain->BuildSegment(
                  (int)((x + i)/mTerrain->mSegmentSize),
                  (int)((y + j)/mTerrain->mSegmentSize)
               );
            }
         }
         
         traverse(node, nv);
      }
      
      
   private:
   
      /**
       * The owning InfiniteTerrain object.
       */
      InfiniteTerrain* mTerrain;
};


/**
 * The ODE InfiniteTerrain class identifier.
 */
static int dInfiniteTerrainClass = 0;


/**
 * Constructor.
 *
 * @param name the instance name
 */
InfiniteTerrain::InfiniteTerrain(string name, osg::Image* textureImage)
   : mSegmentSize(1000.0f),
     mSegmentDivisions(128),
     mHorizontalScale(0.01f),
     mVerticalScale(25.0f),
     mBuildDistance(3000.0f),
     mSmoothCollisionsEnabled(false),
     mClearFlag(false)
{
   SetName(name);
   
   mNode = new osg::Group;

   osg::StateSet* ss = mNode->getOrCreateStateSet();
   
   ss->setMode(GL_CULL_FACE, GL_TRUE);
   
   osg::Material* mat = new osg::Material;
   
   mat->setDiffuse(
      osg::Material::FRONT_AND_BACK, 
      osg::Vec4(1, 1, 1, 1)
   );
   
   ss->setAttribute(mat);
   
   osg::Image* image = NULL;

   if (textureImage != NULL)
      image = textureImage;
   else 
   {
      image = new osg::Image;

      sgPerlinNoise_2D texNoise;

      unsigned char* texture = new unsigned char[256*256*3];

      int k = 0;

      for(int i=0;i<256;i++)
      {
         for(int j=0;j<256;j++)
         {
            float val = 0.7f + texNoise.getNoise(i*0.1f, j*0.1f)*0.3f;

            texture[k++] = 0;
            texture[k++] = (unsigned char)(val*0.5*255);
            texture[k++] = (unsigned char)(val*0.1*255);
         }
      }

      image->setImage(
         256, 256, 1, 3, GL_RGB, GL_UNSIGNED_BYTE, 
         texture, osg::Image::USE_NEW_DELETE
         );
   }
   
   //image->computeMipMaps();
      
   osg::Texture2D* tex = new osg::Texture2D(image);
      
   tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
   
   ss->setTextureAttribute(0, tex);
      
   ss->setTextureMode(
      0, GL_TEXTURE_2D, GL_TRUE
   );
   
   mNode->setCullCallback(
      new InfiniteTerrainCallback(this)
   );
   
   if(dInfiniteTerrainClass == 0)
   {
      dGeomClass gc;
      
      gc.bytes = sizeof(InfiniteTerrain*);
      gc.collider = GetColliderFn;
      gc.aabb = GetAABB;
      gc.aabb_test = AABBTest;
      gc.dtor = 0;
      
      dInfiniteTerrainClass = dCreateGeomClass(&gc);
   }
   
   dGeomID geom = dCreateGeom(dInfiniteTerrainClass);
   
   *(InfiniteTerrain**)dGeomGetClassData(geom) = this;
   
   SetCollisionGeom(geom);
   
   RegisterInstance(this);
}

/**
 * Destructor.
 */
InfiniteTerrain::~InfiniteTerrain()
{
   DeregisterInstance(this);
}

/**
 * Returns this object's OpenSceneGraph node.
 *
 * @return the OpenSceneGraph node
 */
osg::Node* InfiniteTerrain::GetOSGNode()
{
   return mNode.get();
}

/**
 * Regenerates the terrain surface.
 */
void InfiniteTerrain::Regenerate()
{
   mNoise.regenerate();
   
   mClearFlag = true;
}

/**
 * Sets the size of the terrain segments.
 *
 * @param segmentSize the new segment size
 */
void InfiniteTerrain::SetSegmentSize(float segmentSize)
{
   mSegmentSize = segmentSize;
   
   mClearFlag = true;
}

/**
 * Returns the size of the terrain segments.
 *
 * @return the current segment size
 */
float InfiniteTerrain::GetSegmentSize() const
{
   return mSegmentSize;
}

/**
 * Sets the number of divisions in each segment.
 *
 * @param segmentDivisions the new segment divisions
 */
void InfiniteTerrain::SetSegmentDivisions(int segmentDivisions)
{
   mSegmentDivisions = segmentDivisions;
   
   mClearFlag = true;
}

/**
 * Returns the number of divisions in each segment.
 *
 * @return the current segment divisions
 */
int InfiniteTerrain::GetSegmentDivisions() const
{
   return mSegmentDivisions;
}
         
/**
 * Sets the horizontal scale, which affects the
 * feature frequency.
 *
 * @param horizontalScale the new horizontal scale
 */
void InfiniteTerrain::SetHorizontalScale(float horizontalScale)
{
   mHorizontalScale = horizontalScale;
   
   mClearFlag = true;
}

/**
 * Returns the horizontal scale.
 *
 * @return the horizontal scale
 */
float InfiniteTerrain::GetHorizontalScale() const
{
   return mHorizontalScale;
}

/**
 * Sets the vertical scale, which affects the feature
 * amplitude.
 *
 * @param verticalScale the new vertical scale
 */
void InfiniteTerrain::SetVerticalScale(float verticalScale)
{
   mVerticalScale = verticalScale;
   
   mClearFlag = true;
}

/**
 * Returns the vertical scale.
 *
 * @return the vertical scale
 */
float InfiniteTerrain::GetVerticalScale() const
{
   return mVerticalScale;
}

/**
 * Sets the build distance: the distance from the eyepoint for
 * which terrain is guaranteed to be generated.
 *
 * @param buildDistance the new build distance
 */
void InfiniteTerrain::SetBuildDistance(float buildDistance)
{
   mBuildDistance = buildDistance;
}

/**
 * Returns the build distance.
 *
 * @return the build distance
 */
float InfiniteTerrain::GetBuildDistance() const
{
   return mBuildDistance;
}

/**
 * Enables or disables smooth collision detection (collision detection
 * based on the underlying noise function, rather than the triangle
 * mesh).
 *
 * @param enable true to enable, false to disable
 */
void InfiniteTerrain::EnableSmoothCollisions(bool enable)
{
   mSmoothCollisionsEnabled = enable;
}

/**
 * Checks whether smooth collision detection is enabled.
 *
 * @return true if enabled, false if disabled
 */
bool InfiniteTerrain::SmoothCollisionsEnabled() const
{
   return mSmoothCollisionsEnabled;
}
         
/**
 * Determines the height of the terrain at the specified location.
 *
 * @param x the x coordinate to check
 * @param y the y coordinate to check
 * @param smooth if true, use height of underlying noise function
 * instead of triangle mesh height
 * @return the height at the specified location
 */
float InfiniteTerrain::GetHeight(float x, float y, bool smooth)
{
   if(smooth)
   {
      return mVerticalScale * 2.0f *
             mNoise.getNoise(x * mHorizontalScale, y * mHorizontalScale) - 1.0f;
   }
   else
   {
      float scale = mSegmentSize / mSegmentDivisions;
      
      x /= scale;
      y /= scale;
      
      float fx = floor(x), fy = floor(y),
            cx = ceil(x), cy = ceil(y),
            ix = x - fx, iy = y - fy;
            
      if(ix < iy)
      {
         float p00 = GetHeight(fx*scale, fy*scale, true),
               p01 = GetHeight(fx*scale, cy*scale, true),
               p11 = GetHeight(cx*scale, cy*scale, true),
               p00_01 = p00 + iy*(p01-p00);
               
         return p00_01 + ix*(p11 - p00_01);
      }
      else
      {
         float p00 = GetHeight(fx*scale, fy*scale, true),
               p10 = GetHeight(cx*scale, fy*scale, true),
               p11 = GetHeight(cx*scale, cy*scale, true),
               p10_11 = p10 + iy*(p11-p10);
               
         return p00 + ix*(p10_11 - p00);
      }      
   }
}

/**
 * Retrieves the normal of the terrain at the specified location.
 *
 * @param x the x coordinate to check
 * @param y the y coordinate to check
 * @param normal the location at which to store the normal
 * @param smooth if true, use height of underlying noise function
 * instead of triangle mesh height
 */
void InfiniteTerrain::GetNormal(float x, float y, sgVec3 normal, bool smooth)
{
   if(smooth)
   {
      float z = GetHeight(x, y, true);
   
      sgVec3 v1 = { 0.1f, 0.0f, GetHeight(x + 0.1f, y, true) - z },
             v2 = { 0.0f, 0.1f, GetHeight(x, y + 0.1f, true) - z };
                   
      sgVectorProductVec3(normal, v1, v2);
            
      sgNormalizeVec3(normal);
   }
   else
   {
      float scale = mSegmentSize / mSegmentDivisions;
      
      x /= scale;
      y /= scale;
      
      float fx = floor(x), fy = floor(y),
            cx = ceil(x), cy = ceil(y),
            ix = x - fx, iy = y - fy;
            
      if(ix < iy)
      {
         float p00 = GetHeight(fx*scale, fy*scale, true),
               p01 = GetHeight(fx*scale, cy*scale, true),
               p11 = GetHeight(cx*scale, cy*scale, true);
               
         sgVec3 v1 = { 0.0f, -scale, p00 - p01 },
                v2 = { scale, 0.0f, p11 - p01 };
         
         sgVectorProductVec3(normal, v1, v2);
            
         sgNormalizeVec3(normal);
      }
      else
      {
         float p00 = GetHeight(fx*scale, fy*scale, true),
               p10 = GetHeight(cx*scale, fy*scale, true),
               p11 = GetHeight(cx*scale, cy*scale, true);
               
         sgVec3 v1 = { 0.0f, scale, p11 - p10 },
                v2 = { -scale, 0.0f, p00 - p10 };
         
         sgVectorProductVec3(normal, v1, v2);
            
         sgNormalizeVec3(normal);
      }      
   }
}

/**
 * Builds a single terrain segment.
 *
 * @param x the x coordinate at which to build the segment
 * @param y the y coordinate at which to build the segment
 */
void InfiniteTerrain::BuildSegment(int x, int y)
{
   Segment coord(x, y);
   
   if(mBuiltSegments.count(coord) > 0)
   {
      return;
   }
   else
   {
      mBuiltSegments.insert(coord);
   }
   
   osg::LOD* lod = new osg::LOD;
   
   osg::Geode* geode = new osg::Geode;
   
   osg::Geometry* geom = new osg::Geometry;
   
   int width = mSegmentDivisions + 1,
       height = mSegmentDivisions + 1;
   
   sgVec2 minimum = { x * mSegmentSize, y * mSegmentSize };
          
   float halfStep = 0.5f * (mSegmentSize / mSegmentDivisions);
   
   osg::ref_ptr<osg::Vec3Array> vertices = 
      new osg::Vec3Array(width*height);
   
   osg::ref_ptr<osg::Vec3Array> normals =
      new osg::Vec3Array(width*height);
   
   osg::ref_ptr<osg::Vec2Array> textureCoordinates =
      new osg::Vec2Array(width*height);
      
   int i, j;
   
   for(i=0;i<height;i++)
   {
      for(j=0;j<width;j++)
      {
         float x = minimum[0] + j * (mSegmentSize / mSegmentDivisions),
               y = minimum[1] + i * (mSegmentSize / mSegmentDivisions);
               
         (*vertices)[i*width+j].set(
            x, y,
            GetHeight(x, y, true)
         );
         
         sgVec3 normal;
         
         GetNormal(x, y, normal, true);
         
         (*normals)[i*width+j].set(normal[0], normal[1], normal[2]);
         
         (*textureCoordinates)[i*width+j].set(x*0.1, y*0.1);
      }
   }
   
   geom->setVertexArray(vertices.get());
   
   geom->setNormalArray(normals.get());

   geom->setTexCoordArray(0, textureCoordinates.get());
   
   osg::ref_ptr<osg::IntArray> indices = 
      new osg::IntArray(mSegmentDivisions*width*2);
   
   for(i=0;i<mSegmentDivisions;i++)
   {
      for(j=0;j<width;j++)
      {
         (*indices)[i*width*2 + j*2] = (i+1)*width + j;
         
         (*indices)[i*width*2 + j*2 + 1] = i*width + j;
      }
   }
   
   geom->setVertexIndices(indices.get());
   
   geom->setNormalIndices(indices.get());
   
   geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
   
   geom->setTexCoordIndices(0, indices.get());
   
   for(i=0;i<mSegmentDivisions;i++)
   {
      geom->addPrimitiveSet(
         new osg::DrawArrays(
            osg::PrimitiveSet::TRIANGLE_STRIP,
            i*width*2,
            width*2
         )
      );
   }
   
   geom->setUseDisplayList(true);
   
   geode->addDrawable(geom);
   
   lod->addChild(geode, 0.0f, mBuildDistance);
   
   mNode->addChild(lod);
}

/**
 * ODE collision function: Gets the contact points between two
 * geoms.
 *
 * @param o1 the first (InfiniteTerrain) geom
 * @param o2 the second geom
 * @param flags collision flags
 * @param contact the array of contact geoms to fill
 * @param skip the distance between contact geoms in the array
 * @return the number of contact points generated
 */
int InfiniteTerrain::Collider(dGeomID o1, dGeomID o2, int flags,
                              dContactGeom* contact, int skip)
{
   InfiniteTerrain* it = *(InfiniteTerrain**)dGeomGetClassData(o1);
   
   int numContacts = 0,
       maxContacts = flags & 0xFFFF;
   
   int geomClass = dGeomGetClass(o2);
   
   const dReal* position = dGeomGetPosition(o2);
   const dReal* rotation = dGeomGetRotation(o2);
   
   sgMat4 mat = {
      { rotation[0], rotation[4], rotation[8], 0.0f },
      { rotation[1], rotation[5], rotation[9], 0.0f },
      { rotation[2], rotation[6], rotation[10], 0.0f },
      { position[0], position[1], position[2], 1.0f }
   };
   
   if(geomClass == dBoxClass)
   {
      dVector3 lengths;
      
      dGeomBoxGetLengths(o2, lengths);
      
      lengths[0] *= 0.5f;
      lengths[1] *= 0.5f;
      lengths[2] *= 0.5f;
      
      sgVec3 corners[8] =
      {
         {-lengths[0], -lengths[1], -lengths[2]},
         {-lengths[0], -lengths[1], +lengths[2]},
         {-lengths[0], +lengths[1], -lengths[2]},
         {-lengths[0], +lengths[1], +lengths[2]},
         {+lengths[0], -lengths[1], -lengths[2]},
         {+lengths[0], -lengths[1], +lengths[2]},
         {+lengths[0], +lengths[1], -lengths[2]},
         {+lengths[0], +lengths[1], +lengths[2]}
      };
      
      for(int i=0;i<8 && i<maxContacts;i++)
      {
         sgXformPnt3(corners[i], mat);
         
         sgVec3 point =
         {
            corners[i][0], 
            corners[i][1], 
            it->GetHeight(
               corners[i][0], 
               corners[i][1], 
               it->mSmoothCollisionsEnabled
            )
         };
      
         sgVec3 normal;
      
         it->GetNormal(
            corners[i][0], 
            corners[i][1], 
            normal, 
            true
         );
      
         sgVec4 plane;
      
         sgMakePlane(plane, normal, point);
      
         float dist = sgDistToPlaneVec3(plane, corners[i]);
      
         if(dist <= 0.0f)
         {
            contact->pos[0] = corners[i][0] - dist*normal[0];
            contact->pos[1] = corners[i][1] - dist*normal[1];
            contact->pos[2] = corners[i][2] - dist*normal[2];
            
            contact->normal[0] = -normal[0];
            contact->normal[1] = -normal[1];
            contact->normal[2] = -normal[2];
            
            contact->depth = -dist;
            
            contact->g1 = o1;
            contact->g2 = o2;
            
            numContacts++;
            
            contact = (dContactGeom*)(((char*)contact) + skip);
         }   
      }
   }
   else if(geomClass == dSphereClass)
   {
      dReal radius = dGeomSphereGetRadius(o2);
      
      sgVec3 center = { 0.0f, 0.0f, 0.0f };
      
      sgXformPnt3(center, mat);
      
      sgVec3 point =
      {
         center[0], 
         center[1], 
         it->GetHeight(
            center[0], 
            center[1], 
            it->mSmoothCollisionsEnabled
         )
      };
      
      sgVec3 normal;
      
      it->GetNormal(
         center[0], 
         center[1], 
         normal, 
         true
      );
      
      sgVec4 plane;
      
      sgMakePlane(plane, normal, point);
      
      float dist = sgDistToPlaneVec3(plane, center);
      
      if(dist <= radius && maxContacts >= 1)
      {
         contact->pos[0] = center[0] - dist*normal[0];
         contact->pos[1] = center[1] - dist*normal[1];
         contact->pos[2] = center[2] - dist*normal[2];
         
         contact->normal[0] = -normal[0];
         contact->normal[1] = -normal[1];
         contact->normal[2] = -normal[2];
         
         contact->depth = radius - dist;
         
         contact->g1 = o1;
         contact->g2 = o2;
         
         numContacts++;
      }
   }
   
   return numContacts;
}

/**
 * ODE collision function: Finds the collider function appropriate
 * to detect collisions between InfiniteTerrain geoms and other
 * geoms.
 *
 * @param num the class number of the geom class to check
 * @return the appropriate collider function, or NULL for none
 */
dColliderFn* InfiniteTerrain::GetColliderFn(int num)
{
   if(num == dBoxClass ||
      num == dSphereClass)
   {
      return Collider;
   }
   else
   {
      return NULL;
   }
}

/**
 * ODE collision function: Computes the axis-aligned bounding box
 * for InfiniteTerrain instances.
 *
 * @param g the geom to check
 * @param aabb the location in which to store the axis-aligned
 * bounding box
 */
void InfiniteTerrain::GetAABB(dGeomID g, dReal aabb[6])
{
   InfiniteTerrain* it = *(InfiniteTerrain**)dGeomGetClassData(g);
   
   dInfiniteAABB(g, aabb);
   
   aabb[5] = it->GetVerticalScale();
}

/**
 * ODE collision function: Checks whether the specified axis-aligned
 * bounding box intersects with an InfiniteTerrain instance.
 *
 * @param o1 the first (InfiniteTerrain) geom
 * @param o2 the second geom
 * @param aabb2 the axis-aligned bounding box of the second geom
 * @return 1 if it intersects, 0 if it does not
 */
int InfiniteTerrain::AABBTest(dGeomID o1, dGeomID o2, dReal aabb2[6])
{
   InfiniteTerrain* it = *(InfiniteTerrain**)dGeomGetClassData(o1);
   
   if(it->GetHeight(aabb2[0], aabb2[1]) >= aabb2[2] ||
      it->GetHeight(aabb2[0], aabb2[4]) >= aabb2[2] ||
      it->GetHeight(aabb2[3], aabb2[1]) >= aabb2[2] ||
      it->GetHeight(aabb2[3], aabb2[4]) >= aabb2[2])
   {
      return 1;
   }
   else
   {
      return 0;
   }
}
