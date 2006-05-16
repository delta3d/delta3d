/* 
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
 */

#ifndef DELTA_DIS_TYPES
#define DELTA_DIS_TYPES

// distypes.h: DIS/RPR-FOM data types.
//
//////////////////////////////////////////////////////////////////////

#include <dtHLAGM/export.h>
#include <dtUtil/enumeration.h>

namespace dtHLAGM
{
   /**
    * DIS/RPR-FOM entity kinds.
    */
   enum EntityKind
   {
      OtherKind = 0,
      PlatformKind = 1,
      MunitionKind = 2,
      LifeFormKind = 3,
      EnvironmentalKind = 4,
      CulturalFeatureKind = 5,
      SupplyKind = 6,
      RadioKind = 7,
      ExpendableKind = 8,
      SensorEmitterKind = 9
   };
   
   /**
    * DIS/RPR-FOM platform domains.
    */
   enum PlatformDomain
   {
      OtherPlatformDomain = 0,
      LandPlatformDomain = 1,
      AirPlatformDomain = 2,
      SurfacePlatformDomain = 3,
      SubsurfacePlatformDomain = 4,
      SpacePlatformDomain = 5
   };
   
   /**
    * DIS/RPR-FOM damage states.
    */
   enum DamageState
   {
      NoDamage = 0,
      SlightDamage = 1,
      ModerateDamage = 2,
      Destroyed = 3
   };
   
   class DT_HLAGM_EXPORT AttributeType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(AttributeType);
      
      public:
         /**
          * A vector of three floats.  This is a world location and will be converted
          * with the coordinate converter.
          */
         static const AttributeType WORLD_COORDINATE_TYPE;
       
         ///Three angles used to define a 3D rotation.
         static const AttributeType EULER_ANGLES_TYPE;
         
         /** 
          * Three double values specifying the velocity vector in the coordinate system
          * of the entity.  This will be multiplied by the rotation offset.
          */
         static const AttributeType VELOCITY_VECTOR_TYPE;
         
         ///An unsigned integer.
         static const AttributeType UNSIGNED_INT_TYPE;
         
         ///A unsigned character.
         static const AttributeType UNSIGNED_CHAR_TYPE;
         
         ///An unsigned short integer.
         static const AttributeType UNSIGNED_SHORT_TYPE;
         
         ///double precision floating point value.
         static const AttributeType DOUBLE;
         
         ///struct holding a DIS entity enumeration.
         static const AttributeType ENTITY_TYPE;
         
         ///An indentifier uniquely marking an event.
         static const AttributeType EVENT_IDENTIFIER_TYPE;
         
         ///Indentifier for an entity marking used to hold both a marking type and the content of the marking.
         static const AttributeType MARKING_TYPE;
         
         unsigned char GetId() const { return mId; }

