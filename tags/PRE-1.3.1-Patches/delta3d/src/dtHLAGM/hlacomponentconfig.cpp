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

#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>

#include "dtHLAGM/hlacomponentconfig.h"
#include "dtHLAGM/exceptionenum.h"
#include "dtHLAGM/hlacomponent.h"
#include "dtHLAGM/objecttoactor.h"
#include "dtHLAGM/attributetoproperty.h"
#include "dtHLAGM/parametertoparameter.h"
#include "dtHLAGM/interactiontomessage.h"
#include "dtHLAGM/distypes.h"
#include "dtHLAGM/hlafomconfigxml.h"

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>

namespace dtHLAGM
{
   static const std::string logName("hlafomtranslatorconfig.cpp");

   HLAComponentConfig::HLAComponentConfig() throw (dtUtil::Exception):
      mHandler(new HLAFOMConfigContentHandler)
   {
      try
      {
         xercesc_dt::XMLPlatformUtils::Initialize();
      }
      catch (const xercesc_dt::XMLException& toCatch)
      {
         //if this happens, something is very very wrong.
         char* message = xercesc_dt::XMLString::transcode( toCatch.getMessage() );
         std::string msg(message);
         LOG_ERROR("Error during parser initialization!: "+ msg)
         xercesc_dt::XMLString::release( &message );
         EXCEPT(dtHLAGM::ExceptionEnum::XML_INTERNAL_EXCEPTION, "Error, unable to initialize Xerces XML parser.  Aborting.");
      }

      mLogger = &dtUtil::Log::GetInstance(logName);

      mXercesParser = xercesc_dt::XMLReaderFactory::createXMLReader();

      mXercesParser->setFeature(xercesc_dt::XMLUni::fgSAX2CoreValidation, true);
      mXercesParser->setFeature(xercesc_dt::XMLUni::fgXercesDynamic, false);

      mXercesParser->setFeature(xercesc_dt::XMLUni::fgSAX2CoreNameSpaces, true);
      mXercesParser->setFeature(xercesc_dt::XMLUni::fgXercesSchema, true);
      mXercesParser->setFeature(xercesc_dt::XMLUni::fgXercesSchemaFullChecking, true);
      mXercesParser->setFeature(xercesc_dt::XMLUni::fgXercesValidationErrorAsFatal, true); 
      mXercesParser->setFeature(xercesc_dt::XMLUni::fgSAX2CoreNameSpacePrefixes, true);
      mXercesParser->setFeature(xercesc_dt::XMLUni::fgXercesUseCachedGrammarInParse, true);
      mXercesParser->setFeature(xercesc_dt::XMLUni::fgXercesCacheGrammarFromParse, true);

      std::string schemaFileName = osgDB::findDataFile("Federations/HLAMapping.xsd");

      if (!dtUtil::FileUtils::GetInstance().FileExists(schemaFileName))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
             "Error, unable to load required file \"Federations/HLAMapping.xsd\".  Aborting.");
         EXCEPT(dtHLAGM::ExceptionEnum::XML_CONFIG_EXCEPTION, "Error, unable to load required file \"Federations/HLAMapping.xsd\".  Aborting.");
      }

      XMLCh* schemaFileNameXMLCh = xercesc_dt::XMLString::transcode(schemaFileName.c_str());
      xercesc_dt::LocalFileInputSource inputSource(schemaFileNameXMLCh);
      //cache the schema
      mXercesParser->loadGrammar(inputSource, xercesc_dt::Grammar::SchemaGrammarType, true);
      xercesc_dt::XMLString::release(&schemaFileNameXMLCh);
   }
   
   HLAComponentConfig::~HLAComponentConfig()
   {
      delete mXercesParser;
   }
   
   void HLAComponentConfig::LoadConfiguration(HLAComponent& translator, const std::string& dataFilePath) throw (dtUtil::Exception)
   {
        try
        {
            std::string path = osgDB::findDataFile(dataFilePath);
            if (!dtUtil::FileUtils::GetInstance().FileExists(path))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                   "XML configuration file \"%s\" not found.  Aborting.", dataFilePath.c_str());
               EXCEPT(dtHLAGM::ExceptionEnum::XML_CONFIG_EXCEPTION, "Error, unable to load required file \"" + dataFilePath  +  "\".  Aborting.");
            }
            
            if (translator.GetGameManager() == NULL)
               EXCEPT(dtHLAGM::ExceptionEnum::XML_CONFIG_EXCEPTION, "Translators must be associated with a game manager before.  Aborting.");               
            
            mHandler->SetTargetTranslator(translator);
            mXercesParser->setContentHandler(mHandler.get());
            mXercesParser->setErrorHandler(mHandler.get());
            mXercesParser->parse(path.c_str());
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.\n");
        }
        catch (const xercesc_dt::OutOfMemoryException&)
        {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
            EXCEPT(dtHLAGM::ExceptionEnum::XML_INTERNAL_EXCEPTION, "Ran out of memory parsing save file.");
        }
        catch (const xercesc_dt::XMLException& toCatch)
        {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                toCatch.getMessage());
            EXCEPT(dtHLAGM::ExceptionEnum::XML_INTERNAL_EXCEPTION, "Error while parsing hla config file. See log for more information.");
        }
        catch (const xercesc_dt::SAXParseException&)
        {
            //this will already by logged by the content handler
            EXCEPT(dtHLAGM::ExceptionEnum::XML_INTERNAL_EXCEPTION, "Error while parsing hla config file. See log for more information.");
        }
   }


}
