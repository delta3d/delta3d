
#ifndef DELTA_MAKESKYDOME
#define DELTA_MAKESKYDOME

#include <dtCore/export.h>
#include <osg/Array>
#include <osg/Geode>

namespace osg
{
   class Geometry;
}

namespace dtCore
{

   class SkyDome;

   /** Internal class used by dtCore::SkyDome to create the actual
     * geometry.
     */
   class DT_CORE_EXPORT MakeSkyDome
   {
   public:
      MakeSkyDome( const SkyDome &skyDome);
      ~MakeSkyDome();
      osg::Geode* Compute();

   protected:

   private:
      unsigned int GetNumLevels();
      void SetCoordinatesAndColors();
      void SetCapCoordinatesAndColors();
      void CreateTriangleStrips();
      osg::StateSet* CreateStateSet() const;
      const SkyDome *mSkyDome;

      float mRadius;
      osg::Geometry *mGeom;
      osg::Vec3Array *mCoordArray;
      osg::Vec4Array *mColorArray;
      std::vector<float> mLevelHeight;
      std::vector<osg::Vec3> mCCArray;
      static const unsigned int VERTS_IN_CIRCUM;
   };
}
#endif // DELTA_MAKESKYDOME
