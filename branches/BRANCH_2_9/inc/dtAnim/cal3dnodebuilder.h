
#ifndef __DELTA_CAL3DNODEBUILDER_H__
#define __DELTA_CAL3DNODEBUILDER_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// DELTA3D
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtAnim/nodebuilderinterface.h>



namespace dtAnim
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3DBoundingSphereCalculator : public osg::Node::ComputeBoundingSphereCallback
   {
      public:
         Cal3DBoundingSphereCalculator(dtAnim::Cal3DModelWrapper& wrapper);

         /*virtual*/ osg::BoundingSphere computeBound(const osg::Node&) const;

      private:
         dtCore::RefPtr<dtAnim::Cal3DModelWrapper> mWrapper;
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_ANIM_EXPORT Cal3dNodeBuilder : public dtAnim::BaseNodeBuilder<dtAnim::Cal3DModelWrapper>
   {
   public:
      typedef dtAnim::BaseNodeBuilder<dtAnim::Cal3DModelWrapper> BaseClass;

      Cal3dNodeBuilder();

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::Cal3DModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::Cal3DModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateHardware(osg::RenderInfo* renderInfo, dtAnim::Cal3DModelWrapper* wrapper);

   protected:
      virtual ~Cal3dNodeBuilder();

      dtCore::RefPtr<osg::Node> CreateSoftwareInternal(osg::RenderInfo* renderInfo, Cal3DModelWrapper* wrapper, bool vbo);

      void CalcNumVertsAndIndices(Cal3DModelWrapper* wrapper,
                                                int& numVerts, int& numIndices);
   };
}

#endif
