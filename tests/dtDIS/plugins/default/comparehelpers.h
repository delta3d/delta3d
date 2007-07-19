#ifndef _plugin_compare_helpers_h_
#define _plugin_compare_helpers_h_

#include <string>                         // for param

namespace dtGame
{
   class ActorUpdateMessage;
}

namespace dtTest
{
   void CompareParams(const std::string& name,
                        const dtGame::ActorUpdateMessage& should_be,
                        const dtGame::ActorUpdateMessage& truth);

   // a utility used while waiting for comparison operators in the DIS types.
   template<typename V>
   bool CompareVec3(const V& lhs, const V& rhs)
   {
      return( lhs.getX()==rhs.getX() && lhs.getY()==rhs.getY() && lhs.getZ()==rhs.getZ() );
   }

   // a utility used while waiting for comparison operators in the DIS types.
   template<typename V>
   bool CompareOrientation(const V& lhs, const V& rhs)
   {
      return( lhs.getPhi()==rhs.getPhi() && lhs.getTheta()==rhs.getTheta() && lhs.getPsi()==rhs.getPsi() );
   }
}

#endif  // _plugin_compare_helpers_h_
