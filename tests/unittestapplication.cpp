#include "unittestapplication.h"
#include <osgViewer/CompositeViewer>
#include <dtCore/deltawin.h>
#include <osg/Version>

UnitTestApplication::UnitTestApplication():
dtABC::Application("config.xml")
{
#if defined(OPENSCENEGRAPH_MAJOR_VERSION) && OPENSCENEGRAPH_MAJOR_VERSION >= 2 && defined(OPENSCENEGRAPH_MINOR_VERSION) && OPENSCENEGRAPH_MINOR_VERSION >= 6
   /** Little temporary hack to get the unit tests operational with OSG 2.6.0,
    *  which by default, will release the OpenGL context after every frame.
    *  This disables that feature and allows the unit tests to assume there is
    *  a valid context at all times.
    */
   GetCompositeViewer()->setReleaseContextAtEndOfFrameHint(false);
#endif

   GetWindow()->SetPosition(0, 0, 50, 50);
}

UnitTestApplication::~UnitTestApplication()
{

}

