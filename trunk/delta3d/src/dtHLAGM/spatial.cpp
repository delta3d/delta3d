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
   bool Spatial::HasVelocity() const
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
   size_t Spatial::GetBaseSize()
   {
      return 8 + mWorldCoordinate.EncodedLength() + 4 + mOrientation.EncodedLength();
   }
   
   /////////////////////////////////////////////////////////////////////////////////
   size_t Spatial::Encode(char * buffer, size_t maxSize)
   {
      size_t result = 0;

      if (maxSize == 0)
      {
         return 0;
      }
      
      //this needs a way to prevent resizing.
      dtUtil::DataStream ds(buffer, maxSize, false);
      ds.SetForceLittleEndian(mLittleEndian);
      ds.ClearBuffer();

      if (maxSize >= GetBaseSize())
      {
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
               if (maxSize >= ds.GetBufferSize() + mVelocity.EncodedLength())
               {
                  WriteVec(mVelocity, ds);
                  result = ds.GetBufferSize();
               }
               else
               {
                  result = 0;
               }
               break;
   
            case 5:
            case 9:
               if (maxSize >= ds.GetBufferSize() + mVelocity.EncodedLength()
                        + mAcceleration.EncodedLength())
               {
                  WriteVec(mVelocity, ds);
                  WriteVec(mAcceleration, ds);
                  result = ds.GetBufferSize();
               }
               else
               {
                  result = 0;
               }
               break;
   
            case 3:
            case 7:
               if (maxSize >= ds.GetBufferSize() + mVelocity.EncodedLength()
                        + mAngularVelocity.EncodedLength())
               {
                  WriteVec(mVelocity, ds);
                  WriteVec(mAngularVelocity, ds);
                  result = ds.GetBufferSize();
               }
               else
               {
                  result = 0;
               }
               break;
               
            case 4:
            case 8:
               if (maxSize >= ds.GetBufferSize() + mVelocity.EncodedLength()
                        + mAcceleration.EncodedLength()
                        + mAngularVelocity.EncodedLength()
                        )
               {
                  WriteVec(mVelocity, ds);
                  WriteVec(mAcceleration, ds);
                  WriteVec(mAngularVelocity, ds);
                  result = ds.GetBufferSize();
               }
               else
               {
                  result = 0;
               }
               break;
               
            default:
               result = 0;
               break;
         }

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
      bool result = false;
      ///ewww, const cast, but the data stream won't use a non-const pointer.
      dtUtil::DataStream ds(const_cast<char *>(buffer), size, false);
      ds.SetForceLittleEndian(mLittleEndian);

      size_t baseSize = GetBaseSize();
      if (size >= baseSize)
      {
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
               result = true;
               break;
            case 2:
            case 6:
               if (size >= baseSize + mVelocity.EncodedLength())
               {
                  ReadVec(mVelocity, ds);
                  result = true;
               }
               break;
               
            case 5:
            case 9:
               if (size >= baseSize + mVelocity.EncodedLength()
                        + mAcceleration.EncodedLength()
                        )
               {
                  ReadVec(mVelocity, ds);
                  ReadVec(mAcceleration, ds);
                  result = true;
               }
               break;
   
            case 3:
            case 7:
               if (size >= baseSize + mVelocity.EncodedLength()
                        + mAngularVelocity.EncodedLength()
                        )
               {
                  ReadVec(mVelocity, ds);
                  ReadVec(mAngularVelocity, ds);
               }
               result = true;
               break;
               
            case 4:
            case 8:
               if (size >= baseSize + mVelocity.EncodedLength()
                        + mAcceleration.EncodedLength()
                        + mAngularVelocity.EncodedLength()
                        )
               {
                  ReadVec(mVelocity, ds);
                  ReadVec(mAcceleration, ds);
                  ReadVec(mAngularVelocity, ds);
                  result = true;
               }
               break;
               
            default:
               result = false;
               break;
         }
      }
      return result;
   }
}
