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

#include "dtCore/deltadrawable.h"
#include "dtUtil/deprecationmgr.h"
#include <osg/Group>
#include <osg/Vec3>

///\todo Make this pure virtual once sg has been removed
namespace dtCore
{
   ///A base class for all Environmental Effects
   class DT_EXPORT EnvEffect : public dtCore::DeltaDrawable
   {
   public:
      DECLARE_MANAGEMENT_LAYER(EnvEffect)

      EnvEffect( const std::string& name = 0 );
      virtual ~EnvEffect();

      // Must override this to supply the repainting routine
      virtual void Repaint(   const osg::Vec3& skyColor, const osg::Vec3& fogColor,
                              double sunAngle, double sunAzimuth,
                              double visibility );

   };
}

#endif // DELTA_ENVEFFECT
