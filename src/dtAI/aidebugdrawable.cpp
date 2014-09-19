/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 Alion Science and Technology
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
 * Bradley Anderegg
 */


#include <dtAI/aidebugdrawable.h>
#include <dtAI/waypointinterface.h>
#include <dtAI/waypointrenderinfo.h>
#include <dtAI/waypointpair.h>
#include <dtAI/navmesh.h>

#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/mathdefines.h>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>

#include <osg/Point>
#include <osg/LineWidth>

#include <osgText/Text>

#include <dtUtil/hashmap.h>

namespace dtAI
{
   class RenderData: public osg::Referenced
   {
   public:
      dtCore::RefPtr<osgText::Text> mTextNode;
   };

   typedef dtUtil::HashMap<unsigned, dtCore::RefPtr<RenderData> > RenderDataMap;

   class AIDebugDrawableImpl
   {
   public:

      //////////////////////////////////////////////////////////////////////////
      AIDebugDrawableImpl(WaypointRenderInfo& pRenderInfo)
   : mRenderInfo(&pRenderInfo)
   {
   }

      //////////////////////////////////////////////////////////////////////////
      void Init()
      {
         mNode = new osg::Group();
         mGeodeWayPoints = new osg::Geode();
         mGeodeNavMesh = new osg::Geode();
         mGeodeIDs = new osg::Geode();
         mWaypointGeometry = new osg::Geometry();
         mNavMeshGeometry = new osg::Geometry();

         mWaypointIDs = new osg::UIntArray();
         mWaypointPairs = new osg::UIntArray();
         mVerts = new osg::Vec3Array();
         mWaypointColors = new osg::Vec4Array();

         mWaypointGeometry->setVertexArray(mVerts.get());
         mWaypointGeometry->setColorArray(mWaypointColors.get());
         mNavMeshGeometry->setVertexArray(mVerts.get());

         mNode->addChild(mGeodeWayPoints.get());
         mNode->addChild(mGeodeNavMesh.get());
         mNode->addChild(mGeodeIDs.get());

         mGeodeNavMesh->addDrawable(mNavMeshGeometry.get());
         mGeodeWayPoints->addDrawable(mWaypointGeometry.get());

         mWaypointGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
         mNavMeshGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

         // Don't light waypoint data
         mNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

         mRenderInfo->Init();
      }

      //////////////////////////////////////////////////////////////////////////
      void OnRenderInfoChanged()
      {
         // Turn on or off by setting the node mask
         mGeodeIDs->setNodeMask(mRenderInfo->GetRenderWaypointID() ? ~0: 0);
         mGeodeNavMesh->setNodeMask(mRenderInfo->GetRenderNavMesh() ? ~0: 0);
         mGeodeWayPoints->setNodeMask(mRenderInfo->GetRenderWaypoints() ? ~0: 0);

         osg::StateAttribute::Values depthAttrib =
               (mRenderInfo->GetEnableDepthTest()) ? osg::StateAttribute::ON : osg::StateAttribute::OFF;

         // Allow waypoint data to be obscured or always visible?
         mNode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, depthAttrib);

         ResetWaypointColorsToDefault();

         osg::Vec4Array* navmeshColors = new osg::Vec4Array(1);
         (*navmeshColors)[0] = mRenderInfo->GetNavMeshColor();
         mNavMeshGeometry->setColorArray(navmeshColors);

         osg::Point* p = new osg::Point(mRenderInfo->GetWaypointSize());
         mWaypointGeometry->getOrCreateStateSet()->setAttribute(p, osg::StateAttribute::ON);

