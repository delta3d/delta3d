// dis_types.cpp: Implementation of DIS/RPR-FOM data types.
//
//////////////////////////////////////////////////////////////////////

#include "ul.h"

#include "dis_types.h"

using namespace dtHLA;


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
int EntityIdentifier::EncodedLength() const
{
   return 6;
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
                  
   if(ulIsLittleEndian)
   {
      ulEndianSwap(&siteIdentifier);
      ulEndianSwap(&applicationIdentifier);
      ulEndianSwap(&entityIdentifier);
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

   if(ulIsLittleEndian)
   {
      ulEndianSwap(&siteIdentifier);
      ulEndianSwap(&applicationIdentifier);
      ulEndianSwap(&entityIdentifier);
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
void EntityIdentifier::SetApplicationIdentifier(
   unsigned short applicationIdentifier)
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
int EntityType::EncodedLength() const
{
   return 8;
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

   if(ulIsLittleEndian)
   {
      ulEndianSwap(&country);
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

   if(ulIsLittleEndian)
   {
      ulEndianSwap(&country);
   }
   
   mCountry = country;

   mCategory = buf[4];
   mSubcategory = buf[5];
   mSpecific = buf[6];
   mExtra = buf[7];
}

/**
 * Sets the value of the kind field.
 *
 * @param kind the value of the kind field
 */
void EntityType::SetKind(unsigned char kind)
{
   mKind = kind;
}

/**
 * Returns the value of the kind field.
 *
 * @return the value of the kind field
 */
unsigned char EntityType::GetKind() const
{
   return mKind;
}

/**
 * Sets the value of the domain field.
 *
 * @param domain the value of the domain field
 */
void EntityType::SetDomain(unsigned char domain)
{
   mDomain = domain;
}

/**
 * Returns the value of the domain field.
 *
 * @return the value of the domain field
 */
unsigned char EntityType::GetDomain() const
{
   return mDomain;
}

/**
 * Sets the value of the country field.
 *
 * @param country the value of the country field
 */
void EntityType::SetCountry(unsigned short country)
{
   mCountry = country;
}

/**
 * Returns the value of the country field.
 *
 * @return the value of the country field
 */
unsigned short EntityType::GetCountry() const
{
   return mCountry;
}

/**
 * Sets the value of the category field.
 *
 * @param category the value of the category field
 */
void EntityType::SetCategory(unsigned char category)
{
   mCategory = category;
}

/**
 * Returns the value of the category field.
 *
 * @return the value of the category field
 */
unsigned char EntityType::GetCategory() const
{
   return mCategory;
}

/**
 * Sets the value of the subcategory field.
 *
 * @param subcategory the value of the subcategory field
 */
void EntityType::SetSubcategory(unsigned char subcategory)
{
   mSubcategory = subcategory;
}

/**
 * Returns the value of the subcategory field.
 *
 * @return the value of the subcategory field
 */
unsigned char EntityType::GetSubcategory() const
{
   return mSubcategory;
}

/**
 * Sets the value of the specific field.
 *
 * @param specific the value of the specific field
 */
void EntityType::SetSpecific(unsigned char specific)
{
   mSpecific = specific;
}

/**
 * Returns the value of the specific field.
 *
 * @return the value of the specific field
 */
unsigned char EntityType::GetSpecific() const
{
   return mSpecific;
}

/**
 * Sets the value of the extra field.
 *
 * @param extra the value of the extra field
 */
void EntityType::SetExtra(unsigned char extra)
{
   mExtra = extra;
}

/**
 * Returns the value of the extra field.
 *
 * @return the value of the extra field
 */
unsigned char EntityType::GetExtra() const
{
   return mExtra;
}

/**
 * Constructor.
 *
 * @param siteIdentifier the site identifier
 * @param applicationIdentifier the application identifier
 * @param eventIdentifier the event identifier
 */
EventIdentifier::EventIdentifier(unsigned short siteIdentifier,
                                 unsigned short applicationIdentifier,
                                 unsigned short eventIdentifier)
   : mSiteIdentifier(siteIdentifier),
     mApplicationIdentifier(applicationIdentifier),
     mEventIdentifier(eventIdentifier)
{}

/**
 * Returns the encoded length of this object.
 *
 * @return the encoded length of this object, in bytes
 */
int EventIdentifier::EncodedLength() const
{
   return 6;
}

/**
 * Encodes this object into the specified buffer.
 *
 * @param buf the buffer to contain the encoded object
 */
void EventIdentifier::Encode(char* buf) const
{
   unsigned short siteIdentifier = mSiteIdentifier,
                  applicationIdentifier = mApplicationIdentifier,
                  eventIdentifier = mEventIdentifier;
                  
   if(ulIsLittleEndian)
   {
      ulEndianSwap(&siteIdentifier);
      ulEndianSwap(&applicationIdentifier);
      ulEndianSwap(&eventIdentifier);
   }

   *(unsigned short *)(&buf[0]) = siteIdentifier;
   *(unsigned short *)(&buf[2]) = applicationIdentifier;
   *(unsigned short *)(&buf[4]) = eventIdentifier;
}

/**
 * Decodes the values contained in the specified buffer.
 *
 * @param buf the buffer containing the encoded object
 */
void EventIdentifier::Decode(const char* buf)
{
   unsigned short siteIdentifier = *(unsigned short*)(&buf[0]),
                  applicationIdentifier = *(unsigned short*)(&buf[2]),
                  eventIdentifier = *(unsigned short*)(&buf[4]);

   if(ulIsLittleEndian)
   {
      ulEndianSwap(&siteIdentifier);
      ulEndianSwap(&applicationIdentifier);
      ulEndianSwap(&eventIdentifier);
   }
   
   mSiteIdentifier = siteIdentifier;
   mApplicationIdentifier = applicationIdentifier;
   mEventIdentifier = eventIdentifier;
}

/**
 * Sets the value of the site identifier field.
 *
 * @param siteIdentifier the value of the site identifier field
 */
void EventIdentifier::SetSiteIdentifier(unsigned short siteIdentifier)
{
   mSiteIdentifier = siteIdentifier;
}

/**
 * Returns the value of the site identifier field.
 *
 * @return the value of the site identifier field
 */
unsigned short EventIdentifier::GetSiteIdentifier() const
{
   return mSiteIdentifier;
}

/**
 * Sets the value of the application identifier field.
 *
 * @param applicationIdentifier the value of the application
 * identifier field
 */
void EventIdentifier::SetApplicationIdentifier(unsigned short applicationIdentifier)
{
   mApplicationIdentifier = applicationIdentifier;
}

/**
 * Returns the value of the application identifier field.
 *
 * @return the value of the application identifier field
 */
unsigned short EventIdentifier::GetApplicationIdentifier() const
{
   return mApplicationIdentifier;
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
int WorldCoordinate::EncodedLength() const
{
   return 24;
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
                  
   if(ulIsLittleEndian)
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

   if(ulIsLittleEndian)
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
int EulerAngles::EncodedLength() const
{
   return 12;
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
                  
   if(ulIsLittleEndian)
   {
      ulEndianSwap((unsigned int *)(&psi));
      ulEndianSwap((unsigned int *)(&theta));
      ulEndianSwap((unsigned int *)(&phi));
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

   if(ulIsLittleEndian)
   {
      ulEndianSwap((unsigned int *)(&psi));
      ulEndianSwap((unsigned int *)(&theta));
      ulEndianSwap((unsigned int *)(&phi));
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
int VelocityVector::EncodedLength() const
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
                  
   if(ulIsLittleEndian)
   {
      ulEndianSwap((unsigned int *)(&x));
      ulEndianSwap((unsigned int *)(&y));
      ulEndianSwap((unsigned int *)(&z));
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

   if(ulIsLittleEndian)
   {
      ulEndianSwap((unsigned int *)(&x));
      ulEndianSwap((unsigned int *)(&y));
      ulEndianSwap((unsigned int *)(&z));
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