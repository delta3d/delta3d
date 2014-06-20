#ifndef axistoaxistransformation_h__
#define axistoaxistransformation_h__

#include <dtCore/logicalinputdevice.h>
#include <dtUtil/transformation.h>

namespace dtCore {

   /**
   * Maps an axis to a logical axis with an arbitrary transformation.
   */
   class DT_CORE_EXPORT AxisToAxisTransformation : public AxisMapping,
                                                   public AxisHandler
   {
   public:

      /**
      * Constructor.
      *
      * @param sourceAxis the source axis
      * @param transformation the arbitrary transformation
      */
      AxisToAxisTransformation(Axis* sourceAxis, dtUtil::Transformation<double>* transformation);

   protected:

      virtual ~AxisToAxisTransformation();

   public:

      /**
      * Sets the source axis.
      *
      * @param sourceAxis the new source axis
      */
      void SetSourceAxis(Axis* sourceAxis);

      /**
      * Returns the source axis.
      *
      * @return the current source axis
      */
      Axis* GetSourceAxis();

      /**
      * Sets the target axis.
      *
      * @param targetAxis the new target axis
      */
      virtual void SetTargetAxis(LogicalAxis* targetAxis);

      /**
      * Gets the target axis.
      *
      * @return the current target axis
      */
      virtual LogicalAxis* GetTargetAxis();

      /**
      * Sets this mapping's transformation.
      *
      * @param transformation the new arbitrary transformation
      */
      void SetTransformation(dtUtil::Transformation<double>* transformation);

      /**
      * Retrieves this mapping's transformation.
      *
      * @return The transformation.
      */
      dtUtil::Transformation<double>* GetTransformation() const;

      /**
      * Called when an axis' state has changed.
      *
      * @param axis the changed axis
      * @param oldState the old state of the axis
      * @param newState the new state of the axis
      * @param delta a delta value indicating stateless motion
      * @return Whether the event was handled.
      */
      virtual bool HandleAxisStateChanged(const Axis *axis,
                                    double oldState,
                                    double newState,
                                    double delta);

   private:
      /// The source axis.
      ObserverPtr<Axis> mSourceAxis;

      /// The target axis.
      ObserverPtr<LogicalAxis> mTargetAxis;

      /// The arbitrary transformation.
      dtUtil::Transformation<double>* mpTransformation;

      /// Updates the state of the target axis.
      bool UpdateTargetAxisState();
   };

} // namespace dtCore

#endif // axistoaxistransformation_h__
