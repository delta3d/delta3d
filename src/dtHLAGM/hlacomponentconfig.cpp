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

#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>

#include <dtCore/actorproperty.h>

#include <dtHLAGM/hlacomponentconfig.h>
#include <dtHLAGM/exceptionenum.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlafomconfigxml.h>
#include <dtHLAGM/ddmregioncalculator.h>

#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#if XERCES_VERSION_MAJOR < 3
#include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#endif

namespace dtHLAGM
{
   const std::string HLAComponentConfig::LOG_NAME("hlafomtranslatorconfig.cpp");

   HLAComponentConfig::HLAComponentConfig():
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
         throw dtHLAGM::XmlInternalException(
            "Error, unable to initialize Xerces XML parser.  Aborting.", __FILE__, __LINE__);
      }

      mLogger = &dtUtil::Log::GetInstance(LOG_NAME);

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

      std::string schemaFileName = dtUtil::FindFileInPathList("Federations/HLAMapping.xsd");

      if (!dtUtil::FileUtils::GetInstance().FileExists(schemaFileName))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
             "Error, unable to load required file \"Federations/HLAMapping.xsd\".  Aborting.");
         throw dtHLAGM::XmlConfigException(
            "Error, unable to load required file \"Federations/HLAMapping.xsd\".  Aborting.", __FILE__, __LINE__);
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

   void HLAComponentConfig::LoadConfiguration(HLAComponent& translator, const std::string& dataFilePath)
   {
        try
        {
            std::string path = dtUtil::FindFileInPathList(dataFilePath);
            if (!dtUtil::FileUtils::GetInstance().FileExists(path))
            {
               mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__,
                   "XML configuration file \"%s\" not found.  Aborting.", dataFilePath.c_str());
               throw dtHLAGM::XmlConfigException( "Error, unable to load required file \"" + dataFilePath  +  "\".  Aborting.", __FILE__, __LINE__);
            }

            if (translator.GetGameManager() == NULL)
               throw dtHLAGM::XmlConfigException( "Translators must be associated with a game manager before.  Aborting.", __FILE__, __LINE__);

            mHandler->SetTargetTranslator(translator);
            mXercesParser->setContentHandler(mHandler.get());
            mXercesParser->setErrorHandler(mHandler.get());
            mXercesParser->parse(path.c_str());
            mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.\n");
        }
        catch (const xercesc_dt::OutOfMemoryException&)
        {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
            throw dtHLAGM::XmlInternalException( "Ran out of memory parsing save file.", __FILE__, __LINE__);
        }
        catch (const xercesc_dt::XMLException& toCatch)
        {
            mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %ls :\n",
                toCatch.getMessage());
            throw dtHLAGM::XmlInternalException( "Error while parsing hla config file. See log for more information.", __FILE__, __LINE__);
        }
        catch (const xercesc_dt::SAXParseException&)
        {
            //this will already by logged by the content handler
            throw dtHLAGM::XmlInternalException( "Error while parsing hla config file. See log for more information.", __FILE__, __LINE__);
        }
   }


}
