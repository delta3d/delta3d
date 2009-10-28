/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008, Alion Science and Technology, BMH Operation
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
 * Chris Rodgers
 */

#ifndef DELTA_LABELACTOR_H
#define DELTA_LABELACTOR_H



////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtABC/export.h>
#include <dtDAL/actorproperty.h>
#include <osgText/Text>
#include <dtCore/transformable.h>



namespace dtABC
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ABC_EXPORT LabelActor : public dtCore::Transformable
   {
      public:
         typedef dtCore::Transformable BaseClass;

         static const dtUtil::RefString PROPERTY_TEXT;
         static const dtUtil::RefString PROPERTY_FONT;
         static const dtUtil::RefString PROPERTY_FONT_SIZE;
         static const dtUtil::RefString PROPERTY_TEXT_COLOR;
         static const dtUtil::RefString PROPERTY_BACK_COLOR;
         static const dtUtil::RefString PROPERTY_BACK_SIZE;
         static const dtUtil::RefString PROPERTY_BACK_VISIBLE;
         static const dtUtil::RefString PROPERTY_LIGHTING_ENABLED;
         static const dtUtil::RefString PROPERTY_DEPTH_TESTING_ENABLED;
         static const dtUtil::RefString PROPERTY_TEXT_ALIGNMENT;


         static const dtUtil::RefString DEFAULT_FONT;
         static const float DEFAULT_FONT_SIZE;
         static const osg::Vec4 DEFAULT_COLOR_TEXT;
         static const osg::Vec4 DEFAULT_COLOR_BACK;
         static const osg::Vec2 DEFAULT_BACK_SIZE;

         LabelActor(const std::string& name = "LabelActor");

         /**
          * Set the text that is to appear on the label.
          */
         void SetText(const std::string& text);
         const std::string GetText() const;

         /**
          * Set the font to be used for the text.
          * NOTE: The font file will be the name of a system font.
          *       The internal OSG code inherently searches the system's
          *       font folder for the font file.
          *       You can simply specify "Arial.ttf" for example.
          */
         void SetFont(const std::string& fontFile);
         const std::string GetFont() const;

         /**
          * Set the metric size of the font.
          * Ex: 1 is one world unit (usually a meter).
          */
         void SetFontSize(float fontSize);
         float GetFontSize() const;

         /**
          * Set the maximum width of the text box.
          * With horizontal layouts any characters which do not fit are wrapped around.
          * 0 or negative values indicate that no maximum width is set, lines can be as long as 
          * they need be to fit thre required text
          */
         void SetMaximumWidth(float maximumWidth);

         /**
          * Get the maximum width of the text box.
          */
         float GetMaximumWidth() const;

         /**
          * Set the maximum height of the text box.
          * With horizontal layouts any characters which do not fit are wrapped around.
          * 0 or negative values indicate that no maximum height is set, lines can be as long as 
          * they need be to fit the required text
          */
         void SetMaximumHeight(float maximumHeight);

         /**
          * Get the maximum height of the text box.
          */
         float GetMaximumHeight() const;

         /**
         * Set the spacing between each line.
         * This is a percent value of the size of the text.
         */
         void SetLineSpacing(float lineSpacing);
         float GetLineSpacing() const;

         /**
         * Retrieves the line count of the text.
         */
         unsigned int GetLineCount() const;

         /**
          * Set the RGBA color of the label text.
          */
         void SetTextColor(const osg::Vec4& color);
         const osg::Vec4& GetTextColor() const;

         /**
          * Set the RGBA color of the label background.
          */
         void SetBackColor(const osg::Vec4& color);
         const osg::Vec4 GetBackColor() const;

         /**
          * Set the metric width of the label background.
          */
         void SetBackWidth(float width);
         float GetBackWidth() const;

         /**
          * Set the metric height of the label background.
          */
         void SetBackHeight(float height);
         float GetBackHeight() const;

         /**
          * Set the metric width and height of the label background.
          */
         void SetBackSize(const osg::Vec2& backSize);
         const osg::Vec2& GetBackSize() const;

         /**
         * Sets the background border size.
         *
         * @param[in]  size  The width/height of the border.
         */
         void SetBackBorderSize(osg::Vec2 size);

         /**
          * This will flag the object to use an auto calculated
          * text back size.
          *
          * @param[in]  enabled  Use it or not?
          */
         void SetAutoBackSizeEnabled(bool enabled);

         /**
          * Set whether the label background should be visible or not.
          */
         void SetBackVisible(bool visible);
         bool IsBackVisible() const;
         
         class DT_ABC_EXPORT AlignmentEnum : public dtUtil::Enumeration
         {
            DECLARE_ENUM(AlignmentEnum)
         public:
            static AlignmentEnum LEFT_TOP;
            static AlignmentEnum LEFT_CENTER;
            static AlignmentEnum LEFT_BOTTOM;

            static AlignmentEnum CENTER_TOP;
            static AlignmentEnum CENTER_CENTER;
            static AlignmentEnum CENTER_BOTTOM;

            static AlignmentEnum RIGHT_TOP;
            static AlignmentEnum RIGHT_CENTER;
            static AlignmentEnum RIGHT_BOTTOM;

            static AlignmentEnum LEFT_BASE_LINE;
            static AlignmentEnum CENTER_BASE_LINE;
            static AlignmentEnum RIGHT_BASE_LINE;

            static AlignmentEnum LEFT_BOTTOM_BASE_LINE;
            static AlignmentEnum CENTER_BOTTOM_BASE_LINE;
            static AlignmentEnum RIGHT_BOTTOM_BASE_LINE;

            static AlignmentEnum BASE_LINE;

            ///Conversion from AlignmenEnum to OSG's AlignmentType
            osgText::TextBase::AlignmentType ToOSGType() const;

            ///Conversion form OSG's AlignmentType to corresponding AlignmentEnum
            static AlignmentEnum& FromOSGType(osgText::TextBase::AlignmentType type);

         private:
            AlignmentEnum(const std::string& name): dtUtil::Enumeration(name)
            {
               AddInstance(this);;
            }
         };

         /** 
          * Adjust how the text is aligned/justified.
          * @param alignment The alignment to use
          */
         void SetTextAlignment(AlignmentEnum& alignment);
         AlignmentEnum& GetTextAlignment() const;


         /** 
          * Enable the text geometry to be shaded by Lights.  Defaults to false.
          * @param enable True to enable shading by the Lights
          */
         void SetEnableLighting(bool enable);
         bool GetEnableLighting();

         /** 
          * Enable the depth testing for the LabelActor.  This will affect whether
          * or not the label can be covered by other geometry in the Scene.  Defaults
          * to true.
          * @param enable True to allow other geometry to render on top, false
          *               to always render the text on top.
          */
         void SetEnableDepthTesting(bool enable);
         bool GetEnableDepthTesting();

         /**
          * Creates the properties that any proxy may need to map to this actor.
          * This is handy if using an extended proxy that does not require
          * and extended version of this class. In other words, any proxy
          * can use this class and access and map its properties without having
          * to redeclare the properties.
          * @param outProperties Vector that will receive a new collection of properties
          *        associated with THIS actor instance. This should only be called by
          *        the proxy that is directly associated with this actor.
          */
         typedef std::vector<dtCore::RefPtr<dtDAL::ActorProperty> > ActorPropertyArray;
         void CreateActorProperties(ActorPropertyArray& outProperties);

      protected:
         virtual ~LabelActor();

         /**
          * Helper method for updating the label drawable when any property changes,
          * such as label background dimensions.
          */
         void Update();

      private:
         bool      mAutoBackSize;
         osg::Vec2 mBackBorderSize;

         osg::Vec2 mBackdropDims;
         dtCore::RefPtr<osgText::Text> mTextNode;
         dtCore::RefPtr<osg::Geometry> mBackdrop;
         dtCore::RefPtr<osg::Vec3Array> mBackdropVerts;
         std::string mFontFile;
   };
}
#endif
