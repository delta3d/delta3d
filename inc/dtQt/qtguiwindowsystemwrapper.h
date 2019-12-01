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
#ifndef qtguiwindowsystemwrapper_h__
#define qtguiwindowsystemwrapper_h__


#include <dtQt/export.h>
#include <dtQt/glwidgetfactory.h>
#include <osg/Version>
#include <osg/GraphicsContext>
#include <dtCore/refptr.h>

namespace dtQt
{

   class DT_QT_EXPORT QtGuiWindowSystemWrapper: public osg::GraphicsContext::WindowingSystemInterface
   {
   public:
      static void EnableQtGUIWrapper();
      static void DisableQtGUIWrapper();

      QtGuiWindowSystemWrapper(osg::GraphicsContext::WindowingSystemInterface& oldInterface);

      virtual unsigned int getNumScreens(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier =
         osg::GraphicsContext::ScreenIdentifier());

      virtual void getScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettings& resolution);

      virtual void enumerateScreenSettings(const osg::GraphicsContext::ScreenIdentifier& si, osg::GraphicsContext::ScreenSettingsList & rl);

   #if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION < 3
      virtual void getScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               unsigned int& width, unsigned int& height);

      virtual bool setScreenResolution(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               unsigned int width, unsigned int height);

      virtual bool setScreenRefreshRate(const osg::GraphicsContext::ScreenIdentifier& screenIdentifier,
               double refreshRate);
   #else
      virtual bool setScreenSettings(const osg::GraphicsContext::ScreenIdentifier& /*screenIdentifier*/, const osg::GraphicsContext::ScreenSettings & /*resolution*/);
   #endif

      virtual osg::GraphicsContext* createGraphicsContext(osg::GraphicsContext::Traits* traits);

      ///Supply a custom factory to create custom QGLWidgets.
      void SetGLWidgetFactory(GLWidgetFactory* factory);

   protected:
      virtual ~QtGuiWindowSystemWrapper() {};
   private:
      dtCore::RefPtr<osg::GraphicsContext::WindowingSystemInterface> mInterface;
      dtCore::RefPtr<GLWidgetFactory> mWidgetFactory;
   };

} // namespace dtQt

#endif // qtguiwindowsystemwrapper_h__
