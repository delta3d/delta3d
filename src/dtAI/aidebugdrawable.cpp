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

#ifdef __GNUG__
#  include <ext/hash_map>
#elif defined(_MSC_VER)
#  include <hash_map>
#else
#  include <map>
#endif

namespace dtAI
{

   class RenderData: public osg::Referenced
   {
   public:
      dtCore::RefPtr<osgText::Text> mTextNode;
   };

#ifdef __GNUG__
   typedef __gnu_cxx::hash_map<unsigned, dtCore::RefPtr<RenderData> > RenderDataMap;
#elif defined(_MSC_VER)
   typedef stdext::hash_map<unsigned, dtCore::RefPtr<RenderData> > RenderDataMap;
#else
   typedef std::map<unsigned, dtCore::RefPtr<RenderData> > RenderDataMap;
#endif

   class AIDebugDrawableImpl
   {
   public:
      AIDebugDrawableImpl(WaypointRenderInfo& pRenderInfo)
      : mRenderInfo(&pRenderInfo)
      {

      }

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

         mWaypointGeometry->setVertexArray(mVerts.get());
         mNavMeshGeometry->setVertexArray(mVerts.get());

         mNode->addChild(mGeodeWayPoints.get());
         mNode->addChild(mGeodeNavMesh.get());
         mNode->addChild(mGeodeIDs.get());

         mGeodeNavMesh->addDrawable(mNavMeshGeometry.get());
         mGeodeWayPoints->addDrawable(mWaypointGeometry.get());

         mWaypointGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

         mNavMeshGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

         mRenderInfo->Init();
      }

