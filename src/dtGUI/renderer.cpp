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
#include <dtGUI/renderer.h>
#include <dtGUI/resourceprovider.h>

using namespace dtGUI;

Renderer::Renderer(unsigned int max_quads, int width, int height): OpenGLRenderer(max_quads)
{
   DEPRECATE("dtGUI::Renderer",
              "n/a");
}

CEGUI::ResourceProvider* Renderer::createResourceProvider(void)
{
   d_resourceProvider = new dtGUI::ResourceProvider();
   return d_resourceProvider;
}
