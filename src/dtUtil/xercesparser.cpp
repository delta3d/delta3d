#include <prefix/dtutilprefix.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/xerceserrorhandler.h>
#include <dtUtil/log.h>

#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/validators/common/Grammar.hpp>

using namespace dtUtil;

XERCES_CPP_NAMESPACE_USE

class XercesParserInit
{
public:
   XercesParserInit() { XercesParser::StaticInit(); }
   ~XercesParserInit() { XercesParser::StaticShutdown(); }
};

static XercesParserInit gInitMe;

////////////////////////////////////////////////////////////////////////////////
XercesParser::XercesParser()
   : mParser(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
XercesParser::~XercesParser()
{
   if (mParser != NULL)
   {
      delete mParser;
      mParser = NULL;
   }
}

void XercesParser::StaticInit()
{
   try
   {
      XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& toCatch)
   {
      //if this happens, something is very very wrong.
      char* message = XMLString::transcode( toCatch.getMessage() );
      std::string msg(message);
      LOG_ERROR("Error during parser initialization!: "+ msg)
         XMLString::release( &message );
      return;
   }
}

void XercesParser::StaticShutdown()
{
   // shutting down xerces causes a crash.
}

////////////////////////////////////////////////////////////////////////////////
bool XercesParser::Parse(const std::string& datafile,
                         XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler& handler,
                         const std::string& schemafile)
{
   std::string filename = dtUtil::FindFileInPathList(datafile);

   if (filename.empty())
   {
      LOG_ERROR("Can't find file: " + datafile);
      return false;
   }

   try  // to inialize the xmlutils
   {
      XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& e)
   {
      char* message = XMLString::transcode(e.getMessage());
      LOG_ERROR(message)
      XMLString::release(&message);
      return false;
   }
   catch (...)
   {
      LOG_ERROR("An exception occurred while trying to initialize Xerces.");
      return false;
   }

   dtUtil::XercesErrorHandler xmlerror;                         // instantiate the error handler

   try // to create a reader
   {
      mParser = XMLReaderFactory::createXMLReader();        // allocate the mParser
      mParser->setContentHandler(&handler);
      mParser->setErrorHandler(&xmlerror);

      if (!schemafile.empty())
      {
         std::string schema = dtUtil::FindFileInPathList(schemafile);

         if (schema.empty())
         {
            LOG_WARNING("Scheme file, " + schemafile + ", not found, check your DELTA_DATA environment variable, schema checking disabled.")
         }
         else // turn on schema checking
         {
            // In some cases, schema will contain a url that is
            // relative to the current working directory which
            // may cause problems with xerces correctly finding it
            schema = osgDB::getRealPath(schema);

            mParser->setFeature(XMLUni::fgXercesSchema, true);                  // enables schema checking.
            mParser->setFeature(XMLUni::fgSAX2CoreValidation, true);            // posts validation errors.
            mParser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);  // does not allow parsing if schema is not fulfilled.
            mParser->loadGrammar(schema.c_str(), Grammar::SchemaGrammarType);
            XMLCh* SCHEMA = XMLString::transcode(schema.c_str());
            mParser->setFeature(XMLUni::fgXercesSchema, true);
            mParser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, SCHEMA);
            XMLString::release(&SCHEMA);
         }
      }
   }
   catch (const XMLException& e)
   {
      char* message = XMLString::transcode(e.getMessage());
      std::string msg(message);
      LOG_ERROR("An exception occurred during XMLReaderFactory::createXMLReader() with message: " + msg);
      XMLString::release(&message);
      return false;
   }
   catch (...)
   {
      LOG_ERROR("Could not create a Xerces SAX2XMLReader")
      return false;
   }

   bool retVal(false);
   try // to parse the file
   {
      LOG_DEBUG("About to parse file: " + filename)
      mParser->parse(filename.c_str());
      LOG_DEBUG("...done parsing file: " + filename)
      retVal = true;
   }
   catch (const XMLException& e)
   {
      char* message = XMLString::transcode(e.getMessage());
      LOG_ERROR(std::string("Exception message is: ") + message)
      XMLString::release(&message);
   }
   catch (const SAXParseException&)
   {
      // problem with the xml parsing.  Not much to do here since the XercesErrorHandler
      // will report it for us.
      retVal = false;
   }

   return retVal;
}

////////////////////////////////////////////////////////////////////////////////
