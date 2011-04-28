/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * @author Roy Newton
 */

#include <prefix/unittestprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/datapathutils.h>
#include <cppunit/extensions/HelperMacros.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <map>

XERCES_CPP_NAMESPACE_USE

/**
 * @class XercesTests
 * @brief Unit tests for xercesparser,
 */
class XercesTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(XercesTests);
   CPPUNIT_TEST(TestXercesParser);
   CPPUNIT_TEST(TestFindAttributeValueFor);
   CPPUNIT_TEST(TestXercesWriter);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      /**
       * Tests parsing the test xml file.  Also tests AttributeSearch since we have the Attributes handy.
       */
      void TestXercesParser();

      /**
       * Tests searching for values in the test xml file
       */
      void TestFindAttributeValueFor();

      /**
       * Tests writing an XML file
       */
      void TestXercesWriter();

      static void doAttributeSearch(const Attributes& attrs, std::string searchKey);

   private:
      dtUtil::Log* mLogger;
      dtUtil::XercesParser mDTUTILParser;

      class TestContentHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
      {
      public:
         TestContentHandler(){mGotTopLevel=false; mGotElem1=false; mGotElem2=false;};
         ~TestContentHandler(){};

         // inherited pure virtual functions
#if XERCES_VERSION_MAJOR < 3 
         virtual void characters(const XMLCh* const chars, const unsigned int length) {}
         virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
#else
         virtual void characters(const XMLCh* const chars, const XMLSize_t length) {}
         virtual void ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {}
#endif
         virtual void endDocument() {}
         virtual void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname) {}
         virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
         virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
         virtual void startDocument() {}
         virtual void startElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);
         virtual void startPrefixMapping(const XMLCh* const prefix,const XMLCh* const uri) {}
         virtual void endPrefixMapping(const XMLCh* const prefix) {}
         virtual void skippedEntity(const XMLCh* const name) {}

         bool mGotTopLevel;
         bool mGotElem1;
         bool mGotElem2;
      };

      TestContentHandler mTestContentHandler;

      std::string mXMLFileName;
      std::string mSchemaFileName;

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(XercesTests);

void XercesTests::TestContentHandler::startElement(const XMLCh* const uri,
                                                   const XMLCh* const localname,
                                                   const XMLCh* const qname,
                                                   const Attributes& attrs)
{
   if (strcmp(XMLString::transcode(localname), "TopLevel") == 0)
   {
      mGotTopLevel = true;
   }
   else if (strcmp(XMLString::transcode(localname), "Elem1") == 0)
   {
      mGotElem1 = true;
      doAttributeSearch(attrs, "Elem1Attr1");
   }
   else if (strcmp(XMLString::transcode(localname), "Elem2") == 0)
   {
      mGotElem2 = true;
      doAttributeSearch(attrs, "Elem2Attr1");
   }
}


///////////////////////////////////////////////////////////////////////////////
void XercesTests::setUp()
{
   try
   {

      mLogger = &dtUtil::Log::GetInstance();

      try  // to inialize the xmlutils
      {
         XMLPlatformUtils::Initialize();
      }
      catch (const XMLException& e)
      {
         char* message = XMLString::transcode(e.getMessage());
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, message );
         XMLString::release( &message );
         return;
      }
      catch (...)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "An exception occurred while trying to initialize Xerces.");
         return;
      }

      std::string root = dtUtil::GetDeltaRootPath();
      if (root.size() == 0)
      {
         CPPUNIT_FAIL("DELTA_ROOT is not set");
      }
      mSchemaFileName = root + "/tests/dtUtil/testxsd.xsd";
      mXMLFileName = root + "/tests/dtUtil/testxml.xml";
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
   CPPUNIT_ASSERT(mLogger != NULL);
}

///////////////////////////////////////////////////////////////////////////////
void XercesTests::tearDown()
{
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void XercesTests::TestXercesParser()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Xerces Parser.\n");
      CPPUNIT_ASSERT(!mDTUTILParser.Parse("", mTestContentHandler, ""));
      CPPUNIT_ASSERT(mDTUTILParser.Parse(mXMLFileName, mTestContentHandler, mSchemaFileName));
      CPPUNIT_ASSERT_MESSAGE("Failed to get TopLevel.", mTestContentHandler.mGotTopLevel);
      CPPUNIT_ASSERT_MESSAGE("Failed to get Elem1.", mTestContentHandler.mGotElem1);
      CPPUNIT_ASSERT_MESSAGE("Failed to get Elem2.", mTestContentHandler.mGotElem2);
   }

   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////
