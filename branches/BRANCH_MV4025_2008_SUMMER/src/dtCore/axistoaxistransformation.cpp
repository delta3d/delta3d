#include <dtCore/axistoaxistransformation.h>

namespace dtCore {

   /**
   * Constructor.
   *
   * @param sourceAxis the source axis
   * @param transformation the arbitrary transformation
   */
   AxisToAxisTransformation::AxisToAxisTransformation(Axis *sourceAxis, dtUtil::Transformation<double> *transformation)
      : mSourceAxis(sourceAxis)
      , mTargetAxis(NULL)
      , mpTransformation(transformation)
   {}


   AxisToAxisTransformation::~AxisToAxisTransformation()
   {
      if (mSourceAxis.valid())
      {
         mSourceAxis->RemoveAxisListener(this);
      }
      if (mpTransformation)
      {
         delete mpTransformation;
         mpTransformation = NULL;
      }
   }


   /**
   * Sets the source axis.
   *
   * @param sourceAxis the new source axis
   */
   void AxisToAxisTransformation::SetSourceAxis(Axis *sourceAxis)
   {
      if (mSourceAxis.valid())
      {
         mSourceAxis->RemoveAxisListener(this);
      }

      mSourceAxis = sourceAxis;

      if (mSourceAxis.valid() && mTargetAxis.valid())
      {
         mSourceAxis->AddAxisListener(this);
      }

      UpdateTargetAxisState();
   }


   /**
   * Returns the source axis.
   *
   * @return the current source axis
   */
   Axis *AxisToAxisTransformation::GetSourceAxis()
   {
      return mSourceAxis.get();
   }


   /**
   * Sets the target axis.
   *
   * @param targetAxis the new target axis
   */
   void AxisToAxisTransformation::SetTargetAxis(LogicalAxis *targetAxis)
   {
      mTargetAxis = targetAxis;

      if (mSourceAxis.valid() && mTargetAxis.valid())
      {
         mSourceAxis->AddAxisListener(this);
      }
      else
      {
         mSourceAxis->RemoveAxisListener(this);
      }

      UpdateTargetAxisState();
   }


   /**
   * Gets the target axis.
   *
   * @return the current target axis
   */
   LogicalAxis *AxisToAxisTransformation::GetTargetAxis()
   {
      return mTargetAxis.get();
   }


   /**
   * Sets this mapping's transformation.
   *
   * @param transformation the new arbitrary transformation
   */
   void AxisToAxisTransformation::SetTransformation(dtUtil::Transformation<double> *transformation)
   {
      mpTransformation = transformation;

      UpdateTargetAxisState();
   }


   /**
   * Retrieves this mapping's transformation.
   *
   * @return The transformation.
   */
   dtUtil::Transformation<double> *AxisToAxisTransformation::GetTransformation() const
   {
      return mpTransformation;
   }


   /**
   * Called when an axis' state has changed.
   *
   * @param axis the changed axis
   * @param oldState the old state of the axis
   * @param newState the new state of the axis
   * @param delta a delta value indicating stateless motion
   */
   bool AxisToAxisTransformation::AxisStateChanged(const Axis *axis,
      double oldState, 
      double newState, 
      double delta)
   {
      if (mTargetAxis.valid())
      {
         //return mTargetAxis->SetState(newState*mScale + mOffset, delta*mScale);
         // ??? how to tweak delta ???
         return mTargetAxis->SetState((*mpTransformation)(newState), 0.0f);
      }

      return false;
   }


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

} // namespace dtCore
