/*
 * jointtype.h
 *
 *  Created on: Dec 1, 2014
 *      Author: david
 */

#ifndef JOINTTYPE_H_
#define JOINTTYPE_H_

#include <dtPhysics/physicsexport.h>

#include <dtUtil/enumeration.h>

namespace dtPhysics
{

   class DT_PHYSICS_EXPORT JointType: public dtUtil::Enumeration
   {
   public:

      DECLARE_ENUM(JointType);

      static JointType GENERIC_6DOF;
      static JointType REVOLUTE;
      static JointType FIXED;
      static JointType SPHERICAL;
      static JointType SLIDER;

      int GetPalLinkType() const { return mPalLinkType; }

   protected:
      JointType(const std::string& name, int palType);
      virtual ~JointType();
      int mPalLinkType;
   };

} /* namespace dtPhysics */

#endif /* JOINTTYPE_H_ */