void XercesTests::TestFindAttributeValueFor()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing TestFindAttributeValueFor.\n");
      std::string result;
      XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser mXercesParser;
      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* mDoc;
      mXercesParser.parse( mXMLFileName.c_str() );
      mDoc = mXercesParser.getDocument();

      result = dtUtil::FindAttributeValueFor("", NULL);
      CPPUNIT_ASSERT_MESSAGE("result should be empty.", result == "");

      result = dtUtil::FindAttributeValueFor("", mDoc->getFirstChild()->getAttributes());
      CPPUNIT_ASSERT_MESSAGE("result should be empty.", result == "");

      DOMNode* root = mDoc->getFirstChild();
      XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList* list = root->getChildNodes();
      for (unsigned int i=0; i<list->getLength(); i++)
      {
         char* nodeName = XMLString::transcode(list->item(i)->getNodeName());
         if (strcmp(nodeName, "Elem1") == 0)
         {
            result = dtUtil::FindAttributeValueFor("Elem1Attr1", list->item(i)->getAttributes());
            CPPUNIT_ASSERT_MESSAGE("result should be Elem1Attr1Value.", result == "Elem1Attr1Value");

            result = dtUtil::FindAttributeValueFor("elem1attr1", list->item(i)->getAttributes());
            CPPUNIT_ASSERT_MESSAGE("result should be empty.", result == "");
         }
         else if (strcmp(nodeName, "Elem2") == 0)
         {
            result = dtUtil::FindAttributeValueFor("Elem2Attr1", list->item(i)->getAttributes());
            CPPUNIT_ASSERT_MESSAGE("result should be Elem2Attr1Value.", result == "Elem2Attr1Value");

            result = dtUtil::FindAttributeValueFor("elem2attr1", list->item(i)->getAttributes());
            CPPUNIT_ASSERT_MESSAGE("result should be empty.", result == "");
         }
      }

   }

   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////
void XercesTests::TestXercesWriter()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing XercesWriter.\n");

      dtUtil::XercesWriter* writer3 = new dtUtil::XercesWriter();
      writer3->CreateDocument( "TopLevel" );
      writer3->WriteFile( "testxerceswriter3.xml" );

      dtUtil::XercesWriter* writer4 = new dtUtil::XercesWriter();
      writer4->CreateDocument( "TopLevel" );
      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc4 = writer4->GetDocument();
      DOMElement* toplevel4 = doc4->getDocumentElement();
      DOMElement* elem4 = doc4->createElement(XMLString::transcode("elem4"));
      toplevel4->appendChild(elem4);
      writer4->WriteFile( "testxerceswriter4.xml" );

      dtUtil::XercesWriter* writer5 = new dtUtil::XercesWriter();
      writer5->CreateDocument( "TopLevel" );
      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc5 = writer5->GetDocument();
      DOMElement* toplevel5 = doc5->getDocumentElement();
      DOMElement* elem5 = doc5->createElement(XMLString::transcode("elem5"));
      elem5->setAttribute( XMLString::transcode("name"), XMLString::transcode("value") );
      toplevel5->appendChild(elem5);
      writer5->WriteFile( "testxerceswriter5.xml" );

      remove("testxerceswriter2.xml");
      remove("testxerceswriter3.xml");
      remove("testxerceswriter4.xml");
      remove("testxerceswriter5.xml");
   }

   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
}


void XercesTests::doAttributeSearch(const Attributes& attrs, std::string searchKey)
{
   try
   {
      dtUtil::AttributeSearch   attrSearch;
      //attrSearch.GetSearchKeys().push_back(searchKey);
      dtUtil::AttributeSearch::ResultMap results =   attrSearch(attrs);

      dtUtil::AttributeSearch::ResultMap::iterator   iter = results.find(searchKey);
      if (iter   !=   results.end())
      {
         std::string   left((*iter).second);
         std::string right = searchKey + "Value";
         CPPUNIT_ASSERT_MESSAGE("Did find   Elem1Attr1 in AttributeSearch.\n", left == right);
         CPPUNIT_ASSERT_MESSAGE("Size should   be   1.\n", results.size() == 1);
      }
   }

   catch   (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") +   e.What()).c_str());
   }
   catch   (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") +   e.what()).c_str());
   }
}
