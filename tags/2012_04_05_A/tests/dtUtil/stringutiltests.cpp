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
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/stringutils.h>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>

/**
 * @class StringUtilTests
 * @brief Unit tests for the string utils class
 */
class StringUtilTests : public CPPUNIT_NS::TestFixture {
   CPPUNIT_TEST_SUITE(StringUtilTests);
   CPPUNIT_TEST(TestTrim);
   CPPUNIT_TEST(TestStrCompare);
   CPPUNIT_TEST(TestToString);
   CPPUNIT_TEST(TestToType);
   CPPUNIT_TEST(TestMatch);
   CPPUNIT_TEST(TestTokenizer);
   CPPUNIT_TEST(TestParseVec);
   CPPUNIT_TEST(TestToPrimitives);
   CPPUNIT_TEST(TestMakeIndexString);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      /**
       * Tests Trim function
       */
      void TestTrim();

      /**
       * Tests Trim function
       */
      void TestStrCompare();

      /**
       * Tests ToString with all the basic types
       */
      void TestToString();

      /**
       * Tests ToType for the basic types
       */
      void TestToType();

      /**
       * Tests Match function
       */
      void TestMatch();

      /**
       * Tests StringTokenizer
       */
      void TestTokenizer();

      /**
       * Tests ParseVec
       */
      void TestParseVec();

      /**
       * Tests ToFloat, ToUnsignedInt, ToDouble
       */
      void TestToPrimitives();

      void TestMakeIndexString();

