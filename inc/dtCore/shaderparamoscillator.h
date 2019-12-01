/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Curtiss Murphy
 * Erik Johnson
 */

#ifndef DELTA_ShaderParameterFloatTimer
#define DELTA_ShaderParameterFloatTimer

#include <dtCore/export.h>
#include <dtCore/shaderparameter.h>

////////////////////////////////////////////////////////////////////////////////

namespace dtCore
{
   /**
    * This class is a shader parameter that has an oscillating parameter.  If you assign
    * this to a node, it will oscillate the value as if it were a time constraint.
    */
   class DT_CORE_EXPORT ShaderParamOscillator : public ShaderParameter
   {

   public:
      /**
       * The type of oscillation for this parameter.
       */
      class DT_CORE_EXPORT OscillationType : public dtUtil::Enumeration
      {
         DECLARE_ENUM(OscillationType);
      public:
         static const OscillationType UP; // from offset to offset+range, then jumps back to offset.
         static const OscillationType DOWN; // from offset+range down to offset, then jumps back.
         static const OscillationType UPANDDOWN; // does Up and then Down (ie. oscillates)
         static const OscillationType DOWNANDUP; // does Down and then Up (ie. oscillates)
      private:
         OscillationType(const std::string& name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };

      /**
       * The type of trigger for this parameter.
       * Determines when the oscillation begins.
       */
      class DT_CORE_EXPORT OscillationTrigger : public dtUtil::Enumeration
      {
         DECLARE_ENUM(OscillationTrigger);
      public:
         static const OscillationTrigger AUTO;   // Starts immediately by itself
         static const OscillationTrigger MANUAL; // Waits for the user to trigger it
      private:
         OscillationTrigger(const std::string& name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
      };


      public:

         static const int INFINITE_CYCLE = -1;

         /**
          * Constructs the float parameter.
          * @param name Name given to this parameter.  If it is to be used in a hardware shader,
          *   this name must match the corresponding uniform variable in the shader.
          */
         ShaderParamOscillator(const std::string& name);

         /**
          * Gets the type of this parameter.
          */
         virtual const ShaderParameter::ParamType& GetType() const { return ShaderParameter::ParamType::TIMER_FLOAT; }

         /**
          * Assigns the necessary uniform variables for this parameter to the
          * render state.  This allows the parameter to communicate to the underlying
          * shader.
          */
         virtual void AttachToRenderState(osg::StateSet& stateSet);

         /**
          * Method called by the shader owning this parameter if it detects the parameter has changed state.
          * This method when called, sends the current float value in this parameter to the hardware shader.
          * Since this value oscillates continuously, it will push the value every tick.
          */
         virtual void Update();

         /**
          * Sets the value - temporarily. This method is only provided for consistency.
          * Because this parameter oscillates, the value you set will be overridden.  Typically,
          * you should never call this method.
          * @param newValue The new value to set - only changed temporarily until next tick.
          * @note This does NOT change the param on the actual uniform, just internally until next tick.
          */
         void SetValue(float newValue) { mValue = newValue; }

         /**
          * Gets the current value in this parameter.
          * @return The current floating point value.
          */
         float GetValue() const { return mValue; }

         /**
          * Makes a deep copy of the Shader Parameter. Used when a user assigns
          * a shader to a node because we clone the template shader and its parameters.
          * Note - Like Update(), this is a pure virtual method that must be implemented on each param.
          */
         virtual ShaderParameter* Clone();

         /**
          * The starting point for the timer oscillation. In [1.0, 5.0], this would be 1.0.
          * For [-8.0, 12.0], this would be -8.0. Default is 0.0.
          * @param offset The offset from zero for the bottom value of the timer.
          * @note Marks as dirty so it will cause a recalculation.
          */
         void SetOffset(float offset) { mOffset = offset; SetDirty(true); }

         /**
          * Gets the starting point for the timer oscillation. In [1.0, 5.0], this would be 1.0.
          * For [-8.0, 12.0], this would be -8.0.
          * @return The offset from zero for the bottom value of the timer.
          */
         float GetOffset() const { return mOffset; }

