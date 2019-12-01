/* -*-c++-*-
 * Delta3D
 * Copyright 2011, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#ifndef PROJECTCONFIGXMLHANDLER_H_
#define PROJECTCONFIGXMLHANDLER_H_

#include <dtCore/basexmlhandler.h>
#include <dtCore/basexml.h>
#include <dtCore/projectconfig.h>
#include <iosfwd>

namespace dtCore
{

   class DT_CORE_EXPORT ProjectConfigXMLConstants
   {
   public:
      static const char* const SCHEMA_VERSION;

      static XMLCh* PROJECT_CONFIG_ELEMENT;
      static XMLCh* PROJECT_CONFIG_NAMESPACE;
      static XMLCh* HEADER_ELEMENT;
      static XMLCh* NAME_ELEMENT;
      static XMLCh* DESCRIPTION_ELEMENT;
      static XMLCh* AUTHOR_ELEMENT;
      static XMLCh* COMMENT_ELEMENT;
      static XMLCh* COPYRIGHT_ELEMENT;
      static XMLCh* SCHEMA_VERSION_ELEMENT;

      static XMLCh* CONTEXTS_ELEMENT;
      static XMLCh* READ_ONLY_ATTRIBUTE;
      static XMLCh* CONTEXT_ELEMENT;
      static XMLCh* PATH_ATTRIBUTE;

      static void StaticInit();
      static void StaticShutdown();
   private:
      ProjectConfigXMLConstants();
      ProjectConfigXMLConstants(const ProjectConfigXMLConstants&);
      ProjectConfigXMLConstants& operator = (const ProjectConfigXMLConstants&);
   };


   /**
    * Sax parser handler class for loading a project config xml file.
    */
   class DT_CORE_EXPORT ProjectConfigXMLHandler : public BaseXMLHandler
   {
   public:
      class ParsingData
      {
      public:
         ParsingData();

         bool mInHeader;
         bool mInContexts;
      };

      ProjectConfigXMLHandler();
      virtual ~ProjectConfigXMLHandler();

      ProjectConfig& GetProjectConfig();

      virtual void CombinedCharacters(const XMLCh* const chars, size_t length);

      virtual void ElementStarted(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const xercesc::Attributes& attrs);
      virtual void ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

      virtual void Reset();
   private:
      dtCore::RefPtr<ProjectConfig> mConfig;

      ParsingData mData;
   };

   /**
    * @class ProjectConfigXMLWriter
    * @brief Writes a project config out to an XML file
    */
   class DT_CORE_EXPORT ProjectConfigXMLWriter: public BaseXMLWriter
   {
   public:

      /**
       * Constructor.
       */
      ProjectConfigXMLWriter();

      /**
       * Saves the project config to an XML file.
       * The create time will be set on the config if this is the first time it has been saved.
       * @param projConf the ProjectConfig object to save.
       * @param outStr the ostream to write into.
       * @throws ProjectConfigSaveException if any errors occur saving the file.
       */
      void Save(const ProjectConfig& projConf, std::ostream& outStr);


   protected:
      virtual ~ProjectConfigXMLWriter(); ///Protected destructor so that this could be subclassed.

   private:

      //disable copy constructor
      ProjectConfigXMLWriter(const ProjectConfigXMLWriter& toCopy): BaseXMLWriter(toCopy) {}
      //disable operator =
      ProjectConfigXMLWriter& operator=(const ProjectConfigXMLWriter&) { return *this;}
   };

}

#endif /* PROJECTCONFIGXMLHANDLER_H_ */
