/*
 * jointtype.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: david
 */

#include <dtPhysics/jointtype.h>
#include <pal/palLinks.h>

namespace dtPhysics
{
   IMPLEMENT_ENUM(JointType);

   JointType::JointType(const std::string& name, int palType)
   : dtUtil::Enumeration(name)
   , mPalLinkType(palType)
   {
      AddInstance(this);
   }

   JointType::~JointType()
   {
   }

   JointType JointType::GENERIC_6DOF("GENERIC_6DOF", PAL_LINK_GENERIC);
   JointType JointType::REVOLUTE("REVOLUTE", PAL_LINK_REVOLUTE);
   JointType JointType::FIXED("FIXED", PAL_LINK_RIGID);
   JointType JointType::SPHERICAL("SPHERICAL", PAL_LINK_SPHERICAL);
   JointType JointType::SLIDER("SLIDER", PAL_LINK_PRISMATIC);


} /* namespace dtPhysics */
