#include <dtHLAGM/spatial.h>

#include <dtUtil/datastream.h>

namespace dtHLAGM
{
   /////////////////////////////////////////////////////////////////////////////////
   Spatial::Spatial(bool littleEndian):
      mLittleEndian(littleEndian)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   Spatial::~Spatial()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   WorldCoordinate& Spatial::GetWorldCoordinate()
   {
      return mWorldCoordinate;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const WorldCoordinate& Spatial::GetWorldCoordinate() const
   {
      return mWorldCoordinate;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   EulerAngles& Spatial::GetOrientation()
   {
      return mOrientation;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const EulerAngles& Spatial::GetOrientation() const
   {
      return mOrientation;
   }

   /////////////////////////////////////////////////////////////////////////////////
   VelocityVector& Spatial::GetVelocity()
   {
      return mVelocity;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const VelocityVector& Spatial::GetVelocity() const
   {
      return mVelocity;
   }

   /////////////////////////////////////////////////////////////////////////////////
   VelocityVector& Spatial::GetAcceleration()
   {
      return mAcceleration;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const VelocityVector& Spatial::GetAcceleration() const
   {
      return mAcceleration;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   VelocityVector& Spatial::GetAngularVelocity()
   {
      return mAngularVelocity;
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   const VelocityVector& Spatial::GetAngularVelocity() const
   {
      return mAngularVelocity;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Spatial::HasVelocityVector() const
   {
      return mDeadReckoningAlgorithm > 1;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Spatial::HasAcceleration() const
   {
      bool result = false;
      
      switch (mDeadReckoningAlgorithm)
      {
         case 5:
         case 9:
         case 4:
         case 8:
            result = true;
            break;
            
         default:
            result = false;
            break;
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Spatial::HasAngularVelocity() const
   {
      bool result = false;
      
      switch (mDeadReckoningAlgorithm)
      {
         case 3:
         case 7:
         case 4:
         case 8:
            result = true;
            break;
            
         default:
            result = false;
            break;
      }
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   static void WriteVec(const osg::Vec3f& vec, dtUtil::DataStream& writeStream)
   {
      for (size_t i = 0; i < 3; ++i)
      {
         writeStream << vec[i];
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   size_t Spatial::Encode(char * buffer, size_t maxSize)
   {
      size_t result = 0;

      //this needs a way to prevent resizing.
      dtUtil::DataStream ds(buffer, maxSize, false);
      ds.SetForceLittleEndian(mLittleEndian);
      ds.ClearBuffer();

      ds << mDeadReckoningAlgorithm;
      ds.WriteBytes(0, 7U);

      ds << mWorldCoordinate.x();
      ds << mWorldCoordinate.y();
      ds << mWorldCoordinate.z();

      ds << mIsFrozen;
      ds.WriteBytes(0, 3U);

      WriteVec(mOrientation, ds);

      switch (mDeadReckoningAlgorithm)
      {
         case 0:
         case 1:
            result = ds.GetBufferSize();
            break;

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
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   static void ReadVec(osg::Vec3f& vec, dtUtil::DataStream& readStream)
   {
      for (size_t i = 0; i < 3; ++i)
      {
         readStream >> vec[i];
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool Spatial::Decode(const char * buffer, size_t size)
   {
      bool result = true;
      ///ewww, const cast, but the data stream won't use a non-const pointer.
      dtUtil::DataStream ds(const_cast<char *>(buffer), size, false);
      ds.SetForceLittleEndian(mLittleEndian);

      ds >> mDeadReckoningAlgorithm;
      ds.Seekg(7U, dtUtil::DataStream::SeekTypeEnum::CURRENT);

      ds >> mWorldCoordinate.x();
      ds >> mWorldCoordinate.y();
      ds >> mWorldCoordinate.z();

      ds >> mIsFrozen;
      ds.Seekg(3U, dtUtil::DataStream::SeekTypeEnum::CURRENT);


      ReadVec(mOrientation, ds);

      switch (mDeadReckoningAlgorithm)
      {
         case 0:
         case 1:
            break;
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
