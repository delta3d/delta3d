#include <dtUtil/log.h>
#include <boost/python.hpp>
#include <dtCore/refptr.h>
#include <python/dtPython.h>
#include <string>

using namespace dtUtil;
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LM_overloads, LogMessage, 3, 4)

void init_LogBindings()
{

   void (Log::*LogMessage2)( const std::string&, int, const std::string&, Log::LogMessageType) = &Log::LogMessage;

   Log& (*GetInstance1)() = &Log::GetInstance;
   Log& (*GetInstance2)(const std::string&) = &Log::GetInstance;


   class_<Log, bases<osg::Referenced>, dtCore::RefPtr<Log>, boost::noncopyable>("Log", no_init)
      .def("LogMessage", LogMessage2, LM_overloads())
      .def("LogHorizRule", &Log::LogHorizRule)
      .def("IsLevelEnabled", &Log::IsLevelEnabled)
      .def("SetLogLevel", &Log::SetLogLevel)
      .def("GetLogStringLevel", &Log::GetLogLevelString)
      .def("GetInstance", GetInstance1, return_internal_reference<>())
      .def("GetInstance", GetInstance2, return_internal_reference<>())
      .staticmethod("GetInstance");
      

   enum_<Log::LogMessageType>("LogMessageType")
      .value("LOG_DEBUG", Log::LOG_DEBUG)
      .value("LOG_INFO", Log::LOG_INFO)
      .value("LOG_WARNING", Log::LOG_WARNING)
      .value("LOG_ERROR", Log::LOG_ERROR)
      .value("LOG_ALWAYS", Log::LOG_ALWAYS)
      .export_values();

}


