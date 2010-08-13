#include <BaseEvent.h>

#include <QtGui/QPainter>

#include <fstream>

///////////////////////////////////////////////////////////////////////////////
static const QColor DEFAULT_COLOR(125, 125, 125, 255);
static const QColor DEFAULT_FRAME_COLOR(0, 0, 0, 255);
static const QColor HIGHLIGHT_COLOR(0, 125, 255, 255);

///////////////////////////////////////////////////////////////////////////////
BaseEvent::BaseEvent(int newStartTime /*= 0*/, int newEndTime /*= 1000000*/)
: mStartTime(newStartTime)
, mEndTime(newEndTime)
, mColor(DEFAULT_COLOR)
, mLeftResizerColor(DEFAULT_FRAME_COLOR)
, mRightResizerColor(DEFAULT_FRAME_COLOR)
, mFrameColor(DEFAULT_FRAME_COLOR)
, mDefaultColor(DEFAULT_COLOR)
, mIsSelected(false)
, mIsMovable(true)
{
}

///////////////////////////////////////////////////////////////////////////////
BaseEvent::~BaseEvent()
{
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::SetStartTime(int value)
{
   mStartTime = std::min(value, mEndTime);
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::SetEndTime(int value)
{
   mEndTime = std::max(value, mStartTime);
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::SetNewTimes(int newStartTime, int newEndTime)
{
   mStartTime = newStartTime;
   mEndTime = std::max(newEndTime, mStartTime);
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::SetDefaultColor(QColor newColor)
{
   mDefaultColor = newColor;
   mColor = mDefaultColor;
}

///////////////////////////////////////////////////////////////////////////////
bool BaseEvent::IsPointOnLeftResizer(QPoint position) const
{
   QRect leftResizer = mBoundingBox;
   leftResizer.setLeft(leftResizer.left() - (FRAME_WIDTH + RESIZE_END_WIDTH));
   leftResizer.setRight(leftResizer.left() + RESIZE_END_WIDTH);

   return leftResizer.contains(position);
}

///////////////////////////////////////////////////////////////////////////////
bool BaseEvent::IsPointOnEvent(QPoint position) const
{
   QRect box = mBoundingBox;
   box.setLeft(box.left() - FRAME_WIDTH);
   box.setRight(box.right() + FRAME_WIDTH);

   return box.contains(position);
}

///////////////////////////////////////////////////////////////////////////////
bool BaseEvent::IsPointOnRightResizer(QPoint position) const
{
   QRect rightResizer = mBoundingBox;
   rightResizer.setLeft(rightResizer.right() + (FRAME_WIDTH * 2) - 1);
   rightResizer.setRight(rightResizer.left() + RESIZE_END_WIDTH);

   return rightResizer.contains(position);
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::ClearEventHighlights()
{
   HighlightEventLeftResizer(false);
   HighlightEventRightResizer(false);
   HighlightEvent(false);
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::HighlightEventLeftResizer(bool highlighted)
{
   if (highlighted)
   {
      mLeftResizerColor = HIGHLIGHT_COLOR;
      HighlightEventRightResizer(false);
      HighlightEvent(false);
   }
   else
   {
      mLeftResizerColor = DEFAULT_FRAME_COLOR;
   }
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::HighlightEvent(bool highlighted)
{
   if (highlighted)
   {
      mFrameColor = HIGHLIGHT_COLOR;
      HighlightEventLeftResizer(false);
      HighlightEventRightResizer(false);
   }
   else
   {
      mFrameColor = DEFAULT_FRAME_COLOR;
   }
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::HighlightEventRightResizer(bool highlighted)
{
   if (highlighted)
   {
      mRightResizerColor = HIGHLIGHT_COLOR;
      HighlightEventLeftResizer(false);
      HighlightEvent(false);
   }
   else
   {
      mRightResizerColor = DEFAULT_FRAME_COLOR;
   }
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::SelectEvent(bool selected)
{
   mIsSelected = selected;
   if (mIsSelected)
   {
      mColor = HIGHLIGHT_COLOR;
   }
   else
   {
      mColor = mDefaultColor;
   }
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::PaintEvent(QPainter* painter)
{
   // Draw left resize box
   painter->setBrush(mLeftResizerColor);
   painter->setPen(mLeftResizerColor);
   painter->drawRect(mBoundingBox.left() - (FRAME_WIDTH + RESIZE_END_WIDTH),
      mBoundingBox.top(), RESIZE_END_WIDTH, mBoundingBox.height());

   // Draw right resize box
   painter->setBrush(mRightResizerColor);
   painter->setPen(mRightResizerColor);
   painter->drawRect(mBoundingBox.right() + (FRAME_WIDTH * 2) - 1, mBoundingBox.top(),
      RESIZE_END_WIDTH, mBoundingBox.height());

   // Draw frame
   painter->setBrush(mFrameColor);
   painter->setPen(mFrameColor);
   painter->drawRect(mBoundingBox.left() - FRAME_WIDTH, mBoundingBox.top(),
      mBoundingBox.width() + (FRAME_WIDTH * 2), mBoundingBox.height());

   // Draw event box
   painter->setBrush(mColor);
   painter->setPen(mColor);
   painter->drawRect(mBoundingBox.left(), mBoundingBox.top() + FRAME_WIDTH,
      mBoundingBox.width(), mBoundingBox.height() - (FRAME_WIDTH * 2) - 1);
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::SaveEvent(std::ofstream* factFile)
{
   // Save times
   factFile->write(reinterpret_cast<char *>(&mStartTime), sizeof(int));
   factFile->write(reinterpret_cast<char *>(&mEndTime), sizeof(int));
}

///////////////////////////////////////////////////////////////////////////////
void BaseEvent::LoadEvent(std::ifstream* factFile)
{
   // Save times
   factFile->read(reinterpret_cast<char *>(&mStartTime), sizeof(int));
   factFile->read(reinterpret_cast<char *>(&mEndTime), sizeof(int));
}

///////////////////////////////////////////////////////////////////////////////

