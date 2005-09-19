#include <dtGUI/renderer.h>
#include <dtGUI/resourceprovider.h>

using namespace dtGUI;

Renderer::Renderer(unsigned int max_quads, int width, int height):
OpenGLRenderer(max_quads, width, height)
{
}


CEGUI::ResourceProvider* Renderer::createResourceProvider(void)
{
   d_resourceProvider = new dtGUI::ResourceProvider();
   return d_resourceProvider;
}
