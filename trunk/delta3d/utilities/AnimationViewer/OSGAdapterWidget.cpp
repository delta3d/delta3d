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
#include "OSGAdapterWidget.h"
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>

#include <OpenGL/OpenGL.h>
#include <AGL/agl.h>
#include <dtCore/system.h>

OSGAdapterWidget::OSGAdapterWidget( QWidget * parent, const char * name, const QGLWidget * shareWidget, Qt::WindowFlags f):
    QGLWidget(parent, shareWidget, f)
{
}

osgViewer::GraphicsWindow& OSGAdapterWidget::GetGraphicsWindow() 
{ 
   return *mGraphicsWindow; 
}

const osgViewer::GraphicsWindow& OSGAdapterWidget::GetGraphicsWindow() const 
{ 
   return *mGraphicsWindow; 
}

void OSGAdapterWidget::initializeGL()
{
}

void OSGAdapterWidget::SetGraphicsWindow(osgViewer::GraphicsWindow* newWindow)
{
   mGraphicsWindow = newWindow;
}

void OSGAdapterWidget::ThreadedUpdateGL()
{
   makeCurrent();
   dtCore::System& system = dtCore::System::GetInstance();
   if (system.IsRunning())
      system.StepWindow();
   swapBuffers();
}


void OSGAdapterWidget::paintGL()
{
}

void OSGAdapterWidget::resizeGL( int width, int height )
{
   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->windowResize(0, 0, width, height );
      mGraphicsWindow->resized(0,0,width,height);
   }
}

void OSGAdapterWidget::keyPressEvent( QKeyEvent* event )
{
   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->keyPress( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
   }
}

void OSGAdapterWidget::keyReleaseEvent( QKeyEvent* event )
{
   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->keyRelease( (osgGA::GUIEventAdapter::KeySymbol) *(event->text().toAscii().data() ) );
   }
}

void OSGAdapterWidget::mousePressEvent( QMouseEvent* event )
{
   int button = 0;
   switch(event->button())
   {
      case(Qt::LeftButton): button = 1; break;
      case(Qt::MidButton): button = 2; break;
      case(Qt::RightButton): button = 3; break;
      case(Qt::NoButton): button = 0; break;
      default: button = 0; break;
   }
   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->mouseButtonPress(event->x(), event->y(), button);
   }
}

void OSGAdapterWidget::mouseReleaseEvent( QMouseEvent* event )
{
   int button = 0;
   switch(event->button())
   {
      case(Qt::LeftButton): button = 1; break;
      case(Qt::MidButton): button = 2; break;
      case (Qt::RightButton): button = 3; break;
      case(Qt::NoButton): button = 0; break;
      default: button = 0; break;
   }

   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->mouseButtonRelease(event->x(), event->y(), button);
   }
}

void OSGAdapterWidget::mouseMoveEvent( QMouseEvent* event )
{
   if (mGraphicsWindow.valid())
   {
      mGraphicsWindow->getEventQueue()->mouseMotion(event->x(), event->y());
   }
}
