
#ifndef __DELTA_CHARACTERSHADERBUILDER_H__
#define __DELTA_CHARACTERSHADERBUILDER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtAnim/export.h>
#include <osg/Referenced>
#include <string>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace osg
{
   class Geode;
}

namespace dtCore
{
   class ShaderProgram;
}

namespace dtAnim
{
   class BaseModelData;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT CharacterShaderBuilder : public osg::Referenced
   {
   public:
      static const std::string BONE_TRANSFORM_UNIFORM;

      CharacterShaderBuilder();

      dtCore::ShaderProgram* LoadShaders(dtAnim::BaseModelData& modelData, osg::Geode& geode) const;



   protected:
      virtual ~CharacterShaderBuilder();
   };

}

#endif
