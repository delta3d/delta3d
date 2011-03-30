#include <AnimationEvent.h>

#include <QtGui/QPainter>

///////////////////////////////////////////////////////////////////////////////
static const QColor DEFAULT_COLOR(125, 0, 0, 155);

///////////////////////////////////////////////////////////////////////////////
AnimationEvent::AnimationEvent(int newStartTime, int newEndTime)
: BaseEvent(newStartTime, newEndTime)
, mBlendIn(250)
, mBlendOut(250)
, mWeight(1.0f)
{
   int dur = newEndTime - newStartTime;
   if (dur < 500)
   {
      mBlendIn = mBlendOut = dur / 2;
   }
   SetDefaultColor(DEFAULT_COLOR);
}

///////////////////////////////////////////////////////////////////////////////
AnimationEvent::~AnimationEvent()
{
}

////////////////////////////////////////////////////////////////////////////////
bool AnimationEvent::IsPointOnLeftResizer(QPoint position) const
{
   return false;
}

////////////////////////////////////////////////////////////////////////////////
bool AnimationEvent::IsPointOnEvent(QPoint position) const
{
   QRect leftResizer = GetBoundingBox();
   leftResizer.setLeft(leftResizer.left() - 2);
   leftResizer.setRight(leftResizer.right() + 5);

   return leftResizer.contains(position);
}

////////////////////////////////////////////////////////////////////////////////
bool AnimationEvent::IsPointOnRightResizer(QPoint position) const
{
   return false;
}

///////////////////////////////////////////////////////////////////////////////
void AnimationEvent::HighlightEvent(bool highlighted)
{
   if (highlighted)
   {
      SetFrameColor(QColor(0, 125, 255, 155));
      HighlightEventLeftResizer(false);
      HighlightEventRightResizer(false);
   }
   else
   {
      SetFrameColor(QColor(0, 0, 0, 155));
   }
}

///////////////////////////////////////////////////////////////////////////////
void AnimationEvent::SelectEvent(bool selected)
{
   BaseEvent::SelectEvent(selected);
   if (IsSelected())
   {
      SetColor(QColor(0, 125, 255, 155));
   }
   else
   {
      SetColor(GetDefaultColor());
   }
}

///////////////////////////////////////////////////////////////////////////////
void AnimationEvent::PaintEvent(QPainter* painter)
{
   QRect boundingBox = GetBoundingBox();

   float stepSize = 0.0f;
   float dur = float(GetEndTime() - GetStartTime());
   if (dur > 0.0f)
   {
      stepSize = boundingBox.width() / dur;
   }

   // Draw our animation bar.
   {
      painter->setBrush(GetColor());
      painter->setPen(GetColor());

      QPolygon poly;
      poly.push_back(QPoint(boundingBox.left(), boundingBox.bottom()));
      poly.push_back(QPoint(boundingBox.left() + (mBlendIn * stepSize), boundingBox.bottom() * (1.0f - mWeight)));
      poly.push_back(QPoint(boundingBox.right() - (mBlendOut * stepSize), boundingBox.bottom() * (1.0f - mWeight)));
      poly.push_back(QPoint(boundingBox.right(), boundingBox.bottom()));
      painter->drawPolygon(poly);
   }
}

///////////////////////////////////////////////////////////////////////////////

