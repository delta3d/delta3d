/**
 * @author ECS, Inc. ( Joseph Del Rocco )
 * @date 2006/07/07 - 11:33
 *
 * @file job_xml.cpp
 * @version 1.0
 * @brief This class wraps the functionality for creating, writing &
 * serializing an XML document.
 */

// local
#include "job_xml.h"
// xerces
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
// ansi
#include <iostream>



//======================================
// PUBLIC FUNCTIONS
//======================================

JobXML::JobXML() : JobSGML(), mDocument(NULL), mDOMImplementor(NULL)
{
}

JobXML::~JobXML()
{ 
}

void JobXML::Execute( PackageProfile *profile )
{
   // do something
}

XMLNode *JobXML::CreateDocument( std::string rootName )
{
   // internal reset
   unload();

   // create xml document
   try
   {
      const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
      mDOMImplementor = DOMImplementationRegistry::getDOMImplementation(gLS); // X("Core")
      XMLCh *name = XMLString::transcode( rootName.c_str() );
      mDocument = mDOMImplementor->createDocument( 0, name, 0 );
      XMLString::release( &name );
   }
   catch ( const OutOfMemoryException &e )
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::cout << "[Error] XML writer error:\n  " << message << std::endl;
      XMLString::release( &message );
      return NULL;
   }
   catch ( const DOMException &e )
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::cout << "[Error] XML writer error:\n  " << message << std::endl;
      XMLString::release( &message );
      return NULL;
   }
   catch (...)
   {      
      std::cout << "[Error] Creating XML document!" << std::endl;
      return NULL;
   }
   
   return mDocument->getDocumentElement();
}

void JobXML::CloseDocument()
{
   unload();
}

void JobXML::SaveToFile( std::string strFile )
{
   // error check filename
   if ( strFile.length() == 0 )
   {      
      std::cout << "[Error] Filename: '" << strFile << "' is not valid." << std::endl; 
      return;
   }

   // locals
   DOMWriter *writer = NULL;

   // create writer
   try
   {
      writer = mDOMImplementor->createDOMWriter();
      if ( writer == NULL ) return;
   }   
   catch ( const OutOfMemoryException &e )
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::cout << "[Error] XML writer error:\n  " << message << std::endl;
      XMLString::release( &message );
      return;
   }
   catch ( const DOMException &e )
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::cout << "[Error] XML writer error:\n  " << message << std::endl;
      XMLString::release( &message );
      return;
   }
   catch (...)
   {      
      std::cout << "[Error] Creating XML document!" << std::endl;
      return;
   }

   // set writer features
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
   XMLCh *newline = XMLString::transcode( "\r\n" );
   writer->setNewLine( newline );
   XMLString::release( &newline );
#else
   XMLCh *newline = XMLString::transcode( "\n" );
   writer->setNewLine( newline );
   XMLString::release( &newline );
#endif
   XMLCh *encoding = XMLString::transcode( "UTF-8" );
   writer->setEncoding( encoding );
   XMLString::release( &encoding );

   // set writer features
   if ( writer->canSetFeature( XMLUni::fgDOMWRTCanonicalForm, true ) )
      writer->setFeature( XMLUni::fgDOMWRTCanonicalForm, true );
   //if ( writer->canSetFeature( XMLUni::fgDOMXMLDeclaration, true ) )
   //   writer->setFeature( XMLUni::fgDOMXMLDeclaration, true );
   if ( writer->canSetFeature( XMLUni::fgDOMWRTSplitCdataSections, true ) )
      writer->setFeature( XMLUni::fgDOMWRTSplitCdataSections, true );
   if ( writer->canSetFeature( XMLUni::fgDOMWRTDiscardDefaultContent, true ) )
      writer->setFeature( XMLUni::fgDOMWRTDiscardDefaultContent, true );
   if ( writer->canSetFeature( XMLUni::fgDOMWRTFormatPrettyPrint, true ) )
      writer->setFeature( XMLUni::fgDOMWRTFormatPrettyPrint, true );
   if ( writer->canSetFeature( XMLUni::fgDOMWRTBOM, true ) )
      writer->setFeature( XMLUni::fgDOMWRTBOM, true ); // byte-order-mark

   /*
   // set filter
   DOMWriterFilter *filter = new DOMWriterFilter(
      DOMNodeFilter::SHOW_ALL       |
      DOMNodeFilter::SHOW_DOCUMENT  |
      DOMNodeFilter::SHOW_ELEMENT   |
      DOMNodeFilter::SHOW_ATTRIBUTE |
      DOMNodeFilter::SHOW_DOCUMENT_TYPE );
   writer->setFilter( filter );
   */

   // is this necessary?
   //mDocument->normalizeDocument();

   // do it
   XMLCh *filename = XMLString::transcode( strFile.c_str() );   
   writer->writeNode( &LocalFileFormatTarget( filename ), (*mDocument) );
   XMLString::release( &filename );

   // cleanup
   writer->release();   
}

