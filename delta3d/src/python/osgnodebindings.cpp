#include <python/dtpython.h>
#include <osg/Node>

using namespace boost::python;
using namespace osg;

void initOSGNodeBindings()
{
   class_<osg::Node, dtCore::RefPtr<osg::Node>, boost::noncopyable>("Node", no_init)
      ;
}

