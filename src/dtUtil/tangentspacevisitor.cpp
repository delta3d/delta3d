/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
 * Portions taken from OpenSceneGraph
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
 */
#include <prefix/dtutilprefix.h>
#include <dtUtil/tangentspacevisitor.h>

#include <osg/Program>
#include <osg/Geode>

#include <osgUtil/TangentSpaceGenerator>
#include <dtCore/refptr.h>

namespace dtUtil
{
   // constructor
   TangentSpaceVisitor::TangentSpaceVisitor(const std::string& vertexAttributeName, 
         osg::Program* shaderProgram, int tangentVertexAttribNumber)
         : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN)
         , mVertexAttributeName(vertexAttributeName)
         , mShaderProgram(shaderProgram)
         , mTangentVertexAttribNumber(tangentVertexAttribNumber)
   {
   }

   // apply - called for each node
   void TangentSpaceVisitor::apply(osg::Geode& geode)
   {
      for (unsigned i = 0; i < geode.getNumDrawables(); ++i) 
      {
         osg::Geometry* geo = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
         if (geo != NULL) 
         {
            // if the tangents are not already set, then put the tangents on.
            if (!geo->getVertexAttribArray(mTangentVertexAttribNumber))
            {
               dtCore::RefPtr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
               tsg->generate(geo, 0);
               geo->setVertexAttribArray(mTangentVertexAttribNumber, tsg->getTangentArray());
               geo->setVertexAttribBinding(mTangentVertexAttribNumber, osg::Geometry::BIND_PER_VERTEX);
            }
            // old code left in in case you eventually want to put normals and binormals. 
            // binormals are typically generated in the shader anyway, since its a simple bit of math.
            //if (!geo->getVertexAttribArray(7))
            //{
            //   geo->setVertexAttribArray(7, tsg->getBinormalArray());
            //   geo->setVertexAttribBinding(7, osg::Geometry::BIND_PER_VERTEX);
            //}
            //if (!geo->getVertexAttribArray(15))
            //{
            //   geo->setVertexAttribArray(15, tsg->getNormalArray());
            //   geo->setVertexAttribBinding(15, osg::Geometry::BIND_PER_VERTEX);
            //}

            // give the attribute a name so you can use it in your vertex shader.
            if (mShaderProgram != NULL)
            {
               mShaderProgram->addBindAttribLocation(mVertexAttributeName, mTangentVertexAttribNumber);
            }
         }
      }

      NodeVisitor::apply(geode);
   }
}
