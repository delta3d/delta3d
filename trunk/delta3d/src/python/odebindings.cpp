#include <ode/ode.h>
#include <boost/python.hpp>

void initODE()
{
	using namespace boost::python;

	def("dBodyAddForce", dBodyAddForce);
	def("dBodyAddTorque", dBodyAddTorque);
}


