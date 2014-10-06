#ifndef __DELTA_DTANIM_CONSTANTS_H__
#define __DELTA_DTANIM_CONSTANTS_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <dtUtil/refstring.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Constants
   {
   public:
      static const dtUtil::RefString CHARACTER_SYSTEM_CAL3D;
      static const dtUtil::RefString CHARACTER_SYSTEM_OSG;
      static const int DEFAULT_BONES_PER_VERTEX = 3;

   private:
      // Not implemented
      Constants() {}
      Constants(const Constants& other) {}
      ~Constants() {}
   };
}

#endif