         /**
          * Gets the current range. Remember offset is the 'bottom', and the 'top' is offset + range.
          * Whenever a tick happens where IsDirty() is true, this value is recomputed.
          * @return The texture unit.
          * @note This is not settable. Set the RangeMin and/or RangeMax to cause a change in this.
          */
         float GetCurrentRange() const { return mCurrentRange; }

         /**
          * The minimum range value for the timer. On first update (or first update after a dirty),
          * the min and max are used to compute an actual range.  Then, the timer will oscillate
          * between the 'bottom' (offset) and the 'top' (offset plus range). Default is 1.0.
          * @param minimum The minimum value for the range. Is reset to <= max on next dirty Update.
          * @note Marks as dirty so it will cause a recalculation.  Negative ranges are
          * not allowed (will reset to 1 on Update).
          */
         void SetRangeMin(float minimum) { mRangeMin = minimum; SetDirty(true); }

         /**
          * The minimum range value for the timer. On first update (or first update after a dirty),
          * the min and max are used to compute an actual range.  Then, the timer will oscillate
          * between the 'bottom' (offset) and the 'top' (offset plus range).
          * @return The minimum range
          */
         float GetRangeMin() const { return mRangeMin; }

         /**
          * The max range value for the timer. On first update (or first update after a dirty),
          * the min and max are used to compute an actual range.  Then, the timer will oscillate
          * between the 'bottom' (offset) and the 'top' (offset plus range). Default is 1.0.
          * @param maximum The max value for the range. Is reset to >= min on next dirty Update.
          * @note Marks as dirty so it will cause a recalculation.  Negative ranges are
          * not allowed (will reset to 1 on Update).
          */
         void SetRangeMax(float maximum) { mRangeMax = maximum; SetDirty(true); }

         /**
          * The max range value for the timer. On first update (or first update after a dirty),
          * the min and max are used to compute an actual range.  Then, the timer will oscillate
          * between the 'bottom' (offset) and the 'top' (offset plus range).
          * @return The maximum range
          */
         float GetRangeMax() const { return mRangeMax; }

         /**
          * Gets the current cycle time. This is the actual amount of time (in seconds) that
          * it takes to go from bottom to top (or reverse).  2X this for a full oscillation.
          * This value is NOT settable. Whenever a tick happens where IsDirty() is true, this value is recomputed.
          * @return The texture unit.
          * @note This is not settable. Set the CycleTimeMin and/or CycleTimeMax to cause a change in this.
          */
         float GetCurrentCycleTime() const { return mCurrentCycleTime; }

         /**
          * The minimum amount for Cycle time. Cycle Time is defined as the
          * amount of time (in seconds) it takes to go from bottom to top, or reverse.
          * Note, if oscillating up and down or down and up, the complete cycle will take
          * 2X cycle time. Default is 1.0.
          * @param cycleTime The time in secs to go from bottom to top or top to bottom.
          * @note Marks as dirty so it will cause a recalculation.  Negative ranges are
          * not allowed (will reset to 1 on Update).
          */
         void SetCycleTimeMin(float cycleTimeMin) { mCycleTimeMin = cycleTimeMin; }

         /**
          * The min amount of cycle time (see SetCycleTimeMin().
          * @return The min cycle time.
          */
         float GetCycleTimeMin() const { return mCycleTimeMin; }

         /**
          * The maximum amount for Cycle time. Cycle Time is defined as the
          * amount of time (in seconds) it takes to go from bottom to top, or reverse.
          * Note, if oscillating up and down or down and up, the complete cycle will take
          * 2X cycle time. Default is 1.0.
          * @param cycleTimeMax The time in secs to go from bottom to top or top to bottom.
          * @note Marks as dirty so it will cause a recalculation.  Negative ranges are
          * not allowed (will reset to 1 on Update).
          */
         void SetCycleTimeMax(float cycleTimeMax) { mCycleTimeMax = cycleTimeMax; }

         /**
          * The max amount of cycle time (see SetCycleTimeMax().
          * @return The max cycle time.
          */
         float GetCycleTimeMax() const { return mCycleTimeMax; }

         /**
         * Set min and max amount of cycle time (see SetCycleTimeMax().
         */
         void SetCycleTimeMinMax(float cycleTimeMin, float cycleTimeMax);

