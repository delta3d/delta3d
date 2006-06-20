#ifndef DELTA_GUI
#define DELTA_GUI

#include <dtGUI/export.h>
#ifdef __APPLE__
#include <CEGUIOpenGLRenderer/openglrenderer.h>
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
      ~Renderer(void) {};

      ///Create the ResourceProvider (a dtGUI::ResourceProvider)
      virtual CEGUI::ResourceProvider* createResourceProvider(void);
   };
}

#endif //DELTA_GUI
