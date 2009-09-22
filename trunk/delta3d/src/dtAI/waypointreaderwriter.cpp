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


#include <dtAI/waypointreaderwriter.h>
#include <dtAI/aiplugininterface.h>
#include <dtAI/waypointinterface.h>
#include <dtAI/waypointcollection.h>
#include <dtAI/navmesh.h>
#include <dtAI/waypointpair.h>

#include <dtAI/waypointtypes.h>
#include <dtAI/waypointgraph.h>
#include <dtAI/navmesh.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/exceptionenum.h>

#include <dtUtil/datastream.h>
#include <dtUtil/exception.h>
#include <fstream>

namespace dtAI
{
   //////////////////////////////////////////////////////////////////////////
   //file saving and loading utils
   //////////////////////////////////////////////////////////////////////////
   namespace WaypointFileHeader
   {
      const unsigned FILE_IDENT = 5705313;
      
      const unsigned VERSION_MAJOR = 1;
      const unsigned VERSION_MINOR = 0;

      const char FILE_START_END_CHAR = '!';
   };


   //////////////////////////////////////////////////////////////////////////
   //WaypointReaderWriter
   //////////////////////////////////////////////////////////////////////////
   WaypointReaderWriter::WaypointReaderWriter(AIPluginInterface& aiInterface)
      : mAIInterface(&aiInterface)
   {

   }

   WaypointReaderWriter::~WaypointReaderWriter()
   {

   }

   void WaypointReaderWriter::Clear()
   {
      mCollectionChildren.clear();
   }

