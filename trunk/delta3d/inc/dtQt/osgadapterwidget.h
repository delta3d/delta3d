/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef OSGADAPTERWIDGET_H_
#define OSGADAPTERWIDGET_H_

#include <dtUtil/warningdisable.h>
DT_DISABLE_WARNING_ALL_START
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLContext>
#include <dtQt/export.h>
DT_DISABLE_WARNING_END

#include <dtCore/refptr.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osgViewer
{
   class GraphicsWindow;
}
/// @endcond

namespace dtQt
{

   class DT_QT_EXPORT OSGAdapterWidget : public QGLWidget
   {
      Q_OBJECT

   public:
      OSGAdapterWidget(const QGLFormat& format, bool drawOnSeparateThread,  QWidget* parent = NULL,
         const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = NULL);

      OSGAdapterWidget(bool drawOnSeparateThread, QWidget* parent = NULL,
         const QGLWidget* shareWidget = NULL, Qt::WindowFlags f = NULL);

      virtual ~OSGAdapterWidget();

      osgViewer::GraphicsWindow& GetGraphicsWindow();
      const osgViewer::GraphicsWindow& GetGraphicsWindow() const;

      void SetGraphicsWindow(osgViewer::GraphicsWindow& newWindow);

      void ThreadedInitializeGL();
      void ThreadedMakeCurrent();

   public slots:
      // does the actual painting.
      void ThreadedUpdateGL();

   protected:
      // This draws, but only if .
      virtual void paintGL();
      void paintGLImpl();

      virtual void initializeGL();

      virtual void resizeGL(int width, int height);
      void resizeGLImpl(int width, int height);

      virtual void keyPressEvent(QKeyEvent* event);
      virtual void keyReleaseEvent(QKeyEvent* event);
      virtual void mousePressEvent(QMouseEvent* event);
      virtual void mouseReleaseEvent(QMouseEvent* event);
      virtual void mouseMoveEvent(QMouseEvent* event);
      virtual void mouseDoubleClickEvent(QMouseEvent* event);
      virtual void wheelEvent(QWheelEvent* event);

      dtCore::RefPtr<osgViewer::GraphicsWindow> mGraphicsWindow;

      QTimer mTimer;

      QGLContext* mThreadGLContext;
      bool mDrawOnSeparateThread;
      volatile bool mDoResize;
   };

} // namespace dtQt

#endif /*OSGADAPTERWIDGET_H_*/
