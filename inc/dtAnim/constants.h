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
      static const int DEFAULT_BONES_PER_VERTEX = 3;

      static const dtUtil::RefString CHARACTER_SYSTEM_CAL3D;
      static const dtUtil::RefString CHARACTER_SYSTEM_OSG;

      static const dtUtil::RefString BONE_TRANSFORM_UNIFORM;
      static const dtUtil::RefString BONE_WEIGHTS_ATTRIB;
      static const dtUtil::RefString BONE_INDICES_ATTRIB;
      static const dtUtil::RefString TANGENT_SPACE_ATTRIB;

   private:
      // Not implemented
      Constants() {}
      Constants(const Constants& other) {}
      ~Constants() {}
   };
}

#endif
