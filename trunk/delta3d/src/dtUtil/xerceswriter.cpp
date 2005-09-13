#include "dtUtil/xerceswriter.h"
#include "dtUtil/log.h"

#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMWriter.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
//#include <xercesc/dom/DomException.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace dtUtil;

XercesWriter::XercesWriter(): mImplementation(0), mDocument(0), mCORE(0), mRootName(0)
{
   try  // to initialize the xml tools
   {
      XMLPlatformUtils::Initialize();
   }
   catch(const XMLException& e)
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::string msg(message);
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize() with message: " + msg);
      XMLString::release( &message );
   }
   catch(...)
   {
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize()");
      return;
   }

   try
   {
      DOMImplementation* impl;
      mCORE = XMLString::transcode("Core");  // xerces example used "Core", no idea if that is necessary.
      impl = DOMImplementationRegistry::getDOMImplementation( mCORE );

      mImplementation = impl;
   }

   catch(...)
   {
      ///\todo log the exceptions
      LOG_ERROR("There was a problem creating a Xerces DOMImplementation.")
      mImplementation = 0;

      if( mCORE )
      {
         XMLString::release( &mCORE );
      }
      mCORE = 0;
   }
}

XercesWriter::~XercesWriter()
{
   // clean up the _document stuff
   if( mDocument )
      mDocument->release();

   // clean up the document's root string
   XMLString::release( &mRootName );

   // clean up the _implementation stuff
   if( mCORE )
      XMLString::release( &mCORE );
}

void XercesWriter::CreateDocument(const std::string& rootname)
{
   if( mImplementation )
   {
      if( mDocument )
      {  // blow away the old document
         mDocument->release();
      }

      if( mRootName )
      {
         XMLString::release( &mRootName );
      }

      try
      {
         mRootName = XMLString::transcode( rootname.c_str() );
         mDocument = mImplementation->createDocument(0,mRootName,0);
      }
      catch(...)
      {
         LOG_ERROR("There was a problem creating a new Xerces DOMDocument.")
      }
   }

   else
   {
      LOG_ERROR("Could not create the XML document because the requested Xerces implementation is not valid.");
   }
}

void XercesWriter::WriteFile(const std::string& outputfile)
{
   if( mImplementation )
   {  // log it?
   }
   else
   {
      LOG_ERROR("Can not write file, "+ outputfile +", because the requested Xerces implementation is not valid.");
      return;
   }

   // make a writer
   DOMWriter* writer;
   LocalFileFormatTarget* xmlstream;
   XMLCh* OUTPUT;
   try
   {
      OUTPUT = XMLString::transcode( outputfile.c_str() );
      xmlstream = new LocalFileFormatTarget( OUTPUT );
      writer = mImplementation->createDOMWriter();
   }
   catch(...)
   {
      if( OUTPUT )
         XMLString::release( &OUTPUT );
      LOG_ERROR("Can not write file, "+ outputfile +", because creation of a writing tools failed.");
      return;
   }

   // turn on pretty print
   if( writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint,true) )
   {
      writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint,true);
   }

   // write it!
   try
   {
      writer->writeNode(xmlstream, *mDocument);
   }
   catch (const OutOfMemoryException&)
   {
      //XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
      LOG_ERROR("When writing file," +outputfile+ ", an OutOfMemoryException occurred.");
   }
//    catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException&)
//    {
//       //XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
//       LOG_ERROR("When writing file, " +outputfile+ ", a DOMException occurred");
//    }
   catch (...)
   {
      //XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
      LOG_ERROR("When writing file," +outputfile+ ", an exception occurred.");
   }
}
