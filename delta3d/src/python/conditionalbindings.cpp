#include <dtAI/conditional.h>
#include <dtAI/worldstate.h>

#include <boost/python.hpp>
#include <string>

using namespace boost::python;
using namespace dtAI;


struct ConditionalWrap: IConditional, wrapper<IConditional>
{
   const std::string& GetName() const
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<const std::string&>( this->get_override("GetName").ptr());
      #else
         return this->get_override("GetName")();
      #endif
   }

   bool Evaluate(const WorldState* pWS)
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<bool>( this->get_override("Evaluate").ptr(), pWS);
      #else
         return this->get_override("Evaluate")(pWS);
      #endif
   }
};


void init_ConditionalBindings()
{   

   class_<ConditionalWrap, boost::noncopyable>("IConditional")
      .def("GetName", pure_virtual(&IConditional::GetName), return_value_policy<copy_const_reference>())
      .def("Evaluate", pure_virtual(&IConditional::Evaluate))
      ;

}
