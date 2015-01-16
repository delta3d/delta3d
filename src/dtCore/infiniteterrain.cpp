// infiniteterrain.cpp: Implementation of the InfiniteTerrain class.
//
//////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/infiniteterrain.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/matrixutil.h>

#include <osg/Drawable>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Plane>
#include <osg/TexEnv>
#include <osg/Texture2D>
#include <osg/PrimitiveSet>
#include <osg/LOD>
#include <osgDB/ReadFile>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(InfiniteTerrain)

/**
 * The terrain callback class.  Builds terrain segments
 * around viewer.
 */
class InfiniteTerrainCallback : public osg::NodeCallback
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
         if (mTerrain->mClearFlag)
         {
            mTerrain->GetMatrixNode()->removeChild(0, mTerrain->GetMatrixNode()->getNumChildren());

            mTerrain->mBuiltSegments.clear();

            mTerrain->mClearFlag = false;
         }

         osg::Vec3 eyepoint = nv->getEyePoint();

         float bd = mTerrain->GetBuildDistance(),
               bd2 = bd*2,
               x = eyepoint[0] - bd,
               y = eyepoint[1] - bd;

         for (float i=0.0f;i<=bd2;i+=mTerrain->mSegmentSize)
         {
            for (float j=0.0f;j<=bd2;j+=mTerrain->mSegmentSize)
            {
               mTerrain->BuildSegment(
                  int((x + i)/mTerrain->mSegmentSize),
                  int((y + j)/mTerrain->mSegmentSize)
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
 * Constructor.
 *
 * @param name the instance name
 * @param textureImage An image to apply to the terrain.
 *
 */
InfiniteTerrain::InfiniteTerrain(const std::string& name, osg::Image* textureImage)
   :  Transformable(name),
      mSegmentSize(800.0f),
      mSegmentDivisions(128),
      mHorizontalScale(0.0035f),
      mVerticalScale(30.0f),
      mBuildDistance(3000.0f),
      mSmoothCollisionsEnabled(false),
      mClearFlag(false),
      mLOSPostSpacing(0.0f)
{
   SetName(name);

   SetupColorInfo();

   osg::StateSet* ss = GetOSGNode()->getOrCreateStateSet();

   ss->setMode(GL_CULL_FACE, GL_TRUE);

   osg::Image* image = 0;

   if (textureImage != 0)
   {
      image = textureImage;
   }
   else
   {
      image = new osg::Image;

      dtUtil::Noise2f texNoise;

      unsigned char* texture = new unsigned char[256*256*3];

      int k = 0;

      for (int i=0;i<256;i++)
      {
         for (int j=0;j<256;j++)
         {
            float val = 0.7f + texNoise.GetNoise(osg::Vec2f(i*0.1f, j*0.1f))*0.3f;

            texture[k++] = dtUtil::Min( 50 + (unsigned char)(val*255), 255);
            texture[k++] = dtUtil::Min( 50 + (unsigned char)(val*255), 255);
            texture[k++] = dtUtil::Min( 50 + (unsigned char)(val*255), 255);
         }
      }

      image->setImage(
         256, 256, 1, 3, GL_RGB, GL_UNSIGNED_BYTE,
         texture, osg::Image::USE_NEW_DELETE
         );
   }

   osg::Texture2D* tex = new osg::Texture2D(image);

   tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
   tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

   osg::TexEnv* texenv = new osg::TexEnv;
   texenv->setMode(osg::TexEnv::MODULATE);

   ss->setTextureAttribute(0, tex);
   ss->setTextureAttribute(0, texenv);

   ss->setTextureMode(
      0, GL_TEXTURE_2D, GL_TRUE
   );

   GetOSGNode()->setCullCallback( new InfiniteTerrainCallback(this) );


   RegisterInstance(this);

   SetLineOfSightSpacing(25.0f); // a bit less than DTED L2
}

/**
 * Destructor.
 */
InfiniteTerrain::~InfiniteTerrain()
{
   DeregisterInstance(this);
}

/**
 * Regenerates the terrain surface.
 */
void InfiniteTerrain::Regenerate()
{
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

//initializes info used for the GetColor function
void InfiniteTerrain::SetupColorInfo()
{
   //this is set based on the get height function
   //since the GetNoise will return a value from 0-1
   //the min and max height could be as follows

   mMinHeight = -2.0f * mVerticalScale;
   mMaxHeight = 2.0f * mVerticalScale;

   float difference = mMaxHeight - mMinHeight;
   float increment = difference / 3.0f;

   mMinColorIncrement = (increment * 2.0f);
   mMaxColorIncrement = increment;

   mIdealHeight = mMinColorIncrement + mMinHeight;

   //set the colors to interpolate between
   mMinColor.set((89.0f / 255.0f), (80.0f / 255.0f), (67.0f / 255.0f));
   mMaxColor.set((192.0f / 255.0f), (162.0f / 255.0f), (127.0f / 255.0f));
}

//returns an interpolated color based on the height
osg::Vec4 InfiniteTerrain::GetColor(float height)
{
   float r,g,b;

   float minPercent, maxPercent;
   osg::Vec3* minColor;
   osg::Vec3* maxColor;

   minPercent = std::min<float>(std::max<float>(0.0f, (mIdealHeight - height) / mMinColorIncrement), 1.0f);
   maxPercent = 1 - minPercent;
   maxColor = &mMaxColor;
   minColor = &mMinColor;

   r = (*maxColor)[0] * maxPercent;
   g = (*maxColor)[1] * maxPercent;
   b = (*maxColor)[2] * maxPercent;


   r += (*minColor)[0] * minPercent;
   g += (*minColor)[1] * minPercent * minPercent;
   b += (*minColor)[2] * minPercent * minPercent;

   return osg::Vec4(r, g, b, 1.0f);
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
   if (smooth)
   {
      osg::Vec2f osgvec((x + mBuildDistance) * mHorizontalScale, (y + mBuildDistance) * mHorizontalScale);
      return mVerticalScale * 2.0f * mNoise.GetNoise(osgvec) - 1.0f;
   }
   else
   {
      float scale = mSegmentSize / mSegmentDivisions;

      x /= scale;
      y /= scale;

      float fx = floor(x), fy = floor(y),
            cx = ceil(x), cy = ceil(y),
            ix = x - fx, iy = y - fy;

      if (ix < iy)
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
void InfiniteTerrain::GetNormal(float x, float y, osg::Vec3& normal, bool smooth)
{
   if (smooth)
   {
      float z = GetHeight(x, y, true);

      osg::Vec3 v1(0.1f, 0.0f, GetHeight(x + 0.1f, y, true) - z);
      osg::Vec3 v2(0.0f, 0.1f, GetHeight(x, y + 0.1f, true) - z );

      normal = v1 ^ v2;

      normal.normalize();
   }
   else
   {
      float scale = mSegmentSize / mSegmentDivisions;

      x /= scale;
      y /= scale;

      float fx = floor(x), fy = floor(y),
            cx = ceil(x), cy = ceil(y),
            ix = x - fx, iy = y - fy;

      if (ix < iy)
      {
         float p00 = GetHeight(fx*scale, fy*scale, true),
               p01 = GetHeight(fx*scale, cy*scale, true),
               p11 = GetHeight(cx*scale, cy*scale, true);

         osg::Vec3 v1(0.0f, -scale, p00 - p01);
         osg::Vec3 v2(scale, 0.0f, p11 - p01);

         normal = v1 ^ v2;

         normal.normalize();
      }
      else
      {
         float p00 = GetHeight(fx*scale, fy*scale, true),
               p10 = GetHeight(cx*scale, fy*scale, true),
               p11 = GetHeight(cx*scale, cy*scale, true);

         osg::Vec3 v1(0.0f, scale, p11 - p10);
         osg::Vec3 v2(-scale, 0.0f, p00 - p10 );

         normal = v1 ^ v2;

         normal.normalize();
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

   if (mBuiltSegments.count(coord) > 0)
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

   osg::Vec2 minimum(x * mSegmentSize, y * mSegmentSize);

   RefPtr<osg::Vec3Array> vertices =
      new osg::Vec3Array(width*height);

   RefPtr<osg::Vec3Array> normals =
      new osg::Vec3Array(width*height);

   RefPtr<osg::Vec4Array> colors =
      new osg::Vec4Array(width*height);

   RefPtr<osg::Vec2Array> textureCoordinates =
      new osg::Vec2Array(width*height);

   int i, j;

   for (i=0;i<height;i++)
   {
      for (j=0;j<width;j++)
      {
         float x = minimum[0] + j * (mSegmentSize / mSegmentDivisions),
               y = minimum[1] + i * (mSegmentSize / mSegmentDivisions);

         float heightAtXY = GetHeight(x, y, true);

         (*vertices)[i*width+j].set(
            x, y,
            heightAtXY
         );

         osg::Vec3 normal;

         GetNormal(x, y, normal, true);

         (*normals)[i*width+j].set(normal[0], normal[1], normal[2]);

         (*colors)[i*width+j] = GetColor(heightAtXY);

         (*textureCoordinates)[i*width+j].set(x*0.1, y*0.1);
      }
   }

   
   for (i=0;i<mSegmentDivisions;i++)
   {
      RefPtr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP);
      indices->reserveElements(width * 2);

      for (j=0;j<width;j++)
      {
         indices->addElement((i+1)*width + j);
         indices->addElement(i*width + j);
      }
      
      geom->addPrimitiveSet(indices);
   }

   geom->setVertexArray(vertices.get());

   geom->setNormalArray(normals.get());
   geom->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

   geom->setColorArray(colors.get());
   geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

   geom->setTexCoordArray(0, textureCoordinates.get());


   geom->setUseDisplayList(true);

   geode->addDrawable(geom);

   lod->addChild(geode, 0.0f, mBuildDistance);

   GetMatrixNode()->addChild(lod);
}



bool InfiniteTerrain::IsClearLineOfSight( const osg::Vec3& pointOne,
                                         const osg::Vec3& pointTwo )
{
   // A smarter version would check basic generation parameters
   // to see if two points are above any generated terrain

   // cant see undergound (or underwater)
   if (pointOne.z() < 0 || pointTwo.z() < 0)
      return false;

   osg::Vec3 ray = pointTwo - pointOne;
   double length( ray.length() );
   // If closer than post spacing, then clear LOS
   if ( length < GetLineOfSightSpacing() )
   {
      return true;
   }

   float stepsize( GetLineOfSightSpacing() / length );
   double s( 0.0 );

   while ( s < 1.0 )
   {
      osg::Vec3 testPt = pointOne + ray*s;
      double h( GetHeight( testPt.x(), testPt.y() ) );

      // Segment blocked by terrain
      if ( h >= testPt.z() )
      {
         return false;
      }
      s += stepsize;
   }

   // Walked full ray, so clear LOS
   return true;
}

void InfiniteTerrain::SetMinColor(const osg::Vec3 &rgb)
{
   mMinColor.set((rgb[0] / 255.0f), (rgb[1] / 255.0f), (rgb[2] / 255.0f));
}

void InfiniteTerrain::SetMaxColor(const osg::Vec3 &rgb)
{
   mMaxColor.set((rgb[0] / 255.0f), (rgb[1] / 255.0f), (rgb[2] / 255.0f));
}

}
