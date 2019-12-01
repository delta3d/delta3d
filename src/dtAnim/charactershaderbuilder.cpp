
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/charactershaderbuilder.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/constants.h>
#include <dtCore/shadermanager.h>
#include <dtCore/shaderprogram.h>
#include <dtUtil/log.h>
#include <osg/Geode>



namespace dtAnim
{
   ////////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   ////////////////////////////////////////////////////////////////////////////////
   const std::string CharacterShaderBuilder::BONE_TRANSFORM_UNIFORM("boneTransforms");

   CharacterShaderBuilder::CharacterShaderBuilder()
   {}

   CharacterShaderBuilder::~CharacterShaderBuilder()
   {}

   dtCore::ShaderProgram* CharacterShaderBuilder::LoadShaders(dtAnim::BaseModelData& modelData, osg::Geode& geode) const
   {
      static const std::string hardwareSkinningSPGroup = "HardwareSkinning";
      dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
      dtCore::ShaderProgram* shaderProgram = NULL;
      if (!modelData.GetShaderGroupName().empty())
      {
         dtCore::ShaderGroup* spGroup = shaderManager.FindShaderGroupPrototype(modelData.GetShaderGroupName());
         if (spGroup != NULL)
         {
            if (!modelData.GetShaderName().empty())
            {
               shaderProgram = spGroup->FindShader(modelData.GetShaderName());
               if (shaderProgram == NULL)
               {
                  LOG_ERROR("Shader program \"" + modelData.GetShaderName() + "\" from group \""
                        + modelData.GetShaderGroupName() + "\" was not found, using the default from the group.");
                  shaderProgram = spGroup->GetDefaultShader();

                  if (shaderProgram == NULL)
                  {
                     LOG_ERROR("Shader Group \""  + modelData.GetShaderGroupName()
                           + "\" was not found, overriding to use the default group.");
                  }
               }
            }
            else
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
            // TODO: Change OSG implementation to use the same shader code.
            std::string vertProgramFile = "shaders/HardwareCharacter.vert";
            if (modelData.GetCharacterSystemType() == dtAnim::Constants::CHARACTER_SYSTEM_OSG)
            {
               vertProgramFile = "shaders/HardwareCharacter.vert";
            }

            defSPGroup = new dtCore::ShaderGroup(hardwareSkinningSPGroup);
            shaderProgram = new dtCore::ShaderProgram("Default");
            shaderProgram->AddVertexShader(vertProgramFile);
            defSPGroup->AddShader(*shaderProgram, true);
            shaderManager.AddShaderGroupPrototype(*defSPGroup);
         }
         else
         {
            shaderProgram = defSPGroup->GetDefaultShader();
         }
         modelData.SetShaderGroupName(hardwareSkinningSPGroup);
      }

      dtCore::ShaderProgram* shadProg = shaderManager.AssignShaderFromPrototype(*shaderProgram, geode);
   
      /* Begin figure out if this open gl implementation uses [0] on array uniforms
      * This seems to be an ATI/NVIDIA thing.
      * This requires me to force the shader to compile.
      */
      if (shadProg != NULL)
      {
         osg::Program* prog = shadProg->GetShaderProgram();
         dtCore::RefPtr<osg::State> tmpState = new osg::State;
         tmpState->setContextID(0);
         prog->compileGLObjects(*tmpState);

         //Cannot do getPCP(0) anymore, it takes a state& now, commented out until we know if we still need this -bga
         // TODO: Change OSG implementation to use the same shader code.
         /*std::string boneTransformUniform = BONE_TRANSFORM_UNIFORM;
         osg::Program::PerContextProgram* pcp = prog->getPCP(0);
         if (pcp != NULL && pcp->getUniformLocation(boneTransformUniform) == -1)
         {
            if (pcp != NULL && pcp->getUniformLocation(boneTransformUniform + "[0]") == -1)
            {
               LOG_ERROR("Can't find uniform named \"" + boneTransformUniform
                         + "\" which is required for skinning.");
            }
            else
            {
               boneTransformUniform.append("[0]");
            }
         }*/
      }

      return shadProg;
   }

}
