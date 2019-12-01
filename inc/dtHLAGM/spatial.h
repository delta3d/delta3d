#ifndef DELTA_SPATIAL
#define DELTA_SPATIAL

#include <dtHLAGM/distypes.h>
#include <dtHLAGM/export.h>

namespace dtHLAGM
{

   /**
    * Class for encoding and decoding the RPR 2 spatial structure.  This structure holds
    * position, orientation, and motion data and the suggested dead-reckoning algorithm.
    * The data is the encoded structure changes based on the dead-reckoning algorithm chosen.
    */
   class DT_HLAGM_EXPORT Spatial
   {
      public:

         /**
          * creates a spatial.
          * @param littleEndian true to encode/decode in little endian.  False for big endian.
          */
         Spatial(bool littleEndian = false);

         ~Spatial();

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

         /// @return true if the dead-reckoning algorithm suggests this should have a valid velocity.
         bool HasVelocity() const;
         /// @return true if the dead-reckoning algorithm suggests this should have a valid acceleration.
         bool HasAcceleration() const;
         /// @return true if the dead-reckoning algorithm suggests this should have a valid angular velocity.
         bool HasAngularVelocity() const;

         /**
          * Fills the given buffer with the encoded version of this struct.
          * @param buffer the buffer to fill.
          * @param maxSize the maximum size allocated in this buffer.
          * @return the used buffer size or 0 is encoding failed, probably due to the maxSize being 
          *         too small.
          */
         size_t Encode(char * buffer, size_t maxSize) const;
         
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

         //Helper method used to get the size of the base structure.
         size_t GetBaseSize() const;
 };

}

#endif /*DELTA_SPATIAL*/
