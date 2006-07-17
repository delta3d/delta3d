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
 * @author Bradley Anderegg 06/07/2006
 */

#include <dtAI/waypointmanager.h>

#include <dtDAL/actortype.h>
#include <dtDAL/librarymanager.h>

#include <osg/Vec3>
#include <osg/Matrix>
#include <dtCore/scene.h>
#include <dtCore/isector.h>

#include <dtUtil/log.h>

#include <cassert>
#include <fstream>
#include <algorithm>


namespace dtAI
{
   
   osg::ref_ptr<WaypointManager> WaypointManager::mSingleton = 0;

   WaypointManager::WaypointManager()
      : mLoadActors(0)
      , mDrawWaypoints(false) 
      , mDrawNavMesh(false)
      , mReadingFile(false)
      , mLoadActorsLock(false)
      , mWaypointSize(15.0f)
      , mWaypointColor(0.0f, 0.0f, 1.0f, 1.0f)
      , mNavMeshWidth(3.0f)
      , mNavMeshColor(0.0f, 1.0f, 0.35f, 1.0f)
      , mDrawable(new osg::Geode)
   {
      mDrawable->addDrawable(new WaypointManagerDrawable(this));
   }
   
   WaypointManager::~WaypointManager()
   {
      mDrawable->removeDrawable(0, 1);
      mDrawable = 0;

      Clear();         
   }

   void WaypointManager::CreateInstance()  
   {
      if(!mSingleton)
      {
         mSingleton = new WaypointManager();
      }      
   }

   WaypointManager* WaypointManager::GetInstance()  
   {
      if(!mSingleton)
      {
         CreateInstance();
      }
      return mSingleton.get();
   }

   void WaypointManager::AddWaypoint(WaypointActor* pWaypointActor)
   {
      if(mLoadActors) return;

      unsigned pIndex = mWaypoints.size();
      pWaypointActor->SetIndex(pIndex);
      mWaypoints.insert(std::pair<unsigned, Waypoint*>(pIndex, new Waypoint(pWaypointActor)));      
   }

   void WaypointManager::RemoveWaypoint(const WaypointActor* pWaypoint)
   {
      mWaypoints.erase(pWaypoint->GetIndex());
   }

   void WaypointManager::MoveWaypoint(unsigned pIndex, const osg::Vec3& pPos)
   {
      mWaypoints[pIndex]->SetPosition(pPos);
   }

   bool WaypointManager::WriteFile(const std::string& pFileToWrite) const
   {
      std::ofstream outfile;

      outfile.open(pFileToWrite.c_str(), std::ofstream::out);      
      if(outfile.fail()) return false;

      //write the file id
      int id = WAYPOINT_HELPER_FILE_ID;
      outfile << id << std::endl;
      
      //write the number of verts to read
      unsigned size = mWaypoints.size();
      outfile << size << std::endl;

      WaypointMap::const_iterator iter = mWaypoints.begin();
      WaypointMap::const_iterator endOfMap = mWaypoints.end();

      while(iter != endOfMap)
      {
         osg::Vec3 pPos = (*iter).second->GetPosition();
         outfile << pPos[0] << " " << pPos[1] << " " << pPos[2] << std::endl;
         ++iter;
      }

      outfile.close();

      return true;
   }

   bool WaypointManager::ReadFile(const std::string& pFileToRead) 
   {
      //we only want to allow loading after reading   
      //also we maintain this flag because when we create actor proxies
      //we will key off of it in our insert to make sure we dont insert twice
      mLoadActors = true;
      mReadingFile = true;

      Clear();

      std::ifstream infile;

      infile.open(pFileToRead.c_str(), std::ifstream::in);     
      if(infile.fail()) return false;

      try
      {      

         //read the file id
         int id = 0;
         infile >> id;;

         if(id != WAYPOINT_HELPER_FILE_ID) return false;

         //read the number of verts to read
         unsigned size = 0;
         infile >> size;

         for(unsigned i = 0; i < size; ++i)
         {
            osg::Vec3 pPos;
            infile >> pPos[0] >> pPos[1] >> pPos[2];
            
            Waypoint* pNewWaypoint = new Waypoint(pPos);
            pNewWaypoint->SetRenderFlag(0);

            mWaypoints.insert(std::pair<unsigned, Waypoint*>(i, pNewWaypoint));      
         }
      }
      catch(...)
      {
         LOG_ERROR("Exception thrown reading Waypoint file, invalid file format.");
      }

      infile.close();
      
      mReadingFile = false;
      return true;
   }


   bool WaypointManager::ObtainLock()
   {
      if(!mLoadActorsLock && mLoadActors && !mReadingFile)
      {
         mLoadActorsLock = true;
         return true;
      }
      return false;
   }

   void WaypointManager::ReleaseLock()
   {
      mLoadActors = false;
      mLoadActorsLock = false;
   }

   std::ostream& WaypointManager::GetWaypoints(std::ostream& pStream)
   {
      WaypointIterator iter = mWaypoints.begin();
      WaypointIterator endOfMap = mWaypoints.end();

      pStream << "Num Waypoints: " << mWaypoints.size() << std::endl;

      while(iter != endOfMap)
      {
         osg::Vec3 pPos = (*iter).second->GetPosition();
         pStream << "Waypoint: (" << float(pPos[0]) << ", " << float(pPos[1]) << ", " << float(pPos[2]) << ")" << std::endl;
         ++iter;
      }
      
      return pStream;
   }

