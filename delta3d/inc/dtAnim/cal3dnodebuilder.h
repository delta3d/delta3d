
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
   class DT_ANIM_EXPORT Cal3dBoundingSphereCalculator : public osg::Node::ComputeBoundingSphereCallback
   {
      public:
         Cal3dBoundingSphereCalculator(dtAnim::Cal3DModelWrapper& wrapper);

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

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(dtAnim::Cal3DModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(dtAnim::Cal3DModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateHardware(dtAnim::Cal3DModelWrapper* wrapper);

   protected:
      virtual ~Cal3dNodeBuilder();

      dtCore::RefPtr<osg::Node> CreateSoftwareInternal(Cal3DModelWrapper* wrapper, bool vbo);

      void CalcNumVertsAndIndices(Cal3DModelWrapper* wrapper,
                                                int& numVerts, int& numIndices);
   };
}

#endif
