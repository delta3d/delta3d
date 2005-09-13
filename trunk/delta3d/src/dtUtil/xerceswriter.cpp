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
      DOMImplementation* impl;
      _CORE = XMLString::transcode("Core");  // xerces example used "Core", no idea if that is necessary.
      impl = DOMImplementationRegistry::getDOMImplementation( _CORE );

      _implementation = impl;
   }

   catch(...)
   {
      ///\todo log the exceptions
      LOG_ERROR("There was a problem creating a Xerces DOMImplementation.")
      _implementation = 0;

      if( _CORE )
      {
         XMLString::release( &_CORE );
      }
      _CORE = 0;
   }
}

XercesWriter::~XercesWriter()
{
   // clean up the _document stuff
   if( _document )
      _document->release();

   // clean up the document's root string
   XMLString::release( &_root_name );

   // clean up the _implementation stuff
   if( _CORE )
      XMLString::release( &_CORE );
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
         XMLString::release( &_root_name );
      }

      try
      {
         _root_name = XMLString::transcode( rootname.c_str() );
         _document = _implementation->createDocument(0,_root_name,0);
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
   if( _implementation )
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
      writer = _implementation->createDOMWriter();
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
      writer->writeNode(xmlstream, *_document);
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
