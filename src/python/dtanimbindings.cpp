// dtanimbindings.cpp: Python bindings for dtAnim library.
//
//////////////////////////////////////////////////////////////////////

#include <boost/python.hpp>

void init_CharacterWrapperBindings();

BOOST_PYTHON_MODULE(PyDtAnim)
{
   init_CharacterWrapperBindings();
}
