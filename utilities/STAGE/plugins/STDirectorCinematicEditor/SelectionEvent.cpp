#include <SelectionEvent.h>

#include <QtGui/QPainter>

///////////////////////////////////////////////////////////////////////////////
SelectionEvent::SelectionEvent(int newStartTime /*= 0*/, int newEndTime /*= 1000000*/)
: BaseEvent(newStartTime, newEndTime)
{
}

///////////////////////////////////////////////////////////////////////////////
SelectionEvent::~SelectionEvent()
{
}

///////////////////////////////////////////////////////////////////////////////
void SelectionEvent::PaintEvent(QPainter* painter)
{
   QRect boundingBox = GetBoundingBox();
   // Draw left resize box
   painter->setBrush(GetLeftResizerColor());
   painter->setPen(GetLeftResizerColor());
   painter->drawRect(boundingBox.left() - (BaseEvent::RESIZE_END_WIDTH + BaseEvent::FRAME_WIDTH),
      boundingBox.top(), BaseEvent::RESIZE_END_WIDTH + BaseEvent::FRAME_WIDTH, boundingBox.height());

   // Draw right resize box
   painter->setBrush(GetRightResizerColor());
   painter->setPen(GetRightResizerColor());
   painter->drawRect(boundingBox.right() + 1, boundingBox.top(),
      BaseEvent::RESIZE_END_WIDTH + BaseEvent::FRAME_WIDTH, boundingBox.height());
}

///////////////////////////////////////////////////////////////////////////////

