#ifndef transformation_h__
#define transformation_h__


////////////////////////////////////////////////////////////////////////////////
namespace dtUtil
{

   /** An interface class to use for "things that transform" i.e., things
     * that return a value given an input value.
     */
   template <typename T> class Transformation
   {
   public:
      virtual T operator()(T) const = 0;
      virtual ~Transformation() {};
   };

   ////////////////////////////////////////////////////////////////////////////////

   /** The EdgeStepFilter will return back 1.0, 0.0, or -1.0 depending if the 
     * supplied input value is greater than the high, in between high and low,
     * or less than the low.
     */
   class EdgeStepFilter : public Transformation<double>
   {
   public:
      EdgeStepFilter(double low, double high)
         : mLow(low)
         , mHigh(high)
      { }

      double operator()(double input) const
      {
         return ((input > mHigh) ? 1.0 : 0.0) - ((input < mLow) ? 1.0 : 0.0);
      }

   private:
      double mLow, mHigh;
   };
} //namespace dtUtil

#endif // transformation_h__
