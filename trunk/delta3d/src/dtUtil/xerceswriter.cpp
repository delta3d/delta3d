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

XercesWriter::XercesWriter(): _implementation(0), _document(0), _CORE(0), _root_name(0)
{
   try
   {
      XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
   }

   catch(const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& /*toCatch*/)
   {
      /// \todo log this

      //char *pMsg = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage());
      //XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
      //      << "  Exception message:"
      //      << pMsg;
      //XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&pMsg);
   }

   try
   {
      XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* impl;
      _CORE = ConvertToTranscode("Core");  // xerces example used "Core", no idea if that is necessary.
      impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation( _CORE );

      _implementation = impl;
   }

   catch(...)
   {
      ///\todo log the exceptions
      _implementation = 0;

      ReleaseTranscode( _CORE );
      _CORE = 0;
   }
}

XercesWriter::~XercesWriter()
{
   // clean up the _document stuff
   _document->release();

   // clean up the document's root string
   ReleaseTranscode( _root_name );

   // clean up the _implementation stuff
   if( _CORE )
      ReleaseTranscode( _CORE );

   // shutdown the system
   XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();
}

XMLCh* XercesWriter::ConvertToTranscode(const char* str)
{
   return XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(str);
}

void XercesWriter::ReleaseTranscode(XMLCh* str)
{
   XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&str);
}

void XercesWriter::CreateDocument(const std::string& rootname)
{
   if( _implementation )
   {
      if( _document )
      {  // blow away the old document
         _document->release();
      }

      if( _root_name )
      {
         ReleaseTranscode( _root_name );
      }

      try
      {
         _root_name = ConvertToTranscode( rootname.c_str() );
         _document = _implementation->createDocument(0,_root_name,0);
      }
      catch(...)
      {
         ///\todo log something
      }
   }

   else
   {
      LOG_ERROR("Could not create the XML document because the requested Xerces implementation is not valid.");
   }
}

///\todo use <i>more</i> exceptions here.
void XercesWriter::WriteFile(const std::string& outputfile)
{
   if( _implementation )
   {  // log it?
   }
   else
   {
      LOG_ERROR("Can not write file, "+ outputfile +", because the requested Xerces implementation is not valid.");
      return;
   }

   // make a writer
   XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* writer;
   XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget* xmlstream;
   try
   {
      xmlstream = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget( ConvertToTranscode(outputfile.c_str()) );
      writer = _implementation->createDOMWriter();
   }
   catch(...)
   {
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
      writer->writeNode(xmlstream, *_document);
   }
   catch (const XERCES_CPP_NAMESPACE_QUALIFIER OutOfMemoryException&)
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
