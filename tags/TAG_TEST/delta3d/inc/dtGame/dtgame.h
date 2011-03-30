#ifndef DELTA_GAME
#define DELTA_GAME

#if defined (WIN32) || defined(__BORLANDC__)
   #define __STR2__(x) #x
   #define __STR1__(x) __STR2__(x)
   #define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
   #pragma message (__LOC__"This header is deprecated. Please #include the individual header files.")
#elif defined(__GNUC__) || defined(__HP_aCC) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
   #warning "This header is deprecated. Please #include the individual header files."
#endif

#include <dtGame/actorupdatemessage.h>
#include <dtGame/basemessages.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/environmentactor.h>
#include <dtGame/export.h>
#include <dtGame/gameactor.h>
#include <dtGame/gameapplication.h>
#include <dtGame/gameentrypoint.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/invokable.h>
#include <dtGame/logcontroller.h>
#include <dtGame/loggermessages.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/logkeyframeindex.h>
#include <dtGame/logstatus.h>
#include <dtGame/logstream.h>
#include <dtGame/logtag.h>
#include <dtGame/logtagindex.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messagefactory.h>
#include <dtGame/message.h>
#include <dtGame/messageparameter.h>
#include <dtGame/messagetype.h>
#include <dtGame/serverloggercomponent.h>

#endif
