#ifndef __DT_ANIM_SKELETAL_DRAWABLE_H__
#define __DT_ANIM_SKELETAL_DRAWABLE_H__

#include <dtAnim/export.h>              // for export symbol
#include <dtAnim/cal3dmodelwrapper.h>
#include <osg/Drawable>                 // for base class
#include <dtCore/refptr.h>              // for member variable


namespace dtAnim
{

   /// Renders only the skeleton.
   class DT_ANIM_EXPORT SkeletalDrawable : public osg::Drawable
   {
   public:
      SkeletalDrawable(const Cal3DModelWrapper* model);

      void drawImplementation(osg::RenderInfo& RenderInfo) const;

      osg::Object* cloneType() const;
      osg::Object* clone(const osg::CopyOp& copyop) const;

   protected:
      ~SkeletalDrawable();

   private:
      SkeletalDrawable(const SkeletalDrawable&);
      SkeletalDrawable& operator =(const SkeletalDrawable&);

      struct IPrimitiveRenderObject
      {
         enum RenderModeBits
         {
            RENDER_MODE_NONE         = 0,
            RENDER_MODE_POINTS       = 1 << 0,
            RENDER_MODE_LINESEGMENTS = 1 << 1
         };

         virtual void AddChild(const IPrimitiveRenderObject* prims)=0;
         virtual void AddID(int id)=0;
         virtual void Render(const Cal3DModelWrapper& model) const=0;
         virtual void SetRenderMode(unsigned char bits)=0;

         virtual ~IPrimitiveRenderObject();

         /// a functor to be used in a loop algorithm
         struct RenderPrimitive
         {
            dtCore::RefPtr<const Cal3DModelWrapper> mModelWrapper;

            RenderPrimitive(const Cal3DModelWrapper* model) : mModelWrapper(model) {}
            RenderPrimitive(const RenderPrimitive& same): mModelWrapper(same.mModelWrapper) {}
            void operator ()(const dtAnim::SkeletalDrawable::IPrimitiveRenderObject* ptr) const
            {
               ptr->Render( *mModelWrapper );
            }

         private:
            RenderPrimitive();  ///< not implemented by design
            RenderPrimitive& operator =(const RenderPrimitive&);  ///< not implemented by design
         };
      };

      typedef std::vector<const IPrimitiveRenderObject*> VectorPrimitives;
      typedef std::vector<int> VectorInt;

      struct CPrimitiveRenderObject : public IPrimitiveRenderObject
      {
         void AddChild(const IPrimitiveRenderObject* prims);
         void AddID(int id);
         void Render(const Cal3DModelWrapper& model) const;
         void SetRenderMode(unsigned char bits);

         CPrimitiveRenderObject();
         ~CPrimitiveRenderObject();

      private:
         void RenderBoneIDs(GLenum primitive, const dtAnim::Cal3DModelWrapper& model) const;

         VectorPrimitives mChildren;
         VectorInt mIDs;

         typedef unsigned char RenderBitContainer;
         RenderBitContainer mRenderMode;
      };


      static void Init( SkeletalDrawable* instance );

      /// makes a primitive for rendering.
      /// primitives are defined as lists of point data, where each point has only one child.
      /// a new primitive is created when a point is discovered to have multiple children.
      static void PopulatePrimitive(const Cal3DModelWrapper& model,
                                    int boneID,
                                    IPrimitiveRenderObject* primitive);

      dtCore::RefPtr<const Cal3DModelWrapper> mModelWrapper;

      VectorPrimitives mRootPrimitives;
   };
}

#endif // __DT_ANIM_SKELETAL_DRAWABLE_H__