XMLNode *JobXML::AddElement( XMLNode *parent, std::string element, std::string content )
{
   // error check
   if ( parent == NULL ) return NULL;

   // create element
   XMLCh *elementName = XMLString::transcode( element.c_str() );   
   DOMElement *elementNode = mDocument->createElement( elementName );
   XMLString::release( &elementName );
   if ( elementNode == NULL )
   {     
      std::cout << "[Error] Creating element: " << element << "." << std::endl;
      return NULL;
   }

   // attach it
   parent->appendChild( elementNode );

   // create content
   if ( content.length() != 0 )
   {
      XMLCh *contentText = XMLString::transcode( content.c_str() );
      DOMText *textNode = mDocument->createTextNode( contentText );
      XMLString::release( &contentText );
      if ( textNode == NULL )
      {
         std::cout << "[Error] Creating content: " << content
            << " for element: " << element << "." << std::endl;         
      }
      else
      {
         elementNode->appendChild( textNode );
      }
   }

   return elementNode;
}

XMLNode *JobXML::AddElement( XMLNode *parent, const XMLNode *element )
{
   // error check
   if ( parent == NULL )  return NULL;
   if ( element == NULL ) return NULL;

   // append this element to the parent and recursively append all children
   // elements including their text content and attributes.
   XMLNode *newElement = deepCopyElement( &parent, element );

   return newElement;
}

void JobXML::AddComment( XMLNode *parent, std::string comment )
{
   // error check
   if ( parent == NULL ) return;

   // create comment
   XMLCh *commentStr = XMLString::transcode( comment.c_str() );
   DOMComment *commentNode = mDocument->createComment( commentStr );
   XMLString::release( &commentStr );
   if ( commentNode == NULL )
   {     
      std::cout << "[Warning] Creating comment: " << comment << "." << std::endl;
      return;
   }

   // attach it
   parent->appendChild( commentNode );
}

void JobXML::SetContent( XMLNode *element, std::string content )
{
   // error check
   if ( element == NULL ) return;
   if ( content.length() == 0 ) return;

   // create content
   XMLCh *contentText = XMLString::transcode( content.c_str() );   
   DOMText *textNode = mDocument->createTextNode( contentText );
   XMLString::release( &contentText );
   if ( textNode == NULL )
   {
      char* name = XMLString::transcode( element->getNodeName() );
      std::cout << "[Error] Creating content: " << content
         << " for element: " << name << "." << std::endl;
      XMLString::release( &name );      
      return;
   }

   // attach it
   element->appendChild( textNode );
}

void JobXML::SetAttribute( XMLNode *element, std::string attribute, std::string value )
{
   // error check
   if ( element == NULL ) return;
   if ( element->getNodeType() != XMLNode::ELEMENT_NODE ) return;
   if ( attribute.length() == 0 ) return;

   // do it
   XMLCh *attrText  = XMLString::transcode( attribute.c_str() );
   XMLCh *valueText = XMLString::transcode( value.c_str() );
   (static_cast<DOMElement*>(element))->setAttribute( attrText, valueText );
   XMLString::release( &attrText );
   XMLString::release( &valueText );
}


//======================================
// PRIVATE FUNCTIONS
//======================================

void JobXML::unload()
{
   //if ( mDocument != NULL )
   //{
      //mDocument->release();
      //delete mDocument;
      //mDocument = NULL;
   //}
}

XMLNode *JobXML::deepCopyElement( XMLNode **copy, const XMLNode *original )
{
   // add the element to the copy
   char *nodeName = XMLString::transcode( original->getNodeName() );
   XMLNode *newElement = AddElement( (*copy), nodeName );
   XMLString::release( &nodeName );

   // add its attributes
   if ( original->hasAttributes() )
   {
      DOMNamedNodeMap *attributes = original->getAttributes();
      for( unsigned int i=0; i<attributes->getLength(); i++ )
      {
         DOMAttr *attr = (DOMAttr*)attributes->item( i );
         char *key = XMLString::transcode( attr->getName() );
         char *val = XMLString::transcode( attr->getValue() );
         SetAttribute( newElement, key, val );
         XMLString::release( &key );
         XMLString::release( &val );
      }
   }

   // recursively copy all child elements
   int childElementCount=0;
   XMLNode *itr = original->getFirstChild();
   for ( ; itr != NULL; itr = itr->getNextSibling() )
   {      
      if ( itr->getNodeType() == XMLNode::ELEMENT_NODE )
      {
         deepCopyElement( &newElement, itr );
         childElementCount++;
      }
   }

   // if no child elements, copy this element's text content and we are done
   if ( childElementCount == 0 )
   {
      char *content = XMLString::transcode( original->getTextContent() );
      SetContent( newElement, content );
      XMLString::release( &content );
   }

   return newElement;
}
