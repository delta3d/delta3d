/* -*-c++-*-
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
 * Matthew W. Campbell
 * David Guthrie
 */

#ifndef DELTA_NAMED_PARAMETER
#define DELTA_NAMED_PARAMETER

#include <dtCore/abstractparameter.h>
#include <dtCore/export.h>

#include <dtUtil/log.h>

namespace dtCore
{
   class ActorProperty;

   /**
    * @class MessageParameter
    */
   class DT_CORE_EXPORT NamedParameter : public AbstractParameter
   {
      public:

         static const char DEFAULT_DELIMETER;

         NamedParameter(DataType& dataType, const dtUtil::RefString& name, bool isList = false);

         /**
          * @param otherParam the message parameter to make this one match.
          */
         virtual void CopyFrom(const NamedParameter& otherParam) = 0;

         ///Writes pertinent data about this parameter to the given logger and log level.
         void WriteToLog(dtUtil::Log& logger, dtUtil::Log::LogMessageType level = dtUtil::Log::LOG_DEBUG) const;

         /**
          * @return True if the paramater contains a list of
          *    data, false if this parameter only stores one value.
          */
         bool IsList() const;

         /**
          * Gets the character used as a delimeter between parameters when serializing
          * a message parameter containing a list of values to a string.
          * @return The delimeter.  By default, this is equal to "|".
          */
         char GetParamDelimeter() const;

         /**
          * Allows one to override the default delimeter used to serialize message
          * parmeters to a string.
          * @param delim The new delimeter.  This should NOT be the space character!
          *    Default value for the delimeter is the pipe character. '|'
          * @see GetParamDelimeter
          */
         void SetParamDelimeter(char delim);

         /**
          * Sets the message parameter's value from the actor property's value
          */
         virtual void SetFromProperty(const dtCore::ActorProperty& property);

         /**
          * Sets the actor property's value from the message parameter's value
          */
         virtual void ApplyValueToProperty(dtCore::ActorProperty& property) const;

         virtual bool operator==(const NamedParameter& toCompare) const = 0;
         bool operator!=(const NamedParameter& toCompare) const { return !(*this == toCompare); }

         virtual bool operator==(const ActorProperty& toCompare) const;
         bool operator!=(const ActorProperty& toCompare) const { return !(*this == toCompare); }

         /**
          * Creates a named parameter that will hold the given type.
          * @param type the datatype the parameter should hold.
          * @param name the name of the parameter to create.
          * @param isList true if the created parameter should be a list type.
          * @throw dtUtil::Exception with dtGame::ExceptionEnum::INVALID_PARAMETER if the type is unsupported.
          */
         static dtCore::RefPtr<NamedParameter> CreateFromType(
                  dtCore::DataType& type,
                  const dtUtil::RefString& name, bool isList = false);


      protected:

         virtual ~NamedParameter() { }

         /**
          * Helper method for each specific parameter type to be able to
          * validate the property type before acting on it
          */
         void ValidatePropertyType(const dtCore::ActorProperty& property) const;

      private:
         //This value is used as a delimeter between list data elements
         //when converting to and from a string.
         char mParamListDelimeter;

         bool mIsList;
   };
}

#endif
