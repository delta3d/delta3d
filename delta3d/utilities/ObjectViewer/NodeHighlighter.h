/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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

#ifndef DELTA_NODE_HIGHLIGHTER_H
#define DELTA_NODE_HIGHLIGHTER_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtCore/pointaxis.h>
#include <dtQt/typedefs.h>



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class NodeHighlighter : public osg::Referenced
{
public:
   NodeHighlighter();

   void SetCacheEnabled(bool enableCache);
   bool IsCacheEnabled() const;

   bool IsHighlightEnabled(osg::Node& node) const;

   void SetHighlightEnabled(osg::Node& node, bool enableHighlight);

   void SetHighlightsEnabled(OsgNodePtrVector& nodes, bool enableHighlight);

   void SetHighlightsDisabled();

   void Clear();

protected:
   virtual ~NodeHighlighter();

   void DetachAll(bool cache);

   void Detach(dtCore::PointAxis& axis);
   void Attach(dtCore::PointAxis& axis, osg::Node& node);

   bool mCacheEnabled;

   typedef std::map<osg::Node*, dtCore::RefPtr<dtCore::PointAxis>> NodeAxisMap;
   NodeAxisMap mAxisMap;

   // Potentially a lot of nodes may be highlighted.
   // This cache allows recycling objects that were already allocated.
   typedef std::list<dtCore::RefPtr<dtCore::PointAxis>> AxisList;
   AxisList mAxisCache;
};

#endif
