/* -*-c++-*-
 * dtPhysics
 * Copyright 2014, David Guthrie
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
 *
 * David Guthrie
 */

#ifndef GEOMETRYVISITOR_H_
#define GEOMETRYVISITOR_H_

#include <dtPhysics/trianglerecorder.h>

#include <osg/NodeVisitor>
#include <osg/TriangleFunctor>
#include <osg/NodeVisitor>
#include <osg/Geode>
#include <osg/Geometry>

namespace dtPhysics
{
   template< class T>
   class TriangleRecorderVisitor : public osg::NodeVisitor
   {
   public:

      bool mExportSpecificMaterial, mSkipSpecificMaterial;

      std::string mSpecificDescription;
      std::string mCurrentDescription;
      osg::TriangleFunctor<T> mFunctor;
      TriangleRecorder::MaterialLookupFunc mMaterialLookup;
      TriangleRecorder::MaterialNameFilterFunc mMaterialNameFilter;

      TriangleRecorderVisitor(TriangleRecorder::MaterialLookupFunc func);
      virtual ~TriangleRecorderVisitor() {}

      // Returns a description string that may be contained in the specified node.
      static std::string GetDescription(const osg::Node& node);

      // Returns the nearest parent description.
      static std::string CheckDescriptionInAncestors(const osg::Node& node);

      // Acquires the current description from the specified node.
      void CheckDesc(osg::Node& node);

      virtual dtPhysics::MaterialIndex GetMaterialID(const std::string& str);

      virtual std::string GetMaterialNameFiltered(const std::string& str);

      void apply(osg::Node& node) override;

      void apply(osg::Group& gnode) override;

      /**
      * Applies this visitor to a geode.
      *
      * @param node the geode to visit
      */
      void apply(osg::Geode& node) override;

      void apply(osg::Billboard& node) override;
   };
}

#include <dtPhysics/trianglerecordervisitor.inl>


#endif /* GEOMETRYVISITOR_H_ */
