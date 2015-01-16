#include <dtUtil/log.h>
#include <boost/python.hpp>
#include <dtCore/refptr.h>
#include <python/dtpython.h>
#include <string>

using namespace dtUtil;
using namespace boost::python;

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(LM_overloads, LogMessage, 3, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(GI_overloads, GetInstance, 0, 1)

void init_LogBindings()
{

   void (Log::*LogMessage2)( const std::string&, int, const std::string&, Log::LogMessageType)const = &Log::LogMessage;

   scope Log_scope = class_<Log, Log*, boost::noncopyable>("Log", no_init)
      .def("LogMessage", LogMessage2, LM_overloads())
      .def("LogHorizRule", &Log::LogHorizRule)
      .def("IsLevelEnabled", &Log::IsLevelEnabled)
      .def("SetLogLevel", &Log::SetLogLevel)
      .def("GetLogStringLevel", &Log::GetLogLevelString)
      .def("GetInstance", &Log::GetInstance, return_value_policy<reference_existing_object>(), GI_overloads())
      .staticmethod("GetInstance");

   enum_<Log::LogMessageType>("LogMessageType")
      .value("LOG_DEBUG", Log::LOG_DEBUG)
      .value("LOG_INFO", Log::LOG_INFO)
      .value("LOG_WARNING", Log::LOG_WARNING)
      .value("LOG_ERROR", Log::LOG_ERROR)
      .value("LOG_ALWAYS", Log::LOG_ALWAYS)
      .export_values();

}


