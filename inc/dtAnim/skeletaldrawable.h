#ifndef __DT_ANIM_SKELETAL_DRAWABLE_H__
#define __DT_ANIM_SKELETAL_DRAWABLE_H__

#include <dtAnim/export.h>              // for export symbol
#include <dtAnim/basemodelwrapper.h>
#include <osg/Drawable>                 // for base class
#include <dtCore/refptr.h>              // for member variable


namespace dtAnim
{
   class BoneInterface;

   /// Renders only the skeleton.
   class DT_ANIM_EXPORT SkeletalDrawable : public osg::Drawable
   {
   public:
      SkeletalDrawable(const dtAnim::BaseModelWrapper* model);

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
         virtual void Render(const dtAnim::BaseModelWrapper& model) const=0;
         virtual void SetRenderMode(unsigned char bits)=0;

         virtual ~IPrimitiveRenderObject();

         /// a functor to be used in a loop algorithm
         struct RenderPrimitive
         {
            dtCore::RefPtr<const dtAnim::BaseModelWrapper> mModelWrapper;

            RenderPrimitive(const dtAnim::BaseModelWrapper* model) : mModelWrapper(model) {}
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
         void Render(const dtAnim::BaseModelWrapper& model) const;
         void SetRenderMode(unsigned char bits);

         CPrimitiveRenderObject();
         ~CPrimitiveRenderObject();

      private:
         void RenderBoneIDs(GLenum primitive, const dtAnim::BaseModelWrapper& model) const;

         VectorPrimitives mChildren;
         VectorInt mIDs;

         typedef unsigned char RenderBitContainer;
         RenderBitContainer mRenderMode;
      };


      static void Init( SkeletalDrawable* instance );

      /// makes a primitive for rendering.
      /// primitives are defined as lists of point data, where each point has only one child.
      /// a new primitive is created when a point is discovered to have multiple children.
      static void PopulatePrimitive(const dtAnim::BaseModelWrapper& model,
                                    dtAnim::BoneInterface& bone,
                                    IPrimitiveRenderObject* primitive);

      dtCore::RefPtr<const dtAnim::BaseModelWrapper> mModelWrapper;

      VectorPrimitives mRootPrimitives;
   };
}

#endif // __DT_ANIM_SKELETAL_DRAWABLE_H__
