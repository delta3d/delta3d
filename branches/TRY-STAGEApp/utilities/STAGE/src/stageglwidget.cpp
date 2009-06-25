#include <dtEditQt/stageglwidget.h>
#include <dtEditQt/editorviewport.h>
#include <QtGui/QMouseEvent>

////////////////////////////////////////////////////////////////////////////////
dtEditQt::STAGEGLWidget::STAGEGLWidget(bool drawOnSeparateThread,  QWidget* parent,
                                       const QGLWidget* shareWidget, Qt::WindowFlags f):
dtQt::OSGAdapterWidget(drawOnSeparateThread, parent, shareWidget, f)
, mViewport(NULL)
{

}

////////////////////////////////////////////////////////////////////////////////
dtEditQt::STAGEGLWidget::~STAGEGLWidget()
{

}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::mouseMoveEvent(QMouseEvent* e)
{
   dtQt::OSGAdapterWidget::mouseMoveEvent(e);

   if (mViewport!= NULL)
   {
      mViewport->mouseMoveEvent(e);
   }

}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::SetViewport(EditorViewport* viewport)
{
   mViewport = viewport;
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::mousePressEvent(QMouseEvent* e)
{
   dtQt::OSGAdapterWidget::mousePressEvent(e);

   if (mViewport != NULL)
   {
      mViewport->mousePressEvent(e);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::mouseReleaseEvent(QMouseEvent* e)
{
   dtQt::OSGAdapterWidget::mouseReleaseEvent(e);

   if (mViewport != NULL)
   {
      mViewport->mouseReleaseEvent(e);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::dragEnterEvent(QDragEnterEvent* event)
{
   if (mViewport != NULL)
   {
      mViewport->dragEnterEvent(event);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
   if (mViewport != NULL)
   {
      mViewport->dragLeaveEvent(event);
   }

}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::dragMoveEvent(QDragMoveEvent* event)
{
   if (mViewport != NULL)
   {
      mViewport->dragMoveEvent(event);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::dropEvent(QDropEvent* event)
{
   if (mViewport != NULL)
   {
      mViewport->dropEvent(event);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::paintGL()
{
   dtQt::OSGAdapterWidget::paintGL();

   if (mViewport != NULL)
   {
      mViewport->paintGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::initializeGL()
{
   dtQt::OSGAdapterWidget::initializeGL();

   if (mViewport != NULL)
   {
      mViewport->initializeGL();
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::resizeGL(int width, int height)
{
   dtQt::OSGAdapterWidget::resizeGL(width, height);

   if (mViewport != NULL)
   {
      mViewport->resizeGL(width, height);
   }

}

////////////////////////////////////////////////////////////////////////////////
void dtEditQt::STAGEGLWidget::wheelEvent(QWheelEvent* e)
{
   dtQt::OSGAdapterWidget::wheelEvent(e);

   if (mViewport != NULL)
   {
      mViewport->wheelEvent(e);
   }
}
