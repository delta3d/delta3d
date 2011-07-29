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
 * @author Chris Rodgers
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <osgDB/Registry>
#include <dtABC/application.h>
#include <dtCore/basexmlhandler.h>
#include <dtCore/basexmlreaderwriter.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/wrapperosgobject.h>
#include <dtUtil/xercesutils.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
extern dtABC::Application& GetGlobalApplication();



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const dtUtil::RefString ELEMENT_ROOT("root");
const dtUtil::RefString ELEMENT_TITLE("title");
const dtUtil::RefString ELEMENT_TAG1("tag1");
const dtUtil::RefString ELEMENT_TAG2("tag2");



////////////////////////////////////////////////////////////////////////////////
// CUSTOM XML WRITER CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class CustomXMLWriter : public dtCore::BaseXMLWriter
{
public:
   typedef dtCore::BaseXMLWriter BaseClass;

   CustomXMLWriter()
   {
   }

   bool Write(const osg::Object& obj, std::ostream& fout)
   {
      // TODO:

      return true;
   }

protected:
   virtual ~CustomXMLWriter() {}
};



////////////////////////////////////////////////////////////////////////////////
// CUSTOM XML HANDLER CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class CustomXMLHandler : public dtCore::BaseXMLHandler
{
public:
   typedef dtCore::BaseXMLHandler BaseClass;
   typedef std::vector<std::string> StrVector;

   CustomXMLHandler()
   {
   }

   const StrVector& GetValues() const
   {
      return mValues;
   }

   const std::string& GetTitle()
   {
      return mTitle;
   }

   virtual void ElementStarted( const XMLCh* const uri,
      const XMLCh*  const  localname,
      const XMLCh*  const  qname,
      const xercesc::Attributes& attrs)
   {
      mCurElement = dtUtil::XMLStringConverter(localname).ToString();
   }

   virtual void CombinedCharacters(const XMLCh* const chars, size_t length)
   {
      BaseClass::CombinedCharacters(chars, length);

      std::string text = dtUtil::XMLStringConverter(chars).ToString();
      dtUtil::Trim(text);

      if ( ! text.empty())
      {
         if (mCurElement == ELEMENT_TAG2)
         {
            mValues.push_back(text);
         }
         else if (mCurElement == ELEMENT_TITLE)
         {
            mTitle = text;
         }
      }
   }

protected:
   virtual ~CustomXMLHandler() {}

   StrVector mValues;
   std::string mTitle;
   std::string mCurElement;
};



////////////////////////////////////////////////////////////////////////////////
// WRAPPER OBJECT CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class WrapperOSGCustomXMLObject : public dtUtil::WrapperOSGObject
{
public:
   typedef dtUtil::WrapperOSGObject BaseClass;

   WrapperOSGCustomXMLObject()
      : BaseClass()
   {}

   explicit WrapperOSGCustomXMLObject(bool threadSafeRefUnref)
      : BaseClass(threadSafeRefUnref)
   {}

   WrapperOSGCustomXMLObject(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
      : BaseClass(obj, copyop)
   {}

   dtCore::RefPtr<CustomXMLHandler> mHandler;
};



////////////////////////////////////////////////////////////////////////////////
// CUSTOM XML READER WRITER CLASS CODE
////////////////////////////////////////////////////////////////////////////////
static const dtUtil::RefString TEST_EXTENSION("xmltest");

class CustomXMLReaderWriter
   : public dtCore::BaseXMLReaderWriter<osg::Object, CustomXMLHandler, CustomXMLWriter>
{
public:
   typedef dtCore::BaseXMLReaderWriter<osg::Object, CustomXMLHandler, CustomXMLWriter> BaseClass;

   CustomXMLReaderWriter()
      : mCalledInitParser(false)
      , mCalledBuildResult(false)
   {
      supportsExtension(TEST_EXTENSION.Get(),"Test XML (XML)");
   }

   /////////////////////////////////////////////////////////////////////////////
   // OVERRIDE METHODS
   /////////////////////////////////////////////////////////////////////////////
   const char* className() const
   {
      return "Custom XML Reader/Writer"; 
   }

   virtual void InitParser(dtCore::BaseXMLParser& parser, CustomXMLHandler& handler) const
   {
      BaseClass::InitParser(parser, handler);

      mCalledInitParser = true;
   }

   virtual osgDB::ReaderWriter::ReadResult BuildResult(
      const osgDB::ReaderWriter::ReadResult& result, CustomXMLHandler& handler) const
   {
      mCalledBuildResult = true;

      using namespace osgDB;

      ReaderWriter::ReadResult newResult(result);

      if (result.status() == ReaderWriter::ReadResult::FILE_LOADED)
      {
         // Create the wrapper object that will carry the file
         // handler object out of this plug-in.
         dtCore::RefPtr<WrapperOSGCustomXMLObject> obj = new WrapperOSGCustomXMLObject;
         obj->mHandler = dynamic_cast<CustomXMLHandler*>(&handler);

         // Pass the object on the result so that code external
         // to this plug-in can access the data acquired by the
         // contained handler.
         newResult = ReaderWriter::ReadResult(obj.get(), ReaderWriter::ReadResult::FILE_LOADED);
      }

      return newResult;
   }

   /////////////////////////////////////////////////////////////////////////////
   // TEST METHODS
   /////////////////////////////////////////////////////////////////////////////
   bool HasCalledInitParser() const
   {
      return mCalledInitParser;
   }

   bool HasCalledBuildResult() const
   {
      return mCalledBuildResult;
   }

private:
   mutable bool mCalledInitParser;
   mutable bool mCalledBuildResult;
};

REGISTER_OSGPLUGIN(xmltest, CustomXMLReaderWriter)



////////////////////////////////////////////////////////////////////////////////
// TEST FIXTURE CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class BaseXMLTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(BaseXMLTests);
      CPPUNIT_TEST(TestCustomXMLReaderWriter);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();

   void TestCustomXMLReaderWriter();

   void SubTestCustomXMLHandler(CustomXMLHandler& handler);

private:
   CustomXMLReaderWriter* mPlugin;
};

