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

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "NodeHighlighter.h"
#include <algorithm>



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
NodeHighlighter::NodeHighlighter()
   : mCacheEnabled(true)
{}

NodeHighlighter::~NodeHighlighter()
{
   Clear();
}

void NodeHighlighter::SetCacheEnabled(bool enableCache)
{
   mCacheEnabled = enableCache;
}

bool NodeHighlighter::IsCacheEnabled() const
{
   return mCacheEnabled;
}

bool NodeHighlighter::IsHighlightEnabled(osg::Node& node) const
{
   return mAxisMap.find(&node) == mAxisMap.end();
}

void NodeHighlighter::SetHighlightEnabled(osg::Node& node, bool enableHighlight)
{
   NodeAxisMap::iterator foundIter = mAxisMap.find(&node);

   // Determine if a highlight is to be removed.
   if ( ! enableHighlight)
   {
      if (foundIter != mAxisMap.end())
      {
         dtCore::RefPtr<dtCore::PointAxis> axis = foundIter->second.get();
         Detach(*axis);

         // Determine if the object should be stored for later use.
         if (mCacheEnabled)
         {
            mAxisCache.push_back(axis.get());
         }

         mAxisMap.erase(foundIter);
      }
   }
   else // Add a highlight
   {
      // Only add the highlight if the node is not highlighted already.
      if (foundIter == mAxisMap.end())
      {
         dtCore::RefPtr<dtCore::PointAxis> axis;

         // Determine if a new object should be created.
         if (mAxisCache.empty())
         {
            axis = new dtCore::PointAxis;
            axis->SetLength(dtCore::PointAxis::X, 1.0f);
            axis->SetLength(dtCore::PointAxis::Y, 1.0f);
            axis->SetLength(dtCore::PointAxis::Z, 1.0f);
            axis->Enable(dtCore::PointAxis::LABEL_X);
            axis->Enable(dtCore::PointAxis::LABEL_Y);
            axis->Enable(dtCore::PointAxis::LABEL_Z);
         }
         else // Re-use a highlight object.
         {
            axis = mAxisCache.front().get();
            mAxisCache.pop_front();
         }

         mAxisMap.insert(std::make_pair(&node, axis.get()));

         Attach(*axis, node);
      }
   }
}

void NodeHighlighter::SetHighlightsEnabled(OsgNodePtrVector& nodes, bool enableHighlight)
{
   std::for_each(nodes.begin(), nodes.end(),
      [&](dtCore::RefPtr<osg::Node> node)
      {
         SetHighlightEnabled(*node, enableHighlight);
      });
}

void NodeHighlighter::Detach(dtCore::PointAxis& axis)
{
   osg::Node* axisNode = axis.GetOSGNode();
   while (axisNode->getNumParents() > 0)
   {
      osg::Group* parent = axisNode->getParent(0);
      parent->removeChild(axisNode);
   }
}

void NodeHighlighter::Attach(dtCore::PointAxis& axis, osg::Node& node)
{
   osg::Node* axisNode = axis.GetOSGNode();
   
   Detach(axis);

   osg::Group* newParent = dynamic_cast<osg::Group*>(&node);
   if (newParent != nullptr)
   {
      newParent->addChild(axisNode);
   }
}

void NodeHighlighter::SetHighlightsDisabled()
{
   DetachAll(mCacheEnabled);
}

void NodeHighlighter::DetachAll(bool cache)
{
   NodeAxisMap::iterator curIter = mAxisMap.begin();
   NodeAxisMap::iterator endIter = mAxisMap.end();
   for (; curIter != endIter; ++curIter)
   {
      dtCore::RefPtr<dtCore::PointAxis> axis = curIter->second.get();
      Detach(*axis);

      // Determine if the object should be stored for later use.
      if (cache)
      {
         mAxisCache.push_back(axis.get());
      }
   }

   mAxisMap.clear();
}

void NodeHighlighter::Clear()
{
   // Detach all objects and make sure nothing is cached.
   DetachAll(false);
}