   bool WaypointReaderWriter::LoadWaypointFile(const std::string& filename)
   {
      bool read_file_ok = false;
      std::ifstream infile;

      //we have to hold all the edges until all the waypoints are added
      typedef std::vector<std::pair<WaypointID, WaypointID> > EdgeArray;
      EdgeArray edgeData;

      infile.open(filename.c_str(), std::ios_base::binary | std::ios_base::in);
      if(infile.is_open() && infile.good() && !infile.eof())
      {
         try
         {
            //get length of file
            infile.seekg (0, std::ios::end);
            int length = infile.tellg();
            infile.seekg (0, std::ios::beg);

            //read data
            char* buffer = new char [length];
            infile.read(buffer, length);

            dtUtil::DataStream ds(buffer, length);

            char fileStart;
            unsigned fileIdent, vMajor, vMinor;

            ds >> fileStart >> fileIdent >> vMajor >> vMinor;
            
            if(fileStart == WaypointFileHeader::FILE_START_END_CHAR &&
               fileIdent == WaypointFileHeader::FILE_IDENT &&
               vMajor == 1 && vMinor == 0)
            {
               unsigned numWaypointTypes = 0;
               ds.Read(numWaypointTypes);
               
               for(unsigned waypointTypeCount = 0; waypointTypeCount < numWaypointTypes; ++waypointTypeCount)
               {
                  std::string objectTypeName;
                  ds.Read(objectTypeName);
                  const dtDAL::ObjectType* ot = mAIInterface->GetWaypointTypeByName(objectTypeName);
                  if(ot != NULL)
                  {
                     unsigned numWaypoints = 0;
                     ds.Read(numWaypoints);

                     dtCore::RefPtr<WaypointPropertyBase> propCon;

                     for(unsigned waypointCount = 0; waypointCount < numWaypoints; ++waypointCount)
                     {
                        //we dont insert until the id is set
                        WaypointInterface* wi = mAIInterface->CreateNoInsert(*ot);

                        //the first thing written out is the Waypoint ID
                        unsigned curWaypointID = 0;
                        ds.Read(curWaypointID);
                        
                        //and now we set the id on the waypoint
                        wi->SetID(curWaypointID);

                        if(!propCon.valid())
                        {
                            propCon = mAIInterface->CreateWaypointPropertyContainer(*ot, wi);
                        }
                        
                        propCon->Set(wi);

                        //we have a special case when for waypoint collections to preserve
                        //their search level
                        int searchLevel = -1;
                        if(*ot == *WaypointTypes::WAYPOINT_COLLECTION)
                        {
                           WaypointCollection* wc = static_cast<WaypointCollection*>(wi);
                           
                           //read our search level
                           ds.Read(searchLevel);

                           //read num children
                           unsigned numColChildren = 0;
                           ds.Read(numColChildren);

                           for(unsigned colChildCount = 0; colChildCount < numColChildren; ++ colChildCount)
                           {
                              unsigned colChildId = 0;
                              ds.Read(colChildId);
                              //this records the fact that this is a parent of the waypoint with id 'colChildId'
                              mCollectionChildren.push_back(std::make_pair(curWaypointID, colChildId));
                           }
                        }

                        unsigned numProperties = 0;
                        ds.Read(numProperties);

                        for(unsigned propertyCount = 0; propertyCount < numProperties; ++propertyCount)
                        {
                           std::string propertyName;
                           ds.Read(propertyName);
                           dtDAL::ActorProperty* prop = propCon->GetProperty(propertyName);
                           //skip read only- note: we still are reading the name, its mentioned
                           //below on the writing code and should most likely be refactored
                           if(prop != NULL && !prop->IsReadOnly())
                           {
                              prop->FromDataStream(ds);
                           }
                           else if(prop == NULL)
                           {
                              throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,
                                 "Error reading Waypoint file '" + filename + ".", __FILE__, __LINE__);
                           }
                        }

                        //Here we insert after all the properties are set.
                        //If this is not a WaypointCollection the searchLevel will be -1
                        //and it will just be added the usual way.
                        Insert(wi, searchLevel);

                        //now read all edges
                        WaypointID currentID = wi->GetID();
                        unsigned numEdges = 0;
                        ds.Read(numEdges);
                        
                        for(unsigned edgeCount = 0; edgeCount < numEdges; ++edgeCount)
                        {
                           unsigned id = 0;
                           ds.Read(id);
                           edgeData.push_back(std::make_pair(currentID, id));
                        }
                     }
                  }
                  else
                  {
                     throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,
                        "Error reading Waypoint file '" + filename + ".", __FILE__, __LINE__);
                  }
               }
               
               ds.Read(fileStart);
               if(fileStart != WaypointFileHeader::FILE_START_END_CHAR)
               {
                  throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,
                     "Error reading Waypoint file '" + filename + ".", __FILE__, __LINE__);
               }

               //read successful, now we must insert all edges
               //todo: move this out into a function
               EdgeArray::iterator edgeDataIter = edgeData.begin();
               EdgeArray::iterator edgeDataIterEnd = edgeData.end();
               
               for(;edgeDataIter != edgeDataIterEnd; ++edgeDataIter)
               {
                  mAIInterface->AddEdge(edgeDataIter->first, edgeDataIter->second);
               }

               //and now assign all children and child edges
               AssignChildren();
               AssignChildEdges();               

               read_file_ok = true;
            }
         }
         catch(dtUtil::Exception& e)
         {
            e.LogException();
         }
      }
      
      Clear();
      infile.close();
      return read_file_ok;
   }

   void WaypointReaderWriter::AssignChildren()
   {
      WaypointIDPairArray::iterator iter = mCollectionChildren.begin();
      WaypointIDPairArray::iterator iterEnd = mCollectionChildren.end();

      WaypointID lastParent = 0;
      WaypointCollection* wc = NULL;

      for(;iter != iterEnd; ++iter)
      {
         WaypointIDPair& wpPair = *iter;

         //each waypoint inserts in order so many of the parent id's will be the same as the last
         if(wc == NULL || lastParent != wpPair.first)
         {
            WaypointInterface* wi = mAIInterface->GetWaypointById(wpPair.first);
            wc = dynamic_cast<WaypointCollection*>(wi);
            if(wc != NULL)
            {
               lastParent = wpPair.first;
            }
            else
            {
               throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,
                  "Error Assigning WaypointCollection children while loading file", __FILE__, __LINE__);
            }
         }

         mAIInterface->Assign(wpPair.second, wc);
      }
   }

   void WaypointReaderWriter::AssignChildEdges()
   {
      WaypointGraph& wpGraph = mAIInterface->GetWaypointGraph();
      unsigned numLevels = wpGraph.GetNumSearchLevels();

      //we skip the first level since they dont hold edges
      for(unsigned level = 1; level < numLevels; ++level)
      {
         WaypointGraph::SearchLevel* slLast = wpGraph.GetSearchLevel(level - 1);
         WaypointGraph::SearchLevel* slCurrent = wpGraph.GetSearchLevel(level);

         CreateWaypointCollectionEdges(slCurrent->mNodes, *slLast->mNavMesh);
      }
   }

   bool WaypointReaderWriter::SaveWaypointFile(const std::string& filename)
   {
      std::ofstream outfile;

      outfile.open(filename.c_str(), std::ios_base::binary | std::ofstream::out);
      if (outfile.fail())
      {
         return false;
      }

      dtUtil::DataStream ds;
      ds.Write(WaypointFileHeader::FILE_START_END_CHAR);
      ds.Write(WaypointFileHeader::FILE_IDENT);
      ds.Write(WaypointFileHeader::VERSION_MAJOR);
      ds.Write(WaypointFileHeader::VERSION_MINOR);
      
      typedef std::vector<dtCore::RefPtr<const dtDAL::ObjectType> > ObjectTypeArray;
      
      ObjectTypeArray waypointTypes;
      mAIInterface->GetSupportedWaypointTypes(waypointTypes);
      
      ds.Write(unsigned(waypointTypes.size()));
      ObjectTypeArray::iterator ob_iter = waypointTypes.begin();
      ObjectTypeArray::iterator ob_iterEnd = waypointTypes.end();

      for(;ob_iter != ob_iterEnd; ++ob_iter)
      {
         AIPluginInterface::WaypointArray wpArray;
         mAIInterface->GetWaypointsByType(**ob_iter, wpArray);
         
         ds.Write((**ob_iter).GetName());
         ds.Write(unsigned(wpArray.size()));

         if(!wpArray.empty())
         {
            dtCore::RefPtr<WaypointPropertyBase> propCon = mAIInterface->CreateWaypointPropertyContainer(**ob_iter, wpArray.front());

            typedef std::vector<const dtDAL::ActorProperty *> PropertyArray;
            PropertyArray propArray;
            propCon->GetPropertyList(propArray);

            AIPluginInterface::WaypointArray::iterator wpIter = wpArray.begin();
            AIPluginInterface::WaypointArray::iterator wpIterEnd = wpArray.end();
            for(;wpIter != wpIterEnd; ++wpIter)
            {
               propCon->Set(*wpIter);

               //write out our Waypoint ID first
               ds.Write((*wpIter)->GetID());

               //we have a special case when writing out waypoint collections to preserve
               //their search level
               if(**ob_iter == *WaypointTypes::WAYPOINT_COLLECTION)
               {
                  int searchLevel = mAIInterface->GetWaypointGraph().GetSearchLevelNum((*wpIter)->GetID());
                  ds.Write(searchLevel);

                  //write num children
                  unsigned numChildren = 0;
                  WaypointCollection* wc = dynamic_cast<WaypointCollection*>(*wpIter);
                  if(wc != NULL)
                  {
                     //write num children
                     numChildren = wc->degree();
                     ds.Write(numChildren);
                     
                     //write child id's
                     WaypointCollection::WaypointTree::child_iterator childWpIter = wc->begin_child();
                     WaypointCollection::WaypointTree::child_iterator childWpIterEnd = wc->end_child();
                     for(; childWpIter != childWpIterEnd; ++childWpIter)
                     {
                        WaypointID id = childWpIter->value->GetID();
                        ds.Write(unsigned(id));
                     }
                  }
                  else
                  {
                     LOG_ERROR("Invalid waypoint type, dynamic cast to WaypointCollection failed when writing file '" + filename + "'.");
                  }

               }
               
               //write the number of properties to read in
               ds.Write(unsigned(propArray.size()));

               PropertyArray::iterator pcIter = propArray.begin();
               PropertyArray::iterator pcIterEnd = propArray.end();
               for(;pcIter != pcIterEnd; ++pcIter)
               {
                  ds.Write((*pcIter)->GetName());
                  
                  //skip read-only
                  //note: we do write their name, which could be more optimal and should
                  //probably be refactored
                  if(!(*pcIter)->IsReadOnly())
                  {
                     (*pcIter)->ToDataStream(ds);
                  }
               }

               //properties written, now lets write navmesh connections
               AIPluginInterface::ConstWaypointArray edges;
               mAIInterface->GetAllEdgesFromWaypoint((*wpIter)->GetID(), edges);

               ds.Write(unsigned(edges.size()));
               
               AIPluginInterface::ConstWaypointArray::iterator edgeIter = edges.begin();
               AIPluginInterface::ConstWaypointArray::iterator edgeIterEnd = edges.end();
               for(;edgeIter != edgeIterEnd; ++edgeIter)
               {
                  //write each destination
                  ds.Write((*edgeIter)->GetID());
               }
            }
         }
      }
      
      ds.Write(WaypointFileHeader::FILE_START_END_CHAR);
      outfile.write(ds.GetBuffer(), ds.GetBufferSize());
      outfile.close();

      return !outfile.fail();
   }

   void WaypointReaderWriter::Insert(WaypointInterface* waypoint, int searchLevel)
   {
      if(searchLevel != -1 || waypoint->GetWaypointType() == *WaypointTypes::WAYPOINT_COLLECTION)
      {
         WaypointCollection* wc = dynamic_cast<WaypointCollection*>(waypoint);

         if(wc != NULL)
         {
            mAIInterface->InsertCollection(wc, searchLevel);
         }
         else
         {
            throw dtUtil::Exception(dtDAL::ExceptionEnum::BaseException,
               "Error reading Waypoint file.", __FILE__, __LINE__);
         }
      }
      else
      {
         mAIInterface->InsertWaypoint(waypoint);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void WaypointReaderWriter::CreateWaypointCollectionEdges(ConstWaypointArray& wps, const NavMesh& nm)
   {
      WaypointGraph& wpGraph = mAIInterface->GetWaypointGraph();

      ConstWaypointArray::iterator iter = wps.begin();
      ConstWaypointArray::iterator iterEnd = wps.end();

      for(; iter != iterEnd; ++iter)
      {
         WaypointCollection* wc = wpGraph.FindCollection((*iter)->GetID());

         WaypointCollection::WaypointTree::const_child_iterator children = wc->begin_child();
         WaypointCollection::WaypointTree::const_child_iterator childrenEnd = wc->end_child();

         for(; children != childrenEnd; ++children)
         {
            const WaypointInterface* childPtr = children->value;

            NavMesh::NavMeshContainer::const_iterator nm_iter = nm.begin(childPtr);
            NavMesh::NavMeshContainer::const_iterator nm_iterEnd = nm.end(childPtr);

            //the actual child edges are stored on the WaypointCollection itself
            //wc->GetNavMesh().InsertCopy(nm_iter, nm_iterEnd);

            //the search level only contains edges relevant to the nodes in it
            for(;nm_iter != nm_iterEnd; ++nm_iter)
            {
               WaypointCollection* wpToParent = wpGraph.GetParent((*nm_iter).second->GetWaypointTo()->GetID());

               //todo- why would wpToParent ever be NULL
               if(wpToParent != NULL && childPtr->GetID() != (*nm_iter).second->GetWaypointTo()->GetID()) //wpParent may be ourself, this is ok because it adds our own child paths
               {
                  //this give the waypoint collection immediate child edges to all siblings
                  wc->AddEdge(wpToParent->GetID(), WaypointCollection::ChildEdge(childPtr->GetID(), (*nm_iter).second->GetWaypointTo()->GetID()));
               }
            }

         }

      }
   }
}//namespace dtAI

