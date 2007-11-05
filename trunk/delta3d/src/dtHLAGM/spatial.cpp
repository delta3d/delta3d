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

   static void WriteVec(const VelocityVector& vec, dtUtil::DataStream& writeStream)
   {
      writeStream << vec.GetX();
      writeStream << vec.GetY();
      writeStream << vec.GetZ();
   }

   size_t Spatial::Encode(char * buffer, size_t maxSize)
   {
      size_t result = 0;
      
      //this needs a way to prevent resizing.
      dtUtil::DataStream ds(buffer, maxSize, false);
      ds << mDeadReckoningAlgorithm;
      ds.Seekp(7U, dtUtil::DataStream::SeekTypeEnum::CURRENT);

      ds << mWorldCoordinate.GetX();
      ds << mWorldCoordinate.GetY();
      ds << mWorldCoordinate.GetZ();

      ds << mIsFrozen;
      ds.Seekp(3U, dtUtil::DataStream::SeekTypeEnum::CURRENT);

      ds << mOrientation.GetPsi();
      ds << mOrientation.GetTheta();
      ds << mOrientation.GetPhi();

      switch (mDeadReckoningAlgorithm)
      {
         case 2:
         case 6:
            WriteVec(mVelocity, ds);
            result = ds.GetBufferSize();
            break;
            
         case 5:
         case 9:
            WriteVec(mVelocity, ds);
            WriteVec(mAcceleration, ds);
            result = ds.GetBufferSize();
            break;

         case 3:
         case 7:
            WriteVec(mVelocity, ds);
            WriteVec(mAngularVelocity, ds);
            result = ds.GetBufferSize();
            break;
            
         case 4:
         case 8:
            WriteVec(mVelocity, ds);
            WriteVec(mAcceleration, ds);
            WriteVec(mAngularVelocity, ds);
            result = ds.GetBufferSize();
            break;
            
         default:
            result = 0;
            break;
      }
      if (result > maxSize)
      {
         result = 0;
         delete[] ds.GetBuffer();
      }
         
      return result;
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
      ds >> mDeadReckoningAlgorithm;
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
