// dis_types.h: DIS/RPR-FOM data types.
//
//////////////////////////////////////////////////////////////////////

#ifndef P51_HLA_DIS_TYPES
#define P51_HLA_DIS_TYPES

namespace dtHLA
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
    * A DIS/RPR-FOM entity identifier.
    */
   class EntityIdentifier
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
   class EntityType
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
          *
          * @param kind the value of the kind field
          */
         void SetKind(unsigned char kind);

         /**
          * Returns the value of the kind field.
          *
          * @return the value of the kind field
          */
         unsigned char GetKind() const;

         /**
          * Sets the value of the domain field.
          *
          * @param domain the value of the domain field
          */
         void SetDomain(unsigned char domain);

         /**
          * Returns the value of the domain field.
          *
          * @return the value of the domain field
          */
         unsigned char GetDomain() const;

         /**
          * Sets the value of the country field.
          *
          * @param country the value of the country field
          */
         void SetCountry(unsigned short country);

         /**
          * Returns the value of the country field.
          *
          * @return the value of the country field
          */
         unsigned short GetCountry() const;

         /**
          * Sets the value of the category field.
          *
          * @param category the value of the category field
          */
         void SetCategory(unsigned char category);

         /**
          * Returns the value of the category field.
          *
          * @return the value of the category field
          */
         unsigned char GetCategory() const;

         /**
          * Sets the value of the subcategory field.
          *
          * @param subcategory the value of the subcategory field
          */
         void SetSubcategory(unsigned char subcategory);
         
         /**
          * Returns the value of the subcategory field.
          *
          * @return the value of the subcategory field
          */
         unsigned char GetSubcategory() const;

         /**
          * Sets the value of the specific field.
          *
          * @param specific the value of the specific field
          */
         void SetSpecific(unsigned char specific);

         /**
          * Returns the value of the specific field.
          *
          * @return the value of the specific field
          */
         unsigned char GetSpecific() const;

         /**
          * Sets the value of the extra field.
          *
          * @param extra the value of the extra field
          */
         void SetExtra(unsigned char extra);

         /**
          * Returns the value of the extra field.
          *
          * @return the value of the extra field
          */
         unsigned char GetExtra() const;


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
   class EventIdentifier
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
   class WorldCoordinate
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
   class EulerAngles
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
   class VelocityVector
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
};

#endif // P51_HLA_DIS_TYPES
