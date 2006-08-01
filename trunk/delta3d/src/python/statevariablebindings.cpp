#include <dtAI/statevariable.h>

#include <boost/python.hpp>

using namespace boost::python;
using namespace dtAI;


struct StateVarWrap: IStateVariable, wrapper<IStateVariable>
{

   IStateVariable* Copy() const
   {
#if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      call<IStateVariable*>( this->get_override("Copy").ptr());
#else
      return this->get_override("Copy")();
#endif
   }
};


void init_StateVarBindings()
{   

   class_<StateVarWrap, boost::noncopyable>("IStateVariable")
      .def("Copy", pure_virtual(&IStateVariable::Copy), return_value_policy<manage_new_object>())
      ;

}