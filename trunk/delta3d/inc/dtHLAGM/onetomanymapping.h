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
 * @author David Guthrie
 */

#ifndef DELTA_ONE_TO_MANY_MAPPING
#define DELTA_ONE_TO_MANY_MAPPING

#include <string>
#include <map>
#include <iostream>
#include <dtDAL/datatype.h>
#include <dtCore/refptr.h>
#include "dtHLAGM/export.h"
#include "dtHLAGM/distypes.h"
#include "dtHLAGM/attributetype.h"

namespace dtHLAGM
{


   /**
    * Logically abstract class used to to define a one to many mapping between and HLA field and many message parameters.
    * The order of the GameParameterDefinitions matter because they code that uses this mapping will assume the order.
    */
   class DT_HLAGM_EXPORT OneToManyMapping
   {
      public:
      
         class DT_HLAGM_EXPORT ParameterDefinition
         {
            public:

               ParameterDefinition():
                  mGameType(&dtDAL::DataType::UNKNOWN),
                  mRequiredForGame(false)
               {}

               ParameterDefinition(const std::string& name, 
                     dtDAL::DataType& type,
                     const std::string& defaultValue,
                     bool required):
                  mGameName(name),
                  mGameType(&type),
                  mDefaultValue(defaultValue),
                  mRequiredForGame(required)
               {}

               /**
                * Destructor.
                */
               ~ParameterDefinition()
               {}

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
                * Gets the Name of a Game Type.
                *
                * @return the Game Name
                */
               const std::string& GetGameName() const
               {
                  return mGameName;
               }

               /**
                * Gets the Data Type of the Game Type.
                *
                * @return the Data Type of the Game Type
                */
               const dtDAL::DataType& GetGameType() const
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
                * Sets the Data Type of the Game Type
                *
                * @param GameType Data Type
                */
               void SetGameType(const dtDAL::DataType& thisGameTypeType)
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
                * Maps an HLA enumerated value to a string-base game enumeration value.
                * @note using this only makes sense if the GameType property is set to ENUMERATION.
                * @param hlaValue the hla value to map.  This should be in a format that the parameter translator can translate
                *                 into the hla type of the hla data.
                * @param gameValue the name of the enumeration value to map to.  This will be passed to the property's
                *                  SetStringValue method.
                */
               void AddEnumerationMapping(const std::string& hlaValue, const std::string& gameValue);

               /**
                * A simple method to clear all of the current enumeration mappings created using
                * AddEnumerationMapping.
                */
               void ClearEnumerationMapping();
               
               /**
                * Get the HLA value for an enumeration based on the given string game value.
                * @return true if the string value has been mapped or false if not.
                * @param gameValue the string name of the enumeration value to get the integer value for.
                * @param hlaValue output parameter that will be set the requested HLA numeric value if
                *                this method returns true or undefined if not.
                */
               bool GetHLAEnumerationValue(const std::string& gameValue, std::string& hlaValue) const;

               /**
                * Get the game value for an enumeration based on the given integer HLA value.
                * @return true if the value has been mapped or false if not.
                * @param hlaValue the value to get the string enumeration value for.
                * @param gameValue output parameter that will be set the requested game string value
                *                   if this method returns true or undefined if not.
                */
               bool GetGameEnumerationValue(const std::string& hlaValue, std::string& gameValue) const;

               /**
                * @return a const map of the HLA values mapped to the game values
                */
               const std::map<std::string, std::string>& GetHLAToGameEnumerationMappings() const
               {
                  return mHLAEnumerationMapping;
               }

               /**
                * @return a const map of the game values mapped to the HLA values
                */
               const std::map<std::string, std::string>& GetGameToHLAEnumerationMappings() const
               {
                  return mGameEnumerationMapping;
               }
               
               bool operator==(const ParameterDefinition& compareTo) const;

            private:
               ///The Game Parameter Name.
               std::string mGameName;

               ///The Game Parameter Type.
               const dtDAL::DataType* mGameType;

               ///The Default Value for this Attribute/Property
               std::string mDefaultValue;

               ///Boolean for whether this field is required for an Actor Update.
               bool mRequiredForGame;

               ///mapping of string to string values to map HLA enumerated values to
               ///ActorProxy stlye type-safe enumeration values.
               std::map<std::string, std::string> mHLAEnumerationMapping;

               ///mapping of string to string values to map ActorProxy stlye
               ///type-safe enumeration values to HLA enumerated values.
               std::map<std::string, std::string> mGameEnumerationMapping;
         };

         /**
          * Constructor.
          */
         OneToManyMapping(): mHLAType(&AttributeType::UNKNOWN),
            mRequiredForHLA(false), mInvalid(false)
         {}

         /**
          * Constructor setting the basic values
          * @param hlaName the name of the HLA value to map to.
          * @param hlaType the type of the HLA value.
          * @param requiredForHLA true if this field is required in the HLA FOM.
          */
         OneToManyMapping(const std::string& hlaName,
            const AttributeType& attributeType,
            bool requiredForHLA):
            mHLAName(hlaName),
            mHLAType(&attributeType),
            mRequiredForHLA(requiredForHLA)
         {}

         /**
          * Destructor.
          */
         ~OneToManyMapping()
         {}

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
          * Sets the Name for this HLA Type
          *
          * @param mHLATypeName HLA Name
          */
         void SetHLAName(const std::string& thisHLATypeName)
         {
           mHLAName = thisHLATypeName;
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
          * Sets the Data Type of the HLA Type
          *
          * @param HLAType HLA Data Type
          */
         void SetHLAType(const AttributeType& thisHLATypeType)
         {
            mHLAType = &thisHLATypeType;
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

         ///@return a reference to the vector of ParameterDefinitions that are used in this mapping.
         std::vector<ParameterDefinition>& GetParameterDefinitions() { return mGameParameters; };

         ///@return a const reference to the vector of ParameterDefinitions that are used in this mapping.
         const std::vector<ParameterDefinition>& GetParameterDefinitions() const { return mGameParameters; };

         virtual bool operator==(const OneToManyMapping& compareTo) const
         {
           return ((mHLAName == compareTo.mHLAName)
                   && (mHLAType == compareTo.mHLAType)
                   && (mRequiredForHLA == compareTo.mRequiredForHLA)
                   && (mGameParameters == compareTo.mGameParameters));
         }
         
      protected:

         ///The HLA field name.
         std::string mHLAName;

         ///The HLA Type.
         const AttributeType* mHLAType;

         ///Boolean for whether this field is required for an Obejct Update.
         bool mRequiredForHLA;

         ///whether or not this mapping is invalid.
         bool mInvalid;
         
         //A vector of the game parameter definitions for this mapping to use.
         std::vector<ParameterDefinition> mGameParameters; 
   };

   DT_HLAGM_EXPORT std::ostream& operator << (std::ostream& os, const OneToManyMapping& otmm);

   DT_HLAGM_EXPORT std::ostream& operator << (std::ostream& os, const OneToManyMapping::ParameterDefinition& pd);

};

#endif // DELTA_ONE_TO_MANY_MAPPING
