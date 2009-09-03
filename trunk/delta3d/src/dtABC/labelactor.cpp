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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <osg/Geometry>
#include <dtDAL/enginepropertytypes.h>
#include <dtABC/labelactor.h>
#include <osg/MatrixTransform>
#include <osg/Geode>



namespace dtABC
{
   /////////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   /////////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString LabelActor::DEFAULT_FONT("Arial.ttf");
   const float LabelActor::DEFAULT_FONT_SIZE = 1.0f;
   const osg::Vec4 LabelActor::DEFAULT_COLOR_TEXT(1.0f, 1.0f, 1.0f, 1.0f);
   const osg::Vec4 LabelActor::DEFAULT_COLOR_BACK(0.0f, 0.0f, 0.0f, 1.0f);
   const osg::Vec2 LabelActor::DEFAULT_BACK_SIZE(1.0f, 1.0f);
   const dtUtil::RefString LabelActor::PROPERTY_TEXT("Text");
   const dtUtil::RefString LabelActor::PROPERTY_FONT("Font");
   const dtUtil::RefString LabelActor::PROPERTY_FONT_SIZE("Font Size");
   const dtUtil::RefString LabelActor::PROPERTY_TEXT_COLOR("Text Color");
   const dtUtil::RefString LabelActor::PROPERTY_BACK_COLOR("Back Color");
   const dtUtil::RefString LabelActor::PROPERTY_BACK_SIZE("Back Size");
   const dtUtil::RefString LabelActor::PROPERTY_BACK_VISIBLE("Back Visible");
   const dtUtil::RefString LabelActor::PROPERTY_LIGHTING_ENABLED("Lighting Enabled");
   const dtUtil::RefString LabelActor::PROPERTY_DEPTH_TESTING_ENABLED("Depth Testing Enabled");
   const dtUtil::RefString LabelActor::PROPERTY_TEXT_ALIGNMENT("Text Alignment");

   //ENUM CODE
   IMPLEMENT_ENUM(dtABC::LabelActor::AlignmentEnum)
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::LEFT_TOP("LEFT_TOP");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::LEFT_CENTER("LEFT_CENTER");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::LEFT_BOTTOM("LEFT_BOTTOM");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::CENTER_TOP("CENTER_TOP");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::CENTER_CENTER("CENTER_CENTER");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::CENTER_BOTTOM("CENTER_BOTTOM");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::RIGHT_TOP("RIGHT_TOP");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::RIGHT_CENTER("RIGHT_CENTER");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::RIGHT_BOTTOM("RIGHT_BOTTOM");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::LEFT_BASE_LINE("LEFT_BASE_LINE");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::CENTER_BASE_LINE("CENTER_BASE_LINE");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::RIGHT_BASE_LINE("RIGHT_BASE_LINE");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::LEFT_BOTTOM_BASE_LINE("LEFT_BOTTOM_BASE_LINE");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::CENTER_BOTTOM_BASE_LINE("CENTER_BOTTOM_BASE_LINE");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::RIGHT_BOTTOM_BASE_LINE("RIGHT_BOTTOM_BASE_LINE");
   dtABC::LabelActor::AlignmentEnum dtABC::LabelActor::AlignmentEnum::BASE_LINE("BASE_LINE");

   //////////////////////////////////////////////////////////////////////////
   osgText::TextBase::AlignmentType LabelActor::AlignmentEnum::ToOSGType() const
   {
      osgText::TextBase::AlignmentType type;
      if (*this == AlignmentEnum::LEFT_TOP) {type = osgText::TextBase::LEFT_TOP;}
      if (*this == AlignmentEnum::LEFT_CENTER) {type = osgText::TextBase::LEFT_CENTER;}
      if (*this == AlignmentEnum::LEFT_BOTTOM) {type = osgText::TextBase::LEFT_CENTER;}
      if (*this == AlignmentEnum::CENTER_TOP) {type = osgText::TextBase::CENTER_TOP;}
      if (*this == AlignmentEnum::CENTER_CENTER) {type = osgText::TextBase::CENTER_CENTER;}
      if (*this == AlignmentEnum::CENTER_BOTTOM) {type = osgText::TextBase::CENTER_CENTER;}
      if (*this == AlignmentEnum::RIGHT_TOP) {type = osgText::TextBase::RIGHT_TOP;}
      if (*this == AlignmentEnum::RIGHT_CENTER) {type = osgText::TextBase::RIGHT_CENTER;}
      if (*this == AlignmentEnum::RIGHT_BOTTOM) {type = osgText::TextBase::RIGHT_CENTER;}
      if (*this == AlignmentEnum::LEFT_BASE_LINE) {type = osgText::TextBase::LEFT_BASE_LINE;}
      if (*this == AlignmentEnum::CENTER_BASE_LINE) {type = osgText::TextBase::CENTER_BASE_LINE;}
      if (*this == AlignmentEnum::RIGHT_BASE_LINE) {type = osgText::TextBase::RIGHT_BASE_LINE;}
      if (*this == AlignmentEnum::LEFT_BOTTOM_BASE_LINE) {type = osgText::TextBase::LEFT_BOTTOM_BASE_LINE;}
      if (*this == AlignmentEnum::CENTER_BOTTOM_BASE_LINE) {type = osgText::TextBase::CENTER_BOTTOM_BASE_LINE;}
      if (*this == AlignmentEnum::RIGHT_BOTTOM_BASE_LINE) {type = osgText::TextBase::RIGHT_BOTTOM_BASE_LINE;}
      if (*this == AlignmentEnum::BASE_LINE) {type = osgText::TextBase::BASE_LINE;}

      return type;
   }

