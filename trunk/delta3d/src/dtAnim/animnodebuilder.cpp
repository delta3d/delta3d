/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 03/28/2007
 */
#include <dtAnim/animnodebuilder.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/cal3dnodebuilder.h>
#include <dtAnim/constants.h>
#include <dtAnim/hardwaresubmesh.h>
#include <dtAnim/modeldatabase.h>
#include <dtAnim/osgnodebuilder.h>
#include <dtAnim/submesh.h>
#include <dtCore/shadergroup.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderprogram.h>
#include <dtUtil/log.h>

#include <osg/Geode>
#include <osg/State>
#include <osg/BoundingSphere>
#include <osg/BoundingBox>
#include <osg/Texture2D>
#include <osg/GLExtensions>
#include <osg/ShapeDrawable>

namespace dtAnim
{

   ///Used to delay the building of the animated characters geometry until
   ///it is first rendered, at which point a valid OpenGL context should be valid
   class CreateGeometryDrawCallback : public osg::Drawable::DrawCallback
   {
   public:
      CreateGeometryDrawCallback(AnimNodeBuilder::CreateFunc& func,
         dtAnim::BaseModelWrapper* wrapper)
         : mCreatedNode(NULL)
         , mCreateFunc(func)
         , mWrapper(wrapper)
      {
      };

      ~CreateGeometryDrawCallback()
      {
      };

      virtual void drawImplementation(osg::RenderInfo&, const osg::Drawable*) const
      {
         if (!mCreatedNode.valid())
         {
            CreateGeometryDrawCallback* const_this = const_cast<CreateGeometryDrawCallback*>(this);
            const_this->mCreatedNode = mCreateFunc(mWrapper);
         }
      }

      dtCore::RefPtr<osg::Node> mCreatedNode;

   private:
      AnimNodeBuilder::CreateFunc mCreateFunc;
      dtCore::RefPtr<dtAnim::BaseModelWrapper> mWrapper;
   };

   ///Used to grab the created geometry from the CreateGeometryDrawCallback
   ///and add it as a child to the supplied Group
   class UpdateCallback : public osg::NodeCallback
   {
   public:
      UpdateCallback(CreateGeometryDrawCallback* callback, osg::Group& group)
         : mCreateCB(callback)
         , mGroupToAddTo(&group)
      {
      }

      ~UpdateCallback()
      {
      }

      virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
      {
         //wait until the create draw-callback has a valid node created
         if (mCreateCB->mCreatedNode.valid())
         {
            //then add it, remove the temp geometry, and remove this callback
            mGroupToAddTo->addChild(mCreateCB->mCreatedNode.get());
            mGroupToAddTo->removeChild(0, 1);
            mGroupToAddTo->setUpdateCallback(NULL);
         }
         else
         {
            traverse(node,nv);
         }
      }

