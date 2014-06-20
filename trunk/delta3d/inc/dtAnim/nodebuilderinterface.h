
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

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(dtAnim::BaseModelWrapper* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(dtAnim::BaseModelWrapper* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateHardware(dtAnim::BaseModelWrapper* wrapper) = 0;
   
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

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(dtAnim::BaseModelWrapper* wrapper)
      {
         return CreateSoftware(dynamic_cast<ModelWrapperType*>(wrapper));
      }

      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(dtAnim::BaseModelWrapper* wrapper)
      {
         return CreateSoftwareNoVBO(dynamic_cast<ModelWrapperType*>(wrapper));
      }

      virtual dtCore::RefPtr<osg::Node> CreateHardware(dtAnim::BaseModelWrapper* wrapper)
      {
         return CreateHardware(dynamic_cast<ModelWrapperType*>(wrapper));
      }

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(ModelWrapperType* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(ModelWrapperType* wrapper) = 0;
      virtual dtCore::RefPtr<osg::Node> CreateHardware(ModelWrapperType* wrapper) = 0;
   
   protected:
      virtual ~BaseNodeBuilder() {}
   };

}

#endif
