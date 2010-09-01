/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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

#include <prefix/dtdalprefix.h>
#include <dtDAL/namedparameter.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/exceptionenum.h>

#include <sstream>

namespace dtDAL
{
   ///////////////////////////////////////////////////////////////////////////////
   const char NamedParameter::DEFAULT_DELIMETER = '|';

   ///////////////////////////////////////////////////////////////////////////////
   NamedParameter::NamedParameter(DataType& dataType, const dtUtil::RefString& name, bool isList):
      AbstractParameter(dataType, name),
      mIsList(isList)
   {
      SetParamDelimeter(DEFAULT_DELIMETER);
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool NamedParameter::IsList() const
   {
      return mIsList;
   }

   ///////////////////////////////////////////////////////////////////////////////
   char NamedParameter::GetParamDelimeter() const
   {
      return mParamListDelimeter;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::SetParamDelimeter(char delim)
   {
      mParamListDelimeter = delim;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::SetFromProperty(const dtDAL::ActorProperty& property)
   {
      throw dtDAL::InvalidParameterException(
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in SetFromProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::ApplyValueToProperty(dtDAL::ActorProperty& property) const
   {
      throw dtDAL::InvalidParameterException(
         "Message parameter[" + GetName() + "] of type[" + GetDataType().GetName() +
         "] does not have an associated actor property type in ApplyValueToProperty()", __FILE__, __LINE__);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::ValidatePropertyType(const dtDAL::ActorProperty& property) const
   {
      if (property.GetDataType() != GetDataType())
      {
         throw dtDAL::InvalidParameterException(
            "Actor Property [" + property.GetName() + "] with Data Type [" + property.GetDataType().GetName() +
            "] does not match the Message Parameter [" + GetName() +
            "] with Data Type [" + GetDataType().GetName() + "]", __FILE__, __LINE__);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void NamedParameter::WriteToLog(dtUtil::Log &logger, dtUtil::Log::LogMessageType level) const
   {
      if (logger.IsLevelEnabled(level))
      {
         std::ostringstream oss("");
         oss << "Message Parameter is: \"" << GetName() << ".\" ";
         oss << "Its message type is: \""  << GetDataType() << ".\" ";
         oss << "Its value is: \"" << ToString() << ".\"";

         logger.LogMessage(level, __FUNCTION__, __LINE__, oss.str().c_str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
}
