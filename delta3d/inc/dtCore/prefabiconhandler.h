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

#ifndef prefabiconhandler_h__
#define prefabiconhandler_h__

#include <dtCore/export.h>
#include <dtCore/basexmlhandler.h>

namespace dtCore
{
   /** 
     * Used as an XML handler to parse just the Map file's header data. Will not
     * create a new Map or any content in the map file. 
     * @see MapParser
     */
   class PrefabIconHandler : public dtCore::BaseXMLHandler
   {
   public:
      PrefabIconHandler();

      /** 
       * Get the parsed icon name.
       * @return The icon name for the prefab file
       */
      std::string GetIconName() const;

      /** 
       * Has the Map file header data been parsed yet?
       * @return true if finished parsing correctly, false otherwise
       */
      virtual bool HandledDesiredData() const;

      virtual void ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
#if XERCES_VERSION_MAJOR < 3
      virtual void characters(const XMLCh* const chars, const unsigned int length);
#else
      virtual void characters(const XMLCh* const chars, const XMLSize_t length);
#endif

   protected:
      virtual ~PrefabIconHandler();
      
   private:
      std::string mIconName;
      bool mIsIconParsed;
   };
}

#endif // prefabiconhandler_h__
