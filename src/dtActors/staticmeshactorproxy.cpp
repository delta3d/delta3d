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
 * David Guthrie
 */
#include <osg/Geometry>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <dtActors/staticmeshactorproxy.h>

#include <dtCore/resourceactorproperty.h>
#include <dtCore/resourcedescriptor.h>
#include <dtCore/actorproxyicon.h>
#include <dtCore/datatype.h>
#include <dtCore/functor.h>
#include <dtCore/object.h>

#include <dtUtil/log.h>

#include <sstream>
#include <set>

namespace dtActors
{
   // This class is not actually used in StaticMeshActorProxy, so it's #ifdef'd out
   // for now to avoid linker warnings. -osb
   #if 0

   /**
    * This is a node visitor which searches for any textures and extracts them.
    * It is important to note that this visitor will only check geodes and their
    * associated drawables.
    */
   class ExtractTexturesVisitor : public osg::NodeVisitor
   {
   public:
      typedef std::set<osg::ref_ptr<osg::Texture2D> > TextureList;

      ExtractTexturesVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      {
         mMaxTexCoordArrayCount = 0;
      }

      virtual ~ExtractTexturesVisitor()
      {

      }

      /**
       * Searches a geode for drawables containing texture coordinates.
       * Based on which texture units the coordinates are mapped to,
       * appropriate texture slots are extracted.
       * @param geode The geometry node to check.
       */
      virtual void apply(osg::Geode &geode)
      {
         FindTextures(geode.getStateSet());
         for (unsigned int i=0; i<geode.getNumDrawables(); i++)
         {
            osg::Geometry *geom = geode.getDrawable(i)->asGeometry();
            if (geom != NULL)
            {
               FindTextures(geom->getStateSet());
               unsigned int texCoordCount = geom->getNumTexCoordArrays();
               if (texCoordCount > mMaxTexCoordArrayCount)
               {
                  mMaxTexCoordArrayCount = texCoordCount;
               }
            }
         }

         traverse(geode);
      }

      /**
       * Searchs a stateset for texture attributes.  These attributes are then
       * extracted into a list of textures.
       * @param ss
       */
      void FindTextures(osg::StateSet *ss)
      {
         if (ss == NULL)
         {
            return;
         }

         osg::StateSet::TextureAttributeList texAttribs;
         osg::StateSet::TextureAttributeList::iterator itor;
//          //unsigned int currTextureCount = mTextureList.size();

         texAttribs = ss->getTextureAttributeList();
         for (itor=texAttribs.begin(); itor!=texAttribs.end(); ++itor)
         {
            osg::StateSet::AttributeList &attribs = *itor;
            osg::StateSet::AttributeList::iterator attribItor = attribs.begin();
            while (attribItor != attribs.end())
            {
               if (attribItor->first.first == osg::StateAttribute::TEXTURE)
               {
                  osg::Texture2D *tex2D =
                     dynamic_cast<osg::Texture2D *>(attribItor->second.first.get());
                  if (tex2D != NULL)
                  {
                     mTextureList.insert(tex2D);
                  }
               }

               ++attribItor;
            }
         }
      }

      /**
       * Gets the list of textures found by the visitor.
       * @return
       */
      TextureList& GetTextureList()
      {
         return mTextureList;
      }

      /**
       * Returns the number of texture coordniate arrays found during
       * the traversal of this visitor.
       * @return
       */
      unsigned int GetMaxTexCoordCount() const
      {
         return mMaxTexCoordArrayCount;
      }

   private:
      TextureList mTextureList;
      unsigned int mMaxTexCoordArrayCount;
   };

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshActorProxy::TextureEntry::LoadFile(const std::string &fileName)
   {
      if (!mTexture.valid())
      {
         return;
      }

      osg::Image *newImage = osgDB::readImageFile(fileName);
      mTexture->setImage(newImage);
      mTexture->dirtyTextureObject();
   }

   #endif // 0

   ///////////////////////////////////////////////////////////////////////////////
   StaticMeshActorProxy::~StaticMeshActorProxy()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshActorProxy::CreateDrawable()
   {
      SetDrawable(*new dtCore::Object);

      static int actorCount = 0;
      std::ostringstream ss;
      ss << "StaticMesh" << actorCount++;
      SetName(ss.str());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshActorProxy::BuildPropertyMap()
   {
      const std::string GROUPNAME = "Mesh";
      DeltaObjectActorProxy::BuildPropertyMap();

      AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::STATIC_MESH,
         "static mesh", "Static Mesh",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &StaticMeshActorProxy::SetStaticMesh),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &StaticMeshActorProxy::GetStaticMesh),
         "The static mesh resource that defines the geometry", GROUPNAME));
   }

   DT_IMPLEMENT_ACCESSOR_GETTER(StaticMeshActorProxy, dtCore::ResourceDescriptor, StaticMesh)

   void StaticMeshActorProxy::SetStaticMesh(const dtCore::ResourceDescriptor& rd)
   {
      mStaticMesh = rd;
      std::string fileName =  dtCore::ResourceActorProperty::GetResourcePath(rd);

      dtCore::Object *obj = NULL;
      GetDrawable(obj);

      //First load the mesh (with cacheing on).
      if (obj != NULL && obj->LoadFile(fileName,true) == NULL)
      {
         if (!fileName.empty())
         {
            LOG_ERROR("Error loading mesh file: " + fileName);
         }
         return;
      }

      //We need this little hack to ensure that when a mesh is loaded, the collision
      //properties get updated properly.
      SetCollisionType(GetCollisionType());

   }


   ///////////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& StaticMeshActorProxy::GetRenderMode()
   {
      dtCore::ResourceDescriptor resource = GetStaticMesh();
      if (resource.IsEmpty() == false)
      {
         if (resource.GetResourceIdentifier().empty() || GetDrawable()->GetOSGNode() == NULL)
         {
            return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
         }
         else
         {
            return dtCore::BaseActorObject::RenderMode::DRAW_ACTOR;
         }
      }
      else
      {
         return dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon *StaticMeshActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         dtCore::ActorProxyIcon::ActorProxyIconConfig cfg(false,false,1.0f);
         mBillBoardIcon =
            new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_STATICMESH, cfg);
      }

      return mBillBoardIcon.get();
   }
}
