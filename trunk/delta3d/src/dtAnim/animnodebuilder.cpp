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
#include <dtAnim/geometrybuilder.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/project.h>
#include <dtUtil/mathdefines.h>
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
   GeometryBuilder AnimNodeBuilder::mGeometryBuilder;

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

      virtual void drawImplementation(osg::RenderInfo& renderInfo, const osg::Drawable*) const
      {
         if (!mCreatedNode.valid())
         {
            mCreatedNode = mCreateFunc(&renderInfo, mWrapper);
            mWrapper = NULL;
         }
      }

      mutable dtCore::RefPtr<osg::Node> mCreatedNode;

   private:
      AnimNodeBuilder::CreateFunc mCreateFunc;
      mutable dtCore::RefPtr<dtAnim::BaseModelWrapper> mWrapper;
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
            mGroupToAddTo->removeChild(0, mGroupToAddTo->getNumChildren());
            mGroupToAddTo->addChild(mCreateCB->mCreatedNode.get());
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
AnimNodeBuilder::AnimNodeBuilder(bool useDeprecatedHardwareModel)
   : mUseDeprecatedHardwareModel(useDeprecatedHardwareModel)
{
   if (SupportsHardware())
   {
      if(mUseDeprecatedHardwareModel)
      {
         SetCreate(CreateFunc(this, &AnimNodeBuilder::CreateHardware));
      }
      else
      {
         SetCreate(CreateFunc(&mGeometryBuilder, &GeometryBuilder::CreateGeometry));
      }
   }
   else
   if (SupportsSoftware())
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
   dtCore::RefPtr<osg::Node> result;
   if(!immediate)
   {
      ///Add a temporary rendered shape with a draw callback to a Group.  The callback
      ///will postpone the creation of the real geometry until a valid openGL
      ///context is available.
      dtCore::RefPtr<CreateGeometryDrawCallback> createCallback = new CreateGeometryDrawCallback(mCreateFunc, wrapper);
      osg::Group* rootNode = new osg::Group();
      rootNode->setUpdateCallback(new UpdateCallback(createCallback, *rootNode));

      osg::Geode* defaultGeode = new osg::Geode();
      osg::Cylinder* shape = new osg::Cylinder(osg::Vec3(0.0f, 0.0f, 0.0f), 2.0f, 4.0f);
      osg::ShapeDrawable* defaultDrawable = new osg::ShapeDrawable(shape);
      defaultDrawable->setDrawCallback(createCallback);

      defaultGeode->addDrawable(defaultDrawable);
      rootNode->addChild(defaultGeode);

      result = rootNode;
   }
   else
   {
      result = mCreateFunc(0, wrapper);
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper)
{
   dtCore::RefPtr<osg::Node> node;
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder = CreateNodeBuilder(*wrapper);
   
   if (nodeBuilder.valid())
   {
      node = nodeBuilder->CreateSoftware(renderInfo, wrapper);
   }
   
   if ( ! node.valid())
   {
      LOG_ERROR("Could note create software mesh.");
   }

   return node;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper)
{
   dtCore::RefPtr<osg::Node> node;
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder = CreateNodeBuilder(*wrapper);
   
   if (nodeBuilder.valid())
   {
      node = nodeBuilder->CreateSoftwareNoVBO(renderInfo, wrapper);
   }
   
   if ( ! node.valid())
   {
      LOG_ERROR("Could note create software mesh without VBO.");
   }

   return node;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateHardware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper)
{
   dtCore::RefPtr<osg::Node> node;
   dtCore::RefPtr<dtAnim::NodeBuilderInterface> nodeBuilder = CreateNodeBuilder(*wrapper);
   
   if (nodeBuilder.valid())
   {
      node = nodeBuilder->CreateHardware(renderInfo, wrapper);
   }
   
   if ( ! node.valid())
   {
      LOG_ERROR("Could note create hardware mesh.");
   }

   return node;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Node> AnimNodeBuilder::CreateNULL(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper)
{
   DTUNREFERENCED_PARAMETER(wrapper);

   dtCore::RefPtr<osg::Geode> geode = new osg::Geode();

   //NULL create function.  Used if hardware and software create functions fail.
   return geode;
}

////////////////////////////////////////////////////////////////////////////////
dtCore::ShaderProgram* AnimNodeBuilder::LoadShaders(BaseModelData& modelData, osg::Geode& geode)
{
   static const std::string hardwareSkinningSPGroup = "HardwareSkinningLegacy";
   dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
   dtCore::ShaderProgram* shaderProgram = NULL;
   if (!modelData.GetShaderGroupName().empty())
   {
      dtCore::ShaderGroup* spGroup = shaderManager.FindShaderGroupPrototype(modelData.GetShaderGroupName());
      if (spGroup != NULL)
      {
         bool editMode = dtCore::Project::GetInstance().GetEditMode();

         if (editMode)
         {
            shaderProgram = spGroup->GetEditorShader();
         }

         if (shaderProgram == NULL && !modelData.GetShaderName().empty())
         {
            shaderProgram = spGroup->FindShader(modelData.GetShaderName());
            if (shaderProgram == NULL)
            {
               LOG_ERROR("Shader program \"" + modelData.GetShaderName() + "\" from group \""
                     + modelData.GetShaderGroupName() + "\" was not found, using the default from the group.");
            }
         }

         if (shaderProgram == NULL)
         {
            shaderProgram = spGroup->GetDefaultShader();
            if (shaderProgram == NULL)
            {
               LOG_ERROR("Shader Group \""  + modelData.GetShaderGroupName()
                     + "\" was not found, overriding to use the default group.");
            }
         }
      }
   }

   //If no shader group is setup, create one.
   if (shaderProgram == NULL)
   {
      dtCore::ShaderGroup* defSPGroup = shaderManager.FindShaderGroupPrototype(hardwareSkinningSPGroup);
      if (defSPGroup == NULL)
      {
         defSPGroup = new dtCore::ShaderGroup(hardwareSkinningSPGroup);
         shaderProgram = new dtCore::ShaderProgram("Default");
         shaderProgram->AddVertexShader("shaders/HardwareCharacter.vert");
         defSPGroup->AddShader(*shaderProgram, true);
         shaderManager.AddShaderGroupPrototype(*defSPGroup);
      }
      else
      {
         shaderProgram = defSPGroup->GetDefaultShader();
      }
      modelData.SetShaderGroupName(hardwareSkinningSPGroup);
   }
   return shaderManager.AssignShaderFromPrototype(*shaderProgram, geode);
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
   return (osg::isGLExtensionSupported(0, "GL_ARB_vertex_buffer_object"));
}

}//namespace dtAnim
