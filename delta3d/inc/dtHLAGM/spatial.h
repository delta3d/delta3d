#ifndef DELTA_SPATIAL
#define DELTA_SPATIAL

#include <dtHLAGM/distypes.h>
#include <dtHLAGM/export.h>

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT Spatial
   {
      public:
         
         /**
          * creates a spatial.
          * @param littleEndian true to encode/decode in little endian.  False for big endian.
          */
         Spatial(bool littleEndian = false);

         virtual ~Spatial();

         char GetDeadReckoningAlgorithm() const { return mDeadReckoningAlgorithm; }
         void SetDeadReckoningAlgorithm(char newAlgorithm) 
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

         bool HasVelocityVector() const;
         bool HasAcceleration() const;
         bool HasAngularVelocity() const;

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
         
         char            mDeadReckoningAlgorithm;
         bool            mIsFrozen;
         bool            mLittleEndian;
   };

}

#endif /*DELTA_SPATIAL*/
