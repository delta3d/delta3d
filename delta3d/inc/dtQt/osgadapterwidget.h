/* -*-c++-*-
 * Stealth Viewer - OSGAdapterWidget (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */
#ifndef OSGADAPTERWIDGET_H_
#define OSGADAPTERWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLContext>
#include <dtQt/export.h>

#include <dtCore/refptr.h>

namespace osgViewer
{
   class GraphicsWindow;
}

namespace dtQt
{

   class DT_QT_EXPORT OSGAdapterWidget : public QGLWidget
   {
      Q_OBJECT

      public:

         OSGAdapterWidget(bool drawOnSeparateThread,  QWidget * parent = NULL,
                  const QGLWidget * shareWidget = NULL, Qt::WindowFlags f = NULL );

         virtual ~OSGAdapterWidget();

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

         virtual void resizeGL( int width, int height );
         void resizeGLImpl(int width, int height);

         virtual void keyPressEvent( QKeyEvent* event );
         virtual void keyReleaseEvent( QKeyEvent* event );
         virtual void mousePressEvent( QMouseEvent* event );
         virtual void mouseReleaseEvent( QMouseEvent* event );
         virtual void mouseMoveEvent( QMouseEvent* event );

         dtCore::RefPtr<osgViewer::GraphicsWindow> mGraphicsWindow;

         QTimer mTimer;

         QGLContext* mThreadGLContext;
         bool mDrawOnSeparateThread;
         volatile bool mDoResize;

   };
}
#endif /*OSGADAPTERWIDGET_H_*/