   private:
      dtUtil::Log* mLogger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(StringUtilTests);

///////////////////////////////////////////////////////////////////////////////
void StringUtilTests::setUp()
{
   try
   {
      mLogger = &dtUtil::Log::GetInstance();
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
void StringUtilTests::tearDown()
{
   mLogger = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void StringUtilTests::TestTrim()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Trim.\n");
      std::string testString = "";
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 0", testString.size() == 0);

      testString = "1";
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 1", testString.size() == 1);

      testString = " 1";//<space>char
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 1", testString.size() == 1);

      testString = "1 ";//char<space>
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 1", testString.size() == 1);

      testString = " 1 ";//<space>char<space>
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 1", testString.size() == 1);

      testString = "\t1";  //<tab>char
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 1", testString.size() == 1);

      testString = "1\t"; //char<tab>
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 1", testString.size() == 1);

      testString = "\t1\t"; //<tab>char<tab>
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 1", testString.size() == 1);

      testString = "12 345"; //12<space>345
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 6", testString.size() == 6);

      testString = "12\t345"; //12<tab>345
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 6", testString.size() == 6);

      testString = "12\t3 4\t5"; //12<tab>3<space>4<tab>5
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 8", testString.size() == 8);

      testString = " \t 12345 \t "; //<space><tab><space>12345<space><tab><space>
      dtUtil::Trim(testString);
      CPPUNIT_ASSERT_MESSAGE("Size should be 5", testString.size() == 5);

      testString.empty();
      std::string aReallyBigString;
      for (int i = 0; i <= 2048; ++i)
      {
         aReallyBigString += "c";
      }
      std::string aReallyBigStringCopy = aReallyBigString;
      unsigned len = aReallyBigString.size();
      dtUtil::Trim(aReallyBigString);
      CPPUNIT_ASSERT(aReallyBigString.size() == len);

      aReallyBigString = aReallyBigStringCopy;
      aReallyBigString.insert(0, "\t");
      dtUtil::Trim(aReallyBigString);
      CPPUNIT_ASSERT(aReallyBigString.size() == len);

      aReallyBigString = aReallyBigStringCopy;
      aReallyBigString.insert(len, "\t");
      dtUtil::Trim(aReallyBigString);
      CPPUNIT_ASSERT(aReallyBigString.size() == len);

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
void StringUtilTests::TestStrCompare()
{
   std::string first = "joe";
   std::string second = "Joe";

   CPPUNIT_ASSERT(dtUtil::StrCompare(first, second) > 0.0);
   CPPUNIT_ASSERT(dtUtil::StrCompare(second, first) < 0.0);

   CPPUNIT_ASSERT(dtUtil::StrCompare(second, first, false) == 0.0);
   CPPUNIT_ASSERT(dtUtil::StrCompare(first, second, false) == 0.0);
}

///////////////////////////////////////////////////////////////////////////////
void StringUtilTests::TestToString()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToString.\n");

      std::string testString = dtUtil::ToString<double>( 0.0 );  //RRN This returns "0"; should return "0.0"?  If so, uncomment the next line
      //CPPUNIT_ASSERT(testString == "0.0");

      testString = dtUtil::ToString<double>( 19.23 );
      CPPUNIT_ASSERT(testString == "19.23");

      testString = dtUtil::ToString<float>(0.0f); //RRN This returns "0"; should return "0.0"? If so, uncomment the next line
      //CPPUNIT_ASSERT(testString == "0.0");

      testString = dtUtil::ToString<float>(19.23f);
      CPPUNIT_ASSERT(testString == "19.23");

      testString = dtUtil::ToString<int>( 0 );
      CPPUNIT_ASSERT(testString == "0");

      testString = dtUtil::ToString<int>( -1 );
      CPPUNIT_ASSERT(testString == "-1");

      testString = dtUtil::ToString<unsigned int>( 0 );
      CPPUNIT_ASSERT(testString == "0");

      testString = dtUtil::ToString<char>( '0' );
      CPPUNIT_ASSERT(testString == "0");

      testString = dtUtil::ToString<bool>( true );
      CPPUNIT_ASSERT(testString == "1");

      testString = dtUtil::ToString<bool>( false );
      CPPUNIT_ASSERT(testString == "0");

      testString = dtUtil::ToString<short>( 0 );
      CPPUNIT_ASSERT(testString == "0");

      testString = dtUtil::ToString<long>( 0 );
      CPPUNIT_ASSERT(testString == "0");

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
//   }

}

///////////////////////////////////////////////////////////////////////////////
void StringUtilTests::TestToType()
{
   try
   {
      float fVal = dtUtil::ToType<float>("31.3");
      CPPUNIT_ASSERT_DOUBLES_EQUAL(31.3f, fVal, 0.01f);

      double dVal = dtUtil::ToType<double>("31.3");
      CPPUNIT_ASSERT_DOUBLES_EQUAL(31.3, dVal, 0.01);

      int iVal = dtUtil::ToType<int>("46");
      CPPUNIT_ASSERT_EQUAL(46, iVal);

      iVal = dtUtil::ToType<int>("");
      CPPUNIT_ASSERT_EQUAL(0, iVal);

      unsigned int uiVal = dtUtil::ToType<unsigned int>("49");
      CPPUNIT_ASSERT_EQUAL(49U, uiVal);

      short sVal = dtUtil::ToType<short>("100");
      CPPUNIT_ASSERT_EQUAL(short(100), sVal);

      bool bVal = dtUtil::ToType<bool>("true");
      CPPUNIT_ASSERT(bVal);
      bVal = dtUtil::ToType<bool>("1");
      CPPUNIT_ASSERT(bVal);
      bVal = dtUtil::ToType<bool>("True");
      CPPUNIT_ASSERT(bVal);
      bVal = dtUtil::ToType<bool>("true");
      CPPUNIT_ASSERT(bVal);
      bVal = dtUtil::ToType<bool>("gook");
      CPPUNIT_ASSERT_MESSAGE("Anything other than something in the \"True\" list should be regarded as false",
               !bVal);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
//   catch (const std::exception& e)
//   {
//      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
//   }
}

///////////////////////////////////////////////////////////////////////////////
void StringUtilTests::TestMatch()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Match.\n");
      CPPUNIT_ASSERT(dtUtil::Match("", ""));
      CPPUNIT_ASSERT(dtUtil::Match("x", "x"));
      CPPUNIT_ASSERT(dtUtil::Match("*x*", "x"));
      CPPUNIT_ASSERT(!dtUtil::Match("*y*", "x"));
      CPPUNIT_ASSERT(dtUtil::Match("*x*", "yxy"));
      CPPUNIT_ASSERT(dtUtil::Match("*y*", "yyxyy"));
      CPPUNIT_ASSERT(dtUtil::Match("*y", "xy"));
      CPPUNIT_ASSERT(dtUtil::Match("y*", "yx"));
      CPPUNIT_ASSERT(dtUtil::Match("?y", "xy"));
      CPPUNIT_ASSERT(dtUtil::Match("y?", "yx"));
      CPPUNIT_ASSERT(dtUtil::Match("?y?", "xyx"));
      CPPUNIT_ASSERT(dtUtil::Match("?x*y?", "dxsdggyx"));
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
void StringUtilTests::TestTokenizer()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing Tokenizer.\n");
      std::vector<std::string> tokens;
      dtUtil::StringTokenizer<dtUtil::IsSpace> tokenizer;

      #ifdef _MSC_VER
      tokenizer = tokenizer; // Silence unsed variable warning in MSVC.
      #endif

      std::string tokString = "";
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 0", tokens.size() == 0);

      tokString = " ";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 0", tokens.size() == 0);

      tokString = "x";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 1", tokens.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");

      tokString = " x";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 1", tokens.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");

      tokString = "x ";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 1", tokens.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");

