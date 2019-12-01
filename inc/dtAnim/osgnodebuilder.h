
#ifndef __DELTA_OSGNODEBUILDER_H__
#define __DELTA_OSGNODEBUILDER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/nodebuilderinterface.h>
#include <dtAnim/osgmodelwrapper.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT OsgNodeBuilder : public dtAnim::BaseNodeBuilder<dtAnim::OsgModelWrapper>
   {
   public:
      typedef dtAnim::BaseNodeBuilder<dtAnim::OsgModelWrapper> BaseClass;

      OsgNodeBuilder();

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::OsgModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::OsgModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateHardware(osg::RenderInfo* renderInfo, dtAnim::OsgModelWrapper* wrapper);
      
      dtCore::RefPtr<osg::Node> CreateNode(dtAnim::OsgModelWrapper& wrapper);

      // Returns TRUE if the mode has changed.
      bool EnsureMode(dtAnim::OsgModelWrapper& wrapper, osg::Node& modelNode, bool hardware);
   
   protected:
      virtual ~OsgNodeBuilder();
   };
}

#endif
