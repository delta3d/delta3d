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

#include <dtHLAGM/onetomanymapping.h>
#include <iostream>

namespace dtHLAGM
{
   bool OneToManyMapping::operator==(const OneToManyMapping& compareTo) const
   {
     return ((mHLAName == compareTo.mHLAName)
             && (mHLAType == compareTo.mHLAType)
             && (mRequiredForHLA == compareTo.mRequiredForHLA)
             && (mGameParameters == compareTo.mGameParameters)
             && (mSpecial == compareTo.mSpecial)
             && (mIsArray == compareTo.mIsArray));
   }

   void OneToManyMapping::ParameterDefinition::AddEnumerationMapping(const std::string& hlaValue, const std::string& gameValue)
   {
      mHLAEnumerationMapping.insert(std::make_pair(hlaValue, gameValue));
      mGameEnumerationMapping.insert(std::make_pair(gameValue, hlaValue));
   }

   void OneToManyMapping::ParameterDefinition::ClearEnumerationMapping()
   {
      mHLAEnumerationMapping.clear();
      mGameEnumerationMapping.clear();
   }

   bool OneToManyMapping::ParameterDefinition::GetHLAEnumerationValue(const std::string& gameValue, std::string& hlaValue) const
   {
      std::map<std::string, std::string>::const_iterator i = mGameEnumerationMapping.find(gameValue);
      if (i == mGameEnumerationMapping.end())
         return false;

      hlaValue = i->second;
      return true;
   }

   bool OneToManyMapping::ParameterDefinition::GetGameEnumerationValue(const std::string& hlaValue, std::string& gameValue) const
   {
      std::map<std::string, std::string>::const_iterator i = mHLAEnumerationMapping.find(hlaValue);
      if (i == mHLAEnumerationMapping.end())
         return false;

      gameValue = i->second;
      return true;
   }

   bool OneToManyMapping::ParameterDefinition::operator==(const OneToManyMapping::ParameterDefinition& compareTo) const
   {
       return (mGameName == compareTo.mGameName)
       && (mGameType == compareTo.mGameType)
       && (mDefaultValue == compareTo.mDefaultValue)
       && (mRequiredForGame == compareTo.mRequiredForGame)
       && (mHLAEnumerationMapping == compareTo.mHLAEnumerationMapping)
       && (mGameEnumerationMapping == compareTo.mGameEnumerationMapping);
   }

   std::ostream& operator << (std::ostream& os, const OneToManyMapping& otmm)
   {
      os << "HLAName: " << otmm.GetHLAName() << std::endl;
      os << "HLAType: " << otmm.GetHLAType().GetName() << std::endl;
      os << "Required For HLA: " << otmm.IsRequiredForHLA() << std::endl;
      os << "Invalid: " << otmm.GetInvalid() << std::endl;
      os << "Special: " << otmm.GetSpecial() << std::endl;
      os << "Array: " << otmm.GetIsArray() << std::endl;
      os << "Parameters Begin: " << std::endl << std::endl;

      const std::vector<OneToManyMapping::ParameterDefinition>& pds = otmm.GetParameterDefinitions();

      std::vector<OneToManyMapping::ParameterDefinition>::const_iterator i = pds.begin();
      for (;i != pds.end(); ++i)
      {
         os << *i << std::endl;
      }
      os << std::endl << "Parameters End: " << std::endl;

      return os;
   }

   std::ostream& operator << (std::ostream& os, const OneToManyMapping::ParameterDefinition& pd)
   {
      os << "GameName: " << pd.GetGameName() << std::endl;
      os << "DataType: " << pd.GetGameType().GetName() << std::endl;
      os << "Default Value: " << pd.GetDefaultValue() << std::endl;
      os << "Required For Game: " << pd.IsRequiredForGame() << std::endl;
      const std::map<std::string, std::string>& hlaToGameMappings = pd.GetHLAToGameEnumerationMappings();

      std::map<std::string, std::string>::const_iterator i = hlaToGameMappings.begin();
      for (;i != hlaToGameMappings.end(); ++i)
      {
         os << "  HLA Value: " << i->first << " maps to Game Value: " << i->second << std::endl;
      }

      return os;
   }
}