      void OnRenderInfoChanged()
      {
         if (mRenderInfo->GetRenderWaypointID())
         {
            mGeodeIDs->setNodeMask(~0);
         }
         else
         {
            mGeodeIDs->setNodeMask(0);
         }

         if (mRenderInfo->GetRenderNavMesh())
         {
            mGeodeNavMesh->setNodeMask(~0);
         }
         else
         {
            mGeodeNavMesh->setNodeMask(0);
         }

         if (mRenderInfo->GetRenderWaypoints())
         {
            mGeodeWayPoints->setNodeMask(~0);
         }
         else
         {
            mGeodeWayPoints->setNodeMask(0);
         }

         osg::Vec4Array* waypointColors = new osg::Vec4Array(1);
         (*waypointColors)[0] = mRenderInfo->GetWaypointColor();
         mWaypointGeometry->setColorArray(waypointColors);

         osg::Vec4Array* navmeshColors = new osg::Vec4Array(1);
         (*navmeshColors)[0] = mRenderInfo->GetNavMeshColor();
         mNavMeshGeometry->setColorArray(navmeshColors);

         osg::Point* p = new osg::Point(mRenderInfo->GetWaypointSize());
         mWaypointGeometry->getOrCreateStateSet()->setAttribute(p, osg::StateAttribute::ON);

         osg::LineWidth* lw = new osg::LineWidth(mRenderInfo->GetNavMeshWidth());
         mNavMeshGeometry->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);
      }

      void OnGeometryChanged()
      {
         //seems like there should be a generic dirty()- does this only work with display lists?
         mWaypointGeometry->dirtyDisplayList();
         mWaypointGeometry->setVertexArray(mVerts.get());

         mWaypointGeometry->removePrimitiveSet(0);
         osg::PrimitiveSet* ps = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, mVerts->size());
         mWaypointGeometry->addPrimitiveSet(ps);
         //setting it back to zero will ensure any user data does not get removed when this function is called again
         mWaypointGeometry->setPrimitiveSet(0, ps);

         mNavMeshGeometry->removePrimitiveSet(0);
         osg::PrimitiveSet* psLines = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, mWaypointPairs->begin(), mWaypointPairs->end());
         mNavMeshGeometry->addPrimitiveSet(psLines);
         //setting it back to zero will ensure any user data does not get removed when this function is called again
         mNavMeshGeometry->setPrimitiveSet(0, psLines);

         osg::Point* p = new osg::Point(mRenderInfo->GetWaypointSize());
         mWaypointGeometry->getOrCreateStateSet()->setAttribute(p, osg::StateAttribute::ON);

         osg::LineWidth* lw = new osg::LineWidth(mRenderInfo->GetNavMeshWidth());
         mNavMeshGeometry->getOrCreateStateSet()->setAttribute(lw, osg::StateAttribute::ON);
      }

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

      dtCore::RefPtr<WaypointRenderInfo> mRenderInfo;

      dtCore::RefPtr<osg::UIntArray> mWaypointIDs;
      dtCore::RefPtr<osg::Vec3Array> mVerts;
      dtCore::RefPtr<osg::UIntArray> mWaypointPairs;
      dtCore::RefPtr<osg::Geometry> mWaypointGeometry;
      dtCore::RefPtr<osg::Geometry> mNavMeshGeometry;
      dtCore::RefPtr<osg::Geode> mGeodeNavMesh;
      dtCore::RefPtr<osg::Geode> mGeodeWayPoints;
      dtCore::RefPtr<osg::Geode> mGeodeIDs;
      dtCore::RefPtr<osg::Group> mNode;

      RenderDataMap mRenderData;
   };

   //////////////////////////////////////////////////////////////////////////////
   //AIDebugDrawable
   //////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::AIDebugDrawable()
   : mImpl(new AIDebugDrawableImpl(*new WaypointRenderInfo))
   {
      Init();
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::AIDebugDrawable(WaypointRenderInfo& pRenderInfo)
      : mImpl(new AIDebugDrawableImpl(pRenderInfo))
   {
      Init();
   }

   ///////////////////////////////////////////////////////////////////////////////
   AIDebugDrawable::~AIDebugDrawable()
   {
      delete mImpl;
      mImpl = NULL;
   }


   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetRenderInfo(WaypointRenderInfo& pRenderInfo)
   {
      mImpl->mRenderInfo = &pRenderInfo;
      
      mImpl->OnRenderInfoChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   WaypointRenderInfo& AIDebugDrawable::GetRenderInfo()
   {
      return *mImpl->mRenderInfo;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const WaypointRenderInfo& AIDebugDrawable::GetRenderInfo() const
   {
      return *mImpl->mRenderInfo;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::OnRenderInfoChanged()
   {
      mImpl->OnRenderInfoChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::Init()
   {
      mImpl->Init();
      mImpl->OnRenderInfoChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearMemory()
   {
      //todo-
      ClearWaypointGraph();
   }


   ///////////////////////////////////////////////////////////////////////////////
   osg::Geode* AIDebugDrawable::GetGeodeNavMesh()
   {
      return mImpl->mGeodeNavMesh.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetGeodeNavMesh( osg::Geode* geode )
   {
      mImpl->mGeodeNavMesh = geode;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Geode* AIDebugDrawable::GetGeodeWayPoints()
   {
      return mImpl->mGeodeWayPoints.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetGeodeWayPoints(osg::Geode* geode)
   {
      mImpl->mGeodeWayPoints = geode;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Node* AIDebugDrawable::GetOSGNode()
   {
      return mImpl->mNode.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   const osg::Node* AIDebugDrawable::GetOSGNode() const
   {
      return mImpl->mNode.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::SetOSGNode( osg::Group* grp )
   {
      mImpl->mNode = grp;
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Geometry* AIDebugDrawable::GetGeometry()
   {
      return mImpl->mWaypointGeometry.get();
   }

   ///////////////////////////////////////////////////////////////////////////////
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

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::InsertWaypoint(const WaypointInterface& wp)
   {
      int loc = FindWaypoint(wp.GetID());
      if (loc > -1)
      {
         //we already have this waypoint so lets make sure its in the right place
         (*mImpl->mVerts)[loc].set(wp.GetPosition());
      }
      else //lets add it to the existing waypoints
      {
         mImpl->mWaypointIDs->push_back(wp.GetID());
         mImpl->mVerts->push_back(wp.GetPosition());
         dtCore::RefPtr<RenderData> newRenderData = new RenderData;
         mImpl->mRenderData.insert(std::make_pair(wp.GetID(), newRenderData));

         mImpl->CreateWaypointIDText(wp, *newRenderData);

      }

      OnGeometryChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::RemoveWaypoint(unsigned id)
   {
      int loc = FindWaypoint(id);
      if (loc > -1)
      {
         //since this data is easily copied we can perform a faster erase
         //we simply copy the last element to the place of the element to be removed
         //and then we pop off the last element
         //it should be noted that order is not preserved.... if this matters we will need to revist this
         (*mImpl->mVerts)[loc].set((*mImpl->mVerts)[mImpl->mVerts->size() - 1]);
         (*mImpl->mWaypointIDs)[loc] = (*mImpl->mWaypointIDs)[mImpl->mWaypointIDs->size() - 1];

         mImpl->mVerts->pop_back();
         mImpl->mWaypointIDs->pop_back();

         RenderDataMap::iterator i = mImpl->mRenderData.find(id);
         if (i != mImpl->mRenderData.end())
         {
            mImpl->mGeodeIDs->removeDrawable(i->second->mTextNode.get());
            mImpl->mRenderData.erase(i);
         }

         OnGeometryChanged();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::OnGeometryChanged()
   {
      mImpl->OnGeometryChanged();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::UpdateWaypointGraph( const NavMesh& nm )
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
   }

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::AddPathSegment( const WaypointInterface* pFrom, const WaypointInterface* pTo )
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

   ///////////////////////////////////////////////////////////////////////////////
   void AIDebugDrawable::ClearWaypointGraph()
   {
      mImpl->mWaypointPairs->clear();
   }

} // namespace dtAI
