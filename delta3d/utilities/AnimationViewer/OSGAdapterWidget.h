/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * David Guthrie
 */
#ifndef OSGADAPTERWIDGET_H_
#define OSGADAPTERWIDGET_H_

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include <QtOpenGL/QGLWidget>

#include <dtCore/refptr.h>

namespace osgViewer
{
   class GraphicsWindow;
}

class OSGAdapterWidget : public QGLWidget
{
   public:

      OSGAdapterWidget( QWidget * parent = NULL, const char * name = NULL, 
               const QGLWidget * shareWidget = NULL, Qt::WindowFlags f = NULL );

      virtual ~OSGAdapterWidget() {}

      osgViewer::GraphicsWindow& GetGraphicsWindow();
      const osgViewer::GraphicsWindow& GetGraphicsWindow() const;

      void SetGraphicsWindow(osgViewer::GraphicsWindow* newWindow);
      //does the actual painting.
      void ThreadedUpdateGL();

   protected:
      
      //does nothing.  this is called automatically at times by qt, but on the wrong thread.
      virtual void paintGL();

      virtual void initializeGL();

      virtual void resizeGL( int width, int height );
      virtual void keyPressEvent( QKeyEvent* event );
      virtual void keyReleaseEvent( QKeyEvent* event );
      virtual void mousePressEvent( QMouseEvent* event );
      virtual void mouseReleaseEvent( QMouseEvent* event );
      virtual void mouseMoveEvent( QMouseEvent* event );

      dtCore::RefPtr<osgViewer::GraphicsWindow> mGraphicsWindow;
};

#endif /*OSGADAPTERWIDGET_H_*/
