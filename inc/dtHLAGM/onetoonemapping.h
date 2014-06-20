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
 * David Guthrie
 * Olen A. Bruce
 */

#ifndef DELTA_ONE_TO_ONE_MAPPING
#define DELTA_ONE_TO_ONE_MAPPING

//
// onetoonemapping.h: Declaration of the OneToOneMapping class.
//
///////////////////////////////////////////////////////////////////////

#include <string>
#include <map>
#include <dtCore/datatype.h>
#include <dtCore/refptr.h>
#include <dtHLAGM/export.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/attributetype.h>

namespace dtHLAGM
{
   /**
    * Logically abstract class used to to define a one to one mapping between two fields.
    */
   class DT_HLAGM_EXPORT OneToOneMapping
   {
      public:

         /**
          * Constructor.
          */
         OneToOneMapping(): mHLAType(&AttributeType::UNKNOWN),
            mGameType(&dtCore::DataType::UNKNOWN),
            mRequiredForHLA(false),
            mRequiredForGame(false), mInvalid(false)
         {
         }

         /**
          * Constructor setting the basic values
          * @param hlaName the name of the HLA value to map to.
          * @param gameName the name of the Game value to map to.
          * @param hlaType the type of the HLA value.
          * @param gameType the datatype in the game manager.
          * @param defaultValue a string version of the default value for either side.
          * @param requiredForGame true if this field is required in the game manager.
          * @param requiredForHLA true if this field is required in the HLA FOM.
          */
         OneToOneMapping(const std::string& hlaName,
            const std::string& gameName,
            const AttributeType& attributeType,
            const dtCore::DataType& gameType,
            const std::string& defaultValue,
            bool requiredForHLA,
            bool requiredForGame):
            mHLAName(hlaName),
            mGameName(gameName),
            mHLAType(&attributeType),
            mGameType(&gameType),
            mDefaultValue(defaultValue),
            mRequiredForHLA(requiredForHLA),
            mRequiredForGame(requiredForGame)
         {}

         /**
          * Destructor.
          */
         virtual ~OneToOneMapping()
         {
         }

         /**
          * Gets the Name of the HLA value.
          *
          * @return the HLA Name
          */
         const std::string& GetHLAName() const
         {
            return mHLAName;
         }

         /**
          * Gets the Name of a Game Type.
          *
          * @return the Game Name
          */
         const std::string& GetGameName() const
         {
            return mGameName;
         }

         /**
          * Gets the Data Type of the HLA Type.
          *
          * @return the Data Type of the HLA Type
          */
         const AttributeType& GetHLAType() const
         {
            return *mHLAType;
         }

         /**
          * Gets the Data Type of the Game Type.
          *
          * @return the Data Type of the Game Type
          */
         const dtCore::DataType& GetGameType() const
         {
            return *mGameType;
         }

         /**
          * Get the default value of this One to One mapping.
          *
          * @return the default value
          */
         const std::string& GetDefaultValue() const
         {
            return mDefaultValue;
         }

         /**
          * Whether or not this field is required for the Game update.
          *
          * @return is this field required
          */
         bool IsRequiredForGame() const
         {
            return mRequiredForGame;
         }

         /**
          * Whether or not this field is required for an HLA update.
          *
          * @return is this field required
          */
         bool IsRequiredForHLA() const
         {
            return mRequiredForHLA;
         }

         /**
          * Sets the Name for this HLA Type
          *
          * @param mHLATypeName HLA Name
          */
         void SetHLAName(const std::string& thisHLATypeName)
         {
           mHLAName = thisHLATypeName;
         }

         /**
          * Sets the Name for this Game Type
          *
          * @param mGameTypeName Game Name
          */
         void SetGameName(const std::string& thisGameTypeName)
         {
            mGameName = thisGameTypeName;
         }

         /**
          * Sets the Data Type of the HLA Type
          *
          * @param HLAType HLA Data Type
          */
         void SetHLAType(const AttributeType& thisHLATypeType)
         {
            mHLAType = &thisHLATypeType;
         }

         /**
          * Sets the Data Type of the Game Type
          *
          * @param GameType Data Type
          */
         void SetGameType(const dtCore::DataType& thisGameTypeType)
         {
            mGameType = &thisGameTypeType;
         }

         /**
          * Sets the default value of this mapping
          *
          * @param defaultValue Default Value
          */
         void SetDefaultValue(const std::string& defValue)
         {
            mDefaultValue = defValue;
         }

