/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_ENVEFFECT
#define DELTA_ENVEFFECT

#include "dtCore/base.h"
#include "sg.h"
#include <osg/Group>

namespace dtCore
{
   ///A base class for all Environmental Effects
   class DT_EXPORT EnvEffect : public dtCore::Base
   {
   public:
      DECLARE_MANAGEMENT_LAYER(EnvEffect)

      EnvEffect(const std::string name=0);
      virtual ~EnvEffect(void);

      /// Must override this to supply the repainting routine
      virtual void Repaint(sgVec3 skyColor, sgVec3 fogColor,
                           double sunAngle, double sunAzimuth,
                           double visibility) = 0;

      virtual osg::Group *GetNode(void) = 0;

   };
}

#endif // DELTA_ENVEFFECT
