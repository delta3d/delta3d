#include <prefix/dtcoreprefix.h>

#include <dtCore/skydome.h>
#include <dtCore/moveearthtransform.h>
#include <dtCore/makeskydome.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/nodemask.h>
#include <osg/Depth>
#include <osg/Drawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Node>
#include <osg/PolygonMode>

#include <osgDB/ReadFile>

#include <cassert>

namespace dtCore
{

IMPLEMENT_MANAGEMENT_LAYER(SkyDome)

////////////////////////////////////////////////////////////////////////////////
SkyDome::SkyDome(const std::string& name, bool createCapGeometry /*= true*/, float radius /*= 6000.0f*/)
:EnvEffect(name),
mEnableCap(createCapGeometry)
{
   RegisterInstance(this);

   SetOSGNode( new osg::Group() );
   mBaseColor.set(0.5f, 0.5f, 0.2f);
   Config(radius);
}

////////////////////////////////////////////////////////////////////////////////
SkyDome::~SkyDome()
{
   DeregisterInstance(this);
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::SkyDome::Config(float radius)
{
   osg::Group* group = new osg::Group();

    // use a transform to make the sky and base around with the eye point.
   mXform = new MoveEarthySkyWithEyePointTransformAzimuth();
   mXform->SetAzimuth( 0.0f ); //zero out the rotation of the dome

    // Transform's value isn't known until the cull traversal, so its bounding
    // volume can't be determined. Therefore culling will be invalid,
    // so switch it off. This will cause all of our parents to switch culling
    // off as well. But don't worry, culling will be back on once underneath
    // this node or any other branch above this transform.
   mXform->setCullingActive(false);
   mGeode = MakeSkyDome(*this, radius).Compute();
   mXform->addChild(mGeode.get());
   group->addChild(mXform.get());
   group->setNodeMask(dtUtil::NodeMask::BACKGROUND);

   GetOSGNode()->asGroup()->addChild(group);
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::SkyDome::SetBaseColor(const osg::Vec3& color)
{
   osg::Geometry* geom = mGeode->getDrawable(0)->asGeometry();
   osg::Array* array = geom->getColorArray();
   if (array && array->getType() == osg::Array::Vec4ArrayType)
   {
      mBaseColor.set(color);

      osg::Vec4Array* color = static_cast<osg::Vec4Array*>(array);
      unsigned int limit = mEnableCap ? 38: 19;

      for (unsigned int i=0; i<limit; i++)
      {
         assert(i<color->size());
         (*color)[i].set(mBaseColor[0], mBaseColor[1], mBaseColor[2], 1.0f);
      }
      geom->dirtyDisplayList();
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::SkyDome::Repaint(const osg::Vec3& skyColor, 
                              const osg::Vec3& fogColor,
                              double sunAngle, 
                              double sunAzimuth,
                              double visibility)
{

   //rotate the dome to line up with the sun's azimuth.
   mXform->SetAzimuth(sunAzimuth);

   outer_param.set(0.0, 0.0, 0.0);
   middle_param.set(0.0, 0.0, 0.0);

   outer_diff.set(0.0, 0.0, 0.0);
   middle_diff.set(0.0, 0.0, 0.0);

   // Check for sunrise/sunset condition
   if(IsSunsetOrSunrise(sunAngle))
   {
      // 0.0 - 0.4
      outer_param.set((10.0 - std::abs(sunAngle)) / 20.0,
                      (10.0 - std::abs(sunAngle)) / 40.0,
                      -(10.0 - std::abs(sunAngle)) / 30.0);

      middle_param.set((10.0 - std::abs(sunAngle)) / 40.0,
                       (10.0 - std::abs(sunAngle)) / 80.0,
                       0.0);

      outer_diff = outer_param / 9.0;

      middle_diff = middle_param / 9.0;
   } 

   outer_amt.set(outer_param);
   middle_amt.set(middle_param);

   // First, recaclulate the basic colors

   CalcNewColors(visibility, skyColor, fogColor);

   AssignColors();
}

////////////////////////////////////////////////////////////////////////////////
bool SkyDome::IsSunsetOrSunrise(double sunAngle) const
{
   return sunAngle > -10.0 && sunAngle < 10.0;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 SkyDome::CalcCenterColors(double vis_factor,
                                    const osg::Vec3& skyColor, 
                                    const osg::Vec3& fogColor) const
{
   osg::Vec3 center_color;

   for (unsigned int j = 0; j < 3; j++) 
   {
      const osg::Vec3::value_type diff = skyColor[j] - fogColor[j];
      center_color[j] = skyColor[j] - diff * (1.0 - vis_factor);
   }

   return center_color;
}

////////////////////////////////////////////////////////////////////////////////
void SkyDome::CalcNewColors(double visibility, const osg::Vec3& skyColor, 
                            const osg::Vec3& fogColor)
{
   center_color = CalcCenterColors(GetVisibilityFactor(visibility), skyColor, fogColor);

   for (unsigned int i = 0; i < 9; i++) 
   {
      SetUpperMiddleLowerColors(skyColor, fogColor, i, visibility);

      outer_amt -= outer_diff;
      middle_amt -= middle_diff;
   }

   outer_amt.set(0.0, 0.0, 0.0);
   middle_amt.set(0.0, 0.0, 0.0);

   for (unsigned int i = 9; i < 19; i++) 
   {
      SetUpperMiddleLowerColors(skyColor, fogColor, i, visibility);

      outer_amt += outer_diff;
      middle_amt += middle_diff;
   }

   for (unsigned int i = 0; i < 19; i++) 
   {
      bottom_color[i] = fogColor;
   }
}

////////////////////////////////////////////////////////////////////////////////
void SkyDome::AssignColors() const
{
   osg::Geometry* geom = mGeode->getDrawable(0)->asGeometry();
   osg::Array* array = geom->getColorArray();

   if (array && array->getType()==osg::Array::Vec4ArrayType)
   {
      osg::Vec4Array* color = static_cast<osg::Vec4Array*>(array);

      // Set cap color
      if(mEnableCap)
      {
         for (unsigned int i = 0; i < 19; i++)
         {
            assert(i<color->size());
            (*color)[i].set(bottom_color[i][0], bottom_color[i][1], bottom_color[i][2], 1.0f);
         }
      }

      // Set dome colors
      unsigned int c = mEnableCap?19:0;
      for(unsigned int i = 0; i < 19; i++)
      {
         assert(c+19+19+19+19 < color->size());
         (*color)[c].set(bottom_color[i][0], bottom_color[i][1], bottom_color[i][2], 1.0f);
         (*color)[c+19].set(lower_color[i][0], lower_color[i][1], lower_color[i][2], 1.0f);
         (*color)[c+19+19].set(middle_color[i][0], middle_color[i][1], middle_color[i][2], 1.0f);
         (*color)[c+19+19+19].set(upper_color[i][0], upper_color[i][1], upper_color[i][2], 1.0f);
         (*color)[c+19+19+19+19].set(center_color[0], center_color[1], center_color[2], 1.0f);
         c++;
      }
   }

   geom->dirtyDisplayList();
}

////////////////////////////////////////////////////////////////////////////////
double SkyDome::GetVisibilityFactor(double visibility) const
{
   if (visibility < 3000.0) 
   {
      double vis_factor = (visibility - 1000.0) / 2000.0;

      dtUtil::Clamp(vis_factor, 0.0, 1.0);
      return vis_factor;
   }

   return 1.0;
}

////////////////////////////////////////////////////////////////////////////////
void SkyDome::SetUpperMiddleLowerColors(const osg::Vec3& skyColor, const osg::Vec3& fogColor,
                                        unsigned int i, double visibility)
{
   const double cvf = CalcCVF(visibility);

   for (unsigned int j = 0; j < 3; j++) 
   {
      const osg::Vec3::value_type diff = skyColor[j] - fogColor[j];

      upper_color[i][j] = skyColor[j] - diff *
                          (1.0 - GetVisibilityFactor(visibility) * (0.7 + 0.3 * cvf/20000.0f));

      middle_color[i][j] = skyColor[j] - diff *
                          (1.0 - GetVisibilityFactor(visibility) * (0.1 + 0.85 * cvf/20000.0f))
                           + middle_amt[j];

      lower_color[i][j] = fogColor[j] + outer_amt[j];

      dtUtil::Clamp(upper_color[i][j], 0.0f, 1.0f);
      dtUtil::Clamp(middle_color[i][j], 0.0f, 1.0f);
      dtUtil::Clamp(lower_color[i][j], 0.0f, 1.0f);
   }
}

////////////////////////////////////////////////////////////////////////////////
double SkyDome::CalcCVF(double visibility) const
{
   dtUtil::Clamp(visibility, 0.0, 20000.0);
   return visibility;
}

////////////////////////////////////////////////////////////////////////////////

}


