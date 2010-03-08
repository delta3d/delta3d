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


#include <prefix/dtqtprefix.h>
#include <dtQt/qtguiwindowsystemwrapper.h>
#include <dtQt/osggraphicswindowqt.h>
#include <dtQt/glwidgetfactory.h>
#include <dtUtil/log.h>

namespace dtQt
{
   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::EnableQtGUIWrapper()
   {
      osg::GraphicsContext::WindowingSystemInterface* winSys = osg::GraphicsContext::getWindowingSystemInterface();

      if (winSys != NULL)
      {
         osg::GraphicsContext::setWindowingSystemInterface(new dtQt::QtGuiWindowSystemWrapper(*winSys));
      }
      else
      {
         LOG_ERROR("Unable to initialize. OSG reported not having any windowing system interface.");
      }
   }

   /////////////////////////////////////////////////////////////
   QtGuiWindowSystemWrapper::QtGuiWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface)
   : mInterface(&oldInterface)
   , mWidgetFactory(NULL)
   {
   }

   /////////////////////////////////////////////////////////////
   unsigned int QtGuiWindowSystemWrapper::getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier)
   {
      return mInterface->getNumScreens(screenIdentifier);
   }

   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::getScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
            unsigned int& width, unsigned int& height)
   {
      mInterface->getScreenResolution(screenIdentifier, width, height);
   }

   /////////////////////////////////////////////////////////////
   bool QtGuiWindowSystemWrapper::setScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
            unsigned int width, unsigned int height)
   {
      return mInterface->setScreenResolution(screenIdentifier, width, height);
   }

   /////////////////////////////////////////////////////////////
   bool QtGuiWindowSystemWrapper::setScreenRefreshRate(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
            double refreshRate)
   {
      return mInterface->setScreenRefreshRate(screenIdentifier, refreshRate);
   }

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 8
   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::getScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettings & resolution)
   {
      mInterface->getScreenSettings(si, resolution);
   }

   /////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::enumerateScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettingsList & rl)
   {
      mInterface->enumerateScreenSettings(si, rl);
   }
#endif

   /////////////////////////////////////////////////////////////
   osg::GraphicsContext* QtGuiWindowSystemWrapper::createGraphicsContext(osg::GraphicsContext::Traits* traits)
   {
      //return new osgViewer::GraphicsWindowEmbedded(traits);

      if (traits->pbuffer)
      {
         return mInterface->createGraphicsContext(traits);
      }
      else
      {
         return new dtQt::OSGGraphicsWindowQt(traits, mWidgetFactory);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void QtGuiWindowSystemWrapper::SetGLWidgetFactory(GLWidgetFactory* factory)
   {
      mWidgetFactory = factory;
   }
}