         /**
          * @param count The number of cycles that will happen before oscillation stops.
          */
         void SetCycleCountTotal(int newCount) { mCycleCountTotal = newCount; }

         /**
          * @return The number of cycles that will happen before oscillation stops.
          */
         float GetCycleCountTotal() const { return mCycleCountTotal; }

         /**
          * Indicates whether we use real time or simulation time to do our cycle.  Default is true (real time).
          * @param useRealTime True means use real time, false uses simulation time.
          * @note Does not cause Dirty to be true, so no recalculation.
          */
         void SetUseRealTime(bool useRealTime) { mUseRealTime = useRealTime; }

         /**
          * Gets the texture unit currently assigned to this shader parameter.
          * @return The texture unit.
          */
         bool GetUseRealTime() const { return mUseRealTime; }

         /**
          * Sets the way that the timer oscillates values. Basically, the offset is considered the
          * 'bottom' and the offset+range is the 'top'.  So, up goes from bottom to top, down does
          * the reverse, and the other two go back and forth as you'd expect.  Default is UP.
          * @param type The source type.
          * @see OscillationType
          * @note Changing this param at any time causes an instant change in direction (no recalculation)
          */
         void SetOscillationType(const OscillationType& type) { mOscillationType = &type; }

         /**
          * Gets the source texture type of this parameter.
          * @return The source type currently in use by this parameter.
          */
         const OscillationType& GetOscillationType() const { return *mOscillationType; }

         /**
          * Sets when the timer will begin to oscillate its value. If set to AUTO, oscillation will
          * begin immediately by itself.  If set to MANUAL, oscillation will not occur until the
          * decides to call Start.
          * @see OscillationTrigger
          */
         void SetOscillationTrigger(const OscillationTrigger& type) { mOscillationTrigger = &type; }

         /**
          * Returns the way that the oscillator will begin.
          * @return The source trigger currently in use by this parameter.
          */
         const OscillationTrigger& GetOscillationTrigger() const { return *mOscillationTrigger; }

         /**
          * Returns the current cycle direction. Should be either 1.0 or -1.0.  You cannot
          * set this but you can find out which direction it is currently going.
          * @return 1.0 if going up, -1.0 if going down.
          */
         float GetCycleDirection() const{ return mCycleDirection; }

         /**
          * Called by the dtCore::Base class - used to get timer updates
          * @param The data from the message
          * @see dtCore::Base
          */
         virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

         /**
          * Triggers the oscillation to begin if the trigger type is 'manual'
          * Otherwise, it resets the current oscillation to their starting defaults.
          */
         void TriggerOscillationStart();

      protected:
         virtual ~ShaderParamOscillator();

         /**
          * Does the actual work for the shader.  When called, make sure you pass in the
          * real or simulated as appropriate, based on mUseRealTime.
          * @param timeDelta The real or simulated time to use to modify the shader
          */
         void DoShaderUpdate(float timeDelta);


      private:
         float mValue;

         float mOffset;            // Start point.  In [1.0, 5.0], this would be 1.
         float mRangeMin;          // Min size of distance between start and end.
         float mRangeMax;          // Max size of distance between start and end.
         float mCurrentRange;      // the current range (somewhere between min and max) - this is not settable
         float mCycleTimeMin;      // Min time for it to cycle (see mCurrentCycleTime)
         float mCycleTimeMax;      // Max time for it to cycle (see mCurrentCycleTime)
         float mCurrentCycleTime;  // The time in secs it takes to go from bottom to top or the reverse
         int   mCycleCountTotal;   // The number of cycles that will happen before oscillation stops.
         int   mCurrentCycleCount; // The number of cycles that we've already completed
         bool  mUseRealTime;       // True means real time, false means simulation time.
         bool  mWasTriggered;      // True means that oscillation was manually started

         const OscillationType*    mOscillationType;
         const OscillationTrigger* mOscillationTrigger;

         float mCycleDirection; // internal, is 1.0 or -1.0, shows direction of cycle

         /**
          * Increments the current cycle count if there are more cycles left
          * @returns true if there are more cycles left, OW returns false
          */
         bool AdvanceCycle();
   };
} // namespace dtCore

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA_ShaderParameterFloatTimer
