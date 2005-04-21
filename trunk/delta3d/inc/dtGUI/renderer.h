#ifndef RENDERER_INCLUDED
#define RENDERER_INCLUDED

#include "CEGUI/openglrenderer.h"

namespace dtGUI
{
   ///Simple CEGui rendering class based on the CEGUI::OpenGLRender class
   class Renderer :   public CEGUI::OpenGLRenderer
   {
   public:
      Renderer(unsigned int max_quads, int width, int height);
      ~Renderer(void) {};

      ///Create the ResourceProvider (a dtGUI::ResourceProvider)
      virtual CEGUI::ResourceProvider* createResourceProvider(void);
   };
}

#endif //RENDERER_INCLUDED
