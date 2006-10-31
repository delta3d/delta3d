/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * William E. Johnson II
 */

#include "dtActors/meshterrainactorproxy.h"
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/resourcedescriptor.h>
#include <dtDAL/actorproxyicon.h>

#include <dtCore/object.h>

#include <osg/NodeCallback>
#include <osg/PagedLOD>
#include <osg/io_utils>

#include <iostream>

using namespace dtCore;
using namespace dtDAL;

namespace dtActors
{
   
   class TestCullCallback: public osg::NodeCallback
   {
      public:
         
         /** Callback method called by the NodeVisitor when visiting a node.*/
         virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
         { 
               std::cout << "traversing node " << node->className() << std::endl;
               
               std::cout << nv->getEyePoint() << std::endl;
               
               traverse(node,nv);
         }
   };

   class TestNodeVisitor: public osg::NodeVisitor
   {
      public:
         
         TestNodeVisitor(): osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
         {}
         
         virtual void apply(osg::PagedLOD& node)
         {
            std::cout << "Traversing node " << node.className() << std::endl;            
            std::cout << " Database path " << node.getDatabasePath() << std::endl; 
            
            for (unsigned i = 0; i < node.getNumPriorityOffsets(); ++i)
            {
               std::cout << " Priority Offset " << i << " " << node.getPriorityOffset(i) << std::endl;
            }
            
            traverse(node);               
         }
   };


   MeshTerrainActorProxy::MeshTerrainActorProxy()
   {
      SetClassName("dtCore::Object");
   }

   void MeshTerrainActorProxy::BuildPropertyMap()
   {
      const std::string &GROUPNAME = "Terrain";
      DeltaObjectActorProxy::BuildPropertyMap();
      
      Object *obj = dynamic_cast<Object*>(GetActor());
      if (obj == NULL)
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type dtCore::Object");
      
      AddProperty(new dtDAL::ResourceActorProperty(*this, dtDAL::DataType::TERRAIN,
                                                   "terrain mesh", "Terrain Mesh", MakeFunctor(*this, &MeshTerrainActorProxy::LoadFile),
                                                   "The mesh that defines the geometry of the terrain.", GROUPNAME));
   }


   ///////////////////////////////////////////////////////////////////////////////
   void MeshTerrainActorProxy::LoadFile(const std::string &fileName)
   {
      dtCore::Object *obj = dynamic_cast<dtCore::Object*>(GetActor());
      if (obj == NULL)
      {
         EXCEPT(dtDAL::ExceptionEnum::InvalidActorException,
                "Actor should be type dtCore::Object");
      }
      
      if (obj->LoadFile(fileName, false) == NULL)
      {
         if (!fileName.empty())
            LOG_ERROR("Error loading terrain mesh file: " + fileName);
         
         return;
      }
      
      //GetActor()->GetOSGNode()->setCullCallback(new TestCullCallback());
      //dtCore::RefPtr<TestNodeVisitor> visitor = new TestNodeVisitor();
      //GetActor()->GetOSGNode()->accept(*visitor);
      
      //We need this little hack to ensure that when a mesh is loaded, the collision
      //properties get updated properly.
      SetCollisionType(GetCollisionType());
   }

   ///////////////////////////////////////////////////////////////////////////////
   const dtDAL::ActorProxy::RenderMode& MeshTerrainActorProxy::GetRenderMode()
   {
      dtDAL::ResourceDescriptor* resource = GetResource("terrain mesh");
      if (resource != NULL)
      {
         if (resource->GetResourceIdentifier().empty() || GetActor()->GetOSGNode() == NULL)
            return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
         else
            return dtDAL::ActorProxy::RenderMode::DRAW_ACTOR;
      }
      else
         return dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   dtDAL::ActorProxyIcon* MeshTerrainActorProxy::GetBillBoardIcon()
   {
      if(!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtDAL::ActorProxyIcon(dtDAL::ActorProxyIcon::IconType::MESHTERRAIN);
      }
      
      return mBillBoardIcon.get();
   }

   //////////////////////////////////////////////////////////////////////////
   MeshTerrainActor::MeshTerrainActor()
   {

   }

   MeshTerrainActor::~MeshTerrainActor()
   {

   }

   void MeshTerrainActor::AddedToScene(dtCore::Scene* scene)
   {
      dtCore::Object::AddedToScene(scene);
      if(!GetFilename().empty())
         LoadFile(GetFilename());
   }
}
