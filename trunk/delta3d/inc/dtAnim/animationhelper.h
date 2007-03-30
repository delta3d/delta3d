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
 * Bradley Anderegg 03/30/2007
 */

#ifndef __DELTA_ANIMATIONHELPER_H__
#define __DELTA_ANIMATIONHELPER_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>

#include <osg/Referenced>

#include <string>
#include <vector>

namespace osg
{
   class Geode;
}

namespace dtDAL
{
   class ActorProperty;
   class ActorProxy;
}

namespace dtAnim
{
   class Cal3DAnimator;
   class AnimNodeBuilder;

class	DT_ANIM_EXPORT AnimationHelper: public osg::Referenced
{

public:
   AnimationHelper(AnimNodeBuilder* pBuilder);

   osg::Geode* GetGeode();
   Cal3DAnimator* GetAnimator();

   void LoadModel(const std::string& pFilename);

   virtual void GetActorProperties(dtDAL::ActorProxy& pProxy, std::vector<dtCore::RefPtr<dtDAL::ActorProperty> >& pFillVector);      


protected:
   virtual ~AnimationHelper();

private:

   dtCore::RefPtr<osg::Geode> mGeode;
   dtCore::RefPtr<Cal3DAnimator> mAnimator;
   dtCore::RefPtr<AnimNodeBuilder> mNodeBuilder;
   
};

}//namespace dtAnim

#endif // __DELTA_ANIMATIONHELPER_H__

