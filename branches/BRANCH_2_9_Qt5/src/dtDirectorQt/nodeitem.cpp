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
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/nodeitem.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/linkitem.h>
#include <dtDirectorQt/groupitem.h>
#include <dtDirectorQt/editornotifier.h>

#include <dtDirectorQt/undomanager.h>
#include <dtDirectorQt/undopropertyevent.h>
#include <dtDirectorQt/undolinkvisibilityevent.h>

#include <dtDirector/valuenode.h>
#include <dtDirector/groupnode.h>
#include <dtDirector/nodemanager.h>
#include <dtDirectorNodes/externalvaluenode.h>

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsColorizeEffect>
#include <QtGui/QMenu>

#include <dtCore/datatype.h>

#include <dtUtil/mathdefines.h>

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
   void InputData::DrawGlow(float glow)
   {
      linkGraphic->DrawGlow(glow);
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
   void OutputData::DrawGlow(float glow)
   {
      linkGraphic->DrawGlow(glow);
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

   const QColor NodeItem::LINE_COLOR(0, 0, 0, 50);

   //////////////////////////////////////////////////////////////////////////
   NodeItem::NodeItem(Node* node, bool readOnly, bool imported, QGraphicsItem* parent, EditorScene* scene)
       : QGraphicsPolygonItem(parent, scene)
       , mScene(scene)
       , mLoading(true)
       , mIsReadOnly(readOnly)
       , mIsImported(imported)
       , mTitle(NULL)
       , mTitleBG(NULL)
       , mLatentIcon(NULL)
       , mComment(NULL)
       , mNode(node)
       , mLinkDivider(NULL)
       , mValueDivider(NULL)
       , mTitleDivider(NULL)
       , mNodeWidth(MIN_NODE_WIDTH)
       , mNodeHeight(MIN_NODE_HEIGHT)
       , mTitleHeight(0.0f)
       , mLinkWidth(0.0f)
       , mLinkHeight(0.0f)
       , mValueHeight(0.0f)
       , mHasHiddenLinks(false)
       , mChainSelecting(false)
   {
      setFlag(QGraphicsItem::ItemIsMovable, !imported && !readOnly);
      setFlag(QGraphicsItem::ItemIsSelectable, true);

#if(QT_VERSION >= 0x00040600)
      setFlag(QGraphicsItem::ItemSendsGeometryChanges, !imported && !readOnly);
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
      mTitleHeight = 0.0f;
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
   std::string NodeItem::GetNodeTitle()
   {
      if (!mNode)
      {
         return "";
      }

      std::string title = "<i>"+ mNode->GetTypeName() +"</i>";
      if (mHasHiddenLinks) title += "*";
      std::string name  = mNode->GetName();
      if (!name.empty() && !IS_A(mNode.get(), ExternalValueNode*))
      {
         title += "<br><b>"+ name +"</b>";
      }
      return title;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::SetTitle(std::string text)
   {
      if (!mTitle)
      {
         mTitleBG = new QGraphicsRectItem(this, scene());
         mTitle = new GraphicsTextItem(mTitleBG, scene());

         if (mIsReadOnly)
         {
            QFont font = mTitle->font();
            font = QFont(font.family(), font.pointSize(), font.weight(), false);
            mTitle->setFont(font);
            mTitle->setDefaultTextColor(Qt::darkGray);
         }

         mTitleBG->setPen(Qt::NoPen);
         mTitleBG->setBrush(Qt::NoBrush);
         mTitleBG->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
      }

      mTitle->setHtml((std::string("<center>") + text + "</center>").c_str());

      // Create the title background.
      QRectF bounds = mTitle->boundingRect();
      mTitleHeight = bounds.height();

      if (mNodeHeight < int(mTitleHeight)) mNodeHeight = int(mTitleHeight);

      // Create our latent node icon and make room for it.
      if (mNode.valid() && mNode->AsLatentNode())
      {
         if (!mLatentIcon)
         {
            mLatentIcon = new QGraphicsPixmapItem(mTitleBG, scene());
            mLatentIcon->setPixmap(QPixmap(":icons/timer_icon.png"));
            mLatentIcon->setOpacity(0.25f);
            mLatentIcon->setScale(0.4f);
         }

         bounds.setWidth(bounds.width() + mLatentIcon->boundingRect().width() * mLatentIcon->scale());
      }

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

         if (mIsReadOnly)
         {
            QFont font = mComment->font();
            font = QFont(font.family(), font.pointSize(), font.weight(), false);
            mComment->setFont(font);
            mComment->setDefaultTextColor(Qt::darkGray);
         }
         else
         {
            mComment->setDefaultTextColor(Qt::darkGreen);
         }
      }

      mComment->setPlainText(text.c_str());
      DrawComment();
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

            data.node = this;
            data.link = &mNode->GetInputLinks()[index];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new InputLinkItem(this, (int)mInputs.size()-1, this, mScene, data.link->GetComment());
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
            if (mIsReadOnly)
            {
               QFont font = data.linkName->font();
               font = QFont(font.family(), font.pointSize(), font.weight(), false);
               data.linkName->setFont(font);
               data.linkName->setDefaultTextColor(Qt::darkGray);
            }
         }
      }

      if (mNode->OutputsExposed())
      {
         count = (int)mNode->GetOutputLinks().size();
         for (int index = 0; index < count; index++)
         {
            mOutputs.push_back(OutputData());
            OutputData& data = mOutputs.back();

            data.node = this;
            data.link = &mNode->GetOutputLinks()[index];
            if (!data.link->GetVisible()) mHasHiddenLinks = true;

            data.linkGraphic = new OutputLinkItem(this, (int)mOutputs.size()-1, this, mScene, data.link->GetComment());
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
            if (mIsReadOnly)
            {
               QFont font = data.linkName->font();
               font = QFont(font.family(), font.pointSize(), font.weight(), false);
               data.linkName->setFont(font);
               data.linkName->setDefaultTextColor(Qt::darkGray);
            }
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

            data.node = this;
            data.link = link;
            data.linkGraphic = new ValueLinkItem(this, (int)mValues.size()-1, this, mScene, data.link->GetComment());
            data.linkName = new GraphicsTextItem(data.linkGraphic, mScene);
            data.linkName->setAcceptHoverEvents(false);
            if (mIsReadOnly)
            {
               QFont font = data.linkName->font();
               font = QFont(font.family(), font.pointSize(), font.weight(), false);
               data.linkName->setFont(font);
               data.linkName->setDefaultTextColor(Qt::darkGray);
            }
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

      if (mLatentIcon)
      {
         mLatentIcon->setPos(bounds.width() -
            mLatentIcon->boundingRect().width() * mLatentIcon->scale(), 0);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawInputs()
   {
      float maxSize  = 0;

      QPolygonF poly;
      poly << QPointF(0, 0) << QPointF(LINK_LENGTH, 0) <<
         QPointF(LINK_LENGTH, LINK_SIZE) << QPointF(0, LINK_SIZE) <<
         QPointF(LINK_SIZE/2, LINK_SIZE/2);

      int count = (int)mInputs.size();
      for (int index = 0; index < count; index++)
      {
         InputData& data = mInputs[index];
         if (!data.link || !data.link->GetVisible()) continue;

         // Create the link graphic.
         data.linkGraphic->setPolygon(poly);
         data.linkGraphic->setPen(QPen(Qt::darkGray));
         data.linkGraphic->setBrush(GetNodeColor());
         data.linkGraphic->SetPenColor(Qt::darkGray);

         // Set the link text, and position it right aligned with the link graphic.
         data.linkName->setPlainText(data.link->GetName().c_str());
         QRectF nameBounds = data.linkName->boundingRect();

         if (maxSize < nameBounds.width()) maxSize = nameBounds.width();

         float x = LINK_LENGTH;
         float y = -(nameBounds.height()/2) + (LINK_SIZE/2);
         data.linkName->setPos(x, y);
      }

      // Resize the node width if we have to.
      int desiredWidth = maxSize;
      mNodeWidth = std::max( mNodeWidth, desiredWidth );
      mLinkWidth = desiredWidth;

      // Now position all of the links in a single column.
      count = (int)mInputs.size();
      float posY = mTitleHeight + (count ? LINK_PADDING : 0);
      for (int index = 0; index < count; index++)
      {
         InputData& data = mInputs[index];
         if (!data.link || !data.link->GetVisible()) continue;

         QRectF nameBounds = data.linkName->boundingRect();
         data.linkGraphic->setPos(-LINK_LENGTH, posY);

         posY += nameBounds.height();
      }
      mLinkHeight = std::max( mLinkHeight, posY - mTitleHeight );
      mNodeHeight = std::max( mNodeHeight, (int)(mTitleHeight + mLinkHeight) );
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

      int visibleCount = 0;
      unsigned int count = mOutputs.size();
      for (unsigned int index = 0; index < count; index++)
      {
         OutputData& data = mOutputs[index];
         if (!data.link || !data.link->GetVisible()) continue;

         visibleCount++;

         // Create the link graphic.
         data.linkGraphic->setPolygon(poly);
         data.linkGraphic->setPen(QPen(Qt::darkGray));
         data.linkGraphic->setBrush(GetNodeColor());
         data.linkGraphic->SetPenColor(Qt::darkGray);

         // Set the link text, and position it right aligned with the link graphic.
         data.linkName->setPlainText(data.link->GetName().c_str());
         QRectF nameBounds = data.linkName->boundingRect();

         if (maxSize < nameBounds.width()) maxSize = nameBounds.width();

         float x = -nameBounds.width();
         float y = (LINK_SIZE/2) - (nameBounds.height()/2);
         data.linkName->setPos(x, y);
      }

      // Resize the node width if we have to.
      int desiredWidth = maxSize + mLinkWidth;
      mNodeWidth = std::max( mNodeWidth, desiredWidth );

      // Reposition the link divider if the node is already larger than we need.
      if (mNodeWidth > desiredWidth)
      {
         mLinkWidth += ((mNodeWidth - maxSize - mLinkWidth) / 2);
      }

      // Resize the node height if we have to.

      // Now position all of the links in a single column.
      count = mOutputs.size();
      float posY = mTitleHeight + (count ? LINK_PADDING : 0);
      for (unsigned int index = 0; index < count; index++)
      {
         OutputData& data = mOutputs[index];
         if (!data.link || !data.link->GetVisible()) continue;

         QRectF nameBounds = data.linkName->boundingRect();
         data.linkGraphic->setPos(mNodeWidth, posY);

         posY += nameBounds.height();
      }
      mLinkHeight = std::max( mLinkHeight, posY - mTitleHeight );
      mNodeHeight = std::max( mNodeHeight, (int)(mTitleHeight + mLinkHeight) );
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::SetupValues()
   {
      float maxWidth = MIN_VALUE_NAME_SIZE;

      // Mutator nodes do not have a minimum width.
      if (mNode.valid() &&
         mNode->GetType().GetNodeType() == dtDirector::NodeType::MUTATOR_NODE)
      {
         maxWidth = 0.0f;
      }

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

         QColor color = GetValueLinkColor(data.link);

         if (mNode.valid() && !mNode->IsEnabled())
         {
            color.setAlphaF(0.25f);
         }

         data.linkGraphic->setPolygon(poly);
         //if (data.link->AllowMultiple())
         //{
         //   data.linkGraphic->SetPenColor(Qt::black);
         //}
         //else
         {
            int red = color.red() * 0.75f;
            int green = color.green() * 0.75f;
            int blue = color.blue() * 0.75f;
            int alpha = color.alpha();
            data.linkGraphic->SetPenColor(QColor(red, green, blue, alpha));
         }
         data.linkGraphic->SetHighlight(false);
         data.linkGraphic->setBrush(color);
         data.linkGraphic->SetPropertyType(data.link->GetPropertyType().GetTypeId());

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
      float desiredWidth = maxWidth * visibleCount;
      if (mNodeWidth < desiredWidth) mNodeWidth = desiredWidth;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawValues()
   {
      // Resize the node height if we have to.
      float desiredHeight = mTitleHeight + mLinkHeight + mValueHeight;
      if (mNodeHeight < desiredHeight) mNodeHeight = desiredHeight;

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
         float y = data.linkName->y();
         data.linkName->setTextWidth(step);
         data.linkName->setPos(x, y);

         data.linkName->setHtml(data.linkName->toHtml());
         if (!mValues[index].link || !mValues[index].link->GetVisible()) continue;
         x = step * (visibleCount + 1) - (step / 2);
         y = mNodeHeight;

         data.linkGraphic->setPos(x, y);

         visibleCount++;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawDividers()
   {
      QPen pen = QPen(LINE_COLOR, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

      if (!mTitleDivider) mTitleDivider = new QGraphicsRectItem(this, scene());
      mTitleDivider->setPos(1, mTitleHeight);
      mTitleDivider->setRect(0, 0, mNodeWidth - 2, 0);
      mTitleDivider->setPen(pen);

      // Draw the vertical divider if we are displaying both inputs and outputs.
      if (!mInputs.empty() && !mOutputs.empty())
      {
         float x = mLinkWidth;
         float y = mTitleHeight + 1;
         float height = mLinkHeight - 2;

         if (!mLinkDivider) mLinkDivider = new QGraphicsRectItem(this, scene());
         mLinkDivider->setPos(x, y);
         mLinkDivider->setRect(0, 0, 0, height);
         mLinkDivider->setPen(pen);
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
         mValueDivider->setPos(1, mTitleHeight + mLinkHeight);
         mValueDivider->setRect(1, 0, mNodeWidth - 2, 0);
         mValueDivider->setPen(pen);
      }
      else if (mValueDivider)
      {
         delete mValueDivider;
         mValueDivider = NULL;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawComment()
   {
      mComment->setTextWidth(dtUtil::Max(100, mNodeWidth));

      // Create the title background.
      QRectF commentBounds = mComment->boundingRect();
      QRectF nodeBounds = boundingRect();

      mComment->setPos(nodeBounds.x(), -commentBounds.height());
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::DrawGlow()
   {
      if (mScene && mNode.valid())
      {
         EditorNotifier* notifier = mScene->GetEditor()->GetNotifier();
         if (notifier)
         {
            EditorNotifier::GlowData* glowData =
               notifier->GetGlowData(mNode.get());
            if (glowData)
            {
               mNodePen = pen();
               if (glowData->glow > 0.5f)
               {
                  SetHighlight(1.0f);
               }
               else
               {
                  SetHighlight(glowData->glow * 2.0f);
               }

               if (glowData->input > -1 && glowData->input < (int)mInputs.size())
               {
                  InputData& data = mInputs[glowData->input];
                  data.DrawGlow(glowData->inputGlow);
               }

               int outputCount = (int)glowData->outputGlows.size();
               for (int outputIndex = 0; outputIndex < outputCount; ++outputIndex)
               {
                  if (outputIndex >= (int)mOutputs.size())
                  {
                     break;
                  }

                  OutputData& data = mOutputs[outputIndex];
                  data.DrawGlow(glowData->outputGlows[outputIndex]);
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool NodeItem::HasNode(Node* node)
   {
      return node == mNode.get();
   }

   //////////////////////////////////////////////////////////////////////////
   osg::Vec2 NodeItem::GetPosition()
   {
      return mNode.valid() ? mNode->GetPosition() : osg::Vec2(0,0);
   }

   //////////////////////////////////////////////////////////////////////////
   dtDirector::ID NodeItem::GetID()
   {
      if (mNode.valid())
      {
         return mNode->GetID();
      }

      return dtDirector::ID();
   }

   //////////////////////////////////////////////////////////////////////////
   bool NodeItem::HasID(const dtDirector::ID& id)
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
            std::vector<InputLink> removedLinks = output.link->GetOwner()->GetRemovedImportedOutputLinkConnections(output.link->GetName());
            output.ResizeLinks(linkCount + (int)removedLinks.size(), mScene);
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
                     ConnectLinks(output, item->mInputs[inputIndex], linkIndex, true);
                     item->mInputs[inputIndex].linkGraphic->SetHighlight(false);
                     break;
                  }
               }
            }
            for (int linkIndex = 0; linkIndex < (int)removedLinks.size(); linkIndex++)
            {
               InputLink& link = removedLinks[linkIndex];

               NodeItem* item = mScene->GetNodeItem(link.GetOwner()->GetID());
               if (!item) continue;

               int inputCount = (int)item->mInputs.size();
               for (int inputIndex = 0; inputIndex < inputCount; inputIndex++)
               {
                  if (item->mInputs[inputIndex].link->GetOwner()->GetID() == link.GetOwner()->GetID() &&
                      item->mInputs[inputIndex].link->GetName() == link.GetName())
                  {
                     ConnectLinks(output, item->mInputs[inputIndex], linkIndex + linkCount, true);
                     item->mInputs[inputIndex].linkGraphic->SetHighlight(false);
                     break;
                  }
               }
            }

            output.linkGraphic->SetHighlight(false);
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
                        std::vector<InputLink> removedLinks = output->GetOwner()->GetRemovedImportedOutputLinkConnections(output->GetName());
                        outputData.ResizeLinks(count + (int)removedLinks.size(), mScene);
                        for (int index = 0; index < count; index++)
                        {
                           if (output->GetLinks()[index] == input.link)
                           {
                              ConnectLinks(item->mOutputs[outputIndex], input, index, false);
                              break;
                           }
                        }
                        for (int index = 0; index < (int)removedLinks.size(); index++)
                        {
                           InputLink& link = removedLinks[index];

                           if (link.GetOwner()->GetID() == input.link->GetOwner()->GetID() &&
                               link.GetName() == input.link->GetName())
                           {
                              ConnectLinks(item->mOutputs[outputIndex], input, index + count, false);
                              break;
                           }
                        }
                        break;
                     }

                     outputData.linkGraphic->SetHighlight(false);
                  }
               }
            }

            input.linkGraphic->SetHighlight(false);
         }
      }

      count = (int)mValues.size();
      for (int valueIndex = 0; valueIndex < count; valueIndex++)
      {
         ValueData& value = mValues[valueIndex];
         if (value.link)
         {
            int linkCount = (int)value.link->GetLinks().size();
            std::vector<ValueNode*> removedLinks = value.link->GetOwner()->GetRemovedImportedValueLinkConnections(value.link->GetName());
            value.ResizeLinks(linkCount + (int)removedLinks.size(), mScene);
            for (int linkIndex = 0; linkIndex < linkCount; linkIndex++)
            {
               ValueNode* valueNode = value.link->GetLinks()[linkIndex];
               if (!valueNode) continue;

               NodeItem* item = mScene->GetNodeItem(valueNode->GetID());
               if (!item) continue;

               ConnectLinks(value, item, linkIndex);
            }
            for (int linkIndex = 0; linkIndex < (int)removedLinks.size(); ++linkIndex)
            {
               ValueNode* valueNode = removedLinks[linkIndex];
               if (!valueNode) continue;

               NodeItem* item = mScene->GetNodeItem(valueNode->GetID());
               if (!item) continue;

               ConnectLinks(value, item, linkIndex + linkCount);
            }
         }

         value.linkGraphic->SetHighlight(false);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   QPainterPath NodeItem::CreateConnectionH(QPointF start, QPointF end, NodeItem* ownerNode, NodeItem* targetNode, bool drawReverse, bool building)
   {
      // Modify the positions based on the translation of the background item.
      QPointF offset;
      offset = mScene->GetTranslationItem()->scenePos();

      if (!building)
      {
         start -= offset;
         end -= offset;
      }

      float height = 0.0f;
      float topNodeBottom = 0.0f;
      float bottomNodeTop = 0.0f;
      if (!targetNode || ownerNode->scenePos().y() < targetNode->scenePos().y())
      {
         height = ownerNode->scenePos().y();
         if (!building)
         {
            height -= offset.y();
         }

         topNodeBottom = height + ownerNode->GetNodeHeight();

         if (targetNode)
         {
            bottomNodeTop = targetNode->scenePos().y();
            if (!building)
            {
               bottomNodeTop -= offset.y();
            }
         }
         else if (start.y() > end.y())
         {
            bottomNodeTop = start.y();
         }
         else
         {
            bottomNodeTop = end.y();
         }
      }
      else
      {
         height = targetNode->scenePos().y();
         if (!building)
         {
            height -= offset.y();
         }

         topNodeBottom = height + targetNode->GetNodeHeight();

         bottomNodeTop = ownerNode->scenePos().y();
         if (!building)
         {
            bottomNodeTop -= offset.y();
         }
      }

      QPainterPath path;

      if (start.x() < end.x())
      {
         float halfX = (start.x() + end.x()) / 2.0f;

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
         float reverseMul = 1.0f;
         if (drawReverse)
         {
            QPointF temp = start;
            start = end;
            end = temp;
            reverseMul = -1.0f;
         }

         // If the links are far enough apart in height, draw an "S" connection.
         if (bottomNodeTop - topNodeBottom > MIN_NODE_HEIGHT)
         {
            //float halfX = (start.x() + end.x()) / 2.0f;
            float halfY = (bottomNodeTop + topNodeBottom) / 2.0f;

            float rightX = 0.0f;
            float leftX = 0.0f;

            if (start.y() > end.y())
            {
               rightX = start.x() + (start.y() - halfY) * reverseMul;
               leftX = end.x() + (end.y() - halfY) * reverseMul;
            }
            else
            {
               rightX = start.x() - (start.y() - halfY) * reverseMul;
               leftX = end.x() - (end.y() - halfY) * reverseMul;
            }

            path.moveTo(start);
            path.cubicTo(rightX, start.y(), rightX, halfY, start.x(), halfY);
            path.lineTo(end.x(), halfY);
            path.cubicTo(leftX, halfY, leftX, end.y(), end.x(), end.y());
         }
         else
         {
            float top = height;
            float yOffset = std::abs(start.y() - end.y()) / 4.0f + 25;
            if (start.y() < top)
            {
               top = start.y();
            }
            else if (end.y() < top)
            {
               top = end.y();
            }
            top -= yOffset;

            float len = std::abs(start.x() - end.x());

            float higherPoint = dtUtil::Min<float>(start.y(), end.y());
            if (len < higherPoint - top)
            {
               top = higherPoint - len;
            }

            float ctrlXStart = start.x() + dtUtil::Min<float>((start.y() - top) * 0.5f, len) * reverseMul;
            float ctrlXEnd = end.x() - dtUtil::Min<float>((end.y() - top) * 0.5f, len) * reverseMul;

            path.moveTo(start);
            path.cubicTo(ctrlXStart, start.y(), ctrlXStart, top, start.x(), top);
            path.lineTo(end.x(), top);
            path.cubicTo(ctrlXEnd, top, ctrlXEnd, end.y(), end.x(), end.y());
         }
      }

      return path;
   }

   //////////////////////////////////////////////////////////////////////////
   QPainterPath NodeItem::CreateConnectionV(QPointF start, QPointF end, bool drawReverse, bool connectingTwoValueLinks)
   {
      // Modify the positions based on the translation of the background item.
      QPointF offset = mScene->GetTranslationItem()->scenePos();
      start -= offset;
      end -= offset;

      float halfY = (start.y() + end.y()) / 2.0f;

      QPainterPath path;
      // Draw a link from a Value Link to a Value Node.
      if (!connectingTwoValueLinks)
      {
         // If the Value Link is above the Value Node, draw a normal
         // 'S' curve between them.
         if (start.y() < end.y())
         {
            // Sometimes we need to draw in reverse so the connection line looks like
            // it is "pulling" out from our starting point.
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
         // If the Value Link is below the Value Node, we need to
         // draw a more complex curve.
         else
         {
            // Sometimes we need to draw in reverse so the connection line looks like
            // it is "pulling" out from our starting point.
            if (drawReverse)
            {
               QPointF temp = start;
               start = end;
               end = temp;
            }

            // Calculate some guiding parameters for this line.
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
      }
      // If we are connecting two Value Links together, we want to draw
      // a 'U' shaped curve between them.
      else
      {
         // Sometimes we need to draw in reverse so the connection line looks like
         // it is "pulling" out from our starting point.
         if (drawReverse)
         {
            QPointF temp = start;
            start = end;
            end = temp;
         }

         // Calculate some guiding parameters for this line.
         float bottomY = start.y();
         if (end.y() > bottomY)
         {
            bottomY = end.y();
         }
         bottomY += 50.0f;
         float halfX = (start.x() + end.x()) / 2.0f;
         float outside = 20.0f;
         if (start.x() < end.x())
         {
            outside = -20.0f;
         }

         path.moveTo(start);
         path.cubicTo(
            start.x(), bottomY - 20.0f,
            start.x() + outside, bottomY,
            halfX, bottomY);
         path.cubicTo(
            end.x() - outside, bottomY,
            end.x(), bottomY - 20.0f,
            end.x(), end.y());
      }
      return path;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::BeginMoveEvent()
   {
      if (!mNode.valid())
      {
         return;
      }

      dtCore::ActorProperty* prop = mNode->GetProperty("Position");
      if (prop) mOldPosition = prop->ToString();

      mChainSelecting = mScene->IsHoldingShift();
      if (mChainSelecting)
      {
         mScene->BeginBatchSelection();
         int outCount = (int)mOutputs.size();
         for (int outIndex = 0; outIndex < outCount; ++outIndex)
         {
            OutputData& data = mOutputs[outIndex];
            std::vector<InputLink*>& links = data.link->GetLinks();
            int linkCount = (int)links.size();
            for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               InputLink* link = links[linkIndex];
               if (link)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
                  if (item && !item->isSelected())
                  {
                     item->setSelected(true);
                     item->BeginMoveEvent();
                  }
               }
            }
         }
         int valCount = (int)mValues.size();
         for (int valIndex = 0; valIndex < valCount; ++valIndex)
         {
            ValueData& data = mValues[valIndex];
            const std::vector<ValueNode*>& nodes = data.link->GetLinks();
            int nodeCount = (int)nodes.size();
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
               ValueNode* node = nodes[nodeIndex];
               if (node)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(node->GetID());
                  if (item && !item->isSelected())
                  {
                     item->setSelected(true);
                     item->BeginMoveEvent();
                  }
               }
            }
         }
         mScene->EndBatchSelection();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::EndMoveEvent()
   {
      if (!mNode.valid())
      {
         return;
      }

      dtCore::ActorProperty* prop = mNode->GetProperty("Position");
      if (prop)
      {
         std::string value = prop->ToString();

         // Ignore the property if the node did not move.
         if (value == mOldPosition) return;

         // Notify the undo manager of the property changes.
         dtCore::RefPtr<UndoPropertyEvent> event = new UndoPropertyEvent(mScene->GetEditor(), mNode->GetID(), prop->GetName(), mOldPosition, value);
         event->SetDescription("Movement of Node \'" + mNode->GetTypeName() + "\'.");
         mScene->GetEditor()->GetUndoManager()->AddEvent(event.get());
      }

      if (mChainSelecting)
      {
         mScene->BeginBatchSelection();
         int outCount = (int)mOutputs.size();
         for (int outIndex = 0; outIndex < outCount; ++outIndex)
         {
            OutputData& data = mOutputs[outIndex];
            std::vector<InputLink*>& links = data.link->GetLinks();
            int linkCount = (int)links.size();
            for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
            {
               InputLink* link = links[linkIndex];
               if (link)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(link->GetOwner()->GetID());
                  if (item && item->isSelected())
                  {
                     item->setSelected(false);
                     item->EndMoveEvent();
                  }
               }
            }
         }
         int valCount = (int)mValues.size();
         for (int valIndex = 0; valIndex < valCount; ++valIndex)
         {
            ValueData& data = mValues[valIndex];
            const std::vector<ValueNode*>& nodes = data.link->GetLinks();
            int nodeCount = (int)nodes.size();
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
               ValueNode* node = nodes[nodeIndex];
               if (node)
               {
                  dtDirector::NodeItem* item = mScene->GetNodeItem(node->GetID());
                  if (item && item->isSelected())
                  {
                     item->setSelected(false);
                     item->EndMoveEvent();
                  }
               }
            }
         }
         mScene->EndBatchSelection();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeItem::ExposeLink(QAction* action)
   {
      if (!action || !mNode.valid())
      {
         return;
      }

      ValueLink* link = mNode->GetValueLink(action->text().toStdString());
      if (link)
      {
         if (!link->GetExposed())
         {
            link->SetExposed(true);
            link->SetVisible(true);

            dtCore::RefPtr<UndoLinkVisibilityEvent> event = new UndoLinkVisibilityEvent(mScene->GetEditor(), GetID(), 2, link->GetName(), true, true);
            event->SetDescription("Exposing of value link \'" + link->GetName() + "\' for Node \'" + mNode->GetTypeName() + "\'.");
            mScene->GetEditor()->GetUndoManager()->AddEvent(event);

            mScene->Refresh();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeItem::OnEventTriggered(QAction* action)
   {
      if (action && mNode.valid() && mNode->AsEventNode())
      {
         OutputLink* link = mNode->GetOutputLink(action->text().toStdString());
         if (link)
         {
            link->Activate();

            mScene->GetEditor()->GetDirector()->BeginThread(mNode.get(), 0, false, false);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeItem::OnToggleBreakPoint()
   {
      EditorNotifier* notifier = mScene->GetEditor()->GetNotifier();
      if (notifier)
      {
         notifier->ToggleBreakPoint(mNode.get());
         mScene->GetEditor()->RefreshNodeItem(mNode.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::ConnectLinks(OutputData& output, InputData& input, int index, bool isOutput)
   {
      if (index < 0 || (int)output.linkConnectors.size() < index) return;

      QPointF start(output.linkGraphic->scenePos());
      QPointF end(input.linkGraphic->scenePos());

      start.setX(start.x() + LINK_LENGTH);
      start.setY(start.y() + LINK_SIZE/2);
      end.setX(end.x() + LINK_SIZE/2);
      end.setY(end.y() + LINK_SIZE/2);

      QPainterPath path = CreateConnectionH(start, end, output.node, input.node);
      output.linkConnectors[index]->setPath(path);
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

   //////////////////////////////////////////////////////////////////////////
   QColor NodeItem::GetNodeColor() const
   {
      QColor color;

      if (mNode.valid())
      {
         osg::Vec4 rgba = mNode->GetColor();
         color.setRgbF(rgba.r(), rgba.g(), rgba.b(), rgba.a());

         if( !mNode->IsEnabled())
         {
            color.setAlphaF(0.25f);
         }
      }

      if (mIsReadOnly)
      {
         color = color.light(150);
      }

      return color;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QColor NodeItem::GetValueLinkColor(ValueLink* link) const
   {
      QColor color;

      if (!link)
      {
         return color;
      }

      dtCore::DataType& dataType = link->GetPropertyType();
      if (dataType == dtCore::DataType::UNKNOWN)
      {
         osg::Vec4 ownerColor = link->GetOwner()->GetColor();
         color = QColor(ownerColor[0]*255, ownerColor[1]*255, ownerColor[2]*255, ownerColor[3]*255);
         return color;
      }

      const dtDirector::NodeType* type = NodeManager::GetInstance().FindNodeType(dataType);

      if (type)
      {
         color = QColor(type->GetColor()[0], type->GetColor()[1], type->GetColor()[2], 225);
         if(mNode.valid() && !mNode->IsEnabled())
         {
            color.setAlphaF(0.25f);
         }
      }

      return color;
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::SetBackgroundGradient()
   {
      QColor nodeColor = GetNodeColor();
      QColor colorLight = nodeColor.light(150);
      QColor colorDark = nodeColor.dark(100);

      float height = mNodeHeight;
      //if (mIsReadOnly)
      //{
      //   height = mNodeHeight * 0.5f;
      //   colorDark.setAlphaF(0.25f);
      //}

      QLinearGradient linearGradient(0, 0, 0, height);
      linearGradient.setSpread(QGradient::ReflectSpread);

      linearGradient.setColorAt(0.0, colorLight);
      linearGradient.setColorAt(1.0, colorDark);

      setBrush(linearGradient);
   }

   //////////////////////////////////////////////////////////////////////////
   void NodeItem::SetDefaultPen()
   {
      QColor lineColor = LINE_COLOR;
      if (mIsReadOnly)
      {
         lineColor.setAlphaF(lineColor.alphaF() * 0.5f);
         lineColor = lineColor.light(150);
      }

      setPen(QPen(lineColor, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
      if(!mNode || !mNode->IsEnabled())
      {
         mNodePen = pen();
         mPenColor = mNodePen.color();
         return;
      }

      if (mNode->GetNodeLogging())
      {
         setPen(QPen(Qt::white, 3, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
      }

      if (mScene && mScene->GetEditor()->GetNotifier() &&
         mScene->GetEditor()->GetNotifier()->ShouldBreak(mNode.get()))
      {
         setPen(QPen(Qt::red, 3, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
      }

      ValueNode* valueNode = mNode->AsValueNode();
      if (valueNode && valueNode->GetGlobal())
      {
         setPen(QPen(Qt::darkGreen, 3, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
      }

      mNodePen = pen();
      mPenColor = mNodePen.color();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void NodeItem::SetHighlight(float alpha)
   {
      QColor baseColor = mPenColor;
      QColor newColor = Qt::yellow;

      newColor.setRedF((newColor.redF() * alpha) + (baseColor.redF() * (1.0f - alpha)));
      newColor.setGreenF((newColor.greenF() * alpha) + (baseColor.greenF() * (1.0f - alpha)));
      newColor.setBlueF((newColor.blueF() * alpha) + (baseColor.blueF() * (1.0f - alpha)));
      newColor.setAlphaF((newColor.alphaF() * alpha) + (baseColor.alphaF() * (1.0f - alpha)));

      mNodePen.setColor(newColor);
      setPen(mNodePen);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QMenu* NodeItem::GetExposeLinkMenu()
   {
      if (!mNode.valid())
      {
         return NULL;
      }

      QMenu* menu = new QMenu("Expose Value");
      menu->setDisabled(true);
      connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(ExposeLink(QAction*)));

      std::vector<ValueLink> &values = mNode->GetValueLinks();
      int count = (int)values.size();
      for (int index = 0; index < count; index++)
      {
         ValueLink& link = values[index];
         if (!link.GetExposed())
         {
            menu->addAction(link.GetName().c_str());
            menu->setEnabled(true);
         }
      }

      return menu;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QMenu* NodeItem::GetLinkVisibilityMenu()
   {
      QMenu* menu = new QMenu("Link Visibility");

      menu->addAction(mScene->GetEditor()->GetShowLinkAction());
      menu->addAction(mScene->GetEditor()->GetHideLinkAction());
      menu->addSeparator();
      menu->addMenu(GetShowInputMenu());
      menu->addMenu(GetShowOutputMenu());
      menu->addMenu(GetShowValueMenu());

      return menu;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QMenu* NodeItem::GetShowInputMenu()
   {
      QMenu* menu = new QMenu("Input Links");
      menu->setDisabled(true);

      int count = (int)mInputs.size();
      for (int index = 0; index < count; ++index)
      {
         InputData& data = mInputs[index];
         if (!data.link->GetVisible())
         {
            QAction* action = menu->addAction(data.link->GetName().c_str());
            if (action)
            {
               connect(action, SIGNAL(triggered()), data.linkGraphic, SLOT(ShowLink()));
               menu->setEnabled(true);
            }
         }
      }

      return menu;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QMenu* NodeItem::GetShowOutputMenu()
   {
      QMenu* menu = new QMenu("Output Links");
      menu->setDisabled(true);

      int count = (int)mOutputs.size();
      for (int index = 0; index < count; ++index)
      {
         OutputData& data = mOutputs[index];
         if (!data.link->GetVisible())
         {
            QAction* action = menu->addAction(data.link->GetName().c_str());
            if (action)
            {
               connect(action, SIGNAL(triggered()), data.linkGraphic, SLOT(ShowLink()));
               menu->setEnabled(true);
            }
         }
      }

      return menu;
   }

   ////////////////////////////////////////////////////////////////////////////////
   QMenu* NodeItem::GetShowValueMenu()
   {
      QMenu* menu = new QMenu("Value Links");
      menu->setDisabled(true);

      int count = (int)mValues.size();
      for (int index = 0; index < count; ++index)
      {
         ValueData& data = mValues[index];
         if (!data.link->GetVisible())
         {
            QAction* action = menu->addAction(data.link->GetName().c_str());
            if (action)
            {
               connect(action, SIGNAL(triggered()), data.linkGraphic, SLOT(ShowLink()));
               menu->setEnabled(true);
            }
         }
      }

      return menu;
   }
}

//////////////////////////////////////////////////////////////////////////
