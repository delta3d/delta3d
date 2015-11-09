#include <prefix/dtcoreprefix.h>
#include <dtCore/axistoaxistransformation.h>

namespace dtCore {

   /////////////////////////////////////////////////////////////////////////////
   AxisToAxisTransformation::AxisToAxisTransformation(Axis* sourceAxis, dtUtil::Transformation<double>* transformation)
      : mSourceAxis(sourceAxis)
      , mTargetAxis(NULL)
      , mpTransformation(transformation)
   {}

   /////////////////////////////////////////////////////////////////////////////
   AxisToAxisTransformation::~AxisToAxisTransformation()
   {
      if (mSourceAxis.valid())
      {
         mSourceAxis->RemoveAxisHandler(this);
      }

      if (mpTransformation)
      {
         delete mpTransformation;
         mpTransformation = NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void AxisToAxisTransformation::SetSourceAxis(Axis* sourceAxis)
   {
      if (mSourceAxis.valid())
      {
         mSourceAxis->RemoveAxisHandler(this);
      }

      mSourceAxis = sourceAxis;

      if (mSourceAxis.valid() && mTargetAxis.valid())
      {
         mSourceAxis->AddAxisHandler(this);
      }

      UpdateTargetAxisState();
   }

   /////////////////////////////////////////////////////////////////////////////
   Axis* AxisToAxisTransformation::GetSourceAxis()
   {
      return mSourceAxis.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AxisToAxisTransformation::SetTargetAxis(LogicalAxis* targetAxis)
   {
      mTargetAxis = targetAxis;

      if (mSourceAxis.valid() && mTargetAxis.valid())
      {
         mSourceAxis->AddAxisHandler(this);
      }
      else
      {
         mSourceAxis->RemoveAxisHandler(this);
      }

      UpdateTargetAxisState();
   }

   /////////////////////////////////////////////////////////////////////////////
   LogicalAxis* AxisToAxisTransformation::GetTargetAxis()
   {
      return mTargetAxis.get();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AxisToAxisTransformation::SetTransformation(dtUtil::Transformation<double>* transformation)
   {
      mpTransformation = transformation;

      UpdateTargetAxisState();
   }

   /////////////////////////////////////////////////////////////////////////////
   dtUtil::Transformation<double> *AxisToAxisTransformation::GetTransformation() const
   {
      return mpTransformation;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool AxisToAxisTransformation::HandleAxisStateChanged(const Axis* /*axis*/,
                                                   double /*oldState*/,
                                                   double newState,
                                                   double /*delta*/)
   {
      if (mTargetAxis.valid())
      {
         //return mTargetAxis->SetState(newState*mScale + mOffset, delta*mScale);
         // ??? how to tweak delta ???
         return mTargetAxis->SetState((*mpTransformation)(newState), 0.0f);
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   bool AxisToAxisTransformation::UpdateTargetAxisState()
   {
      if (mTargetAxis.valid())
      {
         double value = 0.0;

         if (mSourceAxis.valid())
         {
            value = mSourceAxis->GetState();
         }

         return mTargetAxis->SetState((*mpTransformation)(value));
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////

} // namespace dtCore
