/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 06/07/2006
 */

#include <dtAI/waypointmanager.h>

#include <dtCore/actortype.h>
#include <dtCore/actorfactory.h>

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Version>

#include <dtCore/scene.h>
#include <dtCore/batchisector.h>

#include <dtUtil/log.h>

#include <cassert>
#include <exception>
#include <algorithm>

#include <osgDB/fstream>

namespace dtAI
{
   class InvalidWaypointFileID: public std::exception
   {
      virtual const char* what() const throw()
      {
         return "The Waypoint file ID was invalid";
      }
   };

   class InvalidHelperFileID: public std::exception
   {
      virtual const char* what() const throw()
      {
         return "The Waypoint Helper file ID was invalid";
      }
   };

   osg::ref_ptr<WaypointManager> WaypointManager::mSingleton = 0;

   /////////////////////////////////////////////////////////////////////////////
   WaypointManager::WaypointManager()
      : mLoadActors(0)
      , mDeleteOnClear(true)
      , mDrawWaypoints(false)
      , mDrawNavMesh(false)
      , mDrawNavMeshDetails(false)
      , mReadingFile(false)
      , mLoadActorsLock(false)
      , mWaypointSize(15.0f)
      , mWaypointColor(0.0f, 0.0f, 1.0f, 1.0f)
      , mNavMeshWidth(3.0f)
      , mNavMeshColor(0.0f, 1.0f, 0.35f, 1.0f)
      , mDrawable(new osg::Geode)
      , mNavMesh(new NavMesh())
   {
      mDrawable->addDrawable(new WaypointManagerDrawable(this));
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointManager::~WaypointManager()
   {
      mDrawable->removeDrawables(0, 1);
      mDrawable = NULL;

      Clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::CreateInstance()
   {
      if (!mSingleton)
      {
         mSingleton = new WaypointManager();
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointManager& WaypointManager::GetInstance()
   {
      CreateInstance();
      return *mSingleton;
   }

   /////////////////////////////////////////////////////////////////////////////
   WaypointID WaypointManager::AddWaypoint(const osg::Vec3& pWaypoint)
   {
      Waypoint* pWay = new Waypoint(pWaypoint);
      WaypointID id = pWay->GetID();

      mWaypoints.insert(std::pair<WaypointID, Waypoint*>(id, pWay));
      return id;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::AddWaypoint(Waypoint* wp)
   {
      if(GetWaypoint(wp->GetID()) == NULL)
      {
         mWaypoints.insert(std::pair<WaypointID, Waypoint*>(wp->GetID(), wp));
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::RemoveWaypoint(WaypointID id)
   {
      //we are indexing into map with a key generated on AddWaypoint
      mWaypoints.erase(id);
   }

   /////////////////////////////////////////////////////////////////////////////
   Waypoint* WaypointManager::GetWaypoint(WaypointID id)
   {
      //note: the id is not related to the size of the waypoints
      //checking id < mWaypoints.size() is invalid if waypoints are
      //inserted then removed

      //we return NULL if no waypoint is found matching this id
      WaypointMap::iterator i = mWaypoints.find(id);
      if (i != mWaypoints.end())
      {
         return (*i).second;
      }
      else
      {
         return NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   const Waypoint* WaypointManager::GetWaypoint(WaypointID id) const
   {
      /*
      DJMC note: We cannot simply do the following:

      return mWaypoints[id];

      and still remain a const method since the [] operation might
      add a setting if it is not found inside. We must seek an
      alternative.

      Note, according to http://www.sgi.com/tech/stl/Map.html, m[k]
      should be equivalent to:

      (*((m.insert(value_type(k, data_type()))).first)).second

      ...thus, we should be able to find a read-only analogue by
      changing the line from an insert to a find, so that we may
      remain a const method.
      //*/


      WaypointMap::const_iterator i = mWaypoints.find(id);
      if (i != mWaypoints.end())
      {
         return (*i).second;
      }
      else
      {
         return NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::MoveWaypoint(WaypointID id, const osg::Vec3& pPos)
   {
      // we are indexing into map with a key generated on AddWaypoint
      mWaypoints[id]->SetPosition(pPos);
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::AddPathSegment(WaypointID idFrom, WaypointID idTo)
   {
      mNavMesh->AddPathSegment(mWaypoints[idFrom], mWaypoints[idTo]);
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointManager::WriteFile(const std::string& pFileToWrite)
   {
      osgDB::ofstream outfile;

      outfile.open(pFileToWrite.c_str(), std::ofstream::out);
      if (outfile.fail())
      {
         return false;
      }

      // obtain a temporary vector for writing
      std::vector<Waypoint*> pWaypointVector = CopyWaypointsIntoVector();

      // write the file id
      int id = WAYPOINT_HELPER_FILE_ID;
      outfile << id << std::endl;

      // write the file format version
      int version = WAYPOINT_FILE_VERSION;
      outfile << version << std::endl;

      // write the number of verts to read
      unsigned size = pWaypointVector.size();
      outfile << size << std::endl;

      for (WaypointID i = 0; i < size; ++i)
      {
         // we will use these ids when we write out the navmesh
         pWaypointVector[i]->SetID(i);
         osg::Vec3 pPos = pWaypointVector[i]->GetPosition();
         outfile << pPos[0] << " " << pPos[1] << " " << pPos[2] << std::endl;
      }

      // write out our number of nav mesh paths
      unsigned navMeshSize = mNavMesh->GetNavMesh().size();
      outfile << navMeshSize << std::endl;

      // now loop through the navmesh and write out index pairs
      NavMesh::NavMeshContainer::const_iterator iter = mNavMesh->GetNavMesh().begin();
      NavMesh::NavMeshContainer::const_iterator endOfList = mNavMesh->GetNavMesh().end();

      while (iter != endOfList)
      {
         const WaypointPair* pWayPair = (*iter).second;
         outfile << pWayPair->GetWaypointFrom()->GetID() << " " << pWayPair->GetWaypointTo()->GetID() << std::endl;
         ++iter;
      }

      outfile.close();

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointManager::ReadFile(const std::string& pFileToRead)
   {
      // we only want to allow loading after reading
      // also we maintain this flag because when we create actor proxies
      // we will key off of it in our insert to make sure we dont insert twice
      mLoadActors = true;
      mReadingFile = true;

      Clear();

      bool read_file_ok = false;

      osgDB::ifstream infile;

      infile.open(pFileToRead.c_str());
      if (infile.is_open())
      {
         try
         {
            // read the file id
            int id = 0;
            infile >> id;
            if (id != WAYPOINT_HELPER_FILE_ID) throw InvalidHelperFileID();

            // read in the file version number
            int version = 0;
            infile >> version;
            if (version != WAYPOINT_FILE_VERSION) throw InvalidWaypointFileID();

            // read the number of verts to read
            unsigned size = 0;
            infile >> size;

            for (WaypointID i = 0; i < size; ++i)
            {
               osg::Vec3 pPos;
               infile >> pPos[0] >> pPos[1] >> pPos[2];

               Waypoint* pNewWaypoint = new Waypoint(pPos);
               pNewWaypoint->SetRenderFlag(Waypoint::RENDER_DEFAULT);
               pNewWaypoint->SetID(i);

               mWaypoints.insert(std::pair<WaypointID, Waypoint*>(i, pNewWaypoint));
            }

            // read in the nav mesh
            unsigned navMeshSize = 0;
            infile >> navMeshSize;

            for (unsigned i = 0; i < navMeshSize; ++i)
            {
               WaypointID indexFrom, indexTo;
               infile >> indexFrom >> indexTo;
               mNavMesh->AddPathSegment(mWaypoints[indexFrom], mWaypoints[indexTo]);
            }

            read_file_ok = true;
         }
         catch(const std::exception& e)
         {
            LOG_ERROR("Exception thrown reading Waypoint file: " + std::string(e.what()));
            read_file_ok = false;
         }
      }

      infile.close();
      mReadingFile = false;
      return read_file_ok;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool WaypointManager::ObtainLock()
   {
      if (!mLoadActorsLock && mLoadActors && !mReadingFile)
      {
         mLoadActorsLock = true;
         return true;
      }
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::ReleaseLock()
   {
      mLoadActors = false;
      mLoadActorsLock = false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::CreateNavMesh(dtCore::Scene* pScene)
   {
      float maxDistBetweenWaypoints = AvgDistBetweenWaypoints();

      dtCore::RefPtr<dtCore::BatchIsector> pIsector = new dtCore::BatchIsector(pScene);

      WaypointIterator iter = mWaypoints.begin();
      WaypointIterator endOfMap = mWaypoints.end();

      WaypointID id = 0;
      while (iter != endOfMap)
      {
         Waypoint* pWaypoint1 = (*iter).second;
         pWaypoint1->SetID(id);

         WaypointIterator iter2     = mWaypoints.begin();
         WaypointIterator endOfMap2 = mWaypoints.end();

         while (iter2 != endOfMap2)
         {
            if ((*iter).second != (*iter2).second)
            {
               Waypoint* pWaypoint2 = (*iter2).second;

               if (WaypointPair(pWaypoint1, pWaypoint2).Get3DDistance() < maxDistBetweenWaypoints)
               {
                  // added special case to avoid colliding with the billboards
                  osg::Vec3 vec = pWaypoint2->GetPosition() - pWaypoint1->GetPosition();
                  vec.normalize();
                  pIsector->EnableAndGetISector(0).SetSectorAsLineSegment(
                     pWaypoint1->GetPosition() + vec,
                     pWaypoint2->GetPosition() - vec);

                  // if there is a path between the two points
                  if (!pIsector->Update())
                  {
                     mNavMesh->AddPathSegment(pWaypoint1, pWaypoint2);
                  }

                  pIsector->Reset();
               }
            }

            ++iter2;
         }

         ++id;
         ++iter;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::OnMapLoad(const std::string& pWaypointFilename)
   {
      ReadFile(pWaypointFilename);
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::OnMapSave(const std::string& pWaypointFilename, bool pCreateNavMesh, dtCore::Scene* pScene)
   {
      mNavMesh->Clear();
      if (pCreateNavMesh) CreateNavMesh(pScene);
      WriteFile(pWaypointFilename);
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::OnMapClose()
   {
      Clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   NavMesh& WaypointManager::GetNavMesh()
   {
      return *mNavMesh;
   }

   /////////////////////////////////////////////////////////////////////////////
   const NavMesh& WaypointManager::GetNavMesh() const
   {
      return *mNavMesh;
   }

   /////////////////////////////////////////////////////////////////////////////
   const WaypointManager::WaypointMap& WaypointManager::GetWaypoints() const
   {
      return mWaypoints;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::vector<Waypoint*> WaypointManager::CopyWaypointsIntoVector() const
   {
      std::vector<Waypoint*> pContainer;
      WaypointMap::const_iterator iter     = mWaypoints.begin();
      WaypointMap::const_iterator endOfMap = mWaypoints.end();

      while (iter != endOfMap)
      {
         pContainer.push_back((*iter).second);
         ++iter;
      }
      return pContainer;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::ostream& WaypointManager::GetWaypoints(std::ostream& pStream)
   {
      WaypointIterator iter = mWaypoints.begin();
      WaypointIterator endOfMap = mWaypoints.end();

      pStream << "Num Waypoints: " << mWaypoints.size() << std::endl;

      while (iter != endOfMap)
      {
         osg::Vec3 pPos = (*iter).second->GetPosition();
         pStream << "Waypoint: (" << float(pPos[0]) << ", " << float(pPos[1]) << ", " << float(pPos[2]) << ")" << std::endl;
         ++iter;
      }

      return pStream;
   }

   /////////////////////////////////////////////////////////////////////////////
   const osg::Node* WaypointManager::GetOSGNode() const
   {
      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Node* WaypointManager::GetOSGNode()
   {
      return mDrawable.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::SetDrawWaypoints(bool pDraw)
   {
      mDrawWaypoints = pDraw;
   }

   struct WaypointManagerDeleteFunc
   {
      template<class _Type>
      void operator()(_Type p1)
      {
         delete p1.second;
      }
   };

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::Clear()
   {
      // free memory
      if (mDeleteOnClear)
      {
         std::for_each(mWaypoints.begin(), mWaypoints.end(), WaypointManagerDeleteFunc());
      }
      mWaypoints.clear();
      mNavMesh->Clear();
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::SetWaypointColor(const osg::Vec4& pColor)
   {
      mWaypointColor = pColor;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::SetWaypointSize(float pSize)
   {
      mWaypointSize = pSize;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::SetDrawNavMesh(bool pDraw, bool pDrawDetails)
   {
      mDrawNavMesh = pDraw;
      mDrawNavMeshDetails = pDrawDetails;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::SetNavMeshColor(const osg::Vec4& pColor)
   {
      mNavMeshColor = pColor;
   }

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::SetNavMeshSize(float pSize)
   {
      mNavMeshWidth = pSize;
   }

   /////////////////////////////////////////////////////////////////////////////
   float WaypointManager::AvgDistBetweenWaypoints() const
   {
      int i = 0;
      float pResult = 0;

      WaypointMap::const_iterator iter = mWaypoints.begin();
      WaypointMap::const_iterator endOfMap = mWaypoints.end();

      while (iter != endOfMap)
      {
         const Waypoint* pWaypoint1 = (*iter).second;

         WaypointMap::const_iterator iter2 = mWaypoints.begin();
         WaypointMap::const_iterator endOfMap2 = mWaypoints.end();

         while (iter2 != endOfMap2)
         {
            if ((*iter).second != (*iter2).second)
            {
               const Waypoint* pWaypoint2 = (*iter2).second;

               ++i;
               pResult += WaypointPair(pWaypoint1, pWaypoint2).Get3DDistance();
            }
            ++iter2;
         }
         ++iter;
      }

      return (pResult / float(i));
   }

   //////////////////////////////////////////////////////////////////////////
   void WaypointManager::SetDeleteOnClear(bool b)
   {
      mDeleteOnClear = b;
   }

   //////////////////////////////////////////////////////////////////////////
   bool WaypointManager::GetDeleteOnClear() const
   {
      return mDeleteOnClear;
   }

   //////////////////////////////////////////////////////////////////////////
   // WaypointManagerDrawable
   //////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   void WaypointManager::WaypointManagerDrawable::drawImplementation(osg::RenderInfo& /*renderInfo*/) const
   {
      //this will keep us from drawing when we are loading
      if (mHelper->mReadingFile) { return; }

      glDisable(GL_LIGHTING);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      if (mHelper->mDrawWaypoints)
      {
         float pointSize = mHelper->mWaypointSize;
         if (pointSize < 1.0f) pointSize = 1.0f;
         glPointSize(pointSize);

         glBegin(GL_POINTS);

         WaypointManager::WaypointMap::const_iterator iter = mHelper->mWaypoints.begin();
         WaypointManager::WaypointMap::const_iterator endOfMap = mHelper->mWaypoints.end();

         while (iter != endOfMap)
         {
            const osg::Vec3& color = (*iter).second->GetColor();
            float alpha            = (*iter).second->GetAlpha();

            // If we're using a render flag, output using this waypoint's color
            if ((*iter).second->GetRenderFlag() >= Waypoint::RENDERFLAG_FIRST &&
                (*iter).second->GetRenderFlag() <= Waypoint::RENDERFLAG_LAST)
            {
               glColor4f(color.x(), color.y(), color.z(), alpha);
            }
            else
            {
               float gradient = (*iter).second->GetGradient();

               osg::Vec4 waypointColor(mHelper->mWaypointColor);
               glColor4f(waypointColor[0] * gradient, waypointColor[1] * gradient, waypointColor[2] * gradient, waypointColor[3]);
            }

            osg::Vec3 pPos = (*iter).second->GetPosition();
            glVertex3f(pPos[0], pPos[1], pPos[2] + 0.15f); // added to the z component so they aren't in the ground
            ++iter;
         }

         glEnd();
      }


      if (mHelper->mDrawNavMesh)
      {
         float lineWidth = mHelper->mNavMeshWidth;
         if (lineWidth < 1.0f) { lineWidth = 1.0f; }
         glLineWidth(lineWidth);
         osg::Vec4 lineColor(mHelper->mNavMeshColor);

         glBegin(GL_LINES);

         NavMesh::NavMeshContainer::const_iterator iter = mHelper->mNavMesh->GetNavMesh().begin();
         NavMesh::NavMeshContainer::const_iterator endOfVector = mHelper->mNavMesh->GetNavMesh().end();

         while (iter != endOfVector)
         {
            WaypointPair* pMesh = (*iter).second;
            osg::Vec3 pFrom = pMesh->GetFrom();
            osg::Vec3 pTo = pMesh->GetTo();
            pFrom[2] += 0.15f;
            pTo[2]   += 0.15f;

            glColor4fv(&lineColor[0]);

            glVertex3fv(&pFrom[0]);

            if (mHelper->mDrawNavMeshDetails && mHelper->mNavMesh->IsOneWay(pMesh))
            {
               glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            }

            glVertex3fv(&pTo[0]);
            ++iter;
         }

         glEnd();
      }


      glDisable(GL_BLEND);
      glEnable(GL_LIGHTING);

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   }

} // namespace dtAI