CPPUNIT_TEST_SUITE_REGISTRATION(BaseXMLTests);



////////////////////////////////////////////////////////////////////////////////
void BaseXMLTests::setUp()
{
   // Get the plug-in directly.
   mPlugin = dynamic_cast<CustomXMLReaderWriter*>
      (osgDB::Registry::instance()->getReaderWriterForExtension(TEST_EXTENSION.Get()));
}

////////////////////////////////////////////////////////////////////////////////
void BaseXMLTests::tearDown()
{
   mPlugin = NULL;
}

////////////////////////////////////////////////////////////////////////////////
void BaseXMLTests::TestCustomXMLReaderWriter()
{
   CPPUNIT_ASSERT_MESSAGE("Could not access the Custom XML Reader/Writer directly.", mPlugin != NULL);

   CPPUNIT_ASSERT( ! mPlugin->HasCalledInitParser());
   CPPUNIT_ASSERT( ! mPlugin->HasCalledBuildResult());

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   char sep = dtUtil::FileUtils::PATH_SEPARATOR;
   std::string file = dtUtil::GetDeltaRootPath()
      + sep + "tests"
      + sep + "data"
      + sep + "ProjectContext"
      + sep + "misc"
      + sep + "test_xml.xmltest";

   osg::Object* obj = fileUtils.ReadObject(file);
   CPPUNIT_ASSERT(obj != NULL);

   CPPUNIT_ASSERT(mPlugin->HasCalledInitParser());
   CPPUNIT_ASSERT(mPlugin->HasCalledBuildResult());

   WrapperOSGCustomXMLObject* wrapperObj = dynamic_cast<WrapperOSGCustomXMLObject*>(obj);
   CPPUNIT_ASSERT(wrapperObj != NULL);
   CPPUNIT_ASSERT(wrapperObj->mHandler.valid());

   SubTestCustomXMLHandler(*wrapperObj->mHandler);
}

////////////////////////////////////////////////////////////////////////////////
void BaseXMLTests::SubTestCustomXMLHandler(CustomXMLHandler& handler)
{
   CPPUNIT_ASSERT(handler.GetTitle() == "Test XML");

   const CustomXMLHandler::StrVector& values = handler.GetValues();
   CPPUNIT_ASSERT(values.size() == 4);
   CPPUNIT_ASSERT(values[0] == "Red");
   CPPUNIT_ASSERT(values[1] == "Green");
   CPPUNIT_ASSERT(values[2] == "Blue");
   CPPUNIT_ASSERT(values[3] == "Alpha");
}