         osg::LineWidth* lw = new osg::LineWidth(mRenderInfo->GetNavMeshWidth());
         mNavMeshGeometry->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);
      }

      //////////////////////////////////////////////////////////////////////////
      void OnGeometryChanged()
      {
         //seems like there should be a generic dirty()- does this only work with display lists?
         mWaypointGeometry->dirtyDisplayList();
         mWaypointGeometry->setVertexArray(mVerts.get());
         mWaypointGeometry->setColorArray(mWaypointColors.get());

         if (mWaypointGeometry->getNumPrimitiveSets() > 0)
         {
            mWaypointGeometry->removePrimitiveSet(0);
         }

         if(!mVerts->empty())
         {
            osg::PrimitiveSet* ps = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, mVerts->size());
            mWaypointGeometry->addPrimitiveSet(ps);
            //setting it back to zero will ensure any user data does not get removed when this function is called again
            mWaypointGeometry->setPrimitiveSet(0, ps);
         }

         if (mNavMeshGeometry->getNumPrimitiveSets() > 0)
         {
            mNavMeshGeometry->removePrimitiveSet(0);
         }

         if(!mWaypointPairs->empty())
         {
            osg::PrimitiveSet* psLines = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, mWaypointPairs->begin(), mWaypointPairs->end());
            mNavMeshGeometry->addPrimitiveSet(psLines);
            //setting it back to zero will ensure any user data does not get removed when this function is called again
            mNavMeshGeometry->setPrimitiveSet(0, psLines);
         }

         osg::Point* p = new osg::Point(mRenderInfo->GetWaypointSize());
         mWaypointGeometry->getOrCreateStateSet()->setAttribute(p, osg::StateAttribute::ON);

         osg::LineWidth* lw = new osg::LineWidth(mRenderInfo->GetNavMeshWidth());
         mNavMeshGeometry->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);
      }

      //////////////////////////////////////////////////////////////////////////
      void CreateWaypointIDText(const WaypointInterface& wp, RenderData& renderData)
      {
         //note: we either create a render info in the constructor
         //or we take one by reference so should never be NULL

         osgText::Text* text = new osgText::Text();
         text->setDrawMode(osgText::Text::TEXT);
         text->setAlignment(osgText::TextBase::CENTER_CENTER);
         text->setAxisAlignment(osgText::TextBase::SCREEN);
         text->setAutoRotateToScreen(true);
         text->setFont(mRenderInfo->GetWaypointFontFile());
         text->setCharacterSize(mRenderInfo->GetWaypointFontSizeScalar());
         text->setColor(mRenderInfo->GetWaypointFontColor());
         text->setPosition(wp.GetPosition() + mRenderInfo->GetWaypointTextOffset());
         text->setText(dtUtil::ToString(wp.GetID()));
         renderData.mTextNode = text;
         mGeodeIDs->addDrawable(text);
      }

      //////////////////////////////////////////////////////////////////////////
      //set an individual waypoint render color
      void SetWaypointColor(unsigned int waypointIndex, const osg::Vec4& color)
      {
         if (waypointIndex < mWaypointColors->size())
         {
            (*mWaypointColors)[waypointIndex] = color;
            mWaypointGeometry->setColorArray(mWaypointColors.get());
         }
      }

      //////////////////////////////////////////////////////////////////////////
      //reset all waypoint colors back to the value in the WaypointRenderInfo
      void ResetWaypointColorsToDefault()
      {
         const WaypointRenderInfo::Color color = mRenderInfo->GetWaypointColor();
         for (size_t colorIndex = 0; colorIndex < mWaypointColors->size(); ++colorIndex)
         {
            (*mWaypointColors)[colorIndex] = color;
         }

         mWaypointGeometry->setColorArray(mWaypointColors);
      }

      dtCore::RefPtr<WaypointRenderInfo> mRenderInfo;

      dtCore::RefPtr<osg::UIntArray> mWaypointIDs;
      dtCore::RefPtr<osg::Vec3Array> mVerts;
      dtCore::RefPtr<osg::Vec4Array> mWaypointColors;
      dtCore::RefPtr<osg::UIntArray> mWaypointPairs;
      dtCore::RefPtr<osg::Geometry> mWaypointGeometry;
      dtCore::RefPtr<osg::Geometry> mNavMeshGeometry;
      dtCore::RefPtr<osg::Geode> mGeodeNavMesh;
      dtCore::RefPtr<osg::Geode> mGeodeWayPoints;
      dtCore::RefPtr<osg::Geode> mGeodeIDs;
      dtCore::RefPtr<osg::Group> mNode;

      RenderDataMap mRenderData;
   };

   /////////////////////////////////////////////////////////////////////////////
   //AIDebugDrawable
   /////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::AIDebugDrawable()
   : mImpl(new AIDebugDrawableImpl(*new WaypointRenderInfo))
   {
      Init();
   }

   /////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::AIDebugDrawable(WaypointRenderInfo& pRenderInfo)
   : mImpl(new AIDebugDrawableImpl(pRenderInfo))
   {
      Init();
   }

   /////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::~AIDebugDrawable()
   {
      delete mImpl;
      mImpl = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetRenderInfo(WaypointRenderInfo* pRenderInfo)
   {
      if (pRenderInfo != NULL)
      {
         mImpl->mRenderInfo = pRenderInfo;

         mImpl->OnRenderInfoChanged();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointRenderInfo* AIDebugDrawable::GetRenderInfo()
   {
      return mImpl->mRenderInfo;
   }

   /////////////////////////////////////////////////////////////////////////////
   const WaypointRenderInfo* AIDebugDrawable::GetRenderInfoConst() const
   {
      return mImpl->mRenderInfo;
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::OnRenderInfoChanged()
   {
      mImpl->OnRenderInfoChanged();
   }

   class RenderInfoUpdateCallback : public osg::NodeCallback
   {
   public:
      RenderInfoUpdateCallback(AIDebugDrawable& debugDraw)
      :mAIDebugDrawable(&debugDraw)
      {

      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         mAIDebugDrawable->OnRenderInfoChanged();
         // note, callback is responsible for scenegraph traversal so
         // they must call traverse(node,nv) to ensure that the
         // scene graph subtree (and associated callbacks) are traversed.
         traverse(node,nv);
      }
   private:
      dtCore::RefPtr<AIDebugDrawable> mAIDebugDrawable;
   };

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::Init()
   {
      mImpl->Init();
      mImpl->OnRenderInfoChanged();
      GetOSGNode()->setUpdateCallback(new RenderInfoUpdateCallback(*this));
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearMemory()
   {
      mImpl->mVerts->clear();
      mImpl->mWaypointIDs->clear();
      mImpl->mWaypointColors->clear();

      ClearText();

      // Clears waypoint pairs
      ClearWaypointGraph();

      // Existing primitive sets will be cleared here (mWaypointGeometry,mNavMeshGeometry)
      OnGeometryChanged();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool AIDebugDrawable::HasWaypointData() const
   {
      return !mImpl->mWaypointIDs->empty();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Geode* AIDebugDrawable::GetGeodeNavMesh()
   {
      return mImpl->mGeodeNavMesh.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetGeodeNavMesh(osg::Geode* geode)
   {
      mImpl->mGeodeNavMesh = geode;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Geode* AIDebugDrawable::GetGeodeWayPoints()
   {
      return mImpl->mGeodeWayPoints.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetGeodeWayPoints(osg::Geode* geode)
   {
      mImpl->mGeodeWayPoints = geode;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* AIDebugDrawable::GetOSGNode()
   {
      return mImpl->mNode.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Node* AIDebugDrawable::GetOSGNode() const
   {
      return mImpl->mNode.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetOSGNode(osg::Group* grp)
   {
      mImpl->mNode = grp;
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Geometry* AIDebugDrawable::GetGeometry()
   {
      return mImpl->mWaypointGeometry.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   int AIDebugDrawable::FindWaypoint(unsigned id)
   {
      unsigned numWaypoints = mImpl->mWaypointIDs->size();
      for(unsigned count = 0; count < numWaypoints; ++count)
      {
         if((*mImpl->mWaypointIDs)[count] == id)
         {
            return count;
         }
      }

      return -1;
   }


   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetWaypoints(const std::vector<dtAI::WaypointInterface*>& wpArray, bool createText /*= true*/)
   {
      // Make sure we start with a clean slate
      ClearMemory();

      typedef std::vector<dtAI::WaypointInterface*> WaypointArray;

      //now we must add all current waypoints
      WaypointArray::const_iterator iter = wpArray.begin();
      WaypointArray::const_iterator iterEnd = wpArray.end();

      for (;iter != iterEnd; ++iter)
      {
         mImpl->mWaypointIDs->push_back((*iter)->GetID());
         mImpl->mVerts->push_back((*iter)->GetPosition());
         mImpl->mWaypointColors->push_back(mImpl->mRenderInfo->GetWaypointColor());

         dtCore::RefPtr<RenderData> newRenderData = new RenderData;
         mImpl->mRenderData.insert(std::make_pair((*iter)->GetID(), newRenderData));

         if (createText)
         {
            mImpl->CreateWaypointIDText(**iter, *newRenderData);
         }
      }

      OnGeometryChanged();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetEdges(const std::vector<dtAI::WaypointPair>& edgeArray)
   {
      ClearWaypointGraph();
      AddEdges(edgeArray);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetText(const std::vector<dtAI::WaypointInterface*>& wpArray)
   {
      ClearText();

      for (size_t waypointIndex = 0; waypointIndex < wpArray.size(); ++waypointIndex)
      {
         dtCore::RefPtr<RenderData> newRenderData = new RenderData;
         mImpl->mRenderData.insert(std::make_pair(wpArray[waypointIndex]->GetID(), newRenderData));
         mImpl->CreateWaypointIDText(*wpArray[waypointIndex], *newRenderData);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::InsertWaypoint(const WaypointInterface& wp, bool addText /*= true*/)
   {
      // FindWaypoint can be slow when alot of waypoints have accumulated
      int loc = FindWaypoint(wp.GetID());
      if (loc > -1)
      {
         // we already have this waypoint so lets make sure its in the right place
         (*mImpl->mVerts)[loc].set(wp.GetPosition());

         if (addText)
         {
            RenderData* rd = mImpl->mRenderData[wp.GetID()];
            if (rd && rd->mTextNode.valid())
            {
               rd->mTextNode->setPosition(wp.GetPosition() + mImpl->mRenderInfo->GetWaypointTextOffset());
            }
         }
      }
      else // lets add it to the existing waypoints
      {
         mImpl->mWaypointIDs->push_back(wp.GetID());
         mImpl->mVerts->push_back(wp.GetPosition());
         mImpl->mWaypointColors->push_back(mImpl->mRenderInfo->GetWaypointColor());

         if (addText)
         {
            dtCore::RefPtr<RenderData> newRenderData = new RenderData;
            mImpl->mRenderData.insert(std::make_pair(wp.GetID(), newRenderData));
            mImpl->CreateWaypointIDText(wp, *newRenderData);
         }
      }

      OnGeometryChanged();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemoveWaypoint(unsigned id)
   {
      RemoveWaypoints(std::vector<unsigned int>(1, id));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemoveWaypoints(const std::vector<unsigned int>& idList)
   {
      for (size_t idIndex = 0; idIndex < idList.size(); ++idIndex)
      {
         int loc = FindWaypoint(idList[idIndex]);
         if (loc > -1)
         {
            //since this data is easily copied we can perform a faster erase
            //we simply copy the last element to the place of the element to be removed
            //and then we pop off the last element
            //it should be noted that order is not preserved.... if this matters we will need to revisit this
            (*mImpl->mVerts)[loc].set((*mImpl->mVerts)[mImpl->mVerts->size() - 1]);
            (*mImpl->mWaypointIDs)[loc] = (*mImpl->mWaypointIDs)[mImpl->mWaypointIDs->size() - 1];
            (*mImpl->mWaypointColors)[loc] = (*mImpl->mWaypointColors)[mImpl->mWaypointColors->size() - 1];

            mImpl->mVerts->pop_back();
            mImpl->mWaypointIDs->pop_back();
            mImpl->mWaypointColors->pop_back();

            RenderDataMap::iterator i = mImpl->mRenderData.find(idList[idIndex]);
            if (i != mImpl->mRenderData.end())
            {
               mImpl->mGeodeIDs->removeDrawable(i->second->mTextNode.get());
               mImpl->mRenderData.erase(i);
            }
         }
      }

      OnGeometryChanged();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::AddEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      AddPathSegment(pFrom, pTo);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::AddEdges(const std::vector<WaypointPair>& pairs)
   {
      for (size_t pairIndex = 0; pairIndex < pairs.size(); ++pairIndex)
      {
         AddEdge(pairs[pairIndex].GetWaypointFrom(), pairs[pairIndex].GetWaypointTo());
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemoveEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      RemovePathSegment(pFrom, pTo);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemoveAllEdges(const WaypointInterface* pFrom)
   {
      RemovePathSegments(pFrom);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::UpdateWaypointGraph(const NavMesh& nm)
   {
      //just clear and re-add them all, this should only happen often during editing
      ClearWaypointGraph();

      NavMesh::NavMeshContainer::const_iterator iter = nm.GetNavMesh().begin();
      NavMesh::NavMeshContainer::const_iterator iterEnd = nm.GetNavMesh().end();

      for (;iter != iterEnd; ++iter)
      {
         const WaypointPair* wp = (*iter).second;
         AddPathSegment(wp->GetWaypointFrom(), wp->GetWaypointTo());
      }
      OnRenderInfoChanged();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::OnGeometryChanged()
   {
      mImpl->OnGeometryChanged();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::AddPathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      int indexFrom = FindWaypoint(pFrom->GetID());
      int indexTo = FindWaypoint(pTo->GetID());

      if (indexFrom > -1 && indexTo > -1)
      {
         mImpl->mWaypointPairs->push_back(indexFrom);
         mImpl->mWaypointPairs->push_back(indexTo);
         OnGeometryChanged();
      }
      else
      {
         LOG_ERROR("Invalid path segment");
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemovePathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo)
   {
      int indexFrom = FindWaypoint(pFrom->GetID());
      int indexTo = FindWaypoint(pTo->GetID());

      if (indexFrom > -1 && indexTo > -1)
      {
         for (size_t pairIndex = 0; pairIndex < mImpl->mWaypointPairs->size() - 1; pairIndex += 2)
         {
            if (int(mImpl->mWaypointPairs->at(pairIndex)) == indexFrom &&
                  int(mImpl->mWaypointPairs->at(pairIndex + 1)) == indexTo)
            {
               mImpl->mWaypointPairs->erase(mImpl->mWaypointPairs->begin() + (pairIndex + 1));
               mImpl->mWaypointPairs->erase(mImpl->mWaypointPairs->begin() + pairIndex);
               OnGeometryChanged();
               break;
            }
         }
      }
      else
      {
         LOG_ERROR("Invalid path segment");
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemovePathSegments(const WaypointInterface* pFrom)
   {
      // Bail out if there's nothing here
      if (!mImpl->mWaypointPairs->empty())
      {
         int indexFrom = FindWaypoint(pFrom->GetID());

         for (size_t pairIndex = 0; pairIndex < mImpl->mWaypointPairs->size() - 1; pairIndex += 2)
         {
            if (int(mImpl->mWaypointPairs->at(pairIndex)) == indexFrom ||
                  int(mImpl->mWaypointPairs->at(pairIndex + 1)) == indexFrom)
            {
               size_t currentSize = mImpl->mWaypointPairs->size();

               // Do a fast erase if possible
               if (pairIndex + 4 <= currentSize)
               {
                  (*mImpl->mWaypointPairs)[pairIndex] = mImpl->mWaypointPairs->at(currentSize - 2);
                  (*mImpl->mWaypointPairs)[pairIndex + 1] = mImpl->mWaypointPairs->at(currentSize - 1);

                  mImpl->mWaypointPairs->pop_back();
                  mImpl->mWaypointPairs->pop_back();
               }
               else
               {
                  // These must be the last 2
                  mImpl->mWaypointPairs->erase(mImpl->mWaypointPairs->begin() + (pairIndex + 1));
                  mImpl->mWaypointPairs->erase(mImpl->mWaypointPairs->begin() + pairIndex);
                  break;
               }

               // We just removed 2 so stay at the current index
               pairIndex -= 2;
            }
         }

         OnGeometryChanged();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearWaypointGraph()
   {
      mImpl->mWaypointPairs->clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetWaypointColor(const WaypointInterface& wp, const osg::Vec4& color)
   {
      int loc = FindWaypoint(wp.GetID());
      if (loc > -1)
      {
         mImpl->SetWaypointColor(loc, color);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ResetWaypointColorsToDefault()
   {
      mImpl->ResetWaypointColorsToDefault();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearText()
   {
      mImpl->mGeodeIDs->removeDrawables(0, mImpl->mGeodeIDs->getNumDrawables());
      mImpl->mRenderData.clear();
   }

} // namespace dtAI
