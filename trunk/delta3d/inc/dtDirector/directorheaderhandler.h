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

#ifndef directorheaderhandler_h__
#define directorheaderhandler_h__

#include <dtCore/export.h>
#include <dtCore/basexmlhandler.h>
#include <dtDirector/directorheaderdata.h>

namespace dtDirector
{
   /** 
     * Used as an XML handler to parse just the Map file's header data. Will not
     * create a new Map or any content in the map file. 
     * @see MapParser
     */
   class DirectorHeaderHandler : public dtCore::BaseXMLHandler
   {
   public:
      DirectorHeaderHandler();

      /** 
       * Get the parsed MapHeaderData.
       * @return The header data from the Map file, after it's been parsed
       */
      const DirectorHeaderData& GetHeaderData() const;

      /** 
       * Has the Map file header data been parsed yet?
       * @return true if finished parsing correctly, false otherwise
       */
      virtual bool HandledDesiredData() const;

      virtual void ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
      virtual void CombinedCharacters(const XMLCh* const chars, size_t length);

   protected:
      virtual ~DirectorHeaderHandler();
      
   private:
      DirectorHeaderData mHeaderData;
      std::string mPropertyName;
      bool mIsHeaderParsed;
   };
}

#endif // directorheaderhandler_h__
