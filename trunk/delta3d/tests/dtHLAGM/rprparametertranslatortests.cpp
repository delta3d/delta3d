/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author David Guthrie
 */
 
#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <vector>
#include <string>
#include <osg/Endian>
#include <dtUtil/coordinates.h>
#include <dtUtil/log.h>
#include <dtDAL/datatype.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/rprparametertranslator.h>

class ParameterTranslatorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ParameterTranslatorTests);

      CPPUNIT_TEST(TestOutgoingWorldCoordinateDataTranslation);
      CPPUNIT_TEST(TestOutgoingMarkingTypeDataTranslation);
      CPPUNIT_TEST(TestOutgoingEulerAngleDataTranslation);
      CPPUNIT_TEST(TestOutgoingVectorDataTranslation);
      CPPUNIT_TEST(TestOutgoingEnumDataTranslation);
      CPPUNIT_TEST(TestOutgoingEntityTypeEnumDataTranslation);
      CPPUNIT_TEST(TestOutgoingIntDataTranslation);
      CPPUNIT_TEST(TestOutgoingFloatDataTranslation);
      CPPUNIT_TEST(TestIncomingDataTranslation);
      CPPUNIT_TEST(TestIncomingEntityTypeDataTranslation);
      CPPUNIT_TEST(TestFindTypeByName);
      CPPUNIT_TEST(TestAttributeSupportedQuery);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         mBuffer = NULL;
         mLogger = &dtUtil::Log::GetInstance("parametertranslatortests.cpp");
         mParameterTranslator = new dtHLAGM::RPRParameterTranslator(mCoordinates, mRuntimeMappings);
         mBuffer = new char[512];
         mEndian = osg::getCpuByteOrder();

         mMapping.GetParameterDefinitions().push_back(dtHLAGM::OneToManyMapping::ParameterDefinition());

         mMapping.GetParameterDefinitions()[0].SetGameName("test");
         mMapping.SetHLAName("ChickenMan");
      }

      void tearDown()
      {
         mMapping.GetParameterDefinitions().clear();
         mParameterTranslator = NULL;
         delete[] mBuffer;
         mBuffer = NULL;
      }

      void TestOutgoingWorldCoordinateDataTranslation()
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::VEC3);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE);
         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         osg::Vec3 testVec(5.0f, 4.3f, 73.9f);
         vec3Param->SetValue(testVec);
         messageParameters.push_back(vec3Param.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         osg::Vec3d expectedVec = mCoordinates.ConvertToRemoteTranslation(testVec);

         dtHLAGM::WorldCoordinate wc;

         wc.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), wc.GetX(), 1e-6) &&
                        osg::equivalent(expectedVec.y(), wc.GetY(), 1e-6) &&
                        osg::equivalent(expectedVec.z(), wc.GetZ(), 1e-6) );



         messageParameters.clear();

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::VEC3F);
         dtCore::RefPtr<dtGame::Vec3fMessageParameter> vec3fParam = new dtGame::Vec3fMessageParameter("test");
         osg::Vec3f testVecf(5.0f, 4.3f, 73.9f);
         vec3fParam->SetValue(testVecf);
         
         messageParameters.push_back(vec3fParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);
         wc.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), wc.GetX(), 1e-6) &&
                        osg::equivalent(expectedVec.y(), wc.GetY(), 1e-6) &&
                        osg::equivalent(expectedVec.z(), wc.GetZ(), 1e-6) );

         messageParameters.clear();

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::VEC3D);
         dtCore::RefPtr<dtGame::Vec3dMessageParameter> vec3dParam = new dtGame::Vec3dMessageParameter("test");
         osg::Vec3d testVecd(5.0, 4.3, 73.9);
         vec3dParam->SetValue(testVecd);
         
         messageParameters.push_back(vec3dParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);
         wc.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), wc.GetX(), 1e-6) &&
                        osg::equivalent(expectedVec.y(), wc.GetY(), 1e-6) &&
                        osg::equivalent(expectedVec.z(), wc.GetZ(), 1e-6) );

      }

      void TestOutgoingMarkingTypeDataTranslation()
      {
         //short
         InternalTestOutgoingMarkingTypeDataTranslation("01234");
         //exact
         InternalTestOutgoingMarkingTypeDataTranslation("01234567890");
         //too long.
         InternalTestOutgoingMarkingTypeDataTranslation("0123456789012345");
      }

      void TestOutgoingEulerAngleDataTranslation()
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::VEC3);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE);
         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         osg::Vec3 testVec(1.5f, 3.11f, -2.73f);
         vec3Param->SetValue(testVec);
         messageParameters.push_back(vec3Param.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         osg::Vec3 reorderedVec(testVec.z(), testVec.x(), testVec.y());

         osg::Vec3 expectedVec = mCoordinates.ConvertToRemoteRotation(reorderedVec);

         dtHLAGM::EulerAngles eua;

         eua.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         //must remember to reorder the vector since actor properties store rotation in a different order.
         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), eua.GetPsi(), 1e-6f) &&
                        osg::equivalent(expectedVec.y(), eua.GetTheta(), 1e-6f) &&
                        osg::equivalent(expectedVec.z(), eua.GetPhi(), 1e-6f) );

         //Now testing Vec3f with the same data.

         messageParameters.clear();

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::VEC3F);
         dtCore::RefPtr<dtGame::Vec3fMessageParameter> vec3fParam = new dtGame::Vec3fMessageParameter("test");
         osg::Vec3f testVecf(1.5f, 3.11f, -2.73f);
         vec3fParam->SetValue(testVecf);
         messageParameters.push_back(vec3fParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         eua.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         //must remember to reorder the vector since actor properties store rotation in a different order.
         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), eua.GetPsi(), 1e-6f) &&
                        osg::equivalent(expectedVec.y(), eua.GetTheta(), 1e-6f) &&
                        osg::equivalent(expectedVec.z(), eua.GetPhi(), 1e-6f) );
                        
         //Now testing Vec3d with the same data.
         
         messageParameters.clear();

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::VEC3D);
         dtCore::RefPtr<dtGame::Vec3dMessageParameter> vec3dParam = new dtGame::Vec3dMessageParameter("test");
         osg::Vec3d testVecd(1.5f, 3.11f, -2.73f);
         vec3dParam->SetValue(testVecd);
         messageParameters.push_back(vec3dParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         eua.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         //must remember to reorder the vector since actor properties store rotation in a different order.
         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), eua.GetPsi(), 1e-6f) &&
                        osg::equivalent(expectedVec.y(), eua.GetTheta(), 1e-6f) &&
                        osg::equivalent(expectedVec.z(), eua.GetPhi(), 1e-6f) );

      }

      void TestOutgoingVectorDataTranslation()
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::VEC3);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE);
         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         osg::Vec3 testVec(1.5f, 3.11f, -2.73f);
         vec3Param->SetValue(testVec);
         messageParameters.push_back(vec3Param.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         osg::Vec3 expectedVec =  mCoordinates.GetOriginRotationMatrix().preMult(testVec);

         dtHLAGM::VelocityVector vv;

         vv.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), vv.GetX(), 1e-6f) &&
                        osg::equivalent(expectedVec.y(), vv.GetY(), 1e-6f) &&
                        osg::equivalent(expectedVec.z(), vv.GetZ(), 1e-6f) );
      }

      void TestOutgoingEnumDataTranslation()
      {
         std::string expectedResult("33");
         int iExpectedResult = 33;
         
         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];

         pd.SetGameType(dtDAL::DataType::ENUMERATION);

         pd.AddEnumerationMapping("1", "hello");
         pd.AddEnumerationMapping(expectedResult, "correct");
         pd.AddEnumerationMapping("123", "Another Value");
         pd.SetDefaultValue("hello");

         //There is a mapping the expected result, so it should be the actual result.
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_INT_TYPE);
         InternalTestOutgoingEnumDataTranslation<unsigned int>(iExpectedResult);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_SHORT_TYPE);
         InternalTestOutgoingEnumDataTranslation<unsigned short>(iExpectedResult);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_CHAR_TYPE);
         InternalTestOutgoingEnumDataTranslation<unsigned char>(iExpectedResult);

         pd.ClearEnumerationMapping();
         pd.AddEnumerationMapping("1", "hello");
         pd.AddEnumerationMapping("123", "Another Value");

         //There is no mapping the expected result, so it should use the default.
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_INT_TYPE);
         InternalTestOutgoingEnumDataTranslation<unsigned int>(1);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_SHORT_TYPE);
         InternalTestOutgoingEnumDataTranslation<unsigned short>(1);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_CHAR_TYPE);
         InternalTestOutgoingEnumDataTranslation<unsigned char>(1);
      }

      void TestOutgoingEntityTypeEnumDataTranslation()
      {
         dtHLAGM::EntityType expectedResult(2, 2, 225, 2, 2, 0, 0);
         std::string sExpectedResult("2 2 225 2 2 0 0");
         
         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];

         pd.SetGameType(dtDAL::DataType::ENUMERATION);

         pd.AddEnumerationMapping("2 8 342 3 8 3 9", "hello");
         pd.AddEnumerationMapping(sExpectedResult, "correct");
         pd.AddEnumerationMapping("2 8 342 3 8 3 10", "Another Value");
         pd.SetDefaultValue("hello");

         //There is a mapping the expected result, so it should be the actual result.
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::ENTITY_TYPE);
         InternalTestOutgoingEntityTypeDataTranslation(expectedResult);

         pd.ClearEnumerationMapping();
         pd.AddEnumerationMapping("2 8 342 3 8 3 9", "hello");
         pd.AddEnumerationMapping("2 8 342 3 8 3 10", "Another Value");

         //There is no mapping the expected result, so it should use the default.
         InternalTestOutgoingEntityTypeDataTranslation(dtHLAGM::EntityType(2, 8, 342, 3, 8, 3, 9));
      }

      void TestOutgoingIntDataTranslation()
      {
         unsigned int expectedResult = 33;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::UINT);

         //There is a mapping the expected result, so it should be the actual result.
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_INT_TYPE);
         InternalTestOutgoingIntDataTranslation<unsigned int>(expectedResult);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_SHORT_TYPE);
         InternalTestOutgoingIntDataTranslation<unsigned short>(expectedResult);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_CHAR_TYPE);
         InternalTestOutgoingIntDataTranslation<unsigned char>(expectedResult);
      }

      void TestOutgoingFloatDataTranslation()
      {
         double expectedResult = 33.0;
         //There is a mapping the expected result, so it should be the actual result.
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::FLOAT_TYPE);
         InternalTestOutgoingFloatDataTranslation<float>(float(expectedResult));
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::DOUBLE_TYPE);
         InternalTestOutgoingFloatDataTranslation<double>(expectedResult);
      }

      void TestIncomingEntityTypeDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::ENTITY_TYPE);

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType(enumParam->GetDataType());
         pd.AddEnumerationMapping("2 8 342 3 8 3 9", "hello");
         pd.AddEnumerationMapping("2 8 342 3 8 3 10", "correct");
         pd.SetDefaultValue("hello");

         messageParameters.push_back(enumParam.get());

         dtHLAGM::EntityType inputValue(2, 8, 342, 3, 8, 3, 10);
         inputValue.Encode(mBuffer);

         mParameterTranslator->MapToMessageParameters(mBuffer, inputValue.EncodedLength(), messageParameters, mMapping);

         std::string expectedValue("correct");

         CPPUNIT_ASSERT_EQUAL(expectedValue, enumParam->GetValue());

         dtHLAGM::EntityType inputValue2(2, 9, 111, 1, 0, 3, 9);
         inputValue2.Encode(mBuffer);
         
         expectedValue = "hello";

         mParameterTranslator->MapToMessageParameters(mBuffer, inputValue2.EncodedLength(), messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL(expectedValue, enumParam->GetValue());
      }

      void TestIncomingDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtHLAGM::WorldCoordinate wc(1.1f, 2.2f, 3.3f);
         wc.Encode(mBuffer);

         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         mMapping.GetParameterDefinitions()[0].SetGameType(vec3Param->GetDataType());
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE);

         messageParameters.push_back(vec3Param.get());
         mParameterTranslator->MapToMessageParameters(mBuffer, wc.EncodedLength(), messageParameters, mMapping);
         messageParameters.clear();

         osg::Vec3 expectedVec = mCoordinates.ConvertToLocalTranslation(osg::Vec3d(wc.GetX(), wc.GetY(), wc.GetZ()));
         CPPUNIT_ASSERT(expectedVec == vec3Param->GetValue());

         mBuffer[0] = 1;
         strcpy(mBuffer+1, "FOA");
         dtCore::RefPtr<dtGame::StringMessageParameter> stringParam = new dtGame::StringMessageParameter("test");
         mMapping.GetParameterDefinitions()[0].SetGameType(stringParam->GetDataType());
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::MARKING_TYPE);

         messageParameters.push_back(stringParam.get());
         mParameterTranslator->MapToMessageParameters(mBuffer, 4, messageParameters, mMapping);
         messageParameters.clear();

         CPPUNIT_ASSERT(stringParam->GetValue() == "FOA");

         mBuffer[0] = 1;
         //Test the max size of 11 by going over by one.
         strcpy(mBuffer+1, "123456789012");

         messageParameters.push_back(stringParam.get());
         mParameterTranslator->MapToMessageParameters(mBuffer, 13, messageParameters, mMapping);
         messageParameters.clear();
         CPPUNIT_ASSERT(stringParam->GetValue() == "12345678901");

         dtHLAGM::EulerAngles eua;
         eua.SetPsi(1.50f);
         eua.SetTheta(3.14f);
         eua.SetPhi(-2.44f);
         eua.Encode(mBuffer);

         mMapping.GetParameterDefinitions()[0].SetGameType(vec3Param->GetDataType());
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE);
         messageParameters.push_back(vec3Param.get());
         mParameterTranslator->MapToMessageParameters(mBuffer, eua.EncodedLength(), messageParameters, mMapping);
         messageParameters.clear();

         //do a coordiate conversion to get the expected value.
         expectedVec = mCoordinates.ConvertToLocalRotation(eua.GetPsi(), eua.GetTheta(), eua.GetPhi());
         //must remember to reorder the vector since actor properties store rotation in a different order.
         CPPUNIT_ASSERT(osg::equivalent(expectedVec.y(), vec3Param->GetValue().x(), 1e-6f) &&
                        osg::equivalent(expectedVec.z(), vec3Param->GetValue().y(), 1e-6f) &&
                        osg::equivalent(expectedVec.x(), vec3Param->GetValue().z(), 1e-6f) );

         unsigned int testUInt = 44;
         unsigned short testUShort = 3;
         unsigned char testUChar = 9;

         *((unsigned int*)mBuffer) = testUInt;
         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes(mBuffer, sizeof(unsigned int));
         }
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_INT_TYPE);
         TestSpecificIntTranslations(mMapping, (long)testUInt, sizeof(unsigned));

         *((unsigned short*)mBuffer) = testUShort;
         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes(mBuffer, sizeof(unsigned short));
         }
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_SHORT_TYPE);
         TestSpecificIntTranslations(mMapping, (long)testUShort, sizeof(unsigned short));

         *((unsigned char*)mBuffer) = testUChar;
         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes(mBuffer, sizeof(unsigned char));
         }
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_CHAR_TYPE);
         TestSpecificIntTranslations(mMapping, (long)testUChar, sizeof(unsigned char));

         float testFloat = 33.6f;
         float testDouble = 344.9;

         *((float*)mBuffer) = testFloat;
         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes(mBuffer, sizeof(float));
         }
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::FLOAT_TYPE);
         TestSpecificFloatTranslations<float>(mMapping, testFloat);

         *((double*)mBuffer) = testDouble;
         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes(mBuffer, sizeof(double));
         }
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::DOUBLE_TYPE);
         TestSpecificFloatTranslations<double>(mMapping, testDouble);


      }

      void TestFindTypeByName()
      {
         for (unsigned int i = 0; i < dtHLAGM::RPRAttributeType::Enumerate().size(); ++i)
         {
            //Test all the types to see if it's working properly
            CPPUNIT_ASSERT(mParameterTranslator->GetAttributeTypeForName(dtHLAGM::RPRAttributeType::Enumerate()[i]->GetName()) ==
               *dtHLAGM::RPRAttributeType::Enumerate()[i]);
         }
         //Make sure it returns unknown for anything else.
         CPPUNIT_ASSERT(mParameterTranslator->GetAttributeTypeForName("Some Junk") ==
            dtHLAGM::AttributeType::UNKNOWN);
      }

      void TestAttributeSupportedQuery()
      {
         for (unsigned int i = 0; i < dtHLAGM::RPRAttributeType::Enumerate().size(); ++i)
         {
            //Test all the types to see if it's working properly
            CPPUNIT_ASSERT(mParameterTranslator->TranslatesAttributeType(
               static_cast<dtHLAGM::AttributeType&>(*dtHLAGM::RPRAttributeType::Enumerate()[i])));
         }

         //Make sure UNKNOWN is not supported.
         CPPUNIT_ASSERT(!mParameterTranslator->TranslatesAttributeType(dtHLAGM::AttributeType::UNKNOWN));
      }

   private:
      dtUtil::Log* mLogger;
      dtCore::RefPtr<dtHLAGM::ParameterTranslator> mParameterTranslator;
      dtUtil::Coordinates mCoordinates;
      dtHLAGM::ObjectRuntimeMappingInfo mRuntimeMappings;
      osg::Endian mEndian;
      //shared buffer.
      char* mBuffer;
      dtHLAGM::AttributeToPropertyList mMapping;

      void RunIncomingTranslation(dtGame::MessageParameter& param, dtHLAGM::OneToManyMapping& mapping, size_t bufferSize)
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         bool foundMatchingParameterDef = false;
         for (unsigned i = 0; i < mapping.GetParameterDefinitions().size(); ++i)
         {
            if (mapping.GetParameterDefinitions()[i].GetGameName() == param.GetName())
            {
               mapping.GetParameterDefinitions()[i].SetGameType(param.GetDataType());
               foundMatchingParameterDef = true;
               break;
            }
         }

         CPPUNIT_ASSERT(foundMatchingParameterDef);

         messageParameters.push_back(&param);
         mParameterTranslator->MapToMessageParameters(mBuffer, bufferSize, messageParameters, mapping);
      }

      void TestSpecificIntTranslations(dtHLAGM::OneToManyMapping& mapping,
         long expectedResult, size_t bufferSize)
      {
         //Unsigned Int param
         dtCore::RefPtr<dtGame::UnsignedIntMessageParameter> uIntParam = new dtGame::UnsignedIntMessageParameter("test");
         RunIncomingTranslation(*uIntParam, mapping, bufferSize);
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type unsigned int",
            (unsigned)expectedResult == uIntParam->GetValue());

         //Int param
         dtCore::RefPtr<dtGame::IntMessageParameter> intParam = new dtGame::IntMessageParameter("test");
         RunIncomingTranslation(*intParam, mapping, bufferSize);
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type int",
            (int)expectedResult == intParam->GetValue());

         //short param
         dtCore::RefPtr<dtGame::ShortIntMessageParameter> shortParam = new dtGame::ShortIntMessageParameter("test");
         RunIncomingTranslation(*shortParam, mapping, bufferSize);
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type short",
            (short int)expectedResult == shortParam->GetValue());

         //unsigned short
         dtCore::RefPtr<dtGame::UnsignedShortIntMessageParameter> uShortParam = new dtGame::UnsignedShortIntMessageParameter("test");
         RunIncomingTranslation(*uShortParam, mapping, bufferSize);
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type unsigned short",
            (short int)expectedResult == uShortParam->GetValue());

         //boolean
         dtCore::RefPtr<dtGame::BooleanMessageParameter> boolParam = new dtGame::BooleanMessageParameter("test");
         RunIncomingTranslation(*boolParam, mapping, bufferSize);
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type bool",
            (expectedResult != 0) == boolParam->GetValue());


         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetDefaultValue("hello");

         std::ostringstream ss;
         ss << expectedResult;

         pd.AddEnumerationMapping("1", "hello");
         pd.AddEnumerationMapping(ss.str(), "correct");
         pd.AddEnumerationMapping("800", "Another Value");

         RunIncomingTranslation(*enumParam, mapping, bufferSize);
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign param of type enumeration to \"correct\", but it is: " +  enumParam->GetValue(),
            enumParam->GetValue() == "correct");

         pd.ClearEnumerationMapping();
         pd.AddEnumerationMapping("1", "hello");
         pd.AddEnumerationMapping("800", "Another Value");

         RunIncomingTranslation(*enumParam, mapping, bufferSize);
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type enumeration with the default value",
            enumParam->GetValue() == pd.GetDefaultValue());
      }

      template <typename ValueType>
      void TestSpecificFloatTranslations(dtHLAGM::OneToManyMapping& mapping,
         ValueType expectedResult)
      {
         //Float param
         dtCore::RefPtr<dtGame::FloatMessageParameter> floatParam = new dtGame::FloatMessageParameter("test");
         RunIncomingTranslation(*floatParam, mapping, sizeof(ValueType));
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type float",
            (float)expectedResult == floatParam->GetValue());

         //Double param
         dtCore::RefPtr<dtGame::DoubleMessageParameter> doubleParam = new dtGame::DoubleMessageParameter("test");
         RunIncomingTranslation(*doubleParam, mapping, sizeof(ValueType));
         CPPUNIT_ASSERT_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type double",
            (double)expectedResult == doubleParam->GetValue());
      }
      
      void InternalTestOutgoingMarkingTypeDataTranslation(const std::string& testValue)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::STRING);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::MARKING_TYPE);
         dtCore::RefPtr<dtGame::StringMessageParameter> stringParam = new dtGame::StringMessageParameter("test");
         stringParam->SetValue(testValue);
         messageParameters.push_back(stringParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         //We always use code 1 for ASCII
         CPPUNIT_ASSERT(buffer[0] == 1);

         std::string result;
         for (unsigned i = 1; i < size; ++i)
         {
            if (buffer[i] == '\0')
               break;

            result.append(1, buffer[i]);
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         const std::string& expectedValue = testValue.substr(0, size - 1);

         CPPUNIT_ASSERT_MESSAGE("The result should have been \"" + expectedValue + "\" but it is \"" + result + "\"", result == expectedValue);
      }



      void TranslateOutgoingParameter(char*& buffer, size_t& size,
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& messageParameters, dtHLAGM::AttributeToPropertyList& mapping)
      {
         mParameterTranslator->AllocateBuffer(buffer, size, mapping.GetHLAType());

         CPPUNIT_ASSERT(buffer != NULL);
         CPPUNIT_ASSERT(size > 0);

         mParameterTranslator->MapFromMessageParameters(buffer, size, messageParameters, mapping);

         if (size != mapping.GetHLAType().GetEncodedLength())
         {
            mParameterTranslator->DeallocateBuffer(buffer);
            std::ostringstream ss;
            ss << "the size of the buffer for a " << mapping.GetHLAType().GetName() << " is not correct.  It was \""
               << size << "\" but it should have been \"" <<  mapping.GetHLAType().GetEncodedLength() << "\".";
            CPPUNIT_FAIL(ss.str());
         }

      }

      template <typename ValueType>
      void InternalTestOutgoingEnumDataTranslation(ValueType expectedResult)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         enumParam->SetValue("correct");
         messageParameters.push_back(enumParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         ValueType actualValue = *(ValueType*)(buffer);

         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&actualValue), sizeof(ValueType));
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT_EQUAL(expectedResult, actualValue);
      }

      void InternalTestOutgoingEntityTypeDataTranslation(const dtHLAGM::EntityType& expectedResult)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         enumParam->SetValue("correct");
         messageParameters.push_back(enumParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);
         CPPUNIT_ASSERT_EQUAL(size, size_t(expectedResult.EncodedLength()));

         dtHLAGM::EntityType actualValue;
         actualValue.Decode(buffer);

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT_EQUAL(expectedResult, actualValue);
      }

      template <typename ValueType>
      void InternalTestOutgoingIntDataTranslation(ValueType expectedResult)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         dtCore::RefPtr<dtGame::UnsignedIntMessageParameter> uIntParam = new dtGame::UnsignedIntMessageParameter("test");
         uIntParam->SetValue(unsigned(expectedResult));
         messageParameters.push_back(uIntParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         ValueType actualValue = *(ValueType*)(buffer);

         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&actualValue), sizeof(ValueType));
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(ValueType(actualValue) == expectedResult);
      }

      template <typename ValueType>
      void InternalTestOutgoingFloatDataTranslation(ValueType expectedResult)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         dtCore::RefPtr<dtGame::DoubleMessageParameter> doubleParam = new dtGame::DoubleMessageParameter("test");
         doubleParam->SetValue(double(expectedResult));
         messageParameters.push_back(doubleParam.get());
         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::DOUBLE);

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The size of the buffer should match the size of the outgoing datatype.", size, sizeof(ValueType));
         ValueType actualValue = *(ValueType*)(buffer);
         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&actualValue), sizeof(ValueType));
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         std::ostringstream ss;
         ss << "Actual value is \"" << actualValue << "\" but the expected value is \"" << expectedResult << "\"" << std::endl;
         CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(actualValue, expectedResult, ValueType(1e-4)));
         
         messageParameters.clear();
         dtCore::RefPtr<dtGame::FloatMessageParameter> floatParam = new dtGame::FloatMessageParameter("test");
         floatParam->SetValue(float(expectedResult));
         messageParameters.push_back(floatParam.get());
         mMapping.GetParameterDefinitions()[0].SetGameType(dtDAL::DataType::FLOAT);

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         actualValue = *(ValueType*)(buffer);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The size of the buffer should match the size of the outgoing datatype.", size, sizeof(ValueType));
            
         if (mEndian == osg::LittleEndian)
         {
            osg::swapBytes((char*)(&actualValue), sizeof(ValueType));
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         ss.str("");
         ss << "Actual value is \"" << actualValue << "\" but the expected value is \"" << expectedResult << "\"" << std::endl;
         CPPUNIT_ASSERT_MESSAGE(ss.str(), osg::equivalent(actualValue, expectedResult, ValueType(1e-4)));
      }

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ParameterTranslatorTests);
