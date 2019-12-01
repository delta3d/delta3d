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
 */

#ifndef DELTA_ONE_TO_MANY_MAPPING
#define DELTA_ONE_TO_MANY_MAPPING

#include <string>
#include <map>
#include <iosfwd>
#include <dtCore/datatype.h>
#include <dtCore/refptr.h>
#include <dtHLAGM/export.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/attributetype.h>
#include <dtUtil/getsetmacros.h>

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
                  mGameType(&dtCore::DataType::UNKNOWN),
                  mRequiredForGame(false)
               {}

               ParameterDefinition(const std::string& name,
                     dtCore::DataType& type,
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
               virtual ~ParameterDefinition()
               {}

               DT_DECLARE_ACCESSOR_INLINE(std::string, GameName);
               DT_DECLARE_ACCESSOR_INLINE(dtUtil::EnumerationPointer<dtCore::DataType>, GameType);
               /// The Default Value for this Attribute/Property
               DT_DECLARE_ACCESSOR_INLINE(std::string, DefaultValue);
               /// Boolean for whether this field is required for an Actor Update.
               DT_DECLARE_ACCESSOR_INLINE(bool, RequiredForGame);
               bool IsRequiredForGame() const
               {
                  return mRequiredForGame;
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

               ///mapping of string to string values to map HLA enumerated values to
               ///type-safe enumeration values.
               std::map<std::string, std::string> mHLAEnumerationMapping;

               ///mapping of string to string values to map
               ///type-safe enumeration values to HLA enumerated values.
               std::map<std::string, std::string> mGameEnumerationMapping;
         };

         /**
          * Constructor.
          */
         OneToManyMapping(): mHLAType(&AttributeType::UNKNOWN),
            mRequiredForHLA(false), mInvalid(false),
            mSpecial(false), mIsArray(false)
         {}

         /**
          * Constructor setting the basic values
          * @param hlaName the name of the HLA value to map to.
          * @param hlaType the type of the HLA value.
          * @param requiredForHLA true if this field is required in the HLA FOM.
          * @param special true if the mapping is to be handled as incoming-only.
          */
         OneToManyMapping(const std::string& hlaName,
            const AttributeType& attributeType,
            bool requiredForHLA,
            bool special = false,
            bool array = false):
            mHLAName(hlaName),
            mHLAType(&attributeType),
            mRequiredForHLA(requiredForHLA),
            mInvalid(false),
            mSpecial(special),
            mIsArray(array)
         {}

         /**
          * Destructor.
          */
         virtual ~OneToManyMapping()
         {}

         DT_DECLARE_ACCESSOR_INLINE(std::string, HLAName);
         DT_DECLARE_ACCESSOR_INLINE(dtUtil::EnumerationPointer<const AttributeType>, HLAType);
         DT_DECLARE_ACCESSOR_INLINE(bool, RequiredForHLA);
         bool IsRequiredForHLA() const
         {
            return mRequiredForHLA;
         }

         DT_DECLARE_ACCESSOR_INLINE(bool, Invalid);
         bool IsInvalid() const
         {
            return mInvalid;
         }

         DT_DECLARE_ACCESSOR_INLINE(bool, Special);
         bool IsSpecial() const
         {
            return mSpecial;
         }

         // if this mapping is defined as an array of the hla type.
         DT_DECLARE_ACCESSOR_INLINE(bool, IsArray);

         ///@return a reference to the vector of ParameterDefinitions that are used in this mapping.
         std::vector<ParameterDefinition>& GetParameterDefinitions() { return mGameParameters; };

         ///@return a const reference to the vector of ParameterDefinitions that are used in this mapping.
         const std::vector<ParameterDefinition>& GetParameterDefinitions() const { return mGameParameters; };

         virtual bool operator==(const OneToManyMapping& compareTo) const;

      protected:

         /// A vector of the game parameter definitions for this mapping to use.
         std::vector<ParameterDefinition> mGameParameters;
   };

   DT_HLAGM_EXPORT std::ostream& operator << (std::ostream& os, const OneToManyMapping& otmm);

   DT_HLAGM_EXPORT std::ostream& operator << (std::ostream& os, const OneToManyMapping::ParameterDefinition& pd);

}

#endif // DELTA_ONE_TO_MANY_MAPPING
