#include <EventTrackWidget.h>

#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPalette>

#include <fstream>

///////////////////////////////////////////////////////////////////////////////
static const QColor SELECTION_COLOR(0, 125, 50, 125);

///////////////////////////////////////////////////////////////////////////////
EventTrackWidget::EventTrackWidget(QWidget* parent)
: QWidget(parent)
, mMinimum(0)
, mMaximum(0)
, mSelectionStart(0)
, mSelectionEnd(0)
, mCurrent(0)
, mSelectedEvent(NULL)
, mLeftResizeEvent(NULL)
, mMovingEvent(NULL)
, mRightResizeEvent(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////
EventTrackWidget::~EventTrackWidget()
{
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::SetMinimum(int value, bool resizeEvents /*= true*/)
{
   mMinimum = std::min(value, mMaximum);
   if (resizeEvents)
   {
      // Reset event start/end times
      for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
      {
         mEvents[eventIndex]->SetStartTime(std::max(mMinimum, mEvents[eventIndex]->GetStartTime()));
         mEvents[eventIndex]->SetEndTime(std::max(mMinimum + 1, mEvents[eventIndex]->GetEndTime()));
      }
   }
   ResizeEventBoundingBoxes();
   update();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::SetMaximum(int value, bool resizeEvents /*= true*/)
{
   mMaximum = std::max(value, mMinimum);
   if (resizeEvents)
   {
      // Reset event start/end times
      for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
      {
         mEvents[eventIndex]->SetStartTime(std::max(mMinimum,
            std::min(mMaximum - 1, mEvents[eventIndex]->GetStartTime())));
         mEvents[eventIndex]->SetEndTime(std::min(mMaximum, mEvents[eventIndex]->GetEndTime()));
      }
   }
   ResizeEventBoundingBoxes();
   update();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::SetSelectionTimes(int start, int end)
{
   // Make sure start and end are between min and max and end is greater than or equal to start
   start = std::max(mMinimum, start);
   start = std::min(start, mMaximum);
   end = std::max(start, end);
   end = std::min(end, mMaximum);
   mSelectionStart = start;
   mSelectionEnd = end;
   update();
}

///////////////////////////////////////////////////////////////////////////////
BaseEvent* EventTrackWidget::SetCurrent(int value)
{
   mCurrent = std::min(value, mMaximum);
   mCurrent = std::max(mCurrent, mMinimum);
   update();

   return GetEventAtTime(mCurrent);
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::AddEvent(BaseEvent* event)
{
   ComputeEventBoundingBox(event);

   // Add the event to the list
   mEvents.push_back(event);
   update();
}

////////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::RemoveEvent(BaseEvent* event)
{
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      if (mEvents[eventIndex] == event)
      {
         emit EventDeleted(mEvents[eventIndex]);
         mEvents.erase(mEvents.begin() + eventIndex);
         break;
      }
   }
   update();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::ClearEvents()
{
   mEvents.clear();
   update();
}

////////////////////////////////////////////////////////////////////////////////
int EventTrackWidget::GetEventIndex(BaseEvent* event) const
{
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      if (event == mEvents[eventIndex])
      {
         return eventIndex;
      }
   }

   return -1;
}

////////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::SelectEvent(BaseEvent* event)
{
   DeselectEvents();
   mSelectedEvent = event;

   if (event)
   {
      mSelectedEvent->SelectEvent(true);
      update();
   }
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::DeselectEvents()
{
   if (mSelectedEvent != NULL)
   {
      mSelectedEvent->SelectEvent(false);
      mSelectedEvent = NULL;
      update();
   }
}

////////////////////////////////////////////////////////////////////////////////
BaseEvent* EventTrackWidget::GetPreviousEvent() const
{
   BaseEvent* currentEvent = NULL;
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      if ((currentEvent == NULL && mEvents[eventIndex]->GetEndTime() < mCurrent) ||
         (currentEvent != NULL && mEvents[eventIndex]->GetEndTime() >= currentEvent->GetEndTime() &&
         mEvents[eventIndex]->GetEndTime() < mCurrent))
      {
         currentEvent = mEvents[eventIndex];
      }
   }

   return currentEvent;
}

////////////////////////////////////////////////////////////////////////////////
BaseEvent* EventTrackWidget::GetNextEvent() const
{
   BaseEvent* currentEvent = NULL;
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      if ((currentEvent == NULL && mCurrent < mEvents[eventIndex]->GetStartTime()) ||
         (currentEvent != NULL && mCurrent < mEvents[eventIndex]->GetStartTime() &&
         mEvents[eventIndex]->GetStartTime() < currentEvent->GetStartTime()))
      {
         currentEvent = mEvents[eventIndex];
      }
   }

   return currentEvent;
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::UpdateEventStart(int newTime)
{
   if (mSelectedEvent != NULL)
   {
      mSelectedEvent->SetStartTime(newTime);
      ResizeEventBoundingBoxes();
      update();
   }
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::UpdateEventEnd(int newTime)
{
   if (mSelectedEvent != NULL)
   {
      mSelectedEvent->SetEndTime(newTime);
      ResizeEventBoundingBoxes();
      update();
   }
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::MoveEventToNewStartTime(int newStartTime)
{
   if (mSelectedEvent != NULL)
   {
      int newEndTime = mSelectedEvent->GetEndTime() - mSelectedEvent->GetStartTime() + newStartTime;
      mSelectedEvent->SetNewTimes(newStartTime, newEndTime);
      ResizeEventBoundingBoxes();
      update();
   }
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::MoveEventToNewEndTime(int newEndTime)
{
   if (mSelectedEvent != NULL)
   {
      int newStartTime = newEndTime - (mSelectedEvent->GetEndTime() - mSelectedEvent->GetStartTime());
      mSelectedEvent->SetNewTimes(newStartTime, newEndTime);
      ResizeEventBoundingBoxes();
      update();
   }
}

////////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::ComputeEventBoundingBox(BaseEvent* event)
{
   QRect eventRect = contentsRect();
   eventRect.setLeft(eventRect.left() + BaseEvent::RESIZE_END_WIDTH);
   eventRect.setRight(eventRect.right() - BaseEvent::RESIZE_END_WIDTH - 3);
   float beginWidthPercent = float(event->GetStartTime() - mMinimum) / float(mMaximum - mMinimum);
   float endWidthPercent = float(event->GetEndTime() - mMinimum) / float(mMaximum - mMinimum);
   int leftX = eventRect.left() + beginWidthPercent * eventRect.width();
   int rightX = eventRect.left() + endWidthPercent * eventRect.width();
   eventRect.setLeft(leftX);
   eventRect.setRight(rightX);
   event->SetBoundingBox(eventRect);
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::SaveTrackEvents(std::ofstream* factFile)
{
   factFile->write(reinterpret_cast<char *>(&mMinimum), sizeof(int));
   factFile->write(reinterpret_cast<char *>(&mMaximum), sizeof(int));
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::LoadTrackEvents(std::ifstream* factFile)
{
   factFile->read(reinterpret_cast<char *>(&mMinimum), sizeof(int));
   factFile->read(reinterpret_cast<char *>(&mMaximum), sizeof(int));
   mEvents.clear();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::keyPressEvent(QKeyEvent* event)
{
   switch(event->key())
   {
   case Qt::Key_Backspace:
   case Qt::Key_Delete:
      for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
      {
         if (mEvents[eventIndex]->IsSelected() && mEvents[eventIndex]->IsMovable())
         {
            emit EventDeleted(mEvents[eventIndex]);
            mEvents.erase(mEvents.begin() + eventIndex);
            break;
         }
      }
      update();
      break;
   }
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::leaveEvent(QEvent* event)
{
   // Clear any highlighted colors
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      mEvents[eventIndex]->ClearEventHighlights();
   }
   update();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::mouseMoveEvent(QMouseEvent* event)
{
   BaseEvent* leftHighlightEvent = NULL;
   BaseEvent* highlightEvent = NULL;
   BaseEvent* rightHighlightEvent = NULL;

   // Highlight any event area we are over
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      if(mLeftResizeEvent == mEvents[eventIndex] || (mMovingEvent == NULL && mRightResizeEvent == NULL &&
         mEvents[eventIndex]->IsPointOnLeftResizer(event->pos())))
      {
         leftHighlightEvent = mEvents[eventIndex];
         highlightEvent = NULL;
         rightHighlightEvent = NULL;
      }
      else if(mMovingEvent == mEvents[eventIndex] || (mEvents[eventIndex]->IsPointOnEvent(event->pos()) &&
         mLeftResizeEvent == NULL && mRightResizeEvent == NULL))
      {
         highlightEvent = mEvents[eventIndex];
         leftHighlightEvent = NULL;
         rightHighlightEvent = NULL;
      }
      else if(mRightResizeEvent == mEvents[eventIndex] || (mLeftResizeEvent == NULL && mMovingEvent == NULL &&
         mEvents[eventIndex]->IsPointOnRightResizer(event->pos())))
      {
         rightHighlightEvent = mEvents[eventIndex];
         leftHighlightEvent = NULL;
         highlightEvent = NULL;
      }

      mEvents[eventIndex]->ClearEventHighlights();
   }

   if (leftHighlightEvent != NULL)
   {
      leftHighlightEvent->HighlightEventLeftResizer(true);
   }
   else if(highlightEvent != NULL)
   {
      highlightEvent->HighlightEvent(true);
   }
   else if(rightHighlightEvent != NULL)
   {
      rightHighlightEvent->HighlightEventRightResizer(true);
   }

   // Check if we need to resize an event
   if (mLeftResizeEvent != NULL && mLeftResizeEvent->IsMovable())
   {
      mLeftResizeEvent->SetStartTime(CalculateTimeFromMouseX(event->x() + mMouseOffset));
      emit EventTimesChanged(mLeftResizeEvent->GetStartTime(), mLeftResizeEvent->GetEndTime());
      ResizeEventBoundingBoxes();
   }
   else if (mMovingEvent != NULL && mMovingEvent->IsMovable())
   {
      int newLeftTime = CalculateTimeFromMouseX(event->x() - mMouseOffset);
      int duration = mMovingEvent->GetEndTime() - mMovingEvent->GetStartTime();

      // Clamp the new time to the edges of the event track if necessary.
      if (mMovingEvent->IsMinTimeLocked())
      {
         if (newLeftTime < 0) newLeftTime = 0;
      }

      if (!mMovingEvent->IsMaxTimeLocked() || (newLeftTime + duration <= mMaximum))
      {
         mMovingEvent->SetNewTimes(newLeftTime, newLeftTime + duration);
      }
      else
      {
         mMovingEvent->SetNewTimes(mMaximum - duration, mMaximum);
      }
      emit EventTimesChanged(mMovingEvent->GetStartTime(), mMovingEvent->GetEndTime());
      ResizeEventBoundingBoxes();
   }
   else if (mRightResizeEvent != NULL && mRightResizeEvent->IsMovable())
   {
      int newEndTime = std::min(mMaximum, CalculateTimeFromMouseX(event->x() - mMouseOffset));
      mRightResizeEvent->SetEndTime(newEndTime);
      emit EventTimesChanged(mRightResizeEvent->GetStartTime(), mRightResizeEvent->GetEndTime());
      ResizeEventBoundingBoxes();
   }

   update();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::mousePressEvent(QMouseEvent* event)
{
   mSelectedEvent = NULL;
   int selectedIndex = 0;

   // Check for selected/resizing events
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      if(mEvents[eventIndex]->IsPointOnLeftResizer(event->pos()))
      {
         mMovingEvent = NULL;
         mRightResizeEvent = NULL;
         mLeftResizeEvent = mEvents[eventIndex];
         mMouseOffset = mLeftResizeEvent->GetBoundingBox().left() - event->x();

         // Select this event since we're starting to resize it
         if (mSelectedEvent != NULL)
         {
            mSelectedEvent->SelectEvent(false);
         }
         mEvents[eventIndex]->SelectEvent(true);
         mSelectedEvent = mEvents[eventIndex];
         selectedIndex = eventIndex;
      }
      else if(mEvents[eventIndex]->IsPointOnEvent(event->pos()))
      {
         mLeftResizeEvent = NULL;
         mRightResizeEvent = NULL;
         if (mSelectedEvent != NULL)
         {
            mSelectedEvent->SelectEvent(false);
         }
         mEvents[eventIndex]->SelectEvent(true);
         mSelectedEvent = mEvents[eventIndex];
         selectedIndex = eventIndex;
         mMovingEvent = mEvents[eventIndex];
         mMouseOffset = event->x() - mMovingEvent->GetBoundingBox().left();
      }
      else if(mEvents[eventIndex]->IsPointOnRightResizer(event->pos()))
      {
         mLeftResizeEvent = NULL;
         mMovingEvent = NULL;
         mRightResizeEvent = mEvents[eventIndex];
         mMouseOffset = event->x() - mRightResizeEvent->GetBoundingBox().right();

         // Select this event since we're starting to resize it
         if (mSelectedEvent != NULL)
         {
            mSelectedEvent->SelectEvent(false);
         }
         mEvents[eventIndex]->SelectEvent(true);
         mSelectedEvent = mEvents[eventIndex];
         selectedIndex = eventIndex;
      }
      else
      {
         mEvents[eventIndex]->SelectEvent(false);
      }
   }

   if (mSelectedEvent != NULL)
   {
      mEvents.erase(mEvents.begin() + selectedIndex);
      mEvents.push_back(mSelectedEvent);
   }
   emit EventSelected(mSelectedEvent);

   update();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::mouseReleaseEvent(QMouseEvent* event)
{
   if (mLeftResizeEvent != NULL || mMovingEvent != NULL || mRightResizeEvent != NULL)
   {
      emit EditingFinished();
   }

   mLeftResizeEvent = NULL;
   mMovingEvent = NULL;
   mRightResizeEvent = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::paintEvent(QPaintEvent* event)
{
   QPainter painter(this);
   QRect b = contentsRect();
   b.setLeft(b.left() + BaseEvent::RESIZE_END_WIDTH);
   b.setRight(b.right() - BaseEvent::RESIZE_END_WIDTH - 1);

   // Draw groove
   QColor color = palette().color(QPalette::Normal, QPalette::Mid);
   painter.setBrush(color);
   painter.setPen(color);
   int top = (b.height() - b.top()) / 2 - 2;
   painter.drawRect(b.left(), top++, b.width() - 1, 1);
   color = palette().color(QPalette::Normal, QPalette::Midlight);
   painter.setBrush(color);
   painter.setPen(color);
   painter.drawRect(b.left() + 1, top++, b.width() - 2, 2);
   color = palette().color(QPalette::Normal, QPalette::Button);
   painter.setBrush(color);
   painter.setPen(color);
   painter.drawRect(b.left() + 1, ++top, b.width() - 2, 1);

   if (mMaximum > mMinimum)
   {
      // Draw selection box
      if (mSelectionEnd > mSelectionStart)
      {
         float startWidthPercent = float(mSelectionStart - mMinimum) / float(mMaximum - mMinimum);
         float endWidthPercent = float(mSelectionEnd - mMinimum) / float(mMaximum - mMinimum);
         int startX = b.left() + startWidthPercent * b.width();
         int endX = b.left() + endWidthPercent * b.width();
         painter.setBrush(SELECTION_COLOR);
         painter.setPen(SELECTION_COLOR);
         painter.drawRect(startX, b.top(), endX - startX, b.height());
      }

      // Draw event segments
      for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
      {
         mEvents[eventIndex]->PaintEvent(&painter);
      }

      // Draw current time position
      float widthPercent = float(mCurrent - mMinimum) / float(mMaximum - mMinimum);
      int currentX = b.left() + widthPercent * b.width();
      color = palette().color(QPalette::Normal, QPalette::Dark);
      painter.setBrush(color);
      painter.setPen(color);
      painter.drawLine(currentX, b.top(), currentX, b.bottom());
   }
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::resizeEvent(QResizeEvent* event)
{
   ResizeEventBoundingBoxes();
   update();
}

///////////////////////////////////////////////////////////////////////////////
void EventTrackWidget::ResizeEventBoundingBoxes()
{
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      ComputeEventBoundingBox(mEvents[eventIndex]);
   }
}

////////////////////////////////////////////////////////////////////////////////
int EventTrackWidget::CalculateTimeFromMouseX(int mouseX)
{
   QRect eventRect = contentsRect();
   eventRect.setLeft(eventRect.left() + BaseEvent::RESIZE_END_WIDTH);
   eventRect.setRight(eventRect.right() - BaseEvent::RESIZE_END_WIDTH - 3);
   float widthPercentage = float(mouseX - eventRect.left()) / float(eventRect.width());

   return widthPercentage * (mMaximum - mMinimum) + mMinimum;
}

////////////////////////////////////////////////////////////////////////////////
BaseEvent* EventTrackWidget::GetEventAtTime(int time)
{
   BaseEvent* currentEvent = NULL;
   for (size_t eventIndex = 0; eventIndex < mEvents.size(); ++eventIndex)
   {
      if (mEvents[eventIndex]->GetStartTime() <= time && time <= mEvents[eventIndex]->GetEndTime())
      {
         currentEvent = mEvents[eventIndex];
      }
   }

   return currentEvent;
}

///////////////////////////////////////////////////////////////////////////////
