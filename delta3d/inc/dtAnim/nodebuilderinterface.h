
#ifndef __DELTA_NODEBUILDERINTERFACE_H__
#define __DELTA_NODEBUILDERINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/export.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtCore/refptr.h>
#include <dtUtil/referencedinterface.h>
// OSG
#include <osg/Node>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT NodeBuilderInterface : public osg::Referenced
   {
   public:
      NodeBuilderInterface() {}

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateHardware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper) = 0;
   
   protected:
      virtual ~NodeBuilderInterface() {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template <class T_ModelWrapper>
   class DT_ANIM_EXPORT BaseNodeBuilder : public dtAnim::NodeBuilderInterface
   {
   public:
      typedef dtAnim::NodeBuilderInterface BaseClass;
      typedef T_ModelWrapper ModelWrapperType;

      BaseNodeBuilder() {}

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper)
      {
         return CreateSoftware(renderInfo, dynamic_cast<ModelWrapperType*>(wrapper));
      }

      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper)
      {
         return CreateSoftwareNoVBO(renderInfo, dynamic_cast<ModelWrapperType*>(wrapper));
      }

      virtual dtCore::RefPtr<osg::Node> CreateHardware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper)
      {
         return CreateHardware(renderInfo, dynamic_cast<ModelWrapperType*>(wrapper));
      }

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(osg::RenderInfo* renderInfo, ModelWrapperType* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, ModelWrapperType* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateHardware(osg::RenderInfo* renderInfo, ModelWrapperType* wrapper) = 0;
   
   protected:
      virtual ~BaseNodeBuilder() {}
   };

}

#endif
