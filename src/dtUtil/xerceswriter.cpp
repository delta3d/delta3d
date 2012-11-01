#include <prefix/dtutilprefix.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/log.h>

#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#if XERCES_VERSION_MAJOR >= 3
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#else
#include <xercesc/dom/DOMWriter.hpp>
#endif
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

XERCES_CPP_NAMESPACE_USE
using namespace dtUtil;

////////////////////////////////////////////////////////////////////////////////
XercesWriter::XercesWriter()
   : mImplementation(0)
   , mDocument(0)
{
   try // to initialize the xml tools
   {
      XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& e)
   {
      char* message = XMLString::transcode(e.getMessage());
      std::string msg(message);
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize() with message: " + msg);
      XMLString::release(&message);
   }
   catch (...)
   {
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize()");
      return;
   }

   try
   {
      DOMImplementation* impl;
      XMLCh* CORE = XMLString::transcode("Core");  // xerces example used "Core", no idea if that is necessary.
      impl = DOMImplementationRegistry::getDOMImplementation(CORE);
      XMLString::release(&CORE);

      mImplementation = impl;
   }
   catch (...)
   {
      LOG_ERROR("There was a problem creating a Xerces DOMImplementation.")
      mImplementation = 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
XercesWriter::~XercesWriter()
{
   // clean up the _document stuff
   if (mDocument)
   {
      mDocument->release();
   }
}

////////////////////////////////////////////////////////////////////////////////
void XercesWriter::CreateDocument(const std::string& rootname)
{
   if (mImplementation)
   {
      if (mDocument)
      {
         // blow away the old document
         mDocument->release();
      }

      try
      {
         XMLCh* ROOTNAME = XMLString::transcode(rootname.c_str());
         mDocument = mImplementation->createDocument(0, ROOTNAME, 0);
         XMLString::release(&ROOTNAME);
      }
      catch (...)
      {
         LOG_ERROR("There was a problem creating a new Xerces DOMDocument.")
      }
   }
   else
   {
      LOG_ERROR("Could not create the XML document because the requested Xerces implementation is not valid.");
   }
}

////////////////////////////////////////////////////////////////////////////////
void XercesWriter::WriteFile(const std::string& outputfile)
{
   if (mImplementation)
   {
      // log it?
   }
   else
   {
      LOG_ERROR("Can not write file, " + outputfile + ", because the requested Xerces implementation is not valid.");
      return;
   }

#if XERCES_VERSION_MAJOR >= 3
   DOMLSSerializer* writer;
   DOMLSOutput* xmlstream;
   XMLCh* OUTPUT;
   try
   {
      OUTPUT = XMLString::transcode( outputfile.c_str() );
      xmlstream = mImplementation->createLSOutput();
      xmlstream->setSystemId( OUTPUT );
      writer = mImplementation->createLSSerializer();
   }
   catch(...)
   {
      if( OUTPUT )
         XMLString::release( &OUTPUT );
      LOG_ERROR("Can not write file, "+ outputfile +", because creation of a writing tools failed.");
      return;
   }

   // turn on pretty print
   DOMConfiguration* dc = writer->getDomConfig();
   dc->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint,true);

   // write it!
   try
   {
      if( !writer->write(mDocument, xmlstream) )
      {
         LOG_ERROR("There was a problem writing file, " + outputfile)
      }

      writer->release();
      xmlstream->release();
   }
   catch (const OutOfMemoryException& e)
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::string msg( message );
      XMLString::release( &message );
      LOG_ERROR("When writing file," +outputfile+ ", an OutOfMemoryException occurred." + msg);
      delete xmlstream;
   }
//    catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException&)
//    {
//       //XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
//       LOG_ERROR("When writing file, " +outputfile+ ", a DOMException occurred");
//    }
   catch (...)
   {
      LOG_ERROR("When writing file," +outputfile+ ", an exception occurred.");
      delete xmlstream;
   }
#else
   // make a writer
   DOMWriter* writer;
   LocalFileFormatTarget* xmlstream;
   XMLCh* OUTPUT;
   try
   {
      OUTPUT = XMLString::transcode(outputfile.c_str());
      xmlstream = new LocalFileFormatTarget(OUTPUT);
      writer = mImplementation->createDOMWriter();
   }
   catch (...)
   {
      if (OUTPUT)
      {
         XMLString::release(&OUTPUT);
      }
      LOG_ERROR("Can not write file, " + outputfile + ", because creation of a writing tools failed.");
      return;
   }

   // turn on pretty print
   if (writer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
   {
      writer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
   }

   // write it!
   try
   {
      if (!writer->writeNode(xmlstream, *mDocument))
      {
         LOG_ERROR("There was a problem writing file, " + outputfile)
      }

      writer->release();
      delete xmlstream;
   }
   catch (const OutOfMemoryException& e)
   {
      char* message = XMLString::transcode(e.getMessage());
      std::string msg(message);
      XMLString::release(&message);
      LOG_ERROR("When writing file," + outputfile + ", an OutOfMemoryException occurred." + msg);
      delete xmlstream;
   }
//    catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException&)
//    {
//       //XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
//       LOG_ERROR("When writing file, " +outputfile+ ", a DOMException occurred");
//    }
   catch (...)
   {
      LOG_ERROR("When writing file," + outputfile + ", an exception occurred.");
      delete xmlstream;
   }
#endif
}
