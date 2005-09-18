#include <dtUtil/xercesparser.h>
#include <dtUtil/xerceserrorhandler.h>
#include <dtUtil/log.h>

#include <osgDB/FileUtils>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

using namespace dtUtil;

XERCES_CPP_NAMESPACE_USE

XercesParser::XercesParser()
{
}

XercesParser::~XercesParser()
{
}

bool XercesParser::Parse(const std::string& datafile, ContentHandler& handler, const std::string& schemafile)
{
   std::string filename = osgDB::findDataFile( datafile );
   if( filename.empty() )
      return false;

   bool retVal(false);
   try  // to inialize the xmlutils
   {
      XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& e) 
   {
      char* message = XMLString::transcode(e.getMessage());
      LOG_ERROR( message )
      XMLString::release( &message );
      return false;
   }
   catch(...)
   {
      LOG_ERROR("An exception occurred while trying to initialize Xerces.");
      return false;
   }

   dtUtil::XercesErrorHandler xmlerror;                         // instantiate the error handler
   SAX2XMLReader* parser;   // declare the parser
   try  // to create a reader
   {
      parser = XMLReaderFactory::createXMLReader();        // allocate the parser
      parser->setContentHandler( &handler );
      parser->setErrorHandler( &xmlerror );

      std::string schemafile = osgDB::findDataFile( schemafile );
      if( schemafile.empty() )
      {
         LOG_WARNING("Scheme file not found, check your DELTA_DATA environment variable, schema checking disabled.")
      }
      else   // turn on schema checking
      {
         ///\todo does the sax2parser support checking for features?
         parser->setFeature(XMLUni::fgXercesSchema, true);                  // enables schema checking.
         parser->setFeature(XMLUni::fgSAX2CoreValidation, true);            // posts validation errors.
         parser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);  // does not allow parsing if schema is not fulfilled.
         parser->loadGrammar( schemafile.c_str(), Grammar::SchemaGrammarType );
         XMLCh* SCHEMA = XMLString::transcode( schemafile.c_str() );
         parser->setFeature(XMLUni::fgXercesSchema, true);
         parser->setProperty( XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, SCHEMA );
         XMLString::release( &SCHEMA );
      }
   }
   catch(const XMLException& e)
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::string msg(message);
      LOG_ERROR("An exception occurred during XMLReaderFactory::createXMLReader() with message: " + msg);
      XMLString::release( &message );
      return false;
   }
   catch(...)
   {
      LOG_ERROR("Could not create a Xerces SAX2XMLReader")
      return false;
   }

   try  // to parse the file
   {
      parser->parse(filename.c_str());
      retVal = true;
   }
   catch (const XMLException& e)
   {
      char* message = XMLString::transcode(e.getMessage());
      LOG_ERROR(std::string("Exception message is: ") + message)
      XMLString::release(&message);

      delete parser;
      return false;
   }
   catch (const SAXParseException& e)
   {
      //The error will already be logged by the errorHandler
      char* message = XMLString::transcode(e.getMessage());
      LOG_ERROR(std::string("An exception occurred while parsing file, ") + filename + std::string(", with message: ") + message)
      XMLString::release(&message);

      delete parser;
      return false;
   }
   catch (...) 
   {
      LOG_ERROR("An exception occurred while parsing file, " + filename)

      delete parser;
      return false;
   }

   delete parser;
   return retVal;
}
