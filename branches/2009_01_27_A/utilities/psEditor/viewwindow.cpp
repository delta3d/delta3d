#include "viewwindow.h"
#include <osgViewer/CompositeViewer>
#include <osg/Version>

ViewWindow::ViewWindow()
{
}

ViewWindow::ViewWindow(int x, int y, int w, int h):
MyParent(x,y,w,h)
{
}

ViewWindow::~ViewWindow()
{
}

void ViewWindow::Config( const dtABC::WinData* data /*= NULL */ )
{
   MyParent::Config(data);

#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 6
   /** Little temporary hack to get the particle editor operational with OSG 2.6.0,
   *  which by default, will release the OpenGL context after every frame.
   */
   GetCompositeViewer()->setReleaseContextAtEndOfFrameHint(false);
#endif
}
