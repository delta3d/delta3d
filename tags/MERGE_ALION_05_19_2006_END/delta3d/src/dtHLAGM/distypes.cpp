/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * @author Olen A. Bruce
 * @author David Guthrie
 */

#include <dtHLAGM/distypes.h>
#include <osg/Endian>
#include "dtHLAGM/rprparametertranslator.h"

using namespace osg;

namespace dtHLAGM
{

   /**
    * Constructor.
    *
    * @param siteIdentifier the site identifier
    * @param applicationIdentifier the application identifier
    * @param entityIdentifier the entity identifier
    */
   EntityIdentifier::EntityIdentifier(unsigned short siteIdentifier,
                                      unsigned short applicationIdentifier,
                                      unsigned short entityIdentifier)
      : mSiteIdentifier(siteIdentifier),
        mApplicationIdentifier(applicationIdentifier),
        mEntityIdentifier(entityIdentifier)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t EntityIdentifier::EncodedLength() const
   {
      return RPRAttributeType::ENTITY_IDENTIFIER_TYPE.GetEncodedLength();
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void EntityIdentifier::Encode(char* buf) const
   {
      unsigned short siteIdentifier = mSiteIdentifier,
        applicationIdentifier = mApplicationIdentifier,
        entityIdentifier = mEntityIdentifier;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&siteIdentifier), sizeof(short));
         swapBytes((char*)(&applicationIdentifier), sizeof(short));
         swapBytes((char*)(&entityIdentifier), sizeof(short));
         //ulEndianSwap(&entityIdentifier);
      }

      *(unsigned short *)(&buf[0]) = siteIdentifier;
      *(unsigned short *)(&buf[2]) = applicationIdentifier;
      *(unsigned short *)(&buf[4]) = entityIdentifier;
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void EntityIdentifier::Decode(const char* buf)
   {
      unsigned short siteIdentifier = *(unsigned short*)(&buf[0]),
        applicationIdentifier = *(unsigned short*)(&buf[2]),
        entityIdentifier = *(unsigned short*)(&buf[4]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&siteIdentifier), sizeof(short));
         swapBytes((char*)(&applicationIdentifier), sizeof(short));
         swapBytes((char*)(&entityIdentifier), sizeof(short));
         //ulEndianSwap(&entityIdentifier);
      }

      mSiteIdentifier = siteIdentifier;
      mApplicationIdentifier = applicationIdentifier;
      mEntityIdentifier = entityIdentifier;
   }

   /**
    * Sets the value of the site identifier field.
    *
    * @param siteIdentifier the value of the site identifier field
    */
   void EntityIdentifier::SetSiteIdentifier(unsigned short siteIdentifier)
   {
      mSiteIdentifier = siteIdentifier;
   }

   /**
    * Returns the value of the site identifier field.
    *
    * @return the value of the site identifier field
    */
   unsigned short EntityIdentifier::GetSiteIdentifier() const
   {
      return mSiteIdentifier;
   }

   /**
    * Sets the value of the application identifier field.
    *
    * @param applicationIdentifier the value of the application
    * identifier field
    */
   void EntityIdentifier::SetApplicationIdentifier(unsigned short applicationIdentifier)
   {
      mApplicationIdentifier = applicationIdentifier;
   }

   /**
    * Returns the value of the application identifier field.
    *
    * @return the value of the application identifier field
    */
   unsigned short EntityIdentifier::GetApplicationIdentifier() const
   {
      return mApplicationIdentifier;
   }

   /**
    * Sets the value of the entity identifier field.
    *
    * @param entityIdentifier the value of the entity identifier field
    */
   void EntityIdentifier::SetEntityIdentifier(unsigned short entityIdentifier)
   {
      mEntityIdentifier = entityIdentifier;
   }

   /**
    * Returns the value of the entity identifier field.
    *
    * @return the value of the entity identifier field
    */
   unsigned short EntityIdentifier::GetEntityIdentifier() const
   {
      return mEntityIdentifier;
   }

   bool EntityIdentifier::operator==(const EntityIdentifier& compareTo) const
   {
      return ((mSiteIdentifier == compareTo.mSiteIdentifier)
              && (mApplicationIdentifier == compareTo.mApplicationIdentifier)
              && (mEntityIdentifier == compareTo.mEntityIdentifier));
   }

   bool EntityIdentifier::operator<(const EntityIdentifier& entityId) const
   {
      if(mSiteIdentifier != entityId.mSiteIdentifier)
      {
         return mSiteIdentifier < entityId.mSiteIdentifier;
      }
      else if(mApplicationIdentifier != entityId.mApplicationIdentifier)
      {
         return mApplicationIdentifier < entityId.mApplicationIdentifier;
      }
      else if(mEntityIdentifier != entityId.mEntityIdentifier)
      {
         return mEntityIdentifier < entityId.mEntityIdentifier;
      }
      else
      {
         return false;
      }
   }

   /**
    * Constructor.
    *
    * @param kind the value of the kind field
    * @param domain the value of the kind field
    * @param country the value of the kind field
    * @param category the value of the kind field
    * @param subcategory the value of the kind field
    * @param specific the value of the kind field
    * @param extra the value of the kind field
    */
   EntityType::EntityType(unsigned char kind,
                          unsigned char domain,
                          unsigned short country,
                          unsigned char category,
                          unsigned char subcategory,
                          unsigned char specific,
                          unsigned char extra)
      : mKind(kind),
        mDomain(domain),
        mCountry(country),
        mCategory(category),
        mSubcategory(subcategory),
        mSpecific(specific),
        mExtra(extra)
   {}

   /**
    * Compares this object to another of its type.
    *
    * @param entityType the object to compare this to
    * @return true if this object is equal to the parameter,
    * false otherwise
    */
   bool EntityType::operator==(const EntityType& entityType) const
   {
      return  ((mKind == entityType.mKind)
               && (mDomain == entityType.mDomain)
               && (mCountry == entityType.mCountry)
               && (mCategory == entityType.mCategory)
               && (mSubcategory == entityType.mSubcategory)
               && (mSpecific == entityType.mSpecific)
               && (mExtra == entityType.mExtra));
   }

   /**
    * Compares this object to another of its type.  Imposes
    * a total ordering, allowing this object to be used as a
    * map key.
    *
    * @param entityType the object to compare this to
    * @return true if this object is less than the parameter,
    * false otherwise
    */
   bool EntityType::operator<(const EntityType& entityType) const
   {
      if(mKind != entityType.mKind)
      {
         return mKind < entityType.mKind;
      }
      else if(mDomain != entityType.mDomain)
      {
         return mDomain < entityType.mDomain;
      }
      else if(mCountry != entityType.mCountry)
      {
         return mCountry < entityType.mCountry;
      }
      else if(mCategory != entityType.mCategory)
      {
         return mCategory < entityType.mCategory;
      }
      else if(mSubcategory != entityType.mSubcategory)
      {
         return mSubcategory < entityType.mSubcategory;
      }
      else if(mSpecific != entityType.mSpecific)
      {
         return mSpecific < entityType.mSpecific;
      }
      else if(mExtra != entityType.mExtra)
      {
         return mExtra < entityType.mExtra;
      }
      else
      {
         return false;
      }
   }

   /**
    * Ranks the match between this entity type and another.
    *
    * @param entityType the entity type to compare this to
    * @return the integer rank of the match, where -1 indicates that
    * the types do not match, and increasing ranks indicate better (more
    * specific) matches
    */
   int EntityType::RankMatch(const EntityType& entityType) const
   {
      int rank = 0;

      if(mKind == entityType.mKind)
      {
         rank++;
      }
      else if(mKind != 0 && entityType.mKind != 0)
      {
         return -1;
      }

      if(mDomain == entityType.mDomain)
      {
         rank++;
      }
      else if(mDomain != 0 && entityType.mDomain != 0)
      {
         return -1;
      }

      if(mCountry == entityType.mCountry)
      {
         rank++;
      }
      else if(mCountry != 0 && entityType.mCountry != 0)
      {
         return -1;
      }

      if(mCategory == entityType.mCategory)
      {
         rank++;
      }
      else if(mCategory != 0 && entityType.mCategory != 0)
      {
         return -1;
      }

      if(mSubcategory == entityType.mSubcategory)
      {
         rank++;
      }
      else if(mSubcategory != 0 && entityType.mSubcategory != 0)
      {
         return -1;
      }

      if(mSpecific == entityType.mSpecific)
      {
         rank++;
      }
      else if(mSpecific != 0 && entityType.mSpecific != 0)
      {
         return -1;
      }

      if(mExtra == entityType.mExtra)
      {
         rank++;
      }
      else if(mExtra != 0 && entityType.mExtra != 0)
      {
         return -1;
      }

      return rank;
   }

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t EntityType::EncodedLength() const
   {
      return RPRAttributeType::ENTITY_TYPE.GetEncodedLength();
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void EntityType::Encode(char* buf) const
   {
      buf[0] = mKind;
      buf[1] = mDomain;

      unsigned short country = mCountry;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&country), sizeof(short));
      }

      *(unsigned short *)(&buf[2]) = country;

      buf[4] = mCategory;
      buf[5] = mSubcategory;
      buf[6] = mSpecific;
      buf[7] = mExtra;
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void EntityType::Decode(const char* buf)
   {
      mKind = buf[0];
      mDomain = buf[1];

      unsigned short country = *(unsigned short*)(&buf[2]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&country), sizeof(short));
      }

      mCountry = country;

      mCategory = buf[4];
      mSubcategory = buf[5];
      mSpecific = buf[6];
      mExtra = buf[7];
   }

   //std::ostream& operator<<(std::ostream &o, const EntityType &et)
   //{
      // strange g++ compiler bugs make this not work.
      // std::string space(" ");
       
       /*o << (int)et.GetKind() << space <<  (int)et.GetDomain() << space <<  (int)et.GetCountry() 
         << space <<  (int)et.GetCategory() << space <<  (int)et.GetSubcategory() <<  space <<  (int)et.GetSpecific() 
         << space <<  (int)et.GetExtra();*/
   //    return o;
   //}

   /**
    * Constructor.
    *
    * @param eventIdentifier the event identifier
    */
   EventIdentifier::EventIdentifier(unsigned short eventIdentifier)
      : mEventIdentifier(eventIdentifier)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t EventIdentifier::EncodedLength() const
   {
      return RPRAttributeType::EVENT_IDENTIFIER_TYPE.GetEncodedLength();
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void EventIdentifier::Encode(char* buf) const
   {
      unsigned short eventIdentifier = mEventIdentifier;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&eventIdentifier), sizeof(short));
      }

      *(unsigned short *)(&buf[0]) = eventIdentifier;
      strcpy(buf+2,"oo");  //filler for now
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void EventIdentifier::Decode(const char* buf)
   {
      unsigned short eventIdentifier = *(unsigned short*)(&buf[0]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&eventIdentifier), sizeof(short));
      }
      mEventIdentifier = eventIdentifier;
   }

   /**
    * Sets the value of the event identifier field.
    *
    * @param eventIdentifier the value of the event identifier field
    */
   void EventIdentifier::SetEventIdentifier(unsigned short eventIdentifier)
   {
      mEventIdentifier = eventIdentifier;
   }

   /**
    * Returns the value of the event identifier field.
    *
    * @return the value of the event identifier field
    */
   unsigned short EventIdentifier::GetEventIdentifier() const
   {
      return mEventIdentifier;
   }

   /**
    * Constructor.
    *
    * @param x the value of the x field
    * @param y the value of the y field
    * @param z the value of the z field
    */
   WorldCoordinate::WorldCoordinate(double x,
                                    double y,
                                    double z)
      : mX(x),
        mY(y),
        mZ(z)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t WorldCoordinate::EncodedLength() const
   {
      return RPRAttributeType::WORLD_COORDINATE_TYPE.GetEncodedLength();
   }

   /**
    * Converts the referenced double from little to big
    * endian format, or vice-versa.
    *
    * @param d a pointer to the value to swap
    */
   static void endianSwap(double* d)
   {
      char* c = (char*)d;

      char tmp;

      for(int i=0;i<4;i++)
      {
         tmp = c[i];

         c[i] = c[7-i];

         c[7-i] = tmp;
      }
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void WorldCoordinate::Encode(char* buf) const
   {
      double x = mX,
        y = mY,
        z = mZ;

      if(getCpuByteOrder() == LittleEndian)
      {
         endianSwap(&x);
         endianSwap(&y);
         endianSwap(&z);
      }

      *(double *)(&buf[0]) = x;
      *(double *)(&buf[8]) = y;
      *(double *)(&buf[16]) = z;
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void WorldCoordinate::Decode(const char* buf)
   {
      double x = *(double *)(&buf[0]),
        y = *(double *)(&buf[8]),
        z = *(double *)(&buf[16]);

      if(getCpuByteOrder() == LittleEndian)
      {
         endianSwap(&x);
         endianSwap(&y);
         endianSwap(&z);
      }

      mX = x;
      mY = y;
      mZ = z;
   }

   /**
    * Sets the value of the x field.
    *
    * @param x the value of the x field
    */
   void WorldCoordinate::SetX(double x)
   {
      mX = x;
   }

   /**
    * Returns the value of the x field.
    *
    * @return the value of the x field
    */
   double WorldCoordinate::GetX() const
   {
      return mX;
   }

   /**
    * Sets the value of the y field.
    *
    * @param y the value of the y field
    */
   void WorldCoordinate::SetY(double y)
   {
      mY = y;
   }

   /**
    * Returns the value of the y field.
    *
    * @return the value of the y field
    */
   double WorldCoordinate::GetY() const
   {
      return mY;
   }

   /**
    * Sets the value of the z field.
    *
    * @param z the value of the z field
    */
   void WorldCoordinate::SetZ(double z)
   {
      mZ = z;
   }

   /**
    * Returns the value of the z field.
    *
    * @return the value of the z field
    */
   double WorldCoordinate::GetZ() const
   {
      return mZ;
   }

   /**
    * Constructor.
    *
    * @param psi the value of the psi field
    * @param theta the value of the theta field
    * @param phi the value of the phi field
    */
   EulerAngles::EulerAngles(float psi,
                            float theta,
                            float phi)
      : mPsi(psi),
        mTheta(theta),
        mPhi(phi)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t EulerAngles::EncodedLength() const
   {
      return RPRAttributeType::EULER_ANGLES_TYPE.GetEncodedLength();
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void EulerAngles::Encode(char* buf) const
   {
      float psi = mPsi,
        theta = mTheta,
        phi = mPhi;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&psi), sizeof(float));
         swapBytes((char*)(&theta), sizeof(float));
         swapBytes((char*)(&phi), sizeof(float));
      }

      *(float *)(&buf[0]) = psi;
      *(float *)(&buf[4]) = theta;
      *(float *)(&buf[8]) = phi;
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void EulerAngles::Decode(const char* buf)
   {
      float psi = *(float *)(&buf[0]),
        theta = *(float *)(&buf[4]),
        phi = *(float *)(&buf[8]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&psi), sizeof(float));
         swapBytes((char*)(&theta), sizeof(float));
         swapBytes((char*)(&phi), sizeof(float));
      }

      mPsi = psi;
      mTheta = theta;
      mPhi = phi;
   }

   /**
    * Sets the value of the psi field.
    *
    * @param psi the value of the psi field
    */
   void EulerAngles::SetPsi(float psi)
   {
      mPsi = psi;
   }

   /**
    * Returns the value of the psi field.
    *
    * @return the value of the psi field
    */
   float EulerAngles::GetPsi() const
   {
      return mPsi;
   }

   /**
    * Sets the value of the theta field.
    *
    * @param theta the value of the theta field
    */
   void EulerAngles::SetTheta(float theta)
   {
      mTheta = theta;
   }

   /**
    * Returns the value of the theta field.
    *
    * @return the value of the theta field
    */
   float EulerAngles::GetTheta() const
   {
      return mTheta;
   }

   /**
    * Sets the value of the phi field.
    *
    * @param phi the value of the phi field
    */
   void EulerAngles::SetPhi(float phi)
   {
      mPhi = phi;
   }

   /**
    * Returns the value of the phi field.
    *
    * @return the value of the phi field
    */
   float EulerAngles::GetPhi() const
   {
      return mPhi;
   }

   /**
    * Constructor.
    *
    * @param x the value of the x field
    * @param y the value of the y field
    * @param z the value of the z field
    */
   VelocityVector::VelocityVector(float x,
                                  float y,
                                  float z)
      : mX(x),
        mY(y),
        mZ(z)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t VelocityVector::EncodedLength() const
   {
      return 12;
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void VelocityVector::Encode(char* buf) const
   {
      float x = mX,
        y = mY,
        z = mZ;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&x), sizeof(float));
         swapBytes((char*)(&y), sizeof(float));
         swapBytes((char*)(&z), sizeof(float));
      }

      *(float *)(&buf[0]) = x;
      *(float *)(&buf[4]) = y;
      *(float *)(&buf[8]) = z;
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void VelocityVector::Decode(const char* buf)
   {
      float x = *(float *)(&buf[0]),
        y = *(float *)(&buf[4]),
        z = *(float *)(&buf[8]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&x), sizeof(float));
         swapBytes((char*)(&y), sizeof(float));
         swapBytes((char*)(&z), sizeof(float));
      }

      mX = x;
      mY = y;
      mZ = z;
   }

   /**
    * Sets the value of the x field.
    *
    * @param psi the value of the x field
    */
   void VelocityVector::SetX(float x)
   {
      mX = x;
   }

   /**
    * Returns the value of the x field.
    *
    * @return the value of the x field
    */
   float VelocityVector::GetX() const
   {
      return mX;
   }

   /**
    * Sets the value of the y field.
    *
    * @param theta the value of the y field
    */
   void VelocityVector::SetY(float y)
   {
      mY = y;
   }

   /**
    * Returns the value of the y field.
    *
    * @return the value of the y field
    */
   float VelocityVector::GetY() const
   {
      return mY;
   }

   /**
    * Sets the value of the z field.
    *
    * @param phi the value of the z field
    */
   void VelocityVector::SetZ(float z)
   {
      mZ = z;
   }

   /**
    * Returns the value of the z field.
    *
    * @return the value of the z field
    */
   float VelocityVector::GetZ() const
   {
      return mZ;
   }

   /**
    * Constructor.
    *
    * @param pClass the part class
    * @param typeMetric the type metric
    * @param value the part value
    */
   ArticulatedParts::ArticulatedParts(unsigned int pClass,
                                      unsigned int typeMetric,
                                      float value)
      : mClass(pClass),
        mTypeMetric(typeMetric),
        mValue(value)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t ArticulatedParts::EncodedLength() const
   {
      return 12;
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void ArticulatedParts::Encode(char* buf) const
   {
      unsigned int tClass = mClass,
        typeMetric = mTypeMetric;

      float value = mValue;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&tClass), sizeof(int));
         swapBytes((char*)(&typeMetric), sizeof(int));
         swapBytes((char*)(&value), sizeof(float));
      }

      *(unsigned int *)(&buf[0]) = tClass;
      *(unsigned int *)(&buf[4]) = typeMetric;
      *(float *)(&buf[8]) = value;
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void ArticulatedParts::Decode(const char* buf)
   {
      unsigned int tClass = *(unsigned int *)(&buf[0]),
        typeMetric = *(unsigned int *)(&buf[4]);

      float value = *(float *)(&buf[8]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&tClass), sizeof(int));
         swapBytes((char*)(&typeMetric), sizeof(int));
         swapBytes((char*)(&value), sizeof(float));
      }

      mClass = tClass;
      mTypeMetric = typeMetric;
      mValue = value;
   }

   /**
    * Sets the part class.
    *
    * @param pClass the part class
    */
   void ArticulatedParts::SetClass(unsigned int pClass)
   {
      mClass = pClass;
   }

   /**
    * Returns the part class.
    *
    * @return the part class
    */
   unsigned int ArticulatedParts::GetClass() const
   {
      return mClass;
   }

   /**
    * Sets the type metric.
    *
    * @param typeMetric the type metric
    */
   void ArticulatedParts::SetTypeMetric(unsigned int typeMetric)
   {
      mTypeMetric = typeMetric;
   }

   /**
    * Returns the type metric.
    *
    * @return the type metric
    */
   unsigned int ArticulatedParts::GetTypeMetric() const
   {
      return mTypeMetric;
   }

   /**
    * Sets the part value.
    *
    * @param value the part value
    */
   void ArticulatedParts::SetValue(float value)
   {
      mValue = value;
   }

   /**
    * Returns the part value.
    *
    * @return the part value
    */
   float ArticulatedParts::GetValue() const
   {
      return mValue;
   }

   /**
    * Constructor.
    *
    * @param station the part station
    */
   AttachedParts::AttachedParts(unsigned int station)
      : mStation(station)
   {}

   /**
    * Constructor.
    *
    * @param station the part station
    * @param storeType the store type
    */
   AttachedParts::AttachedParts(unsigned int station,
                                const EntityType& storeType)
      : mStation(station),
        mStoreType(storeType)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t AttachedParts::EncodedLength() const
   {
      return 12;
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void AttachedParts::Encode(char* buf) const
   {
      unsigned int station = mStation;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&station), sizeof(int));
      }

      *(unsigned int *)(&buf[0]) = station;
      mStoreType.Encode(&buf[4]);
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void AttachedParts::Decode(const char* buf)
   {
      unsigned int station = *(unsigned int *)(&buf[0]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&station), sizeof(int));
      }

      mStation = station;

      mStoreType.Decode(&buf[4]);
   }

   /**
    * Sets the part station.
    *
    * @param station the part station
    */
   void AttachedParts::SetStation(unsigned int station)
   {
      mStation = station;
   }

   /**
    * Returns the part station.
    *
    * @return the part station
    */
   unsigned int AttachedParts::GetStation() const
   {
      return mStation;
   }

   /**
    * Sets the store type.
    *
    * @param storeType the store type
    */
   void AttachedParts::SetStoreType(const EntityType& storeType)
   {
      mStoreType = storeType;
   }

   /**
    * Returns the store type.
    *
    * @return the store type
    */
   const EntityType& AttachedParts::GetStoreType() const
   {
      return mStoreType;
   }

   /**
    * Constructor.
    *
    * @param type the articulated parameter type
    */
   ParameterValue::ParameterValue(ArticulatedParameterType type)
      : mArticulatedParameterType(type)
   {}

   /**
    * Constructor.
    *
    * @param articulatedParts the articulated parts
    */
   ParameterValue::ParameterValue(const ArticulatedParts& articulatedParts)
      : mArticulatedParameterType(ArticulatedPart),
        mArticulatedParts(articulatedParts)
   {}

   /**
    * Constructor.
    *
    * @param attachedParts the attached parts
    */
   ParameterValue::ParameterValue(const AttachedParts& attachedParts)
      : mArticulatedParameterType(AttachedPart),
        mAttachedParts(attachedParts)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t ParameterValue::EncodedLength() const
   {
      return 16;
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void ParameterValue::Encode(char* buf) const
   {
      unsigned int articulatedParameterType = mArticulatedParameterType;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&articulatedParameterType), sizeof(int));
      }

      *(unsigned int *)(&buf[0]) = articulatedParameterType;

      if(mArticulatedParameterType == ArticulatedPart)
      {
         mArticulatedParts.Encode(&buf[4]);
      }
      else
      {
         mAttachedParts.Encode(&buf[4]);
      }
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void ParameterValue::Decode(const char* buf)
   {
      unsigned int articulatedParameterType = *(unsigned int *)(&buf[0]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&articulatedParameterType), sizeof(int));
      }

      mArticulatedParameterType =
        (ArticulatedParameterType)articulatedParameterType;

      if(mArticulatedParameterType == ArticulatedPart)
      {
         mArticulatedParts.Decode(&buf[4]);
      }
      else
      {
         mAttachedParts.Decode(&buf[4]);
      }
   }

   /**
    * Sets the articulated parameter type.
    *
    * @param type the articulated parameter type
    */
   void ParameterValue::SetArticulatedParameterType(ArticulatedParameterType type)
   {
      mArticulatedParameterType = type;
   }

   /**
    * Returns the articulated parameter type.
    *
    * @return the articulated parameter type
    */
   ArticulatedParameterType ParameterValue::GetArticulatedParameterType() const
   {
      return mArticulatedParameterType;
   }

   /**
    * Sets the articulated parts structure.
    *
    * @param articulatedParts the articulated parts structure to copy
    */
   void ParameterValue::SetArticulatedParts(const ArticulatedParts& articulatedParts)
   {
      mArticulatedParts = articulatedParts;
   }

   /**
    * Returns the articulated parts structure.
    *
    * @return the articulated parts structure
    */
   const ArticulatedParts& ParameterValue::GetArticulatedParts() const
   {
      return mArticulatedParts;
   }

   /**
    * Sets the attached parts structure.
    *
    * @param attachedParts the attached parts structure to copy
    */
   void ParameterValue::SetAttachedParts(const AttachedParts& attachedParts)
   {
      mAttachedParts = attachedParts;
   }

   /**
    * Returns the attached parts structure.
    *
    * @return the attached parts structure
    */
   const AttachedParts& ParameterValue::GetAttachedParts() const
   {
      return mAttachedParts;
   }

   /**
    * Constructor.
    *
    * @param articulatedParameterChange the articulated parameter change
    * @param partAttachedTo the part attached to
    */
   ArticulatedParameter::ArticulatedParameter(unsigned char articulatedParameterChange,
                                              unsigned short partAttachedTo)
      : mArticulatedParameterChange(articulatedParameterChange),
        mPartAttachedTo(partAttachedTo)
   {}

   /**
    * Constructor.
    *
    * @param articulatedParameterChange the articulated parameter change
    * @param partAttachedTo the part attached to
    * @param parameterValue the parameter value
    */
   ArticulatedParameter::ArticulatedParameter(unsigned char articulatedParameterChange,
                                              unsigned short partAttachedTo,
                                              const ParameterValue& parameterValue)
      : mArticulatedParameterChange(articulatedParameterChange),
        mPartAttachedTo(partAttachedTo),
        mParameterValue(parameterValue)
   {}

   /**
    * Returns the encoded length of this object.
    *
    * @return the encoded length of this object, in bytes
    */
   size_t ArticulatedParameter::EncodedLength() const
   {
      return 20;
   }

   /**
    * Encodes this object into the specified buffer.
    *
    * @param buf the buffer to contain the encoded object
    */
   void ArticulatedParameter::Encode(char* buf) const
   {
      unsigned short partAttachedTo = mPartAttachedTo;

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&partAttachedTo), sizeof(short));
      }

      *(unsigned char *)(&buf[0]) = mArticulatedParameterChange;
      *(unsigned char *)(&buf[1]) = 0; // padding
      *(unsigned short *)(&buf[2]) = partAttachedTo;
      mParameterValue.Encode(&buf[4]);
   }

   /**
    * Decodes the values contained in the specified buffer.
    *
    * @param buf the buffer containing the encoded object
    */
   void ArticulatedParameter::Decode(const char* buf)
   {
      mArticulatedParameterChange = *(unsigned char *)(&buf[0]);

      unsigned short partAttachedTo = *(unsigned short *)(&buf[2]);

      if(getCpuByteOrder() == LittleEndian)
      {
         swapBytes((char*)(&partAttachedTo), sizeof(short));
      }

      mPartAttachedTo = partAttachedTo;

      mParameterValue.Decode(&buf[4]);
   }

   /**
    * Sets the articulated parameter change.
    *
    * @param articulatedParameterChange the articulated parameter change
    */
   void ArticulatedParameter::SetArticulatedParameterChange(unsigned char articulatedParameterChange)
   {
      mArticulatedParameterChange = articulatedParameterChange;
   }

   /**
    * Returns the articulated parameter change.
    *
    * @return the articulated parameter change
    */
   unsigned char ArticulatedParameter::GetArticulatedParameterChange() const
   {
      return mArticulatedParameterChange;
   }

   /**
    * Sets the part attached to.
    *
    * @param partAttachedTo the part attached to
    */
   void ArticulatedParameter::SetPartAttachedTo(unsigned short partAttachedTo)
   {
      mPartAttachedTo = partAttachedTo;
   }

   /**
    * Returns the part attached to.
    *
    * @return the part attached to
    */
   unsigned short ArticulatedParameter::GetPartAttachedTo() const
   {
      return mPartAttachedTo;
   }

   /**
    * Sets the parameter value.
    *
    * @param parameterValue the parameter value to copy
    */
   void ArticulatedParameter::SetParameterValue(const ParameterValue& parameterValue)
   {
      mParameterValue = parameterValue;
   }

   /**
    * Returns the parameter value.
    *
    * @return the parameter value
    */
   const ParameterValue& ArticulatedParameter::GetParameterValue() const
   {
      return mParameterValue;
   }
}
