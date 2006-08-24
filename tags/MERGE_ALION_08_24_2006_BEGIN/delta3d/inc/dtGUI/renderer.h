#ifndef DELTA_GUI
#define DELTA_GUI

#include <dtGUI/export.h>
#include <CEGUI/CEGUIVersion.h>
#ifdef __APPLE__
#include <CEGUIOpenGLRenderer/openglrenderer.h>
#elif defined(CEGUI_VERSION_MAJOR) && CEGUI_VERSION_MAJOR >= 0 && defined(CEGUI_VERSION_MINOR) && CEGUI_VERSION_MINOR >= 5
#include <CEGUI/RendererModules/OpenGLGUIRenderer/openglrenderer.h>
#else
#include <CEGUI/renderers/OpenGLGUIRenderer/openglrenderer.h>
#endif

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
      Renderer(unsigned int max_quads=0, int width=0, int height=0);
      ~Renderer() {};

      ///Create the ResourceProvider (a dtGUI::ResourceProvider)
      virtual CEGUI::ResourceProvider* createResourceProvider();
   };
}

#endif //DELTA_GUI
