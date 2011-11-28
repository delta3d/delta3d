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

#ifndef ENVIRONMENTPROCESSRECORDLIST_H_
#define ENVIRONMENTPROCESSRECORDLIST_H_

#include <dtUtil/getsetmacros.h>
#include <dtCore/namedgroupparameter.h>
#include <osg/Referenced>
#include <dtHLAGM/export.h>

namespace dtUtil
{
   class DataStream;
   class Coordinates;
}

namespace dtHLAGM
{
   class DT_HLAGM_EXPORT EnvironmentProcessRecord : public osg::Referenced
   {
   public:
      enum EnvironmentRecordTypeCode
      {
         COMBICStateRecordType = 256,
         FlareStateRecordType = 259,
         BiologicalStateType = 4096,
         ChemVaporStateType = 4097,
         RadiologicalStateType = 4098,
         ChemLiquidStateType = 4099,
         BoundingSphereRecordType = 65536,
         UniformGeometryRecordType = 327680,
         PointRecord1Type = 655360,
         LineRecord1Type = 786432,
         SphereRecord1Type = 851968,
         EllipsoidRecord1Type = 1048576,
         ConeRecord1Type = 3145728,
         RectangularVolRecord1Type = 5242880,
         RectangularVolRecord3Type = 83886080,
         PointRecord2Type = 167772160,
         LineRecord2Type = 201326592,
         SphereRecord2Type = 218103808,
         EllipsoidRecord2Type = 268435456,
         ConeRecord2Type = 805306368,
         RectangularVolRecord2Type = 1342177280,
         GaussianPlumeRecordType = 1610612736,
         GaussianPuffRecordType = 1879048192,
         GaussianPuffRecordEXType = 1879048193
      };

      static const dtUtil::RefString PARAM_LOCATION;
      static const dtUtil::RefString PARAM_ORIGINATION_LOCATION;
      static const dtUtil::RefString PARAM_ORIENTATION;
      static const dtUtil::RefString PARAM_VELOCITY;
      static const dtUtil::RefString PARAM_ANGULAR_VELOCITY;
      static const dtUtil::RefString PARAM_DIMENSION;
      static const dtUtil::RefString PARAM_DIMENSION_RATE;
      static const dtUtil::RefString PARAM_CENTROID_HEIGHT;
      static const dtUtil::RefString PARAM_RADIUS;
      static const dtUtil::RefString PARAM_RADIUS_RATE;

      static const dtUtil::RefString PARAM_AGENT_ENUM;
      static const dtUtil::RefString PARAM_GEOM_INDEX;
      static const dtUtil::RefString PARAM_TOTAL_MASS;
      static const dtUtil::RefString PARAM_MIN_SIZE;
      static const dtUtil::RefString PARAM_MAX_SIZE;
      static const dtUtil::RefString PARAM_AVG_MASS_PER_UNIT;
      static const dtUtil::RefString PARAM_PURITY;
      static const dtUtil::RefString PARAM_RADIOLOGCIAL_ACTIVITY;
      static const dtUtil::RefString PARAM_PROBABILITY;
      static const dtUtil::RefString PARAM_VIABILITY;

      EnvironmentProcessRecord(const std::string& recName);

      DT_DECLARE_ACCESSOR_GET_SET(unsigned int, Index);
      DT_DECLARE_ACCESSOR_GET_SET(unsigned int, TypeCode);

      dtCore::NamedGroupParameter& GetRecordData();

      /**
       * Fills the given buffer with the encoded version of this struct.
       */
      size_t Encode(dtUtil::DataStream& ds, dtUtil::Coordinates& coord) const;

      void EncodeVariant(dtUtil::DataStream& ds, dtUtil::Coordinates& coord) const;

      /**
       * Decodes the data in the buffer, filling this structure.
       * @return true if the data was parsed with no errors.
       */
      bool Decode(dtUtil::DataStream& ds, dtUtil::Coordinates& coord);

      /**
       * Decodes just the variant part of the structure.  The TypeCode must be set before this is called.
       * Normally, one should just call Decode, which will call this function.
       * @param ds the datastream containing the data to parse
       * @return true if the variant data was parsed with no errors.
       */
      bool DecodeVariant(dtUtil::DataStream& ds, unsigned int lengthInBytes, dtUtil::Coordinates& coord);

      /// @return the encoded size based on the given recod type code.
      size_t GetEncodedSize() const;
   private:
      ~EnvironmentProcessRecord();

      dtCore::RefPtr<dtCore::NamedGroupParameter> mParameterData;
   };

   class DT_HLAGM_EXPORT EnvironmentProcessRecordList
   {
   public:
      EnvironmentProcessRecordList(dtUtil::Coordinates& coordConverter, bool littleEndian = false);
      ~EnvironmentProcessRecordList();

      size_t GetNumRecords() const;

      /// Adds as new environment process record and returns it as a reference.
      EnvironmentProcessRecord& AddRecord();

      /**
       * Removes the record using the key of the index stored in record, not the index into the internal vector.
       * @return true if something was removed.
       */
      bool RemoveRecord(unsigned int indexToRemove);

      std::vector<dtCore::RefPtr<EnvironmentProcessRecord> >& GetRecords();

      /**
       * Fills the given buffer with the encoded version of this struct.
       * @param buffer the buffer to fill.
       * @param maxSize the maximum size allocated in this buffer.
       * @return the used buffer size or 0 is encoding failed, probably due to the maxSize being
       *         too small.
       */
      size_t Encode(char* buffer, size_t maxSize) const;

      /**
       * Decodes the data in the buffer, filling this structure.
       * @param buffer the buffer read data from.
       * @param size the size of the buffer.
       * @return true if the data was possible to decode.
       */
      bool Decode(const char* buffer, size_t size);

      /// @return the size of this record list encoded in bytes.
      size_t GetEncodedSize() const;
      /// @return the size of this record list encoded with no records.
      size_t GetBaseSize() const;
   private:

      std::vector<dtCore::RefPtr<EnvironmentProcessRecord> > mRecords;

      dtUtil::Coordinates& mCoordConverter;
      unsigned int mCounter;
      bool mLittleEndian;
   };

}

#endif /* ENVIRONMENTPROCESSRECORDLIST_H_ */
