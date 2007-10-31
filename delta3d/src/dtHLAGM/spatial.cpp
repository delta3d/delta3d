#include <dtHLAGM/spatial.h>

#include <dtUtil/datastream.h>

namespace dtHLAGM
{

   Spatial::Spatial()
   {
   }

   Spatial::~Spatial()
   {
   }

   WorldCoordinate& Spatial::GetWorldCoordinate()
   {
      return mWorldCoordinate;
   }
   
   const WorldCoordinate& Spatial::GetWorldCoordinate() const
   {
      return mWorldCoordinate;
   }
   
   EulerAngles& Spatial::GetOrientation()
   {
      return mOrientation;
   }
   
   const EulerAngles& Spatial::GetOrientation() const
   {
      return mOrientation;
   }

   VelocityVector& Spatial::GetVelocity()
   {
      return mVelocity;
   }
   
   const VelocityVector& Spatial::GetVelocity() const
   {
      return mVelocity;
   }

   VelocityVector& Spatial::GetAcceleration()
   {
      return mAcceleration;
   }
   
   const VelocityVector& Spatial::GetAcceleration() const
   {
      return mAcceleration;
   }
   
   VelocityVector& Spatial::GetAngularVelocity()
   {
      return mAngularVelocity;
   }
   
   const VelocityVector& Spatial::GetAngularVelocity() const
   {
      return mAngularVelocity;
   }

   size_t Spatial::Encode(char * buffer, size_t maxSize)
   {
      return 0;
   }

   static void ReadVec(VelocityVector& vec, dtUtil::DataStream& readStream)
   {
      float floatTemp;
      readStream >> floatTemp;
      vec.SetX(floatTemp);
      readStream >> floatTemp;
      vec.SetY(floatTemp);
      readStream >> floatTemp;
      vec.SetZ(floatTemp);
   }

   bool Spatial::Decode(const char * buffer, size_t size)
   {
      bool result = true;
      ///ewww, const cast, but the data stream won't use a non-const pointer.
      dtUtil::DataStream ds(const_cast<char *>(buffer), size, false);
      char drAlgoTemp;
      ds << drAlgoTemp;
      mDeadReckoningAlgorithm = int(drAlgoTemp);
      ds.Seekg(7U, dtUtil::DataStream::SeekTypeEnum::CURRENT);

      double doubleTemp;
      ds >> doubleTemp;
      mWorldCoordinate.SetX(doubleTemp);
      ds >> doubleTemp;
      mWorldCoordinate.SetY(doubleTemp);
      ds >> doubleTemp;
      mWorldCoordinate.SetZ(doubleTemp);

      ds >> mIsFrozen;
      ds.Seekg(3U, dtUtil::DataStream::SeekTypeEnum::CURRENT);

      float floatTemp;
      ds >> floatTemp;
      mOrientation.SetPsi(floatTemp);
      ds >> floatTemp;
      mOrientation.SetTheta(floatTemp);
      ds >> floatTemp;
      mOrientation.SetPhi(floatTemp);
      
      switch (mDeadReckoningAlgorithm)
      {
         case 2:
         case 6:
            ReadVec(mVelocity, ds);
            break;
            
         case 5:
         case 9:
            ReadVec(mVelocity, ds);
            ReadVec(mAcceleration, ds);
            break;

         case 3:
         case 7:
            ReadVec(mVelocity, ds);
            ReadVec(mAngularVelocity, ds);
            break;
            
         case 4:
         case 8:
            ReadVec(mVelocity, ds);
            ReadVec(mAcceleration, ds);
            ReadVec(mAngularVelocity, ds);
            break;
            
         default:
            result = false;
            break;
      }
      return result;
   }
}