      tokString = " x ";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 1", tokens.size() == 1);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");

      tokString = "x y";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 2", tokens.size() == 2);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");
      CPPUNIT_ASSERT_MESSAGE("Token should be \"y\"", tokens[1] == "y");

      tokString = " x y ";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString);
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 2", tokens.size() == 2);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");
      CPPUNIT_ASSERT_MESSAGE("Token should be \"y\"", tokens[1] == "y");

      tokString = "%%";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString, "%%");
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 0", tokens.size() == 0);

      tokString = "x%%y";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString, "%%");
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 2", tokens.size() == 2);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");
      CPPUNIT_ASSERT_MESSAGE("Token should be \"y\"", tokens[1] == "y");

      tokString = "%%x%%y";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString, "%%");
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 2", tokens.size() == 2);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");
      CPPUNIT_ASSERT_MESSAGE("Token should be \"y\"", tokens[1] == "y");

      tokString = "%%x%%y%%";
      tokens.clear();
      tokenizer.tokenize(tokens, tokString, "%%");
      CPPUNIT_ASSERT_MESSAGE("Vector size should be 2", tokens.size() == 2);
      CPPUNIT_ASSERT_MESSAGE("Token should be \"x\"", tokens[0] == "x");
      CPPUNIT_ASSERT_MESSAGE("Token should be \"y\"", tokens[1] == "y");

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
void StringUtilTests::TestParseVec()
{
   try
   {
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ParseVec.\n");
      std::string value0("");
      std::string value1("12.3");
      std::string value2("12.3 198.0");
      std::string value3("12.3 198.0 67");
      std::string value4("12.3 198.0 67 -19.8");
      osg::Vec2 newValue2;
      osg::Vec3 newValue3;
      osg::Vec4 newValue4;
      bool result;

      result = dtUtil::ParseVec<osg::Vec2>(value0, newValue2, 1);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue2[0] should be 0.0", newValue2[0] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue2[1] should be 0.0", newValue2[1] == 0.0f);

      result = dtUtil::ParseVec<osg::Vec3>(value0, newValue3, 1);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue3[0] should be 0.0", newValue3[0] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue3[1] should be 0.0", newValue3[1] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue3[2] should be 0.0", newValue3[2] == 0.0f);

      result = dtUtil::ParseVec<osg::Vec4>(value0, newValue4, 1);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue4[0] should be 0.0", newValue4[0] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[1] should be 0.0", newValue4[1] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[2] should be 0.0", newValue4[2] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[3] should be 0.0", newValue4[3] == 0.0f);

      result = dtUtil::ParseVec<osg::Vec2>(value1, newValue2, 1);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue2[0] should be 12.3", newValue2[0] == 12.3f);
      CPPUNIT_ASSERT_MESSAGE("newValue2[1] should be 0.0", newValue2[1] == 0.0f);

      result = dtUtil::ParseVec<osg::Vec3>(value1, newValue3, 1);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue3[0] should be 12.3", newValue3[0] == 12.3f);
      CPPUNIT_ASSERT_MESSAGE("newValue3[1] should be 0.0", newValue3[1] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue3[2] should be 0.0", newValue3[2] == 0.0f);

      result = dtUtil::ParseVec<osg::Vec4>(value1, newValue4, 1);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue4[0] should be 12.3", newValue4[0] == 12.3f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[1] should be 0.0", newValue4[1] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[2] should be 0.0", newValue4[2] == 0.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[3] should be 0.0", newValue4[3] == 0.0f);

      result = dtUtil::ParseVec<osg::Vec2>(value2, newValue2, 2);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue2[0] should be 12.3", newValue2[0] == 12.3f);
      CPPUNIT_ASSERT_MESSAGE("newValue2[1] should be 198.0", newValue2[1] == 198.0f);

      result = dtUtil::ParseVec<osg::Vec3>(value3, newValue3, 3);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue3[0] should be 12.3", newValue3[0] == 12.3f);
      CPPUNIT_ASSERT_MESSAGE("newValue3[1] should be 198.0", newValue3[1] == 198.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue3[2] should be 67", newValue3[2] == 67.0f);

      result = dtUtil::ParseVec<osg::Vec4>(value4, newValue4, 4);
      CPPUNIT_ASSERT(result);
      CPPUNIT_ASSERT_MESSAGE("newValue4[0] should be 12.3", newValue4[0] == 12.3f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[1] should be 198.0", newValue4[1] == 198.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[2] should be 67", newValue4[2] == 67.0f);
      CPPUNIT_ASSERT_MESSAGE("newValue4[3] should be -19.8", newValue4[3] == -19.8f);

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
void StringUtilTests::TestToPrimitives()
{
   const std::string threePointFourString("3.4");
   const float threePointFourFloat(3.4f);
   const double threePointFourDouble(3.4);
   const int threeInt(3);
   const bool trueBool(true);

   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToFloat.\n");
   float floatValue = dtUtil::ToFloat(threePointFourString);
   CPPUNIT_ASSERT( osg::equivalent( threePointFourFloat, floatValue ) );

   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToDouble.\n");
   double doubleValue = dtUtil::ToDouble(threePointFourString);
   CPPUNIT_ASSERT( osg::equivalent( threePointFourDouble, doubleValue ) );

   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToUnsignedInt.\n");
   const std::string threeString("3");
   const unsigned int threeUInt(3);

   unsigned int unsignedValue = dtUtil::ToUnsignedInt(threeString);
   CPPUNIT_ASSERT( threeUInt == unsignedValue );

   // -- test the dtUtil::ToType with fundamental types -- //
   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToType<int>.\n");
   int intValue = dtUtil::ToType<int>(threeString);
   CPPUNIT_ASSERT( threeInt == intValue );

   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToType<bool>.\n");
   std::string boolString("1");
   bool boolValue = dtUtil::ToType<bool>(boolString);
   CPPUNIT_ASSERT( trueBool == boolValue );

   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToType<unsigned int>.\n");
   unsignedValue = dtUtil::ToType<unsigned int>(threeString);
   CPPUNIT_ASSERT( threeUInt == unsignedValue );

   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToType<float>.\n");
   floatValue = dtUtil::ToType<float>(threePointFourString);
   CPPUNIT_ASSERT( threePointFourFloat == floatValue );

   mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Testing ToType<double>.\n");
   doubleValue = dtUtil::ToType<double>(threePointFourString);
   CPPUNIT_ASSERT( threePointFourDouble == doubleValue );
   // -- ---------------------------------------------- -- //
}

void StringUtilTests::TestMakeIndexString()
{
   std::string toFill;
   dtUtil::MakeIndexString(35U, toFill, 4);
   CPPUNIT_ASSERT_EQUAL(std::string("0035"), toFill);

   dtUtil::MakeIndexString(3550U, toFill, 4);
   CPPUNIT_ASSERT_EQUAL(std::string("3550"), toFill);

   dtUtil::MakeIndexString(3550U, toFill, 1);
   CPPUNIT_ASSERT_EQUAL(std::string("3550"), toFill);

   dtUtil::MakeIndexString(3550U, toFill, 3);
   CPPUNIT_ASSERT_EQUAL(std::string("3550"), toFill);

   dtUtil::MakeIndexString(0U, toFill, 3);
   CPPUNIT_ASSERT_EQUAL(std::string("000"), toFill);

   dtUtil::MakeIndexString(0U, toFill, 0);
   CPPUNIT_ASSERT_EQUAL(std::string("0"), toFill);

   dtUtil::MakeIndexString(700U, toFill, 0);
   CPPUNIT_ASSERT_EQUAL(std::string("700"), toFill);

   dtUtil::MakeIndexString(700U, toFill, 10);
   CPPUNIT_ASSERT_EQUAL(std::string("0000000700"), toFill);
}