      private:
         AttributeType(const std::string& name, unsigned char id): dtUtil::Enumeration(name)
         {
            AddInstance(this);
            mId = id;
         }
         virtual ~AttributeType() {}
         unsigned char mId;
   };

   /**
    * A DIS/RPR-FOM entity identifier.
    */
   class DT_HLAGM_EXPORT EntityIdentifier
   {
      public:
     
         /**
          * Constructor.
          *
          * @param siteIdentifier the site identifier
          * @param applicationIdentifier the application identifier
          * @param entityIdentifier the entity identifier
          */
         EntityIdentifier(unsigned short siteIdentifier = 0,
                          unsigned short applicationIdentifier = 0,
                          unsigned short entityIdentifier = 0);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the value of the site identifier field.
          *
          * @param siteIdentifier the value of the site identifier field
          */
         void SetSiteIdentifier(unsigned short siteIdentifier);
         
         /**
          * Returns the value of the site identifier field.
          *
          * @return the value of the site identifier field
          */
         unsigned short GetSiteIdentifier() const;
         
         /**
          * Sets the value of the application identifier field.
          *
          * @param applicationIdentifier the value of the application
          * identifier field
          */
         void SetApplicationIdentifier(unsigned short applicationIdentifier);
         
         /**
          * Returns the value of the application identifier field.
          *
          * @return the value of the application identifier field
          */
         unsigned short GetApplicationIdentifier() const;
         
         /**
          * Sets the value of the entity identifier field.
          *
          * @param entityIdentifier the value of the entity identifier field
          */
         void SetEntityIdentifier(unsigned short entityIdentifier);
         
         /**
          * Returns the value of the entity identifier field.
          *
          * @return the value of the entity identifier field
          */
         unsigned short GetEntityIdentifier() const;
         
	      bool operator==(const EntityIdentifier& compareTo) const;
         
	      bool operator<(const EntityIdentifier& entityId) const;
         
      private:

         /**
          * The value of the site identifier field.
          */
         unsigned short mSiteIdentifier;
         
         /**
          * The value of the application identifier field.
          */
         unsigned short mApplicationIdentifier;
         
         /**
          * The value of the entity identifier field.
          */
         unsigned short mEntityIdentifier;
   };

   /**
    * A DIS/RPR-FOM entity type.
    */
   class DT_HLAGM_EXPORT EntityType
   {
      public:
     
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
         EntityType(unsigned char kind = 0,
                    unsigned char domain = 0,
                    unsigned short country = 0,
                    unsigned char category = 0,
                    unsigned char subcategory = 0,
                    unsigned char specific = 0,
                    unsigned char extra = 0);
         
         /**
          * Compares this object to another of its type.
          *
          * @param entityType the object to compare this to
          * @return true if this object is equal to the parameter,
          * false otherwise
          */
         bool operator==(const EntityType& entityType) const;
         bool operator!=(const EntityType& entityType) const { return !operator==(entityType); };
         
         /**
          * Assigns this object to the value of another.
          *
          * @param entityType the object to compare this to
          * @return *this
          */
         EntityType& operator=(const EntityType& entityType)
         {
            mKind = entityType.mKind;
            mDomain = entityType.mDomain;
            mCountry = entityType.mCountry;
            mCategory = entityType.mCategory;
            mSubcategory = entityType.mSubcategory;
            mSpecific = entityType.mSpecific;
            mExtra = entityType.mExtra;
            
            return *this;
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
         bool operator<(const EntityType& entityType) const;

         /**
          * Allows writing the elements of this class to a stream
          * @param o the ostream to write to
          * @param et the entity type.
          * @return the stream that was written to.
          */
         friend std::ostream& operator<<( std::ostream &o, const EntityType &et )
         {
            //strange g++ compiler bugs make this not work.
            // std::string space(" ");

            /*o << (int)et.GetKind() << space <<  (int)et.GetDomain() << space <<  (int)et.GetCountry() 
            << space <<  (int)et.GetCategory() << space <<  (int)et.GetSubcategory() <<  space <<  (int)et.GetSpecific() 
            << space <<  (int)et.GetExtra();*/
            return o;
         }
         
         /**
          * Ranks the match between this entity type and another.
          *
          * @param entityType the entity type to compare this to
          * @return the integer rank of the match, where -1 indicates that
          * the types do not match, and increasing ranks indicate better (more
          * specific) matches
          */
         int RankMatch(const EntityType& entityType) const;
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the value of the kind field.
          * @param kind the value of the kind field
          */
         void SetKind(unsigned char kind) { mKind = kind; }
        
         /**
          * @return the value of the kind field
          */
         unsigned char GetKind() const { return mKind; }
        
         /**
          * Sets the value of the domain field.
          * @param domain the value of the domain field
          */
         void SetDomain(unsigned char domain) { mDomain = domain; }
        
         /**
`          * @return the value of the domain field
          */
         unsigned char GetDomain() const { return mDomain; }
        
         /**
          * Sets the value of the country field.
          * @param country the value of the country field
          */
         void SetCountry(unsigned short country) { mCountry = country; }
        
         /**
          * @return the value of the country field
          */
         unsigned short GetCountry() const { return mCountry; }
        
         /**
          * Sets the value of the category field.
          * @param category the value of the category field
          */
         void SetCategory(unsigned char category) { mCategory = category; }
        
         /**
          * @return the value of the category field
          */
         unsigned char GetCategory() const { return mCategory; }
        
         /**
          * Sets the value of the subcategory field.
          * @param subcategory the value of the subcategory field
          */
         void SetSubcategory(unsigned char subcategory) { mSubcategory = subcategory; }
        
         /**
          * @return the value of the subcategory field
          */
         unsigned char GetSubcategory() const { return mSubcategory; }
        
         /**
          * Sets the value of the specific field.
          * @param specific the value of the specific field
          */
         void SetSpecific(unsigned char specific) { mSpecific = specific; }
        
         /**
          * @return the value of the specific field
          */
         unsigned char GetSpecific() const { return mSpecific; }
        
         /**
          * Sets the value of the extra field.
          * @param extra the value of the extra field
          */
         void SetExtra(unsigned char extra) { mExtra = extra; }
        
         /**
          * @return the value of the extra field
          */
         unsigned char GetExtra() const { return mExtra; }
         
      private:
         
         /**
          * The value of the kind field.
          */
         unsigned char mKind;
         
         /**
          * The value of the domain field.
          */
         unsigned char mDomain;
         
         /**
          * The value of the country field.
          */
         unsigned short mCountry;
         
         /**
          * The value of the category field.
          */
         unsigned char mCategory;
         
         /**
          * The value of the subcategory field.
          */
         unsigned char mSubcategory;
         
         /**
          * The value of the specific field.
          */
         unsigned char mSpecific;
         
         /**
          * The value of the extra field.
          */
         unsigned char mExtra;
   };

   /**
    * A DIS/RPR-FOM event identifier.
    */
   class DT_HLAGM_EXPORT EventIdentifier
   {
      public:
     
         /**
          * Constructor.
          *          
          * @param eventIdentifier the event identifier
          */
         EventIdentifier(unsigned short eventIdentifier = 0);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the value of the event identifier field.
          *
          * @param eventIdentifier the value of the event identifier field
          */
         void SetEventIdentifier(unsigned short eventIdentifier);
         
         /**
          * Returns the value of the event identifier field.
          *
          * @return the value of the event identifier field
          */
         unsigned short GetEventIdentifier() const;
         
      private:
         
         /**
          * The value of the event identifier field.
          */
         unsigned short mEventIdentifier;
   };
   
   /**
    * A DIS/RPR-FOM world coordinate type.
    */
   class DT_HLAGM_EXPORT WorldCoordinate
   {
      public:
     
         /**
          * Constructor.
          *
          * @param x the value of the x field
          * @param y the value of the y field
          * @param z the value of the z field
          */
         WorldCoordinate(double x = 0.0, 
                         double y = 0.0, 
                         double z = 0.0);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the value of the x field.
          *
          * @param x the value of the x field
          */
         void SetX(double x);
         
         /**
          * Returns the value of the x field.
          *
          * @return the value of the x field
          */
         double GetX() const;
         
         /**
          * Sets the value of the y field.
          *
          * @param y the value of the y field
          */
         void SetY(double y);
         
         /**
          * Returns the value of the y field.
          *
          * @return the value of the y field
          */
         double GetY() const;
         
         /**
          * Sets the value of the z field.
          *
          * @param z the value of the z field
          */
         void SetZ(double z);
         
         /**
          * Returns the value of the z field.
          *
          * @return the value of the z field
          */
         double GetZ() const;
         
      private:
         
         /**
          * The value of the x field.
          */
         double mX;
         
         /**
          * The value of the y field.
          */
         double mY;
         
         /**
          * The value of the z field.
          */
         double mZ;
   };
   
   /**
    * A DIS/RPR-FOM euler angle type.
    */
   class DT_HLAGM_EXPORT EulerAngles
   {
      public:
     
         /**
          * Constructor.
          *
          * @param psi the value of the psi field
          * @param theta the value of the theta field
          * @param phi the value of the phi field
          */
         EulerAngles(float psi = 0.0f,
                     float theta = 0.0f,
                     float phi = 0.0f);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the value of the psi field.
          *
          * @param psi the value of the psi field
          */
         void SetPsi(float psi);
         
         /**
          * Returns the value of the psi field.
          *
          * @return the value of the psi field
          */
         float GetPsi() const;
         
         /**
          * Sets the value of the theta field.
          *
          * @param theta the value of the theta field
          */
         void SetTheta(float theta);
         
         /**
          * Returns the value of the theta field.
          *
          * @return the value of the theta field
          */
         float GetTheta() const;
         
         /**
          * Sets the value of the phi field.
          *
          * @param phi the value of the phi field
          */
         void SetPhi(float phi);
         
         /**
          * Returns the value of the phi field.
          *
          * @return the value of the phi field
          */
         float GetPhi() const;
         
      private:
         
         /**
          * The value of the psi field.
          */
         float mPsi;
         
         /**
          * The value of the theta field.
          */
         float mTheta;
         
         /**
          * The value of the phi field.
          */
         float mPhi;
   };
   
   /**
    * A DIS/RPR-FOM velocity vector type.
    */
   class DT_HLAGM_EXPORT VelocityVector
   {
      public:

         /**
          * Constructor.
          *
          * @param x the value of the x field
          * @param y the value of the y field
          * @param z the value of the z field
          */
         VelocityVector(float x = 0.0f,
                        float y = 0.0f,
                        float z = 0.0f);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the value of the x field.
          *
          * @param psi the value of the x field
          */
         void SetX(float x);
         
         /**
          * Returns the value of the x field.
          *
          * @return the value of the x field
          */
         float GetX() const;
         
         /**
          * Sets the value of the y field.
          *
          * @param theta the value of the y field
          */
         void SetY(float y);
         
         /**
          * Returns the value of the y field.
          *
          * @return the value of the y field
          */
         float GetY() const;
         
         /**
          * Sets the value of the z field.
          *
          * @param phi the value of the z field
          */
         void SetZ(float z);
         
         /**
          * Returns the value of the z field.
          *
          * @return the value of the z field
          */
         float GetZ() const;
         
      private:

         /**
          * The value of the x field.
          */
         float mX;
         
         /**
          * The value of the y field.
          */
         float mY;
         
         /**
          * The value of the z field.
          */
         float mZ;
   };
   
   /**
    * DIS/RPR-FOM articulated part metrics.
    */
   enum ArticulatedTypeMetric
   {
      PositionMetric = 1,
      PositionRateMetric = 2,
      ExtensionMetric = 3,
      ExtensionRateMetric = 4,
      XMetric = 5,
      XRateMetric = 6,
      YMetric = 7,
      YRateMetric = 8,
      ZMetric = 9,
      ZRateMetric = 10,
      AzimuthMetric = 11,
      AzimuthRateMetric = 12,
      ElevationMetric = 13,
      ElevationRateMetric = 14,
      RotationMetric = 15,
      RotationRateMetric = 16
   };
   
   /**
    * A DIS/RPR-FOM articulated part.
    */
   class DT_HLAGM_EXPORT ArticulatedParts
   {
      public:
     
         /**
          * Constructor.
          *
          * @param pClass the part class
          * @param typeMetric the type metric
          * @param value the part value
          */
         ArticulatedParts(unsigned int pClass = 0,
                          unsigned int typeMetric = 0,
                          float value = 0.0f);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the part class.
          *
          * @param pClass the part class
          */
         void SetClass(unsigned int pClass);
         
         /**
          * Returns the part class.
          *
          * @return the part class
          */
         unsigned int GetClass() const;
         
         /**
          * Sets the type metric.
          *
          * @param typeMetric the type metric
          */
         void SetTypeMetric(unsigned int typeMetric);
         
         /**
          * Returns the type metric.
          *
          * @return the type metric
          */
         unsigned int GetTypeMetric() const;
         
         /**
          * Sets the part value.
          *
          * @param value the part value
          */
         void SetValue(float value);
         
         /**
          * Returns the part value.
          *
          * @return the part value
          */
         float GetValue() const;
         
      private:
         
         /**
          * The part class.
          */
         unsigned int mClass;
         
         /**
          * The part type metric.
          */
         unsigned int mTypeMetric;
         
         /**
          * The part value.
          */
         float mValue;
   };
   
   /**
    * DIS/RPR-FOM attached part.
    */
   class DT_HLAGM_EXPORT AttachedParts
   {
      public:
     
         /**
          * Constructor.
          *
          * @param station the part station
          */
         AttachedParts(unsigned int station = 0);
         
         /**
          * Constructor.
          *
          * @param station the part station
          * @param storeType the store type
          */
         AttachedParts(unsigned int station,
                       const EntityType& storeType);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the part station.
          *
          * @param station the part station
          */
         void SetStation(unsigned int station);
         
         /**
          * Returns the part station.
          *
          * @return the part station
          */
         unsigned int GetStation() const;
         
         /**
          * Sets the store type.
          *
          * @param storeType the store type
          */
         void SetStoreType(const EntityType& storeType);
         
         /**
          * Returns the store type.
          *
          * @return the store type
          */
         const EntityType& GetStoreType() const;
         
      private:
         
         /**
          * The part station.
          */
         unsigned int mStation;
         
         /**
          * The part store type.
          */
         EntityType mStoreType;
   };
   
   /**
    * DIS/RPR-FOM articulated parameter type.
    */
   enum ArticulatedParameterType
   {
      ArticulatedPart = 0,
      AttachedPart = 1
   };
   
   /**
    * A DIS/RPR-FOM parameter value.
    */
   class DT_HLAGM_EXPORT ParameterValue
   {
      public:
     
         /**
          * Constructor.
          *
          * @param type the articulated parameter type
          */
         ParameterValue(ArticulatedParameterType type = ArticulatedPart);
         
         /**
          * Constructor.
          *
          * @param articulatedParts the articulated parts
          */
         ParameterValue(const ArticulatedParts& articulatedParts);
         
         /**
          * Constructor.
          *
          * @param attachedParts the attached parts
          */
         ParameterValue(const AttachedParts& attachedParts);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the articulated parameter type.
          *
          * @param type the articulated parameter type
          */
         void SetArticulatedParameterType(ArticulatedParameterType type);
         
         /**
          * Returns the articulated parameter type.
          *
          * @return the articulated parameter type
          */
         ArticulatedParameterType GetArticulatedParameterType() const;
         
         /**
          * Sets the articulated parts structure.
          *
          * @param articulatedParts the articulated parts structure to copy
          */
         void SetArticulatedParts(const ArticulatedParts& articulatedParts);
         
         /**
          * Returns the articulated parts structure.
          *
          * @return the articulated parts structure
          */
         const ArticulatedParts& GetArticulatedParts() const;
         
         /**
          * Sets the attached parts structure.
          *
          * @param attachedParts the attached parts structure to copy
          */
         void SetAttachedParts(const AttachedParts& attachedParts);
         
         /**
          * Returns the attached parts structure.
          *
          * @return the attached parts structure
          */
         const AttachedParts& GetAttachedParts() const;
         
      private:
         
         /**
          * The parameter type (articulated or attached).
          */
         ArticulatedParameterType mArticulatedParameterType;
         
         /**
          * The articulated part structure.
          */
         ArticulatedParts mArticulatedParts;
         
         /**
          * The attached part structure.
          */
         AttachedParts mAttachedParts;
   };
   
   /**
    * A DIS/RPR-FOM articulated parameter.
    */
   class DT_HLAGM_EXPORT ArticulatedParameter
   {
      public:
     
         /**
          * Constructor.
          *
          * @param articulatedParameterChange the articulated parameter change
          * @param partAttachedTo the part attached to
          */
         ArticulatedParameter(unsigned char articulatedParameterChange = 0,
                              unsigned short partAttachedTo = 0);
         
         /**
          * Constructor.
          *
          * @param articulatedParameterChange the articulated parameter change
          * @param partAttachedTo the part attached to
          * @param parameterValue the parameter value
          */
         ArticulatedParameter(unsigned char articulatedParameterChange,
                              unsigned short partAttachedTo,
                              const ParameterValue& parameterValue);
         
         /**
          * Returns the encoded length of this object.
          *
          * @return the encoded length of this object, in bytes
          */
         int EncodedLength() const;
         
         /**
          * Encodes this object into the specified buffer.
          *
          * @param buf the buffer to contain the encoded object
          */
         void Encode(char* buf) const;
         
         /**
          * Decodes the values contained in the specified buffer.
          *
          * @param buf the buffer containing the encoded object
          */
         void Decode(const char* buf);
         
         /**
          * Sets the articulated parameter change.
          *
          * @param articulatedParameterChange the articulated parameter change
          */
         void SetArticulatedParameterChange(unsigned char articulatedParameterChange);
         
         /**
          * Returns the articulated parameter change.
          *
          * @return the articulated parameter change
          */
         unsigned char GetArticulatedParameterChange() const;
         
         /**
          * Sets the part attached to.
          *
          * @param partAttachedTo the part attached to
          */
         void SetPartAttachedTo(unsigned short partAttachedTo);
         
         /**
          * Returns the part attached to.
          *
          * @return the part attached to
          */
         unsigned short GetPartAttachedTo() const;
         
         /**
          * Sets the parameter value.
          *
          * @param parameterValue the parameter value to copy
          */
         void SetParameterValue(const ParameterValue& parameterValue);
         
         /**
          * Returns the parameter value.
          *
          * @return the parameter value
          */
         const ParameterValue& GetParameterValue() const;
         
      private:
      
         /**
          * The articulated parameter change.
          */
         unsigned char mArticulatedParameterChange;
         
         /**
          * The part attached to.
          */
         unsigned short mPartAttachedTo;
         
         /**
          * The parameter value.
          */
         ParameterValue mParameterValue;
   };
};

#endif // DELTA_DIS_TYPES
