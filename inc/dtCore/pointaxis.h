/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004-2005 MOVES Institute 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
*/

#ifndef DELTA_POINTAXIS
#define DELTA_POINTAXIS

#include <dtCore/refptr.h>

#include <osg/Vec3>
#include <osg/Vec4>

#include <osg/MatrixTransform>
#include <dtUtil/macros.h>
#include <dtCore/transformable.h>

#include <string>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Switch;
}
/// @endcond

namespace dtCore
{

   ///creates a visible xyz axis which can be placed anywhere in the scene

   /** The PointAxis class allows you to quickly add a visible axis-rose to the scene.
   * To use, just create an instance of it, and add the PointAxis to the Scene for
   * it to be rendered.
   * 
   * The default setting is to not display the axis labels.  To turn them on call
   * the Enable function with the appropriate FLAG enum (LABEL_X,LABEL_Y,LABEL_Z).
   * They each must be enabled individually (FLAG::LABELS will toggle the label rendering
   * only if the label has been previously enabled).
   *
   * Custom labels can be set or HPR can be enabled.  Colors of the axes and the length
   * of the line can also be set through the interface functions.
   */
   class DT_CORE_EXPORT PointAxis : public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(PointAxis)

   public:
      enum  AXIS              ///< ids used to specify which axis to manipulate
      {
         X              = 0L,
         Y,
         Z,

         NUMAXES
      };

      enum  AXISCOLOR         ///< ids for preset color values
      {
         BLACK          = 0L,
         WHITE,
         RED,
         YELLOW,
         GREEN,
         CYAN,
         BLUE,
         MAGENTA,
         CUSTOM_X,
         CUSTOM_Y,
         CUSTOM_Z,

         NUMAXISCOLORS
      };

      enum  FLAG              ///< bit-masks for toggling features on/of
      {
         AXES           = BIT(0L),
         LABELS         = BIT(1L),
         LABEL_X        = BIT(2L),
         LABEL_Y        = BIT(3L),
         LABEL_Z        = BIT(4L),
         XYZ_LABELS     = BIT(5L),
         HPR_LABELS     = BIT(6L),
         CUSTOM_LABELS  = BIT(7L)
      };

   private:
      enum  BASE_COLOR        ///< internal use enum
      {
         BASECOLORS     = MAGENTA   + 1L
      };

      static   const float       DEF_AXIS_SIZE;          ///< default size of axis lines
      static   const float       DEF_CHAR_SIZE;          ///< multiplier for character size
      static   const AXISCOLOR   DEF_COLOR[NUMAXES];     ///< default axis colors XYZ=RGB
      static   const char*       DEF_LABEL_XYZ[NUMAXES]; ///< default labels "X" "Y" "Z"
      static   const char*       DEF_LABEL_HPR[NUMAXES]; ///< alternate labels "H" "P" "R"
      static   const osg::Vec4   COLOR[BASECOLORS];      ///< preset color values

   public:
      /// default constructor - no parameters
      PointAxis();
   protected:

      /// destructor
      virtual              ~PointAxis();

   public:

      /// enable rendering axes, labels and types of labels
      virtual  void        Enable(int f);
      /// disable rendering axes, labels and types of labels
      virtual  void        Disable(int f);

      /// get the current label for an axis
      virtual  const char* GetLabel(AXIS a)                     const;
      /// set a custom label for an axis
      virtual  void        SetLabel(AXIS a, const char* l);

      /// get the length of an axis
      virtual  float       GetLength(AXIS a)                    const;
      /// set the length of an axis
      virtual  void        SetLength(AXIS a, float l);

      /// get the color-id of an axis
      virtual  AXISCOLOR   GetColor(AXIS a)                     const;
      /// set the color of an axis by a predefined color-id
      virtual  void        SetColor(AXIS a, AXISCOLOR c);

      /// get the color value of an axis (0.0 - 1.0)
      virtual  void        GetColor(AXIS a, osg::Vec4& c)       const;
      /// set a custom color value of an axis
      virtual  void        SetColor(AXIS a, const osg::Vec4 c);

      /// get the color-id of a label
      virtual  AXISCOLOR   GetLabelColor(AXIS a)                const;
      /// set the color of a label by a predefined color-id
      virtual  void        SetLabelColor(AXIS a, AXISCOLOR c);

      /// get the color value of a label (0.0 - 1.0)
      virtual  void        GetLabelColor(AXIS a, osg::Vec4& c)  const;
      /// set a custom color value of an label
      virtual  void        SetLabelColor(AXIS a, const osg::Vec4 c);

      /// set the size that characters will be scaled by when created
      virtual void         SetCharacterScale(float size);

      /// get the size that characters will be scaled by when created
      virtual float        GetCharacterScale() const;

   private:
      inline   void        ctor();
      inline   void        AxesSetup();
      inline   void        LabelSetup(osg::Geode* g, const char* l, osg::Vec3 p, osg::Vec4 c, float s = 1.0f);

   private:
      std::string                   mLabel[NUMAXES];     ///< lable for each axis
      std::string                   mCLabel[NUMAXES];    ///< user labels for each axis
      AXISCOLOR                     mColorID[NUMAXES];   ///< color id for each axis
      AXISCOLOR                     mLColorID[NUMAXES];  ///< color id for each axis label
      osg::Vec4                     mColor[NUMAXES];     ///< color for each axis
      osg::Vec4                     mLColor[NUMAXES];    ///< color for each label
      osg::Vec3                     mPoint[NUMAXES+1L];  ///< data points to construct geometry
      RefPtr<osg::Switch>           mMainSwitch;         ///< toggle for axis and all lables
      RefPtr<osg::Switch>           mLabelSwitch;        ///< toggle for individual labels
      float                         mCharacterScale;     ///< scale the size of label characters
   };
}

#endif // DELTA_POINTAXIS
