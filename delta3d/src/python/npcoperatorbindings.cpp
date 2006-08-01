#include <dtAI/conditional.h>
#include <dtAI/worldstate.h>
#include <dtAI/npcoperator.h>
#include <string>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;
using namespace dtAI;


struct NPCOperatorApplyFunctor_python
{
   virtual bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const = 0;
};


struct NPCOperatorApplyWrap: NPCOperatorApplyFunctor_python, wrapper<NPCOperatorApplyFunctor_python>
{
   bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const 
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
         call<bool>( this->get_override("Apply").ptr(), pCurrent, pWSIn);
      #else
         return this->get_override("Apply")(pCurrent, pWSIn);
      #endif
   }
};


struct NPCOperatorPython
{
   typedef std::vector<IConditional*> ConditionalList;

   NPCOperatorPython(const std::string& pName, NPCOperatorApplyFunctor_python* pFunc)
      : mApplyFunctor(pFunc)
      , mOperator(pName, NPCOperator::ApplyOperatorFunctor(mApplyFunctor, &NPCOperatorApplyFunctor_python::Apply))
   {

   }

   bool operator==(const NPCOperator& pRHS) const
   {
      return mOperator == pRHS;
   }

   const std::string& GetName() const
   {
      return mOperator.GetName();
   }

   bool Apply(const WorldState* pCurrent, WorldState* pWSIn) const
   {
      return mOperator.Apply(pCurrent, pWSIn);
   }

   void AddPreCondition(IConditional* pCondIn)
   {
      return mOperator.AddPreCondition(pCondIn);
   }

   void AddInterrupt(IConditional* pCondIn)
   {
      return mOperator.AddInterrupt(pCondIn);
   }

   void RemovePreCondition(IConditional* pConditional)
   {
      mOperator.RemovePreCondition(pConditional);
   }

   void RemoveInterrupt(IConditional* pConditional)
   {
      mOperator.RemoveInterrupt(pConditional);
   }

   ConditionalList GetPreConditions() const
   {
      return ConditionalList(mOperator.GetPreConditions().begin(), mOperator.GetPreConditions().end());
   }

   ConditionalList GetInterrupts() const
   {
      return ConditionalList(mOperator.GetInterrupts().begin(), mOperator.GetInterrupts().end());
   }



private:
   NPCOperatorApplyFunctor_python* mApplyFunctor;
   NPCOperator mOperator;
};


void init_NPCOperatorBindings()
{   
   class_<NPCOperatorApplyWrap, boost::noncopyable>("ApplyOperatorFunctor")
      .def("Apply", pure_virtual(&NPCOperatorApplyFunctor_python::Apply));

   scope npcOperator = class_<NPCOperatorPython, boost::noncopyable>("NPCOperator", init<const std::string&, NPCOperatorApplyFunctor_python*>())
      .def("GetName", &NPCOperatorPython::GetName, return_value_policy<reference_existing_object>())
      .def("Apply", &NPCOperatorPython::Apply)
      .def("AddPreCondition", &NPCOperatorPython::AddPreCondition)
      .def("AddInterrupt", &NPCOperatorPython::AddInterrupt)
      .def("RemovePreCondition", &NPCOperatorPython::RemovePreCondition)
      .def("RemoveInterrupt", &NPCOperatorPython::RemoveInterrupt)
      ;

   class_<NPCOperatorPython::ConditionalList>("ConditionalList")
      .def(vector_indexing_suite<NPCOperatorPython::ConditionalList>());
}