   //////////////////////////////////////////////////////////////////////////
   LabelActor::AlignmentEnum& LabelActor::AlignmentEnum::FromOSGType(osgText::TextBase::AlignmentType type)
   {
      switch(type)
      {
      case osgText::TextBase::LEFT_TOP: {return AlignmentEnum::LEFT_TOP;} 	break;
      case osgText::TextBase::LEFT_CENTER: {return AlignmentEnum::LEFT_CENTER;} 	break;
      case osgText::TextBase::LEFT_BOTTOM: {return AlignmentEnum::LEFT_BOTTOM;} 	break;
      case osgText::TextBase::CENTER_TOP: {return AlignmentEnum::CENTER_TOP;} 	break;
      case osgText::TextBase::CENTER_CENTER: {return AlignmentEnum::CENTER_CENTER;} 	break;
      case osgText::TextBase::CENTER_BOTTOM: {return AlignmentEnum::CENTER_BOTTOM;} 	break;
      case osgText::TextBase::RIGHT_TOP: {return AlignmentEnum::RIGHT_TOP;} 	break;
      case osgText::TextBase::RIGHT_CENTER: {return AlignmentEnum::RIGHT_CENTER;} 	break;
      case osgText::TextBase::RIGHT_BOTTOM: {return AlignmentEnum::RIGHT_BOTTOM;} 	break;
      case osgText::TextBase::LEFT_BASE_LINE: {return AlignmentEnum::LEFT_BASE_LINE;} 	break;
      case osgText::TextBase::CENTER_BASE_LINE: {return AlignmentEnum::CENTER_BASE_LINE;} 	break;
      case osgText::TextBase::RIGHT_BASE_LINE: {return AlignmentEnum::RIGHT_BASE_LINE;} 	break;
      case osgText::TextBase::LEFT_BOTTOM_BASE_LINE: {return AlignmentEnum::LEFT_BOTTOM_BASE_LINE;} 	break;
      case osgText::TextBase::CENTER_BOTTOM_BASE_LINE: {return AlignmentEnum::CENTER_BOTTOM_BASE_LINE;} 	break;
      case osgText::TextBase::RIGHT_BOTTOM_BASE_LINE: {return AlignmentEnum::RIGHT_BOTTOM_BASE_LINE;} 	break;
      default: return AlignmentEnum::LEFT_BASE_LINE;  break;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   LabelActor::LabelActor(const std::string& name)
      : BaseClass(name)
      , mBackdropDims(DEFAULT_BACK_SIZE)
      , mTextNode(new osgText::Text)
      , mBackdrop(new osg::Geometry)
      , mBackdropVerts(NULL)
      , mFontFile()
      , mAutoBackSize(true)
   {
      osg::Group* root = static_cast<osg::Group*>(GetOSGNode());

      // Move text out by using a matrix node.
      dtCore::RefPtr<osg::MatrixTransform> matrixNode = new osg::MatrixTransform;
      osg::Matrix mtx;
      osg::Vec3 offset(0.0f,0.0f,0.01f);
      mtx.setTrans(offset);
      matrixNode->setMatrix(mtx);

      // Create the node hierarchy for the text node.
      dtCore::RefPtr<osg::Geode> geode = new osg::Geode;
      geode->addDrawable(mTextNode.get());
      matrixNode->addChild(geode.get());
      root->addChild(matrixNode.get());

      // Create the node hierarchy for the back drop node.
      geode = new osg::Geode;
      geode->addDrawable(mBackdrop.get());
      root->addChild(geode.get());

      // Format the text.
      mTextNode->setDrawMode(osgText::Text::TEXT);
      mTextNode->setAlignment(osgText::TextBase::CENTER_CENTER);
      SetFont(DEFAULT_FONT.Get());
      SetFontSize(DEFAULT_FONT_SIZE);
      SetTextColor(DEFAULT_COLOR_TEXT);

      // VERTICES
      mBackdropVerts = new osg::Vec3Array(4);
      // NOTE: A later call to Update will set the vertice values.

      // COLOR
      dtCore::RefPtr<osg::Vec4Array> color = new osg::Vec4Array;
      color->push_back(DEFAULT_COLOR_BACK);

      // UVS
      dtCore::RefPtr<osg::Vec2Array> uvs = new osg::Vec2Array(4);
      (*uvs)[0].set(0.0f, 0.0f); // LEFT-BOTTOM
      (*uvs)[1].set(1.0f, 0.0f); // RIGHT-BOTTOM
      (*uvs)[2].set(1.0f, 1.0f); // RIGHT-TOP
      (*uvs)[3].set(0.0f, 1.0f); // LEFT-TOP

      // NORMALS
      dtCore::RefPtr<osg::Vec3Array> norms = new osg::Vec3Array(1);
      (*norms)[0].set(0.0f, 1.0f, 0.0f);

      // STATES
      osg::StateSet* states = root->getOrCreateStateSet();
      states->setMode(GL_BLEND,osg::StateAttribute::ON);
      states->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

      // Setup the geometry
      mBackdrop->setNormalArray(norms.get());
      mBackdrop->setNormalBinding(osg::Geometry::BIND_OVERALL);
      mBackdrop->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));
      mBackdrop->setTexCoordArray(0, uvs.get());
      mBackdrop->setVertexArray(mBackdropVerts.get());
      mBackdrop->setColorArray(color.get());
      mBackdrop->setColorBinding(osg::Geometry::BIND_OVERALL);

      // Update the back drop size and anything else.
      Update();
   }