   private:
      dtCore::RefPtr<CreateGeometryDrawCallback> mCreateCB;
      osg::observer_ptr<osg::Group> mGroupToAddTo;
   };

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::AnimNodeBuilder()
{
   if (SupportsHardware())
   {
      SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateHardware));
   }
   else if (SupportsSoftware())
   {
      SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateSoftware));
   }
   else
   {
      LOG_WARNING("dtAnim could not deduce the hardware capability, no geometry will be rendered.")
      SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateNULL));
   }
}

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::AnimNodeBuilder(const CreateFunc& pCreate)
   : mCreateFunc(pCreate)
{}

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::~AnimNodeBuilder()
{}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtAnim::NodeBuilderInterface>
   AnimNodeBuilder::CreateNodeBuilder(const std::string& charSystem)
{
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder;

   if (charSystem == dtAnim::Constants::CHARACTER_SYSTEM_CAL3D)
   {
      nodeBuilder = new dtAnim::Cal3dNodeBuilder;
   }
   else if (charSystem == dtAnim::Constants::CHARACTER_SYSTEM_OSG)
   {
      nodeBuilder = new dtAnim::OsgNodeBuilder;
   }

   return nodeBuilder;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<dtAnim::NodeBuilderInterface>
   AnimNodeBuilder::CreateNodeBuilder(dtAnim::BaseModelWrapper& wrapper)
{
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder;

   dtAnim::BaseModelData* modelData = wrapper.GetModelData();

   if (modelData != NULL)
   {
      nodeBuilder = CreateNodeBuilder(modelData->GetCharacterSystemType());
   }

   return nodeBuilder;
}

////////////////////////////////////////////////////////////////////////////////
AnimNodeBuilder::CreateFunc& AnimNodeBuilder::GetCreate()
{
   return mCreateFunc;
}

////////////////////////////////////////////////////////////////////////////////
void AnimNodeBuilder::SetCreate(const CreateFunc& pCreate)
{
   mCreateFunc = pCreate;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateNode(dtAnim::BaseModelWrapper* wrapper, bool immediate)
{
   if(!immediate)
   {
      ///Add a temporary rendered shape with a draw callback to a Group.  The callback
      ///will postpone the creation of the real geometry until a valid openGL
      ///context is available.
      dtCore::RefPtr<CreateGeometryDrawCallback> createCallback = new CreateGeometryDrawCallback(mCreateFunc, wrapper);
      osg::Group* rootNode = new osg::Group();
      rootNode->setUpdateCallback(new UpdateCallback(createCallback, *rootNode));

      osg::Geode* defaultGeode = new osg::Geode();
      osg::Cylinder* shape = new osg::Cylinder(osg::Vec3(0.f, 0.f, 0.f), 2.f, 4.f);
      osg::ShapeDrawable* defaultDrawable = new osg::ShapeDrawable(shape);
      defaultDrawable->setDrawCallback(createCallback);

      defaultGeode->addDrawable(defaultDrawable);
      rootNode->addChild(defaultGeode);
      return rootNode;
   }
   else
   {
      return mCreateFunc(wrapper);
   }
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateSoftware(dtAnim::BaseModelWrapper* wrapper)
{
   dtCore::RefPtr<osg::Node> node;
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder = CreateNodeBuilder(*wrapper);
   
   if (nodeBuilder.valid())
   {
      node = nodeBuilder->CreateSoftware(wrapper);
   }
   
   if ( ! node.valid())
   {
      LOG_ERROR("Could note create software mesh.");
   }

   return node;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateSoftwareNoVBO(dtAnim::BaseModelWrapper* wrapper)
{
   dtCore::RefPtr<osg::Node> node;
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder = CreateNodeBuilder(*wrapper);
   
   if (nodeBuilder.valid())
   {
      node = nodeBuilder->CreateSoftwareNoVBO(wrapper);
   }
   
   if ( ! node.valid())
   {
      LOG_ERROR("Could note create software mesh without VBO.");
   }

   return node;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateHardware(dtAnim::BaseModelWrapper* wrapper)
{
   dtCore::RefPtr<osg::Node> node;
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder = CreateNodeBuilder(*wrapper);
   
   if (nodeBuilder.valid())
   {
      node = nodeBuilder->CreateHardware(wrapper);
   }
   
   if ( ! node.valid())
   {
      LOG_ERROR("Could note create hardware mesh.");
   }

   return node;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateNULL(dtAnim::BaseModelWrapper* wrapper)
{
   DTUNREFERENCED_PARAMETER(wrapper);

   dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

   //NULL create function.  Used if hardware and software create functions fail.
   return geode;
}

////////////////////////////////////////////////////////////////////////////////
bool AnimNodeBuilder::SupportsHardware() const
{
   //check if hardware supports our requirements
   return SupportsVertexBuffers();
}

////////////////////////////////////////////////////////////////////////////////
bool AnimNodeBuilder::SupportsSoftware() const
{
   //check if hardware supports our requirements
   return SupportsVertexBuffers();
}

////////////////////////////////////////////////////////////////////////////////
bool AnimNodeBuilder::SupportsVertexBuffers() const
{
   //see if we can support vertex buffer objects
   osg::Drawable::getExtensions(0, true);

   return (osg::isGLExtensionSupported(0, "GL_ARB_vertex_buffer_object"));
}

}//namespace dtAnim
