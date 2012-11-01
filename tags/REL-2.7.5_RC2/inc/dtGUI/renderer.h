/* 
 * Delta3D Open Source Game and Simulation Engine 
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */
#ifndef DELTA_GUI
#define DELTA_GUI

#include <dtGUI/export.h>
#include <CEGUI/CEGUIVersion.h>
#if defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
   #ifdef __APPLE__
      #include <CEGUIOpenGLRenderer/RendererModules/OpenGLGUIRenderer/openglrenderer.h>
   #else
      #include <CEGUI/RendererModules/OpenGLGUIRenderer/openglrenderer.h>
   #endif
#else
   #ifdef __APPLE__
      #include <CEGUIOpenGLRenderer/openglrenderer.h>
   #else
      #include <CEGUI/renderers/OpenGLGUIRenderer/openglrenderer.h>
   #endif
#endif
#include <dtUtil/deprecationmgr.h>

#ifdef APIENTRY
#undef APIENTRY
#endif //APIENTRY

#ifdef WINGDIAPI
#undef WINGDIAPI
#endif //WINGDIAPI

namespace dtGUI
{
   ///Simple CEGui rendering class based on the CEGUI::OpenGLRender class
   class DT_GUI_EXPORT Renderer : public CEGUI::OpenGLRenderer
   {
   public:
      ///\todo deprecate this unnecessary ctor
      DEPRECATE_FUNC Renderer(unsigned int max_quads=0, int width=0, int height=0);
      ~Renderer() {};

      ///Create the ResourceProvider (a dtGUI::ResourceProvider)
      virtual CEGUI::ResourceProvider* createResourceProvider();
   };
}

#endif //DELTA_GUI
