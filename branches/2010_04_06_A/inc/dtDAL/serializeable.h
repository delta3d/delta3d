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
 * Danny J. McCue
 */

#ifndef DELTA__SERIALIZEABLE__H
#define DELTA__SERIALIZEABLE__H

////////////////////////////////////////////////////////////////////////////////

#include <string>

#include <xercesc/util/XercesDefs.hpp>

namespace dtUtil
{
   class DataStream;
}

XERCES_CPP_NAMESPACE_BEGIN
   class DOMDocument;
XERCES_CPP_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////

namespace dtDAL
{
   class StringSerializeable
   {
   public:
      /**
       * This should write all of the subclass specific data to the string.
       * The base class data will be read by the caller before it calls this method.
       * @param toFill the string to fill.
       */
      virtual void ToString(std::string& toFill) const = 0;

      /**
       * This should read all of the subclass specific data from the string.
       * The base class data will be set by the caller when it creates the object.
       * @return true if it was able to assign the value based on the string or false if not.
       * @param value the string to pull the data from.
       */
      virtual bool FromString(const std::string& value) = 0;
   };

   class DataStreamSerializeable
   {
   public:
      /**
       * This should write all of the subclass specific data to the stream.
       * The base class data will be read by the caller before it calls this method.
       * @param stream the stream to fill.
       */
      virtual void ToDataStream(dtUtil::DataStream& stream) const = 0;

      /**
       * This should read all of the subclass specific data from the stream.
       * The base class data will be set by the caller when it creates the object.
       * @return true if it was able to assign the value based on the stream or false if not.
       * @param stream the stream to pull the data from.
       */
      virtual bool FromDataStream(dtUtil::DataStream& stream) = 0;
   };

   class XMLSerializeable
   {
   public:
      /**
       * This should write all of the subclass specific data to xml.
       * The base class data will be read by the caller before it calls this method.
       * @param doc the XML document to write the data to.
       */
      virtual void ToXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) const = 0;

      /**
       * This should read all of the subclass specific data from xml.
       * The base class data will be set by the caller when it creates the object.
       * @return true if it was able to assign the value based on the xml doc or false if not.
       * @param doc the XML document to pull the data from.
       */
      virtual bool FromXML(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument& doc) = 0;
   };

   class Serializeable : public StringSerializeable, public DataStreamSerializeable
   {
   public:
   protected:
   private:
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // DELTA__SERIALIZEABLE__H
