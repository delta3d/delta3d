// characterbindings.cpp: Character binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include "python/dtpython.h"
#include "dtChar/character.h"

using namespace boost::python;
using namespace dtCore;
using namespace dtChar;

class CharacterWrap : public Character
{
public:

   CharacterWrap(PyObject* self, std::string name = "character")
      : mSelf(self)
   {}

   void LoadFileWrapper1(std::string filename, bool useCache)
   {
      Character::LoadFile(filename,useCache);
   }

   void LoadFileWrapper2(std::string filename)
   {
      Character::LoadFile(filename);
   }

protected:

   PyObject* mSelf;
};

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EA_overloads, ExecuteAction, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EAWS_overloads, ExecuteActionWithSpeed, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EAWA_overloads, ExecuteActionWithAngle, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EAWSAA_overloads, ExecuteActionWithSpeedAndAngle, 3, 5)
//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LF_overloads, LoadFile, 1, 2)

void initCharacterBindings()
{
   Character* (*CharacterGI1)(int) = &Character::GetInstance;
   Character* (*CharacterGI2)(std::string) = &Character::GetInstance;

   class_<Character, bases<Transformable,Loadable>, dtCore::RefPtr<CharacterWrap>, boost::noncopyable >("Character", init<optional<std::string> >())
      .def("GetInstanceCount", &Character::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", CharacterGI1, return_internal_reference<>())
      .def("GetInstance", CharacterGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("LoadFile", &CharacterWrap::LoadFileWrapper1)
      .def("LoadFile", &CharacterWrap::LoadFileWrapper2)
      .def("GetFilename", &Character::GetFilename)
      .def("SetRotation", &Character::SetRotation)
      .def("GetRotation", &Character::GetRotation)
      .def("SetVelocity", &Character::SetVelocity)
      .def("GetVelocity", &Character::GetVelocity)
      .def("ExecuteAction", &Character::ExecuteAction, EA_overloads())
      .def("ExecuteActionWithSpeed", &Character::ExecuteActionWithSpeed, EAWS_overloads())
      .def("ExecuteActionWithAngle", &Character::ExecuteActionWithAngle, EAWA_overloads())
      .def("ExecuteActionWithSpeedAndAngle", &Character::ExecuteActionWithSpeedAndAngle, EAWSAA_overloads())
      .def("StopAction", &Character::StopAction);
}
