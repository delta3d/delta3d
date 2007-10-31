#ifndef DELTA_SPATIAL
#define DELTA_SPATIAL

#include <dtHLAGM/distypes.h>

namespace dtHLAGM
{

   class Spatial
   {
      public:
         Spatial();
         virtual ~Spatial();

         int GetDeadReckoningAlgorithm() const { return mDeadReckoningAlgorithm; }
         void SetDeadReckoningAlgorithm(int newAlgorithm) 
            { mDeadReckoningAlgorithm = newAlgorithm; }
         
         bool IsFrozen() const { return mIsFrozen; }
         void SetFrozen(bool newFrozen) { mIsFrozen = newFrozen; }
         
         WorldCoordinate& GetWorldCoordinate();
         const WorldCoordinate& GetWorldCoordinate() const;
         
         EulerAngles& GetOrientation();
         const EulerAngles& GetOrientation() const;
      
         VelocityVector& GetVelocity();
         const VelocityVector& GetVelocity() const;

         VelocityVector& GetAcceleration();
         const VelocityVector& GetAcceleration() const;
         
         VelocityVector& GetAngularVelocity();
         const VelocityVector& GetAngularVelocity() const;

         /**
          * Fills the given buffer with the encoded version of this struct.
          * @param buffer the buffer to fill.
          * @param maxSize the maximum size allocated in this buffer.
          * @return the used buffer size or 0 is encoding failed, probably due to the maxSize being 
          *         too small.
          */
         size_t Encode(char * buffer, size_t maxSize);
         
         /**
          * Decodes the data in the buffer, filling this structure.
          * @param buffer the buffer read data from.
          * @param size the size of the buffer.
          * @return true if the data was possible to decode.
          */
         bool Decode(const char * buffer, size_t size);
      private:
         WorldCoordinate mWorldCoordinate;
         EulerAngles     mOrientation;
         VelocityVector  mVelocity;
         VelocityVector  mAcceleration;
         VelocityVector  mAngularVelocity;
         
         int             mDeadReckoningAlgorithm;
         bool            mIsFrozen;
   };

}

#endif /*DELTA_SPATIAL*/