         /**
          * Sets whether or not field is required for an Game Update.
          *
          * @param requiredForGame true if this field is required for the game messaging
          */
         void SetRequiredForGame(bool requiredForGame)
         {
            mRequiredForGame = requiredForGame;
         }

         /**
          * Sets whether or not field is required for an HLA Update.
          *
          * @param requiredForHLA true if this field is required for an HLA update
          */
         void SetRequiredForHLA(bool requiredForHLA)
         {
            mRequiredForHLA = requiredForHLA;
         }

         ///@return true if this mapping is not valid and should be ignored
         bool IsInvalid() const { return mInvalid; }

         ///Sets if this mapping is invaild and should be ignored.
         void SetInvalid(bool newInvalid) {  mInvalid = newInvalid; }

         /**
          * Maps an HLA enumerated value to a string-base game enumeration value.
          * @note using this only makes sense if the GameType property is set to ENUMERATION.
          * @param hlaValue the hla value to map.
          * @param gameValue the name of the enumeration value to map to.  This will be passed to the property's
          *                  SetStringValue method.
          */
         void AddEnumerationMapping(const std::string& hlaValue, const std::string& gameValue)
         {
            mHLAEnumerationMapping.insert(std::make_pair(hlaValue, gameValue));
            mGameEnumerationMapping.insert(std::make_pair(gameValue, hlaValue));
         }

         /**
          * A simple method to clear all of the current enumeration mappings created using
          * AddEnumerationMapping.
          */
         void ClearEnumerationMapping()
         {
            mHLAEnumerationMapping.clear();
            mGameEnumerationMapping.clear();
         }
         /**
          * Get the HLA value for an enumeration based on the given string game value.
          * @return true if the string value has been mapped or false if not.
          * @param gameValue the string name of the enumeration value to get the value for.
          * @param hlaValue output parameter that will be set the requested HLA value if
          *                this method returns true or undefined if not.
          */
         bool GetHLAEnumerationValue(const std::string& gameValue, std::string& hlaValue) const
         {
            std::map<std::string, std::string>::const_iterator i = mGameEnumerationMapping.find(gameValue);
            if (i == mGameEnumerationMapping.end())
               return false;

            hlaValue = i->second;
            return true;
         }

         /**
          * Get the game value for an enumeration based on the given HLA value.
          * @return true if the hla value has been mapped or false if not.
          * @param hlaValue the string value to get the string enumeration value for.
          * @param gameValue output parameter that will be set the requested game string value
          *                   if this method returns true or undefined if not.
          */
         bool GetGameEnumerationValue(const std::string& hlaValue, std::string& gameValue) const
         {
            std::map<std::string, std::string>::const_iterator i = mHLAEnumerationMapping.find(hlaValue);
            if (i == mHLAEnumerationMapping.end())
               return false;

            gameValue = i->second;
            return true;
         }

         virtual bool operator==(const OneToOneMapping& compareTo) const
         {
           return ((mHLAName == compareTo.mHLAName)
                   && (mGameName == compareTo.mGameName)
                   && (mHLAType == compareTo.mHLAType)
                   && (mGameType == compareTo.mGameType)
                   && (mDefaultValue == compareTo.mDefaultValue)
                   && (mRequiredForHLA == compareTo.mRequiredForHLA)
                   && (mRequiredForGame == compareTo.mRequiredForGame)
                   && (mHLAEnumerationMapping == compareTo.mHLAEnumerationMapping)
                   && (mGameEnumerationMapping == compareTo.mGameEnumerationMapping));
         }
      protected:

         ///The HLA field name.
         std::string mHLAName;

         ///The game field name.
         std::string mGameName;

         ///The HLA Type.
         const AttributeType* mHLAType;

         ///The Game Type.
         const dtCore::DataType* mGameType;

         ///The Default Value for this Attribute/Property
         std::string mDefaultValue;

         ///Boolean for whether this field is required for an Obejct Update.
         bool mRequiredForHLA;

         ///Boolean for whether this field is required for an Actor Update.
         bool mRequiredForGame;

         ///mapping of string to string values to map HLA enumerated values to
         ///BaseActorObject stlye type-safe enumeration values.
         std::map<std::string, std::string> mHLAEnumerationMapping;

         ///mapping of string to string values to map BaseActorObject stlye
         ///type-safe enumeration values to HLA enumerated values.
         std::map<std::string, std::string> mGameEnumerationMapping;

         ///whether or not this mapping is invalid.
         bool mInvalid;
   };
}
#endif // DELTA_ONE_TO_ONE_MAPPING
