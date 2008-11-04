

#include <ode/objects.h>
#include <boost/python.hpp>

#ifdef __GNUG__
struct dxBody
{
};
#endif //__GNUG__

void initODE()
{
	using namespace boost::python;

	def("dBodyAddForce", dBodyAddForce);
	def("dBodyAddTorque", dBodyAddTorque);
}


