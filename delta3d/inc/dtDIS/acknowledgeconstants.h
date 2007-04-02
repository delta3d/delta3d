#ifndef _DTDIS_ARTICULATION_CONSTANTS_H_
#define _DTDIS_ARTICULATION_CONSTANTS_H_

namespace dtDIS
{
   enum AcknowledgeFlag
   {
      ACKNOWLEDGE_CREATE_ENTITY = 1,
      ACKNOWLEDGE_REMOVE_ENTITY = 2,
      ACKNOWLEDGE_START_RESUME = 3,
      ACKNOWLEDGE_STOP_FREEZE = 4
   };

   enum ResponseFlag
   {
      RESPONSE_OTHER = 0,
      RESPONSE_ABLE_TO_COMPLY = 1,
      RESPONSE_UNABLE_TO_COMPLY = 2,
   };
}

#endif // _DTDIS_ARTICULATION_CONSTANTS_H_
