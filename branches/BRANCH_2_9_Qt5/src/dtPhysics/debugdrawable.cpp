/* -*-c++-*-
 * dtPhysics
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * david
 */

#include <dtPhysics/debugdrawable.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/NodeCallback>
#include <osgText/Text>
#include <pal/palDebugDraw.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/palutil.h>

#include <cfloat>

namespace dtPhysics
{

   class DebugDrawableUpdateCallback: public osg::NodeCallback
   {
   public:
      DebugDrawableUpdateCallback(DebugDrawable& drawable) : mDrawable(drawable) {}
      /** Callback method called by the NodeVisitor when visiting a node.*/
      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         DebugDraw* debug = PhysicsWorld::GetInstance().GetDebugDraw();
         if (debug != NULL)
         {
            mDrawable.Update(*debug);
         }
         traverse(node,nv);
      }
      DebugDrawable& mDrawable;
   };

   ////////////////////////////////////////////////////////
   DebugDrawable::DebugDrawable()
   : mDebugGroup(new osg::Group)
   {
      mDebugGroup->setName("PhysicsDebugSubGraph");
      dtCore::RefPtr<osg::Geode> geode = new osg::Geode;
      mDebugGroup->addChild(geode.get());

      osg::StateSet* ss = geode->getOrCreateStateSet();

      ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
//      dtCore::RefPtr<osg::BlendFunc> trans = new osg::BlendFunc();
//      trans->setFunction( osg::BlendFunc::SRC_ALPHA ,osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
//      ss->setAttributeAndModes(trans.get());
//      ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

      ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      ss->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OFF);

      mLines = new osg::Geometry;
      mTriangles = new osg::Geometry;
      mPoints = new osg::Geometry;

      geode->addDrawable(mLines.get());
      geode->addDrawable(mTriangles.get());
      geode->addDrawable(mPoints.get());

      mLines->setUseDisplayList(false);
      mLines->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      mLines->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 0));

      mTriangles->setUseDisplayList(false);
      mTriangles->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      mTriangles->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 0));

      mPoints->setUseDisplayList(false);
      mPoints->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
      mPoints->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 0));

      mDebugGroup->setUpdateCallback(new DebugDrawableUpdateCallback(*this));
   }

   ////////////////////////////////////////////////////////
   DebugDrawable::~DebugDrawable()
   {
   }

   ////////////////////////////////////////////////////////
   osg::Node* DebugDrawable::GetOSGNode()
   {
      return mDebugGroup.get();
   }

   ////////////////////////////////////////////////////////
   const osg::Node* DebugDrawable::GetOSGNode() const
   {
      return mDebugGroup.get();
   }

   ////////////////////////////////////////////////////////
   void DebugDrawable::Update(dtPhysics::DebugDraw& debugDraw)
   {
      // Change the range for the NEXT frame, not this frame.
      VectorTypeToPalVec(debugDraw.m_vRefPoint, mReferencePos);

      UpdateGeometry(*mPoints, debugDraw.m_Points);
      UpdateGeometry(*mLines, debugDraw.m_Lines);
      UpdateGeometry(*mTriangles, debugDraw.m_Triangles);
   }

   ////////////////////////////////////////////////////////
   void DebugDrawable::SetReferencePosition(const osg::Vec3& referencePos)
   {
      mReferencePos = referencePos;
   }

   ////////////////////////////////////////////////////////
   void DebugDrawable::GetReferencePosition(osg::Vec3& referencePosOut) const
   {
      referencePosOut = mReferencePos;
   }

   ////////////////////////////////////////////////////////
   static void Convert(osg::Vec4& vec4, const palVector4& pv4)
   {
      for (unsigned i = 0; i < 4; ++i)
      {
         vec4[i] = pv4[i];
      }
   }

   ////////////////////////////////////////////////////////
   static void Convert(osg::Vec3& vec3, const palVector3& pv3)
   {
      for (unsigned i = 0; i < 3; ++i)
      {
         vec3[i] = pv3[i];
      }
   }

   ////////////////////////////////////////////////////////
   /*static void Convert(int& i, const int newi)
   {
      i = newi;
   }*/

   template <typename ArrayType, typename PalElemType>
      void UpdateArray(dtCore::RefPtr<ArrayType>& array, const PAL_VECTOR<PalElemType>& palArray)
   {
      typedef typename ArrayType::ElementDataType ElementDataType;
      if (!array.valid())
      {
         array = new ArrayType;
      }

      array->clear();
      array->reserve(palArray.size());

      typename PAL_VECTOR<PalElemType>::const_iterator i, iend;
      i = palArray.begin();
      iend = palArray.end();
      for (; i != iend; ++i)
      {
         const PalElemType& pv = *i;
         ElementDataType element;
         Convert(element, pv);
         //LOG_ALWAYS("Adding Item to render. " + dtUtil::ToString(element));
         array->push_back(element);
      }

   }

   ////////////////////////////////////////////////////////
   void DebugDrawable::UpdateGeometry(osg::Geometry& geomDrawable, palDebugGeometry& debugGeom)
   {
      dtCore::RefPtr<osg::Vec4Array> colors = dynamic_cast<osg::Vec4Array*>(geomDrawable.getColorArray());
      UpdateArray(colors, debugGeom.m_vColors);
      geomDrawable.setColorArray(colors.get());

      dtCore::RefPtr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(geomDrawable.getVertexArray());
      UpdateArray(vertices, debugGeom.m_vVertices);

      // expand vertices if needed
      if (vertices.valid() && !debugGeom.m_vIndices.empty())
      {
         osg::Vec3Array* vertices_lookup = vertices.get();
         osg::Vec3Array* vertices_expanded = new osg::Vec3Array;
         for (unsigned i = 0; i < debugGeom.m_vIndices.size(); ++i)
         {
            int index = debugGeom.m_vIndices[i];
            vertices_expanded->push_back((*vertices_lookup)[index]);
         }
         vertices = vertices_expanded;
      }
      geomDrawable.setVertexArray(vertices.get());

      /*
      if (debugGeom.m_vIndices.empty())
      {
         geomDrawable.setVertexIndices(NULL);
      }
      else
      {
         dtCore::RefPtr<osg::IntArray> indices = dynamic_cast<osg::IntArray*>(geomDrawable.getVertexIndices());
         UpdateArray(indices, debugGeom.m_vIndices);
         geomDrawable.setVertexIndices(indices.get());
      }
      */

      osg::DrawArrays* drawArrays = static_cast<osg::DrawArrays*>(geomDrawable.getPrimitiveSet(0));
      drawArrays->setCount(debugGeom.m_vVertices.size());
      drawArrays->dirty();
    }
}
