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

#include <cal3d/global.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Program;
}
/// @endcond

namespace dtCore
{
   class ShaderProgram;
}

class CalHardwareModel;


namespace dtAnim
{
   class Cal3DModelWrapper;
   class Cal3DModelData;
   class Array;

class	DT_ANIM_EXPORT AnimNodeBuilder: public osg::Referenced
{
public:
   typedef dtUtil::Functor<dtCore::RefPtr<osg::Node>, TYPELIST_1(Cal3DModelWrapper*)> CreateFunc;

   class DT_ANIM_EXPORT Cal3DBoundingSphereCalculator : public osg::Node::ComputeBoundingSphereCallback
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

   dtCore::RefPtr<osg::Node> CreateNode(Cal3DModelWrapper* pWrapper);

   virtual dtCore::RefPtr<osg::Node> CreateSoftware(Cal3DModelWrapper* pWrapper);
   virtual dtCore::RefPtr<osg::Node> CreateHardware(Cal3DModelWrapper* pWrapper);



protected:
   virtual ~AnimNodeBuilder();
   AnimNodeBuilder(const AnimNodeBuilder&);
   AnimNodeBuilder& operator=(const AnimNodeBuilder&);
   
   dtCore::ShaderProgram* LoadShaders(Cal3DModelData& modelData, osg::Geode& geode) const;
   
private:

   template <typename T>
   class Array
   {
   public:
      typedef T value_type;

      Array(size_t size = 0): mArray(NULL)
      {
         if (size > 0)
            mArray = new T[size];
      }

      ~Array()
      {
         delete[] mArray;
      }

      T& operator[](size_t index)
      {
         return mArray[index];
      }

      T* mArray;
   };


   CreateFunc mCreateFunc;

   void CalcNumVertsAndIndices( Cal3DModelWrapper* pWrapper,
                                int &numVerts, int &numIndices );

   void InvertTextureCoordinates( CalHardwareModel* hardwareModel, const size_t stride,
                                 float* vboVertexAttr, Cal3DModelData* modelData,
                                 Array<CalIndex> &indexArray );
};

}//namespace dtAnim

#endif // __DELTA_ANIMNODEBUILDER_H__