   void WaypointManager::CreateNavMesh(dtCore::Scene* pScene)
   {
      osg::ref_ptr<dtCore::Isector> pIsector = new dtCore::Isector(pScene);
   
      WaypointIterator iter = mWaypoints.begin();
      WaypointIterator endOfMap = mWaypoints.end();

      int id = 0;
      while(iter != endOfMap)
      {
         Waypoint* pWaypoint1 = (*iter).second;
         pWaypoint1->SetID(id);

         WaypointIterator iter2 = mWaypoints.begin();
         WaypointIterator endOfMap2 = mWaypoints.end();

         while(iter2 != endOfMap2)
         {
            if((*iter).second != (*iter2).second)
            {               
               Waypoint* pWaypoint2 = (*iter2).second;

               pIsector->SetStartPosition(pWaypoint1->GetPosition());
               pIsector->SetEndPosition(pWaypoint2->GetPosition());

               //if there is a path between the two points
               if(!pIsector->Update())
               {
                  mNavMesh.AddPathSegment(pWaypoint1, pWaypoint2);
               }
               
               pIsector->Reset();
            }

            ++iter2;
         }

         ++id;
         ++iter;
      }

   }

   void WaypointManager::OnMapLoad(const std::string& pWaypointFilename)
   {
      ReadFile(pWaypointFilename);      
   }

   void WaypointManager::OnMapSave(const std::string& pWaypointFilename)
   {
      WriteFile(pWaypointFilename);
   }

   void WaypointManager::OnMapClose()
   {
      Clear();
   }

   NavMesh& WaypointManager::GetNavMesh()
   {
      return mNavMesh;
   }

   const NavMesh& WaypointManager::GetNavMesh() const
   {
      return mNavMesh;
   }

   const WaypointManager::WaypointMap& WaypointManager::GetWaypoints() const
   {
      return mWaypoints;
   }

   const osg::Node* WaypointManager::GetOSGNode() const
   {
      return mDrawable.get();            
   }

   osg::Node* WaypointManager::GetOSGNode()
   {
      return mDrawable.get();      
   }

   void WaypointManager::SetDrawWaypoints(bool pDraw)
   {
       mDrawWaypoints = pDraw;
   }

   struct deleteFunc
   {
      template<class _Type>
         void operator()(_Type p1)
      {
         delete p1.second; 
      }
   };

   void WaypointManager::Clear()
   {
      //free memory      
      std::for_each(mWaypoints.begin(), mWaypoints.end(), deleteFunc());
      mWaypoints.clear();
      mNavMesh.Clear();
   }   


   void WaypointManager::SetWaypointColor(const osg::Vec4& pColor)
   {
      mWaypointColor = pColor;
   }

   void WaypointManager::SetWaypointSize(float pSize)
   {
      mWaypointSize = pSize;
   }


   void WaypointManager::SetDrawNavMesh(bool pDraw)
   {
      mDrawNavMesh = pDraw;
   }

   void WaypointManager::SetNavMeshColor(const osg::Vec4& pColor)
   {
      mNavMeshColor = pColor;
   }

   void WaypointManager::SetNavMeshSize(float pSize)
   {
      mNavMeshWidth = pSize;
   }

//////////////////////////////////////////////////////////////////////////
//WaypointManagerDrawable
//////////////////////////////////////////////////////////////////////////


   void WaypointManager::WaypointManagerDrawable::drawImplementation(osg::State& state) const 
   {     
      //this will keep us from drawing when we are loading
      if(mHelper->mReadingFile) return;

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      if(mHelper->mDrawWaypoints)
      {
         float pointSize = mHelper->mWaypointSize;
         if(pointSize < 1.0f) pointSize = 1.0f;
         glPointSize(pointSize);

         glBegin(GL_POINTS);

         WaypointManager::WaypointMap::const_iterator iter = mHelper->mWaypoints.begin();
         WaypointManager::WaypointMap::const_iterator endOfMap = mHelper->mWaypoints.end();

         while(iter != endOfMap)
         {
            if((*iter).second->GetActive())
            {
               glColor4f(1.0, 0.0, 0.0, 1.0);
            }
            else
            {
               osg::Vec4 waypointColor(mHelper->mWaypointColor);
               glColor4fv(&waypointColor[0]);
            }
            
            osg::Vec3 pPos = (*iter).second->GetPosition();
            glVertex3f(pPos[0], pPos[1], pPos[2] + 0.15f); //added to the z component so they arent in the ground
            ++iter;
         }
         
         glEnd();
      }


      if(mHelper->mDrawNavMesh)
      {
         float lineWidth = mHelper->mNavMeshWidth;
         if(lineWidth < 1.0f) lineWidth = 1.0f;
         glLineWidth(lineWidth);

         osg::Vec4 lineColor(mHelper->mNavMeshColor);
         glColor4fv(&lineColor[0]);

         glBegin(GL_LINES);

         NavMesh::NavMeshContainer::const_iterator iter = mHelper->mNavMesh.GetNavMesh().begin();
         NavMesh::NavMeshContainer::const_iterator endOfVector = mHelper->mNavMesh.GetNavMesh().end();

         while(iter != endOfVector)
         {
            WaypointPair* pMesh = (*iter).second;
            osg::Vec3 pFrom = pMesh->GetFrom();
            osg::Vec3 pTo = pMesh->GetTo();
            pFrom[2] += 0.15f;
            pTo[2] += 0.15f;
            glVertex3fv(&pFrom[0]);
            glVertex3fv(&pTo[0]);
            ++iter;
         }

         glEnd();
      }


      glDisable(GL_BLEND);
   }



}//namespace 
