#ifndef __VIEWWINDOW_H__
#define __VIEWWINDOW_H__

////////////////////////////////////////////////////////////////////////////////

#include <dtQt/export.h>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLContext>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>

#include <dtCore/refptr.h>
#include <dtCore/system.h>

////////////////////////////////////////////////////////////////////////////////

namespace osgViewer
{
   class GraphicsWindow;
}

namespace dtQt
{
   ///Little class used to hold the Delta3D rendering surface
   class DT_QT_EXPORT ViewWindow : public QGLWidget
   {
      Q_OBJECT

   public:
      ViewWindow(bool drawOnSeparateThread,  QWidget* parent = NULL, 
         const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = NULL);
      ~ViewWindow();

      osgViewer::GraphicsWindow& GetGraphicsWindow();
      const osgViewer::GraphicsWindow& GetGraphicsWindow() const;

      void SetGraphicsWindow(osgViewer::GraphicsWindow& newWindow);

      void ThreadedInitializeGL();
      void ThreadedMakeCurrent();

      public slots:
         //does the actual painting.
         void ThreadedUpdateGL();

   protected:

      //This draws, but only if .
      virtual void paintGL();
      void paintGLImpl();

      virtual void initializeGL();

      virtual void resizeGL( int width, int height);
      void resizeGLImpl(int width, int height);

      virtual void keyPressEvent(QKeyEvent* event);
      virtual void keyReleaseEvent(QKeyEvent* event);
      virtual void mousePressEvent(QMouseEvent* event);
      virtual void mouseReleaseEvent(QMouseEvent* event);
      virtual void mouseMoveEvent(QMouseEvent* event);

      dtCore::RefPtr<osgViewer::GraphicsWindow> mGraphicsWindow;

      QTimer mTimer;

      QGLContext* mThreadGLContext;
      bool mDrawOnSeparateThread;
      volatile bool mDoResize;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // __VIEWWINDOW_H__
