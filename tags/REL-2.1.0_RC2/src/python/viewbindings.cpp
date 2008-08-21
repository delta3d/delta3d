#include <python/dtpython.h>
#include <dtCore/deltadrawable.h>
#include <dtCore/view.h>

using namespace boost::python;
using namespace dtCore;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GMPO_overloads, GetMousePickedObject, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GMPP_overloads, GetMousePickPosition, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GPP_overloads, GetPickPosition, 2, 3)

void initViewBindings()
{
   class_< View, bases<dtCore::Base>, dtCore::RefPtr<View>, boost::noncopyable >( "View", init< optional< const std::string& > >() )
	  .def( "GetMousePickedObject", &View::GetMousePickedObject, GMPO_overloads()[ return_internal_reference<>() ] )
	  .def( "GetMousePickPosition", &View::GetMousePickPosition, GMPP_overloads() )
	  .def( "GetPickPosition", &View::GetPickPosition, GPP_overloads() )
      ;
}
