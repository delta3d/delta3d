/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2011, MOVES Institute
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
 * Erik Johnson
 */

#ifndef MAPHEADERHANDLER_h__
#define MAPHEADERHANDLER_h__

#include <dtCore/export.h>
#include <dtCore/basexmlhandler.h>
#include <dtCore/mapheaderdata.h>

namespace dtCore
{
   /** 
     * Used as an XML handler to parse just the Map file's header data. Will not
     * create a new Map or any content in the map file. 
     * @see MapParser
     */
   class MapHeaderHandler : public dtCore::BaseXMLHandler
   {
   public:
      MapHeaderHandler();

#if XERCES_VERSION_MAJOR < 3
      virtual void characters(const XMLCh* const chars, const unsigned int length);
#else
      virtual void characters(const XMLCh* const chars, const XMLSize_t length);
#endif
      virtual void ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

      /** 
       * Get the parsed MapHeaderData.
       * @return The header data from the Map file, after it's been parsed
       */
      const MapHeaderData& GetHeaderData() const;

      /** 
       * Has the Map file header data been parsed yet?
       * @return true if finished parsing correctly, false otherwise
       */
      bool HeaderParsed() const;

   protected:
      virtual ~MapHeaderHandler();
      
   private:
      MapHeaderData mHeaderData;
      bool mIsHeaderParsed;
   };
}

#endif // MAPHEADERHANDLER_h__
