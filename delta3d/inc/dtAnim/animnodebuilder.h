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

#ifndef __DELTA_ANIMNODEBUILDER_H__
#define __DELTA_ANIMNODEBUILDER_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>

#include <osg/Referenced>
#include <osg/Node> //needed for the bounding sphere callback

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Program;
}
/// @endcond

namespace dtCore
{
   class Shader;
}

namespace dtAnim
{
   class Cal3DModelWrapper;
   class Cal3DModelData;

class	DT_ANIM_EXPORT AnimNodeBuilder: public osg::Referenced
{
public:
   typedef dtUtil::Functor<dtCore::RefPtr<osg::Geode>, TYPELIST_1(Cal3DModelWrapper*)> CreateFunc;

   class Cal3DBoundingSphereCalculator : public osg::Node::ComputeBoundingSphereCallback
   {
      public:
         Cal3DBoundingSphereCalculator(Cal3DModelWrapper& wrapper);
         
         /*virtual*/ osg::BoundingSphere computeBound(const osg::Node&) const;
      private:
         dtCore::RefPtr<Cal3DModelWrapper> mWrapper;
   };
   
   AnimNodeBuilder(); //creates default builder
   AnimNodeBuilder(const CreateFunc& pCreate); //uses custom builder

   /// @return the create function
   CreateFunc& GetCreate();
   void SetCreate(const CreateFunc& pCreate);

   dtCore::RefPtr<osg::Geode> CreateGeode(Cal3DModelWrapper* pWrapper);

   virtual dtCore::RefPtr<osg::Geode> CreateSoftware(Cal3DModelWrapper* pWrapper);
   virtual dtCore::RefPtr<osg::Geode> CreateHardware(Cal3DModelWrapper* pWrapper);

protected:
   virtual ~AnimNodeBuilder();
   AnimNodeBuilder(const AnimNodeBuilder&);
   AnimNodeBuilder& operator=(const AnimNodeBuilder&);
   
   dtCore::Shader* AnimNodeBuilder::LoadShaders(Cal3DModelData& modelData, osg::Geode& geode) const;
   
private:

   CreateFunc mCreateFunc;

};

}//namespace dtAnim

#endif // __DELTA_ANIMNODEBUILDER_H__
