
#ifndef DELTA_MAKESKYDOME
#define DELTA_MAKESKYDOME

#include <dtCore/export.h>
#include <osg/Array>
#include <osg/Geode>

///@cond IGNORE
namespace osg
{
   class Geometry;
}
///@endcond

namespace dtCore
{

   class SkyDome;

   /** Internal class used by dtCore::SkyDome to create the actual
     * geometry.
     */
   class DT_CORE_EXPORT MakeSkyDome
   {
   public:

      MakeSkyDome(const SkyDome &skyDome, float radius);
      ~MakeSkyDome();

      osg::Geode* Compute();

   protected:

   private:
      unsigned int GetNumLevels();
      void SetCoordinatesAndColors();
      void SetCapCoordinatesAndColors();
      void CreateTriangleStrips();
      osg::StateSet* CreateStateSet() const;

      float                  mRadius;
      osg::Geometry*         mGeom;
      osg::Vec3Array*        mCoordArray;
      osg::Vec4Array*        mColorArray;
      std::vector<float>     mLevelHeight;
      std::vector<osg::Vec3> mCCArray;

      const SkyDome* mSkyDome;

      static const unsigned int VERTS_IN_CIRCUM;
   };
}
#endif // DELTA_MAKESKYDOME
