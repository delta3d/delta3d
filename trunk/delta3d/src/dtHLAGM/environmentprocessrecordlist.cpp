/* -*-c++-*-
 * Delta3D
 * Copyright 2007-2010, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include <dtHLAGM/environmentprocessrecordlist.h>

#include <dtUtil/datastream.h>
#include <dtUtil/coordinates.h>
#include <dtHLAGM/distypes.h>

#include <dtDAL/namedvectorparameters.h>
#include <dtDAL/namedfloatparameter.h>
#include <dtDAL/namedgroupparameter.inl>

namespace dtHLAGM
{
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_LOCATION("Location");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_ORIGINATION_LOCATION("OriginationLocation");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_ORIENTATION("Orientation");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_VELOCITY("Velocity");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY("AngularVelocity");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_DIMENSION("Dimension");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_DIMENSION_RATE("DimensionRate");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_CENTROID_HEIGHT("CentroidHeight");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_RADIUS("Radius");
   const dtUtil::RefString EnvironmentProcessRecord::PARAM_RADIUS_RATE("RadiusRate");

   /////////////////////////////////////////////////////////
   EnvironmentProcessRecord::EnvironmentProcessRecord(const std::string& recName)
   : mParameterData(new dtDAL::NamedGroupParameter(recName))
   {
   }

   /////////////////////////////////////////////////////////
   EnvironmentProcessRecord::~EnvironmentProcessRecord()
   {
   }

   /////////////////////////////////////////////////////////
   unsigned int EnvironmentProcessRecord::GetIndex() const
   {
      return mParameterData->GetValue("Index", (unsigned int)(0U));
   }

   /////////////////////////////////////////////////////////
   void EnvironmentProcessRecord::SetIndex(unsigned int value)
   {
      mParameterData->SetValue("Index", value);
   }

   /////////////////////////////////////////////////////////
   unsigned int EnvironmentProcessRecord::GetTypeCode() const
   {
      return mParameterData->GetValue("TypeCode", (unsigned int)(0U));
   }

   /////////////////////////////////////////////////////////
   void EnvironmentProcessRecord::SetTypeCode(unsigned int value)
   {
      mParameterData->SetValue("TypeCode", value);
   }

   /////////////////////////////////////////////////////////
   dtDAL::NamedGroupParameter& EnvironmentProcessRecord::GetRecordData()
   {
      return *mParameterData;
   }

   /////////////////////////////////////////////////////////
   size_t EnvironmentProcessRecord::Encode(dtUtil::DataStream& ds, dtUtil::Coordinates& coord) const
   {
      size_t result = GetEncodedSize();
      if (ds.GetBufferCapacity() - ds.GetWritePosition() > GetEncodedSize())
      {
         unsigned int headerStartWritePos = ds.GetWritePosition();
         ds << GetIndex();
         ds.WriteBytes(0, 4U);

         // Need to save the read pos so I can skip to a 64 bit boundary after reading the variant data.
         //unsigned int savedReadPos = ds.GetReadPosition();

         ds << GetTypeCode();
         // Should the length include 4 bytes of the length variable?
         unsigned int savedWritePos = ds.GetWritePosition();
         // This is the size in bytes. write zeros now, then well, rewind and write the correct value.
         ds.WriteBytes(0, 4U);

         // TODO figure out if we have enough length left.
         EncodeVariant(ds, coord);
         // pad to 64 bit.
         ds.WriteBytes(0, (8U - (ds.GetWritePosition() % 8U)) % 8U);

         unsigned int endWritePos = ds.GetWritePosition();
         unsigned int length = endWritePos - savedWritePos;
         // move back to the length position that was saved.
         ds.Seekp(savedWritePos, dtUtil::DataStream::SeekTypeEnum::SET);
         // write the length that was actually written
         ds << length;
         ds.Seekp(endWritePos, dtUtil::DataStream::SeekTypeEnum::SET);
         result = endWritePos - headerStartWritePos;
      }
      return result;
   }

   /////////////////////////////////////////////////////////
   void EnvironmentProcessRecord::EncodeVariant(dtUtil::DataStream& ds, dtUtil::Coordinates& coord) const
   {
      const osg::Vec3d zeroVecd(0.0, 0.0, 0.0);
      const osg::Vec3f zeroVecf(0.0f, 0.0f, 0.0f);
      bool handled = false;
      unsigned typeCode = GetTypeCode();
      switch (EnvironmentRecordTypeCode(typeCode))
      {
         case BoundingSphereRecordType:
         case SphereRecord1Type:
         case SphereRecord2Type:
         {

            osg::Vec3d loc = coord.ConvertToRemoteTranslation(mParameterData->GetValue(PARAM_LOCATION, zeroVecd));
            ds << loc;

            ds << mParameterData->GetValue(PARAM_RADIUS, float(0.0f));

            if (typeCode == SphereRecord2Type)
            {
               ds << mParameterData->GetValue(PARAM_RADIUS_RATE, float(0.0f));
               ds << coord.GetOriginRotationMatrixInverse().preMult(mParameterData->GetValue(PARAM_VELOCITY, zeroVecf));
               ds << mParameterData->GetValue(PARAM_ANGULAR_VELOCITY, zeroVecf);
            }
            else
            {
               ds.Seekp(4U, dtUtil::DataStream::SeekTypeEnum::CURRENT);
            }
            handled = true;
            break;
         }
         case RectangularVolRecord1Type:
         case RectangularVolRecord2Type:
         case EllipsoidRecord1Type:
         case EllipsoidRecord2Type:
         case GaussianPuffRecordType:
         {
            osg::Vec3d loc = coord.ConvertToRemoteTranslation(mParameterData->GetValue(PARAM_LOCATION, zeroVecd));
            ds << loc;

            if (typeCode == GaussianPuffRecordType)
            {
               loc = coord.ConvertToRemoteTranslation(mParameterData->GetValue(PARAM_ORIGINATION_LOCATION, zeroVecd));
               ds << loc;
            }

            ds << mParameterData->GetValue(PARAM_DIMENSION, osg::Vec3f(1.0f, 1.0f, 1.0f));

            if (typeCode != EllipsoidRecord1Type && typeCode != RectangularVolRecord1Type)
            {
               ds << mParameterData->GetValue(PARAM_DIMENSION_RATE, zeroVecf);
            }

            osg::Vec3f orient = coord.ConvertToRemoteRotation(mParameterData->GetValue(PARAM_ORIENTATION, zeroVecf));
            ds << orient;

            if (typeCode != EllipsoidRecord1Type && typeCode != RectangularVolRecord1Type)
            {
               ds << coord.GetOriginRotationMatrix().preMult(mParameterData->GetValue(PARAM_VELOCITY, zeroVecf));
               ds << mParameterData->GetValue(PARAM_ANGULAR_VELOCITY, zeroVecf);

               if (typeCode == GaussianPuffRecordType)
               {
                  ds << mParameterData->GetValue(PARAM_CENTROID_HEIGHT, float(1.0f));
               }
               else
               {
                  ds.Seekp(4U, dtUtil::DataStream::SeekTypeEnum::CURRENT);
               }
            }
            handled = true;
            break;
         }
         case COMBICStateRecordType:
         case FlareStateRecordType:
         case UniformGeometryRecordType:
         case PointRecord1Type:
         case LineRecord1Type:
         case ConeRecord1Type:
         case PointRecord2Type:
         case LineRecord2Type:
         case ConeRecord2Type:
         case RectangularVolRecord3Type:
         case GaussianPlumeRecordType:
         {
            break;
         }
      }
      if (!handled)
      {
         //TODO fix this somehow.  We aren't writing anything, so seeking ahead is not good..
         //ds.Seekp(lengthInBytes, dtUtil::DataStream::SeekTypeEnum::CURRENT);
      }
   }

   /////////////////////////////////////////////////////////
   bool EnvironmentProcessRecord::Decode(dtUtil::DataStream& ds, dtUtil::Coordinates& coord)
   {
      bool result = false;
      if (ds.GetRemainingReadSize() > GetEncodedSize())
      {
         unsigned int temp;
         ds >> temp;
         SetIndex(temp);
         ds.Seekg(4U, dtUtil::DataStream::SeekTypeEnum::CURRENT);
         // Need to save the read pos so I can skip to a 64 bit boundary after reading the variant data.
         //unsigned int savedReadPos = ds.GetReadPosition();

         ds >> temp;
         SetTypeCode(temp);
         unsigned int alternativeLengthInBytes = 0U;
         ds >> alternativeLengthInBytes;
         if (ds.GetRemainingReadSize() > alternativeLengthInBytes)
         {
            result = DecodeVariant(ds, alternativeLengthInBytes, coord);
            ds.Seekg((8U - (ds.GetReadPosition() % 8U)) % 8U,  dtUtil::DataStream::SeekTypeEnum::CURRENT); // goto 64 bit boundary
         }
         // Not sure what to if there is not enough data size left, just nothing is what it does now.
      }
      return result;
   }

   /////////////////////////////////////////////////////////
   bool EnvironmentProcessRecord::DecodeVariant(dtUtil::DataStream& ds, unsigned int lengthInBytes, dtUtil::Coordinates& coord)
   {
      bool handled = false;
      unsigned typeCode = GetTypeCode();
      switch (EnvironmentRecordTypeCode(typeCode))
      {
         case BoundingSphereRecordType:
         case SphereRecord1Type:
         case SphereRecord2Type:
         {
            osg::Vec3d worldCoordinate;
            float radius = 0.0f;

            ds >> worldCoordinate;
            worldCoordinate = coord.ConvertToLocalTranslation(worldCoordinate);
            mParameterData->AddValue(PARAM_LOCATION, worldCoordinate);

            ds >> radius;
            mParameterData->AddValue(PARAM_RADIUS, radius);
            if (typeCode == SphereRecord2Type)
            {
               ds >> radius;
               mParameterData->AddValue(PARAM_RADIUS_RATE, radius);

               osg::Vec3f value;

               ds >> value;
               value = coord.GetOriginRotationMatrixInverse().preMult(value);
               mParameterData->AddValue(PARAM_VELOCITY, value);

               ds >> value;
               mParameterData->AddValue(PARAM_ANGULAR_VELOCITY, value);
            }
            else
            {
               ds.Seekg(4U, dtUtil::DataStream::SeekTypeEnum::CURRENT);
            }
            handled = true;
            break;
         }
         case RectangularVolRecord1Type:
         case RectangularVolRecord2Type:
         case EllipsoidRecord1Type:
         case EllipsoidRecord2Type:
         case GaussianPuffRecordType:
         {
            osg::Vec3d worldCoordinate;
            osg::Vec3f value;

            ds >> worldCoordinate;
            worldCoordinate = coord.ConvertToLocalTranslation(worldCoordinate);
            mParameterData->AddValue(PARAM_LOCATION, worldCoordinate);

            if (typeCode == GaussianPuffRecordType)
            {
               ds >> worldCoordinate;
               worldCoordinate = coord.ConvertToLocalTranslation(worldCoordinate);
               mParameterData->AddValue(PARAM_ORIGINATION_LOCATION, worldCoordinate);
            }

            ds >> value;
            mParameterData->AddValue(PARAM_DIMENSION, value);

            if (typeCode != EllipsoidRecord1Type && typeCode != RectangularVolRecord1Type)
            {
               ds >> value;
               mParameterData->AddValue(PARAM_DIMENSION_RATE, value);
            }

            ds >> value;
            value = coord.ConvertToLocalRotation(value);
            mParameterData->AddValue(PARAM_ORIENTATION, value);

            if (typeCode != EllipsoidRecord1Type && typeCode != RectangularVolRecord1Type)
            {
               ds >> value;
               value = coord.GetOriginRotationMatrixInverse().preMult(value);
               mParameterData->AddValue(PARAM_VELOCITY, value);

               ds >> value;
               mParameterData->AddValue(PARAM_ANGULAR_VELOCITY, value);


               if (typeCode == GaussianPuffRecordType)
               {
                  float centroidHeight = 0.0f;
                  ds >> centroidHeight;
                  mParameterData->AddValue(PARAM_CENTROID_HEIGHT, centroidHeight);
               }
               else
               {
                  ds.Seekg(4U, dtUtil::DataStream::SeekTypeEnum::CURRENT);
               }
            }
            handled = true;
            break;
         }
         case COMBICStateRecordType:
         case FlareStateRecordType:
         case UniformGeometryRecordType:
         case PointRecord1Type:
         case LineRecord1Type:
         case ConeRecord1Type:
         case PointRecord2Type:
         case LineRecord2Type:
         case ConeRecord2Type:
         case RectangularVolRecord3Type:
         case GaussianPlumeRecordType:
         {
            break;
         }
      }
      if (!handled)
      {
         ds.Seekg(lengthInBytes, dtUtil::DataStream::SeekTypeEnum::CURRENT);
      }
      return true;
   }

   /////////////////////////////////////////////////////////
   size_t EnvironmentProcessRecord::GetEncodedSize() const
   {
      size_t encodedSize = 4 + 4 + 4 + 4;  // index (4) + padding (4) + type enum (4) +length of an HLA unsigned long (4).
      // TODO figure out what size each one is.
      switch (EnvironmentRecordTypeCode(GetTypeCode()))
      {
         case BoundingSphereRecordType:
         case SphereRecord1Type:
         {
            encodedSize += 32;
            break;
         }
         case SphereRecord2Type:
         {
            encodedSize += 56;
            break;
         }
         case RectangularVolRecord1Type:
         {
            encodedSize += 48;
            break;
         }
         case RectangularVolRecord2Type:
         {
            encodedSize += 88;
            break;
         }
         case EllipsoidRecord1Type:
         {
            encodedSize += 48;
            break;
         }
         case EllipsoidRecord2Type:
         {
            encodedSize += 88;
            break;
         }
         case GaussianPuffRecordType:
         {
            encodedSize += 112;
            break;
         }
         case COMBICStateRecordType:
         case FlareStateRecordType:
         case UniformGeometryRecordType:
         case PointRecord1Type:
         case LineRecord1Type:
         case ConeRecord1Type:
         case PointRecord2Type:
         case LineRecord2Type:
         case ConeRecord2Type:
         case RectangularVolRecord3Type:
         case GaussianPlumeRecordType:
         {
            break;
         }
      }
      return encodedSize;
   }

   /////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////
   EnvironmentProcessRecordList::EnvironmentProcessRecordList(dtUtil::Coordinates& coordConverter, bool littleEndian)
   : mCoordConverter(coordConverter)
   , mCounter(0U)
   , mLittleEndian(littleEndian)
   {

   }

   /////////////////////////////////////////////////////////
   EnvironmentProcessRecordList::~EnvironmentProcessRecordList()
   {
   }

   /////////////////////////////////////////////////////////
   size_t EnvironmentProcessRecordList::GetNumRecords() const
   {
      return mRecords.size();
   }

   /////////////////////////////////////////////////////////
   size_t EnvironmentProcessRecordList::GetBaseSize() const
   {
      return 8;  // padding + length of an HLA unsigned long (4).
   }

   /////////////////////////////////////////////////////////
   EnvironmentProcessRecord& EnvironmentProcessRecordList::AddRecord()
   {
      std::string index;
      dtUtil::MakeIndexString(mCounter, index, 3);
      mRecords.push_back(new EnvironmentProcessRecord(index));
      ++mCounter;
      return *(mRecords.back());
   }

   struct RemoveProcessRecWithIndex
   {
      RemoveProcessRecWithIndex(unsigned int index)
      : mIndex(index)
      {
      }

      bool operator () (const EnvironmentProcessRecord* rec)
      {
         return rec->GetIndex() == mIndex;
      }

      unsigned int mIndex;
   };

   /////////////////////////////////////////////////////////
   bool EnvironmentProcessRecordList::RemoveRecord(unsigned int indexToRemove)
   {
      RemoveProcessRecWithIndex recRemoveFunc(indexToRemove);
      std::vector<dtCore::RefPtr<EnvironmentProcessRecord> >::iterator i = std::remove_if(mRecords.begin(), mRecords.end(), recRemoveFunc);
      bool result = i != mRecords.end();
      if (result)
      {
         mRecords.erase(i, mRecords.end());
      }
      return result;
   }

   /////////////////////////////////////////////////////////
   std::vector<dtCore::RefPtr<EnvironmentProcessRecord> >& EnvironmentProcessRecordList::GetRecords()
   {
      return mRecords;
   }

   /////////////////////////////////////////////////////////
   size_t EnvironmentProcessRecordList::Encode(char* buffer, size_t maxSize) const
   {
      size_t result = 0U;
      dtUtil::DataStream ds(buffer, maxSize, false);
      ds.SetForceLittleEndian(mLittleEndian);

      size_t baseSize = GetBaseSize();
      if (maxSize >= baseSize)
      {
         size_t remainingSize = maxSize - baseSize;

         ds << unsigned(GetNumRecords()); // I should probably just write the mRecord size.

         // pad to 64 bit.
         ds.WriteBytes(0, 4U);

         bool success = true;
         std::vector<dtCore::RefPtr<EnvironmentProcessRecord> >::const_iterator i, iend;
         i = mRecords.begin();
         iend = mRecords.end();
         for (; i != iend; ++i)
         {
            const EnvironmentProcessRecord& epr = **i;
            size_t requiredSize = epr.Encode(ds, mCoordConverter);
            if (requiredSize > remainingSize)
            {
               success = false;
            }
            else
            {
               remainingSize -= requiredSize;
            }
         }
         if (success)
         {
            result = maxSize - remainingSize;
         }
      }
      // if the result is not set, then the encoding failed, so set the result to the required size.
      if (result == 0)
      {
         result = GetEncodedSize();
      }

      return result;
   }

   /////////////////////////////////////////////////////////
   bool EnvironmentProcessRecordList::Decode(const char* buffer, size_t size)
   {
      bool result = false;
      //ewww, const cast, but the data stream won't use a const pointer.
      // we aren't writing to it, though, and no one else it accessing the data, so it should be fine.
      dtUtil::DataStream ds(const_cast<char *>(buffer), size, false);
      ds.SetForceLittleEndian(mLittleEndian);

      size_t baseSize = GetBaseSize();
      if (size >= baseSize)
      {
         size_t numberOfRecords;
         ds >> numberOfRecords;
         // skip 4 bytes
         ds.Seekg(4U, dtUtil::DataStream::SeekTypeEnum::CURRENT);

         mRecords.clear();
         mRecords.reserve(numberOfRecords);

         result = true;
         for (unsigned i = 0; i < numberOfRecords; ++i)
         {
            EnvironmentProcessRecord& rec = AddRecord();
            if (!rec.Decode(ds, mCoordConverter))
            {
               result = false;
               break;
            }
         }
      }
      return result;
   }

   /////////////////////////////////////////////////////////
   size_t EnvironmentProcessRecordList::GetEncodedSize() const
   {
      size_t requiredSize = 0U;
      std::vector<dtCore::RefPtr<EnvironmentProcessRecord> >::const_iterator i, iend;
      i = mRecords.begin();
      iend = mRecords.end();
      for (; i != iend; ++i)
      {
         const EnvironmentProcessRecord& epr = **i;
         requiredSize += epr.GetEncodedSize();
      }
      return GetBaseSize() + requiredSize;
   }

}
