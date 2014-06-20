/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 03/28/2007
 */

#ifndef __DELTA_MORPHDRIVER_H__
#define __DELTA_MORPHDRIVER_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtAnim/cal3danimator.h>
#include <dtAnim/ical3ddriver.h>

namespace dtAnim
{
class DT_ANIM_EXPORT MorphDriver: public ICal3DDriver
{

public:
   MorphDriver(Cal3DAnimator* animator);

   void Update(double dt);
   void SetAnimator(Cal3DAnimator* animator);

protected:
   virtual ~MorphDriver();

private:
   dtCore::RefPtr<Cal3DAnimator> mAnimator;
};

} // namespace dtAnim

#endif // __DELTA_MORPHDRIVER_H__