   /////////////////////////////////////////////////////////////////////////////
   LabelActor::~LabelActor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetText(const std::string& text)
   {
      mTextNode->setText(text);
      Update();
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string LabelActor::GetText() const
   {
      return mTextNode->getText().createUTF8EncodedString();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetFont(const std::string& fontFile)
   {
      mFontFile = fontFile;
      mTextNode->setFont(fontFile);
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string LabelActor::GetFont() const
   {
      return mFontFile;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetFontSize(float fontSize)
   {
      mTextNode->setCharacterSize(fontSize <= 0.0f ? 1.0f : fontSize);
   }

   /////////////////////////////////////////////////////////////////////////////
   float LabelActor::GetFontSize() const
   {
      return mTextNode->getCharacterHeight();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetMaximumWidth(float maximumWidth)
   {
      mTextNode->setMaximumWidth(maximumWidth);
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LabelActor::GetMaximumWidth() const
   {
      return mTextNode->getMaximumWidth();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetMaximumHeight(float maximumHeight)
   {
      mTextNode->setMaximumHeight(maximumHeight);
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LabelActor::GetMaximumHeight() const
   {
      return mTextNode->getMaximumHeight();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetLineSpacing(float lineSpacing)
   {
      mTextNode->setLineSpacing(lineSpacing);
   }

   ////////////////////////////////////////////////////////////////////////////////
   float LabelActor::GetLineSpacing() const
   {
      return mTextNode->getLineSpacing();
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned int LabelActor::GetLineCount() const
   {
      return mTextNode->getLineCount();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetTextColor(const osg::Vec4& color)
   {
      mTextNode->setColor(color);
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec4& LabelActor::GetTextColor() const
   {
      return mTextNode->getColor();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetBackColor(const osg::Vec4& color)
   {
      dtCore::RefPtr<osg::Vec4Array> colorArray = new osg::Vec4Array;
      colorArray->push_back(color);
      mBackdrop->setColorArray(colorArray.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec4 LabelActor::GetBackColor() const
   {
      const osg::Vec4Array* colorArray
         = static_cast<const osg::Vec4Array*>(mBackdrop->getColorArray());
      return (*colorArray)[0];
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetBackWidth(float width)
   {
      mAutoBackSize = false;
      mBackdropDims.x() = width;
      Update();
   }

   /////////////////////////////////////////////////////////////////////////////
   float LabelActor::GetBackWidth() const
   {
      return mBackdropDims.x();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetBackHeight(float height)
   {
      mAutoBackSize = false;
      mBackdropDims.y() = height;
      Update();
   }

   /////////////////////////////////////////////////////////////////////////////
   float LabelActor::GetBackHeight() const
   {
      return mBackdropDims.y();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetBackSize(const osg::Vec2& backSize)
   {
      mAutoBackSize = false;
      mBackdropDims = backSize;
      Update();
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Vec2& LabelActor::GetBackSize() const
   {
      return mBackdropDims;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetBackBorderSize(osg::Vec2 size)
   {
      mBackBorderSize = size;
      Update();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetAutoBackSizeEnabled(bool enabled)
   {
      mAutoBackSize = enabled;
      Update();
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::SetBackVisible(bool visible)
   {
      mBackdrop->getParent(0)->setNodeMask(visible?0xFFFFFFFF:0);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool LabelActor::IsBackVisible() const
   {
      return mBackdrop->getParent(0)->getNodeMask() != 0;
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::Update()
   {
      if (mAutoBackSize)
      {
         osg::BoundingBox bounds = mTextNode->getBound();

         (*mBackdropVerts)[0].set(bounds.xMin() - mBackBorderSize.x(), bounds.yMin() - mBackBorderSize.y(), 0.0f); // LEFT-BOTTOM
         (*mBackdropVerts)[1].set(bounds.xMax() + mBackBorderSize.x(), bounds.yMin() - mBackBorderSize.y(), 0.0f); // RIGHT-BOTTOM
         (*mBackdropVerts)[2].set(bounds.xMax() + mBackBorderSize.x(), bounds.yMax() + mBackBorderSize.y(), 0.0f); // RIGHT-TOP
         (*mBackdropVerts)[3].set(bounds.xMin() - mBackBorderSize.x(), bounds.yMax() + mBackBorderSize.y(), 0.0f); // LEFT-TOP
      }
      // Old implementation to preserve backward compatibility...
      else
      {
         float halfWidth = (mBackdropDims.x() * 0.5f) + (mBackBorderSize.x() * 2.0f);
         float halfHeight = (mBackdropDims.y() * 0.5f) + (mBackBorderSize.y() * 2.0f);
         (*mBackdropVerts)[0].set(-halfWidth, -halfHeight, 0.0f); // LEFT-BOTTOM
         (*mBackdropVerts)[1].set( halfWidth, -halfHeight, 0.0f); // RIGHT-BOTTOM
         (*mBackdropVerts)[2].set( halfWidth,  halfHeight, 0.0f); // RIGHT-TOP
         (*mBackdropVerts)[3].set(-halfWidth,  halfHeight, 0.0f); // LEFT-TOP
      }

      // Reset the vertex positions.
      mBackdrop->setVertexArray(mBackdropVerts.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   void LabelActor::CreateActorProperties(ActorPropertyArray& outProperties)
   {
      using namespace dtDAL;

      const std::string group("Label Actor");

      // STRING PROPERTIES
      outProperties.push_back(new StringActorProperty(
         LabelActor::PROPERTY_TEXT.Get(),
         LabelActor::PROPERTY_TEXT.Get(),
         StringActorProperty::SetFuncType(this, &LabelActor::SetText),
         StringActorProperty::GetFuncType(this, &LabelActor::GetText),
         "Label text.",
         group));

      outProperties.push_back(new StringActorProperty(
         LabelActor::PROPERTY_FONT.Get(),
         LabelActor::PROPERTY_FONT.Get(),
         StringActorProperty::SetFuncType(this, &LabelActor::SetFont),
         StringActorProperty::GetFuncType(this, &LabelActor::GetFont),
         "Font for label text.",
         group));

      // FLOAT PROPERTIES
      outProperties.push_back(new FloatActorProperty(
         LabelActor::PROPERTY_FONT_SIZE.Get(),
         LabelActor::PROPERTY_FONT_SIZE.Get(),
         FloatActorProperty::SetFuncType(this, &LabelActor::SetFontSize),
         FloatActorProperty::GetFuncType(this, &LabelActor::GetFontSize),
         "Height of the text characters.",
         group));

      // VEC4 PROPERTIES
      outProperties.push_back(new Vec4ActorProperty(
         LabelActor::PROPERTY_TEXT_COLOR.Get(),
         LabelActor::PROPERTY_TEXT_COLOR.Get(),
         Vec4ActorProperty::SetFuncType(this, &LabelActor::SetTextColor),
         Vec4ActorProperty::GetFuncType(this, &LabelActor::GetTextColor),
         "Text color.",
         group));

      outProperties.push_back(new Vec4ActorProperty(
         LabelActor::PROPERTY_BACK_COLOR.Get(),
         LabelActor::PROPERTY_BACK_COLOR.Get(),
         Vec4ActorProperty::SetFuncType(this, &LabelActor::SetBackColor),
         Vec4ActorProperty::GetFuncType(this, &LabelActor::GetBackColor),
         "Fill color behind the text.",
         group));

      // VEC2 PROPERTIES
      outProperties.push_back(new Vec2ActorProperty(
         LabelActor::PROPERTY_BACK_SIZE.Get(),
         LabelActor::PROPERTY_BACK_SIZE.Get(),
         Vec2ActorProperty::SetFuncType(this, &LabelActor::SetBackSize),
         Vec2ActorProperty::GetFuncType(this, &LabelActor::GetBackSize),
         "Dimensions of the text background.",
         group));

      // BOOLEAN PROPERTIES
      outProperties.push_back(new BooleanActorProperty(
         LabelActor::PROPERTY_BACK_VISIBLE.Get(),
         LabelActor::PROPERTY_BACK_VISIBLE.Get(),
         BooleanActorProperty::SetFuncType(this, &LabelActor::SetBackVisible),
         BooleanActorProperty::GetFuncType(this, &LabelActor::IsBackVisible),
         "Determines if the text background should be visible.",
         group));

      outProperties.push_back(new BooleanActorProperty(
         LabelActor::PROPERTY_LIGHTING_ENABLED.Get(),
         LabelActor::PROPERTY_LIGHTING_ENABLED.Get(),
         BooleanActorProperty::SetFuncType(this, &LabelActor::SetEnableLighting),
         BooleanActorProperty::GetFuncType(this, &LabelActor::GetEnableLighting),
         "Enable or disable the ability to be affected by OpenGL lighting.",
         group));

      outProperties.push_back(new BooleanActorProperty(
         LabelActor::PROPERTY_DEPTH_TESTING_ENABLED.Get(),
         LabelActor::PROPERTY_DEPTH_TESTING_ENABLED.Get(),
         BooleanActorProperty::SetFuncType(this, &LabelActor::SetEnableDepthTesting),
         BooleanActorProperty::GetFuncType(this, &LabelActor::GetEnableDepthTesting),
         "Enable or disable the OpenGL depth testing.",
         group));

      // ENUM PROPERTIES
      outProperties.push_back(new EnumActorProperty<AlignmentEnum>(
         LabelActor::PROPERTY_TEXT_ALIGNMENT.Get(),
         LabelActor::PROPERTY_TEXT_ALIGNMENT.Get(),
         EnumActorProperty<AlignmentEnum>::SetFuncType(this,&LabelActor::SetTextAlignment),
         EnumActorProperty<AlignmentEnum>::GetFuncType(this,&LabelActor::GetTextAlignment),
         "Sets the text alignment type.",
         group));

   }

   //////////////////////////////////////////////////////////////////////////
   void LabelActor::SetTextAlignment(AlignmentEnum& alignment)
   {
      mTextNode->setAlignment(alignment.ToOSGType());
   }

   //////////////////////////////////////////////////////////////////////////
   LabelActor::AlignmentEnum& LabelActor::GetTextAlignment() const
   {
      return LabelActor::AlignmentEnum::FromOSGType(mTextNode->getAlignment());
   }

   //////////////////////////////////////////////////////////////////////////
   void LabelActor::SetEnableLighting(bool enable)
   {
      osg::StateSet* states = GetOSGNode()->getOrCreateStateSet();
      states->setMode(GL_LIGHTING, enable ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   }

   //////////////////////////////////////////////////////////////////////////
   bool LabelActor::GetEnableLighting()
   {
      osg::StateSet* states = GetOSGNode()->getOrCreateStateSet();
      osg::StateAttribute::GLModeValue mode = states->getMode(GL_LIGHTING);

      return (mode ? true : false);
   }

   //////////////////////////////////////////////////////////////////////////
   void LabelActor::SetEnableDepthTesting(bool enable)
   {
      osg::StateSet* states = GetOSGNode()->getOrCreateStateSet();
      states->setMode(GL_DEPTH_TEST, enable ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
   }

   //////////////////////////////////////////////////////////////////////////
   bool LabelActor::GetEnableDepthTesting()
   {
      osg::StateSet* states = GetOSGNode()->getOrCreateStateSet();
      osg::StateAttribute::GLModeValue mode = states->getMode(GL_DEPTH_TEST);

      return (mode ? true : false);
   }
}
