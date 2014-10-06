#include "qtglframe.h"

#include <QtCore/QObjectList>
#include <QtGui/QResizeEvent>
#include <QtOpenGL/QGLWidget>

////////////////////////////////////////////////////////////////////////////////
QtGLFrame::QtGLFrame(QObject* parent)
{
}

////////////////////////////////////////////////////////////////////////////////
QtGLFrame::~QtGLFrame()
{
}

////////////////////////////////////////////////////////////////////////////////
void QtGLFrame::enterEvent(QEvent* event)
{
   QObjectList childrenWidgets = children();
   for(int i = 0; i < childrenWidgets.count(); ++i)
   {
      QGLWidget* glWidget = dynamic_cast<QGLWidget*>(childrenWidgets.at(i));
      if (glWidget)
      {
         glWidget->setFocus();
         break;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void QtGLFrame::resizeEvent(QResizeEvent* event)
{
   QObjectList childrenWidgets = children();
   for(int i = 0; i < childrenWidgets.count(); ++i)
   {
      QGLWidget* glWidget = dynamic_cast<QGLWidget*>(childrenWidgets.at(i));
      if (glWidget)
      {
         QSize newSize = event->size();
         glWidget->setGeometry(0, 0, newSize.width(), newSize.height());
         break;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
