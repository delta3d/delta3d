/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
 */

#include <dtDirectorQt/nodeitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/linkitem.h>

#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>

#include <dtDirector/valuenode.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QMenu>

#include <dtDAL/datatype.h>

#include <osg/Vec2>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   InputData::InputData()
      : linkName(NULL)
      , linkGraphic(NULL)
      , link(NULL)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   InputData::~InputData()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void InputData::Remove()
   {
      if (linkName) delete linkName;
      if (linkGraphic) delete linkGraphic;
      linkName = NULL;
      linkGraphic = NULL;
      link = NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   OutputData::OutputData()
      : linkName(NULL)
      , linkGraphic(NULL)
      , link(NULL)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   OutputData::~OutputData()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputData::ResizeLinks(int count, EditorScene* scene)
   {
      while ((int)linkConnectors.size() < count)
      {
         QGraphicsPathItem* item = new QGraphicsPathItem(scene->GetTranslationItem(), scene);
         item->setZValue(20.0f);
         linkConnectors.push_back(item);
      }

      while ((int)linkConnectors.size() > count)
      {
         QGraphicsPathItem* item = linkConnectors[0];
         if (item) delete item;
         linkConnectors.erase(linkConnectors.begin());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void OutputData::Remove(EditorScene* scene)
   {
      ResizeLinks(0, scene);
      if (linkName) delete linkName;
      if (linkGraphic) delete linkGraphic;
      linkName = NULL;
      linkGraphic = NULL;
      link = NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   ValueData::ValueData()
      : linkName(NULL)
      , linkGraphic(NULL)
      , link(NULL)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   ValueData::~ValueData()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueData::ResizeLinks(int count, EditorScene* scene)
   {
      while ((int)linkConnectors.size() < count)
      {
         QGraphicsPathItem* item = new QGraphicsPathItem(scene->GetTranslationItem(), scene);
         item->setZValue(20.0f);
         linkConnectors.push_back(item);
      }

      while ((int)linkConnectors.size() > count)
      {
         QGraphicsPathItem* item = linkConnectors[0];
         if (item) delete item;
         linkConnectors.erase(linkConnectors.begin());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ValueData::Remove(EditorScene* scene)
   {
      ResizeLinks(0, scene);
      if (linkName) delete linkName;
      if (linkGraphic) delete linkGraphic;
      linkName = NULL;
      linkGraphic = NULL;
      link = NULL;
   }

   //////////////////////////////////////////////////////////////////////////
   NodeItem::NodeItem(Node* node, QGraphicsItem* parent, EditorScene* scene)
       : QGraphicsPolygonItem(parent, scene)
       , mScene(scene)
       , mLoading(true)
       , mTitle(NULL)
       , mTitleBG(NULL)
       , mComment(NULL)
       , mNode(node)
       , mLinkDivider(NULL)
       , mValueDivider(NULL)
       , mTitleDivider(NULL)
       , mNodeWidth(MIN_NODE_WIDTH)
       , mNodeHeight(MIN_NODE_HEIGHT)
       , mTextHeight(0.0f)
       , mLinkWidth(0.0f)
       , mLinkHeight(0.0f)
       , mValueHeight(0.0f)
       , mHasHiddenLinks(false)
       , mColorDarken(0, 0, 0, 50)
   {
      setFlag(QGraphicsItem::ItemIsMovable, true);
      setFlag(QGraphicsItem::ItemIsSelectable, true);

#if(QT_VERSION >= 0x00040600)
      setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
#endif
   }

   //////////////////////////////////////////////////////////////////////////
   NodeItem::~NodeItem()
   {
      // Clear all links.
      int count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         mValues[index].Remove(mScene);
      }
      mValues.clear();

      count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         mInputs[index].Remove();
      }
      mInputs.clear();

      count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         mOutputs[index].Remove(mScene);
      }
      mOutputs.clear();
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::Draw()
   {
      mLoading = true;

      mNodeWidth = MIN_NODE_WIDTH;
      mNodeHeight = MIN_NODE_HEIGHT;
      mTextHeight = 0.0f;
      mLinkWidth = 0.0f;
      mLinkHeight = 0.0f;
      mValueHeight = 0.0f;

      FindLinks();

      if (mNode.valid())
      {
         osg::Vec2 pos = mNode->GetPosition();
         setPos(pos.x(), pos.y());

         if (mNode->GetComment().empty())
         {
            setToolTip(mNode->GetDescription().c_str());
         }
         else
         {
            setToolTip(QString(mNode->GetDescription().c_str()) +
               QString("\nComment - ") + QString(mNode->GetComment().c_str()));
         }
      }

      mLoading = false;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonTop()
   {
      mPolygon.clear();

      mPolygon << QPointF(0, 0) << QPointF(mNodeWidth, 0);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonRightRound()
   {
      QPointF point(0, -mNodeHeight / 2);
      QPointF center(mNodeWidth, mNodeHeight / 2);

      int points = mNodeHeight / 5;
      float angle = DegToRad(180 / points);
      for (int index = 0; index <= points; index++)
      {
         mPolygon << RotatePoint(point, angle * index) + center;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonRightFlat()
   {
      mPolygon << QPointF(mNodeWidth, mNodeHeight);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonBottomRound()
   {
      QPointF point(mNodeWidth / 2, 0);
      QPointF center(mNodeWidth / 2, mNodeHeight);

      int points = mNodeWidth / 5;
      float angle = DegToRad(180 / points);
      for (int index = 0; index <= points; index++)
      {
         mPolygon << RotatePoint(point, angle * index) + center;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonBottomFlat()
   {
      mPolygon << QPointF(0, mNodeHeight);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonLeftRound()
   {
      QPointF point(0, mNodeHeight / 2);
      QPointF center(0, mNodeHeight / 2);

      int points = mNodeHeight / 5;
      float angle = DegToRad(180 / points);
      for (int index = 0; index <= points; index++)
      {
         mPolygon << RotatePoint(point, angle * index) + center;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawPolygonLeftFlat()
   {
      mPolygon << QPointF(0, 0);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::SetTitle(std::string text)
   {
      if (!mTitle)
      {
         mTitleBG = new QGraphicsRectItem(this, scene());
         mTitle = new GraphicsTextItem(mTitleBG, scene());

         mTitleBG->setPen(QPen(Qt::transparent, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         mTitleBG->setBrush(QColor(0, 0, 0, 0));
         mTitleBG->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
      }

      if (mHasHiddenLinks) text += "*";
      mTitle->setHtml((std::string("<center>") + text + "</center>").c_str());

      // Create the title background.
      QRectF bounds = mTitle->boundingRect();
      mTextHeight = bounds.height();

      if (mNodeHeight < int(mTextHeight)) mNodeHeight = int(mTextHeight);

      // Clamp the bounds to our min and max.
      if (bounds.width() > MAX_NODE_WIDTH - 2) bounds.setWidth(MAX_NODE_WIDTH - 2);
      if (bounds.width() < MIN_NODE_WIDTH - 2) bounds.setWidth(MIN_NODE_WIDTH - 2);

      // Resize the width of the node if it is not wide enough already.
      if (bounds.width() + 2 > mNodeWidth)
      {
         mNodeWidth = int(bounds.width());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::SetComment(const std::string& text)
   {
      if (!mComment)
      {
         mComment = new GraphicsTextItem(this, scene());

         mComment->setDefaultTextColor(Qt::darkGreen);
      }

      mComment->setPlainText(text.c_str());
      mComment->setTextWidth(500);

      // Create the title background.
      QRectF commentBounds = mComment->boundingRect();
      QRectF nodeBounds = boundingRect();

      mComment->setPos(nodeBounds.x(), -commentBounds.height());
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::FindLinks()
   {
      // First clear all current links.
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         mInputs[index].Remove();
      }
      mInputs.clear();

      count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         mOutputs[index].Remove(mScene);
      }
      mOutputs.clear();

      count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         mValues[index].Remove(mScene);
      }
      mValues.clear();

      mHasHiddenLinks = false;

      if (!mNode.valid()) return;

      if (mNode->InputsExposed())
      {
         count = (int)mNode->GetInputLinks().size();
         for (int index = 0; index < count; index++)
         {
            mInputs.push_back(InputData());
            InputData& data = mInputs.back();

            data.link = &mNode->GetInputLinks()[index];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new InputLinkItem(this, (int)mInputs.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }

      if (mNode->OutputsExposed())
      {
         count = (int)mNode->GetOutputLinks().size();
         for (int index = 0; index < count; index++)
         {
            mOutputs.push_back(OutputData());
            OutputData& data = mOutputs.back();

            data.link = &mNode->GetOutputLinks()[index];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new OutputLinkItem(this, (int)mOutputs.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }

      if (mNode->ValuesExposed())
      {
         count = (int)mNode->GetValueLinks().size();
         for (int index = 0; index < count; index++)
         {
            ValueLink* link = &mNode->GetValueLinks()[index];
            if (!link->GetExposed()) continue;
            if (!link->GetVisible()) mHasHiddenLinks = true;

            mValues.push_back(ValueData());
            ValueData& data = mValues.back();

            data.link = link;
            data.linkGraphic = new ValueLinkItem(this, (int)mValues.size()-1, this, mScene);
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawTitle()
   {
      if (!mTitleBG) return;

      // Create the title background.
      QRectF bounds = mTitle->boundingRect();
      bounds.setWidth(mNodeWidth - 2);

      mTitleBG->setRect(bounds);
      mTitleBG->setPos(1.0f, 1.0f);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawInputs()
   {
      float maxSize = 0;

      QPolygonF poly;
      poly << QPointF(0, 0) << QPointF(LINK_LENGTH, 0) <<
         QPointF(LINK_LENGTH, LINK_SIZE) << QPointF(0, LINK_SIZE) <<
         QPointF(LINK_SIZE/2, LINK_SIZE/2);

      int visibleCount = 0;
      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         InputData& data = mInputs[index];
         if (!data.link || !data.link->GetVisible()) continue;
         
         visibleCount++;
         // Create the link graphic.
         data.linkGraphic->setPolygon(poly);
         data.linkGraphic->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         data.linkGraphic->setBrush(QColor(50, 50, 50));

         // Set the link text, and position it right aligned with the link graphic.
         data.linkName->setPlainText(data.link->GetName().c_str());
         QRectF nameBounds = data.linkName->boundingRect();

         if (maxSize < nameBounds.width()) maxSize = nameBounds.width();

         float x = LINK_LENGTH;
         float y = -(nameBounds.height()/2) + (LINK_SIZE/2);
         data.linkName->setPos(x, y);
      }

      // Resize the node width if we have to.
      float desiredWidth = maxSize;
      if (mNodeWidth < desiredWidth) mNodeWidth = desiredWidth;
      mLinkWidth = desiredWidth;

      // Resize the node height if we have to.
      float desiredHeight = ((visibleCount + 1) * (LINK_SPACING + mTextHeight)) - (LINK_SPACING * 2);
      if (mNodeHeight < desiredHeight) mNodeHeight = desiredHeight;
      mLinkHeight = desiredHeight;

      // Now position all of the links in a single column.
      count = (int)mInputs.size();
      visibleCount = 0;
      for (int index = 0; index < count; index++)
      {
         InputData& data = mInputs[index];
         if (!data.link || !data.link->GetVisible()) continue;

         float x = -LINK_LENGTH;
         float y = ((LINK_SPACING + mTextHeight) * (visibleCount + 1)) + LINK_SIZE/2 + 1;

         data.linkGraphic->setPos(x, y);

         visibleCount++;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawOutputs()
   {
      float maxSize = 0;
      QPolygonF poly;
      poly << QPointF(0, 0) << QPointF(LINK_LENGTH - LINK_SIZE/2, 0) <<
         QPointF(LINK_LENGTH, LINK_SIZE/2) <<
         QPointF(LINK_LENGTH - LINK_SIZE/2, LINK_SIZE) <<
         QPointF(0, LINK_SIZE);
      float offset = 0;

      int visibleCount = 0;
      int count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         OutputData& data = mOutputs[index];
         if (!data.link || !data.link->GetVisible()) continue;

         visibleCount++;

         // Create the link graphic.
         data.linkGraphic->setPolygon(poly);
         data.linkGraphic->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         data.linkGraphic->setBrush(QColor(50, 50, 50));

         bool alwaysHighlight = false;
         data.linkGraphic->SetAlwaysHighlight(false);
         if (mScene->GetEditor()->GetReplayMode() &&
            mScene->GetEditor()->GetReplayOutput() &&
            mScene->GetEditor()->GetReplayOutput() == data.link)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = data.link->GetLinks()[linkIndex];
               if (link && link->GetOwner()->GetID() == mScene->GetEditor()->GetReplayNode().nodeID)
               {
                  data.linkGraphic->SetAlwaysHighlight(true);
                  alwaysHighlight = true;
                  break;
               }
            }
         }

         if (!alwaysHighlight)
         {
            int linkCount = (int)data.link->GetLinks().size();
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = data.link->GetLinks()[linkIndex];

               if (mScene->GetEditor()->GetReplayMode() &&
                  mScene->GetEditor()->GetReplayInput() &&
                  mScene->GetEditor()->GetReplayInput() == link &&
                  mScene->GetEditor()->GetReplayOutput() == data.link)
               {
                  data.linkGraphic->SetAlwaysHighlight(true);
                  break;
               }
            }
         }

         // Set the link text, and position it right aligned with the link graphic.
         data.linkName->setPlainText(data.link->GetName().c_str());
         QRectF nameBounds = data.linkName->boundingRect();

         if (maxSize < nameBounds.width()) maxSize = nameBounds.width();

         float x = -nameBounds.width();
         float y = (LINK_SIZE/2) - (nameBounds.height()/2);
         offset = -y;
         data.linkName->setPos(x, y);
      }

      // Resize the node width if we have to.
      float desiredWidth = maxSize + mLinkWidth + LINK_SPACING;
      if (mNodeWidth < desiredWidth) mNodeWidth = desiredWidth;

      // Reposition the link divider if the node is already larger than we need.
      if (mNodeWidth > desiredWidth)
      {
         mLinkWidth = mLinkWidth + (((mNodeWidth - maxSize + (LINK_SPACING / 2)) - mLinkWidth) / 2);
      }

      // Resize the node height if we have to.
      float desiredHeight = ((visibleCount + 1) * (LINK_SPACING + mTextHeight)) - (LINK_SPACING * 2);
      if (mNodeHeight < desiredHeight + mValueHeight) mNodeHeight = desiredHeight + mValueHeight;
      if (desiredHeight > mLinkHeight) mLinkHeight = desiredHeight;

      // Now position all of the links in a single column.
      visibleCount = 0;
      count = (int)mOutputs.size();
      for (int index = 0; index < count; index++)
      {
         OutputData& data = mOutputs[index];
         if (!data.link || !data.link->GetVisible()) continue;

         float x = mNodeWidth + 1.0f;
         float y = ((LINK_SPACING + mTextHeight) * (visibleCount + 1)) - LINK_SPACING + offset;

         data.linkGraphic->setPos(x, y);

         visibleCount++;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::SetupValues()
   {
      float maxWidth = 0;

      int visibleCount = 0;
      int count = (int)mValues.size();
      for (int index = 0; index < count; index++)
      {
         ValueData& data = mValues[index];
         if (!data.link || !data.link->GetVisible()) continue;

         visibleCount++;
         // Out links are triangular shaped.
         QPolygonF poly;
         if (data.link->IsOutLink())
         {
            poly << QPointF(-LINK_SIZE/2, 0) << QPointF(LINK_SIZE/2, 0) <<
               QPointF(LINK_SIZE/2, LINK_LENGTH - LINK_SIZE/2) <<
               QPointF(0, LINK_LENGTH) <<
               QPointF(-LINK_SIZE/2, LINK_LENGTH - LINK_SIZE/2);
         }
         // In links are rectangular shaped.
         else
         {
            poly << QPointF(-LINK_SIZE/2, 0) << QPointF(LINK_SIZE/2, 0) <<
               QPointF(LINK_SIZE/2, LINK_LENGTH) << QPointF(0, LINK_LENGTH - LINK_SIZE/2) <<
               QPointF(-LINK_SIZE/2, LINK_LENGTH);
         }

         data.linkGraphic->setPolygon(poly);
         data.linkGraphic->SetPropertyType(data.link->GetPropertyType().GetTypeId());

         // Set the color of the link based on the property type.
         dtDAL::DataType& type = data.link->GetPropertyType();
         if (data.link->IsTypeChecking())
         {
            data.linkGraphic->setPen(QPen(GetDarkColorForType(type.GetTypeId()), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         }
         else
         {
            data.linkGraphic->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
         }

         // Set the color of the link based on the property type.
         data.linkGraphic->setBrush(GetColorForType(type.GetTypeId()));

         // Set the link text, and position it right aligned with the link graphic.
         data.linkName->setHtml((std::string("<center>") + data.link->GetDisplayName() + "</center>").c_str());
         QRectF nameBounds = data.linkName->boundingRect();

         if (nameBounds.width() > MAX_VALUE_NAME_SIZE)
         {
            data.linkName->setTextWidth(MAX_VALUE_NAME_SIZE);
            nameBounds = data.linkName->boundingRect();
         }

         if (maxWidth < nameBounds.width()) maxWidth = nameBounds.width();
         if (mValueHeight < nameBounds.height()) mValueHeight = nameBounds.height();

         float x = -nameBounds.width() / 2;
         float y = -nameBounds.height();
         data.linkName->setPos(x, y);
      }

      // Resize the node width if we have to.
      float desiredWidth = (maxWidth + LINK_SPACING) * visibleCount - LINK_SPACING;
      if (mNodeWidth < desiredWidth) mNodeWidth = desiredWidth;

      // Resize the node height if we have to.
      float desiredHeight = mValueHeight + LINK_SPACING + mLinkHeight;
      if (mNodeHeight < desiredHeight) mNodeHeight = desiredHeight;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawValues()
   {
      // Now position all of the links in a single row.
      int count = (int)mValues.size();
      int visibleCount = 0;
      for (int index = 0; index < count; index++)
      {
         if (!mValues[index].link || !mValues[index].link->GetVisible()) continue;
         visibleCount++;
      }

      float step = 0;
      if (visibleCount > 0)
      {
         step = (mNodeWidth / visibleCount);
      }

      visibleCount = 0;
      for (int index = 0; index < count; index++)
      {
         ValueData& data = mValues[index];

         float x = -step / 2;
         float y = data.linkName->pos().y();
         data.linkName->setTextWidth(step);
         data.linkName->setPos(x, y);

         data.linkName->setHtml(data.linkName->toHtml());
         if (!mValues[index].link || !mValues[index].link->GetVisible()) continue;
         x = step * (visibleCount + 1) - (step / 2);
         y = mNodeHeight + 1.0f;

         data.linkGraphic->setPos(x, y);

         visibleCount++;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawDividers()
   {
      if (!mTitleDivider) mTitleDivider = new QGraphicsRectItem(this, scene());
      mTitleDivider->setPos(1, mTextHeight + 1);
      mTitleDivider->setRect(0, 0, mNodeWidth - 2, 0);
      mTitleDivider->setPen(QPen(mColorDarken, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

      // Draw the vertical divider if we are displaying both inputs and outputs.
      if (!mInputs.empty() && !mOutputs.empty())
      {
         float x = mLinkWidth + (LINK_SPACING / 2);
         float y = mTextHeight + 2;
         float height = (mValues.empty() ? std::max((float)MIN_NODE_HEIGHT, mLinkHeight) : mLinkHeight) - y - 1;

         if (!mLinkDivider) mLinkDivider = new QGraphicsRectItem(this, scene());
         mLinkDivider->setPos(x, y);
         mLinkDivider->setRect(0, 0, 0, height);
         mLinkDivider->setPen(QPen(mColorDarken, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      }
      else if (mLinkDivider)
      {
         delete mLinkDivider;
         mLinkDivider = NULL;
      }

      // Draw the horizontal divider if we are displaying value links with inputs and/or outputs.
      if (!mValues.empty() &&
         (!mInputs.empty() || !mOutputs.empty()))
      {
         if (!mValueDivider) mValueDivider = new QGraphicsRectItem(this, scene());
         mValueDivider->setPos(0, mLinkHeight - 1);
         mValueDivider->setRect(1, 0, mNodeWidth - 2, 0);
         mValueDivider->setPen(QPen(mColorDarken, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      }
      else if (mValueDivider)
      {
         delete mValueDivider;
         mValueDivider = NULL;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   QColor NodeItem::GetColorForType(unsigned char type)
   {
      switch (type)
      {
      case dtDAL::DataType::ACTOR_ID:
         return Qt::magenta;
         break;
      case dtDAL::DataType::BOOLEAN_ID:
         return Qt::red;
         break;
      case dtDAL::DataType::INT_ID:
         return Qt::blue;
         break;
      case dtDAL::DataType::FLOAT_ID:
         return Qt::yellow;
         break;
      case dtDAL::DataType::DOUBLE_ID:
         return Qt::green;
         break;
      case dtDAL::DataType::STRING_ID:
         return Qt::cyan;
         break;

      case dtDAL::DataType::STATICMESH_ID:
      case dtDAL::DataType::SKELETAL_MESH_ID:
      case dtDAL::DataType::TEXTURE_ID:
      case dtDAL::DataType::SOUND_ID:
      case dtDAL::DataType::PARTICLESYSTEM_ID:
      case dtDAL::DataType::PREFAB_ID:
      case dtDAL::DataType::SHADER_ID:
         return Qt::magenta;
         break;

      case dtDAL::DataType::VEC2_ID:
      case dtDAL::DataType::VEC3_ID:
      case dtDAL::DataType::VEC4_ID:
      case dtDAL::DataType::VEC2F_ID:
      case dtDAL::DataType::VEC3F_ID:
      case dtDAL::DataType::VEC4F_ID:
      case dtDAL::DataType::VEC2D_ID:
      case dtDAL::DataType::VEC3D_ID:
      case dtDAL::DataType::VEC4D_ID:
         return Qt::magenta;
         break;

      case dtDAL::DataType::UNKNOWN_ID:
      default:
         return Qt::white;
         break;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   QColor NodeItem::GetDarkColorForType(unsigned char type)
   {
      switch (type)
      {
      case dtDAL::DataType::ACTOR_ID:
         return Qt::darkMagenta;
         break;
      case dtDAL::DataType::BOOLEAN_ID:
         return Qt::darkRed;
         break;
      case dtDAL::DataType::INT_ID:
         return Qt::darkBlue;
         break;
      case dtDAL::DataType::FLOAT_ID:
         return Qt::darkYellow;
         break;
      case dtDAL::DataType::DOUBLE_ID:
         return Qt::darkGreen;
         break;
      case dtDAL::DataType::STRING_ID:
         return Qt::darkCyan;
         break;

      case dtDAL::DataType::STATICMESH_ID:
      case dtDAL::DataType::SKELETAL_MESH_ID:
      case dtDAL::DataType::TEXTURE_ID:
      case dtDAL::DataType::SOUND_ID:
      case dtDAL::DataType::PARTICLESYSTEM_ID:
      case dtDAL::DataType::PREFAB_ID:
      case dtDAL::DataType::SHADER_ID:
         return Qt::darkMagenta;
         break;

      case dtDAL::DataType::VEC2_ID:
      case dtDAL::DataType::VEC3_ID:
      case dtDAL::DataType::VEC4_ID:
      case dtDAL::DataType::VEC2F_ID:
      case dtDAL::DataType::VEC3F_ID:
      case dtDAL::DataType::VEC4F_ID:
      case dtDAL::DataType::VEC2D_ID:
      case dtDAL::DataType::VEC3D_ID:
      case dtDAL::DataType::VEC4D_ID:
         return Qt::darkMagenta;
         break;

      case dtDAL::DataType::UNKNOWN_ID:
      default:
         return Qt::gray;
         break;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec2 NodeItem::GetPosition()
   {
      return mNode->GetPosition();
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId NodeItem::GetID()
   {
      if (mNode.valid())
      {
         return mNode->GetID();
      }

      return dtCore::UniqueId();
   }

   //////////////////////////////////////////////////////////////////////////
   bool NodeItem::HasID(const dtCore::UniqueId& id)
   {
      if (mNode.valid())
      {
         return id == mNode->GetID();
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::ConnectLinks(bool fullConnect)
   {
      int count = (int)mOutputs.size();
      for (int outputIndex = 0; outputIndex < count; outputIndex++)
      {
         OutputData& output = mOutputs[outputIndex];
         if (output.link)
         {
            int linkCount = (int)output.link->GetLinks().size();
            output.ResizeLinks(linkCount, mScene);
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               InputLink* link = output.link->GetLinks()[linkIndex];
               if (!link) continue;

               NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
               if (!item) continue;

               int inputCount = (int)item->mInputs.size();
               for (int inputIndex = 0; inputIndex < inputCount; inputIndex++)
               {
                  if (item->mInputs[inputIndex].link == link)
                  {
                     ConnectLinks(output, item->mInputs[inputIndex], linkIndex);
                     break;
                  }
               }
            }
         }
      }

      if (fullConnect)
      {
         int inputCount = (int)mInputs.size();
         for (int inputIndex = 0; inputIndex < inputCount; inputIndex++)
         {
            InputData& input = mInputs[inputIndex];
            if (input.link)
            {
               int linkCount = (int)input.link->GetLinks().size();
               for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
               {
                  OutputLink* output = input.link->GetLinks()[linkIndex];
                  if (!output) continue;

                  NodeItem* item = mScene->GetNodeItem(output->GetOwner()->GetID());
                  if (!item) continue;

                  int outputCount = (int)item->mOutputs.size();
                  for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
                  {
                     OutputData& outputData = item->mOutputs[outputIndex];
                     if (outputData.link == output)
                     {
                        int count = (int)output->GetLinks().size();
                        outputData.ResizeLinks(count, mScene);
                        for (int index = 0; index < count; index++)
                        {
                           if (output->GetLinks()[index] == input.link)
                           {
                              ConnectLinks(item->mOutputs[outputIndex], input, index);
                              break;
                           }
                        }
                        break;
                     }
                  }
               }
            }
         }
      }

      count = (int)mValues.size();
      for (int valueIndex = 0; valueIndex < count; valueIndex++)
      {
         ValueData& value = mValues[valueIndex];
         if (value.link)
         {
            int linkCount = (int)value.link->GetLinks().size();
            value.ResizeLinks(linkCount, mScene);
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueNode* valueNode = value.link->GetLinks()[linkIndex];
               if (!valueNode) continue;

               NodeItem* item = mScene->GetNodeItem(valueNode->GetID());
               if (!item) continue;

               ConnectLinks(value, item, linkIndex);
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   QPainterPath NodeItem::CreateConnectionH(QPointF start, QPointF end, bool drawReverse)
   {
      // Modify the positions based on the translation of the background item.
      QPointF offset = mScene->GetTranslationItem()->scenePos();
      start -= offset;
      end -= offset;

      float halfX = (start.x() + end.x()) / 2.0f;

      QPainterPath path;

      if (start.x() < end.x())
      {
         if (drawReverse)
         {
            QPointF temp = start;
            start = end;
            end = temp;
         }

         path.moveTo(start);
         path.cubicTo(
            halfX, start.y(),
            halfX, end.y(),
            end.x(), end.y());
      }
      else
      {
         if (drawReverse)
         {
            QPointF temp = start;
            start = end;
            end = temp;
         }

         float rightX = start.x() + (start.x() - end.x()) / 4;
         float leftX = end.x() - (start.x() - end.x()) / 4;

         float halfY = (start.y() + end.y()) / 2.0f;

         path.moveTo(start);
         path.cubicTo(
            rightX, start.y(),
            rightX, halfY,
            halfX, halfY);
         path.cubicTo(
            leftX, halfY,
            leftX, end.y(),
            end.x(), end.y());
      }

      return path;
   }

   //////////////////////////////////////////////////////////////////////////
   QPainterPath NodeItem::CreateConnectionV(QPointF start, QPointF end, bool drawReverse)
   {
      // Modify the positions based on the translation of the background item.
      QPointF offset = mScene->GetTranslationItem()->scenePos();
      start -= offset;
      end -= offset;

      float halfY = (start.y() + end.y()) / 2.0f;

      QPainterPath path;
      if (start.y() < end.y())
      {
         if (drawReverse)
         {
            QPointF temp = start;
            start = end;
            end = temp;
         }

         path.moveTo(start);
         path.cubicTo(
            start.x(), halfY,
            end.x(), halfY,
            end.x(), end.y());
      }
      else
      {
         if (drawReverse)
         {
            QPointF temp = start;
            start = end;
            end = temp;
         }

         float bottomY = start.y() + (start.y() - end.y()) / 4;
         float topY = end.y() - (start.y() - end.y()) / 4;

         float halfX = (start.x() + end.x()) / 2.0f;

         path.moveTo(start);
         path.cubicTo(
            start.x(), bottomY,
            halfX, bottomY,
            halfX, halfY);
         path.cubicTo(
            halfX, topY,
            end.x(), topY,
            end.x(), end.y());
      }

      return path;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::BeginMoveEvent()
   {
      dtDAL::ActorProperty* prop = mNode->GetProperty("Position");
      if (prop) mOldPosition = prop->ToString();
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::EndMoveEvent()
   {
      dtDAL::ActorProperty* prop = mNode->GetProperty("Position");
      if (prop)
      {
         std::string value = prop->ToString();

         // Ignore the property if the node did not move.
         if (value == mOldPosition) return;

         // Notify the undo manager of the property changes.
         dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), mNode->GetID(), prop->GetName(), mOldPosition, value);
         mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeItem::ExposeValue(QAction* action)
   {
      if (!action) return;

      ValueLink* link = mNode->GetValueLink(action->text().toStdString());
      if (link)
      {
         link->SetExposed(true);
         link->SetVisible(true);
         mScene->Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::ConnectLinks(OutputData& output, InputData& input, int index)
   {
      if (index < 0 || (int)output.linkConnectors.size() < index) return;

      QPointF start(output.linkGraphic->scenePos());
      QPointF end(input.linkGraphic->scenePos());

      start.setX(start.x() + LINK_LENGTH);
      start.setY(start.y() + LINK_SIZE/2);
      end.setX(end.x() + LINK_SIZE/2);
      end.setY(end.y() + LINK_SIZE/2);

      QPainterPath path = CreateConnectionH(start, end);
      output.linkConnectors[index]->setPath(path);

      output.linkConnectors[index]->setPen(
         QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

      input.linkGraphic->InitHighlight();
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::ConnectLinks(ValueData& output, NodeItem* input, int index)
   {
      if (index < 0 || (int)output.linkConnectors.size() < index) return;

      QPointF start(output.linkGraphic->scenePos());
      QPointF end(input->scenePos());

      if (output.link->IsOutLink())
      {
         start.setY(start.y() + LINK_LENGTH);
      }
      else
      {
         start.setY(start.y() + LINK_LENGTH - LINK_SIZE/2);
      }

      end.setX(end.x() + input->mNodeWidth / 2);
      end.setY(end.y() - LINK_LENGTH);

      QPainterPath path = CreateConnectionV(start, end);
      output.linkConnectors[index]->setPath(path);

      // Set the color of the link line to match the value type.
      ValueNode* valueNode = dynamic_cast<ValueNode*>(input->mNode.get());
      if (valueNode)
      {
         output.linkConnectors[index]->setPen(
            QPen(GetDarkColorForType(output.linkGraphic->GetPropertyType()),
            2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
   {
      setSelected(true);
      QMenu menu;
      menu.addAction(mScene->GetMacroSelectionAction());
      if (!mScene->GetSelection().empty())
      {
         QAction* createGroupAction = menu.addAction("Create Group Around Selection");
         connect(createGroupAction, SIGNAL(triggered()), mScene, SLOT(OnCreateGroupForSelection()));
      }
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetCutAction());
      menu.addAction(mScene->GetEditor()->GetCopyAction());
      menu.addSeparator();
      
      QMenu* exposeMenu = NULL;
      std::vector<ValueLink> &values = mNode->GetValueLinks();
      int count = (int)values.size();
      for (int index = 0; index < count; index++)
      {
         ValueLink& link = values[index];
         if (!link.GetExposed())
         {
            if (!exposeMenu)
            {
               exposeMenu = menu.addMenu("Expose Values");
               connect(exposeMenu, SIGNAL(triggered(QAction*)), this, SLOT(ExposeValue(QAction*)));
            }

            exposeMenu->addAction(link.GetName().c_str());
         }
      }

      menu.addAction(mScene->GetEditor()->GetShowLinkAction());
      menu.addAction(mScene->GetEditor()->GetHideLinkAction());
      menu.addSeparator();
      menu.addAction(mScene->GetEditor()->GetDeleteAction());
      menu.exec(event->screenPos());
   }

   //////////////////////////////////////////////////////////////////////////
   QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
   {
      if (!mNode.valid()) return value;

      if (change == QGraphicsItem::ItemPositionHasChanged)
      {
         QPointF newPos = value.toPointF();

         mNode->SetPosition(osg::Vec2(newPos.x(), newPos.y()));

         if (!mLoading)
         {
            ConnectLinks(true);
         }
      }
      else if (change == QGraphicsItem::ItemSelectedHasChanged)
      {
         if (isSelected())
         {
            setZValue(zValue() + 100.0f);
            mScene->AddSelected(mNode.get());
         }
         else
         {
            setZValue(zValue() - 100.0f);
            mScene->RemoveSelected(mNode.get());
         }
      }

      return value;
   }
}

//////////////////////////////////////////////////////////////////////////
