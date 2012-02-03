/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
* David Guthrie
*/
#include <prefix/unittestprefix.h>

#include <dtUtil/mswinmacros.h>

#ifdef DELTA_WIN32
   #pragma warning(push)
      #pragma warning(disable : 4267)
      #include <cppunit/extensions/HelperMacros.h>
   #pragma warning(pop)
#else
   #include <cppunit/extensions/HelperMacros.h>
#endif

#include <dtCore/uniqueid.h>

#include <dtCore/datatype.h>

#include <dtHLAGM/attributetoproperty.h>
#include <dtHLAGM/distypes.h>
#include <dtHLAGM/hlacomponent.h>
#include <dtHLAGM/interactiontomessage.h>
#include <dtHLAGM/objecttoactor.h>
#include <dtHLAGM/onetoonemapping.h>
#include <dtHLAGM/parametertoparameter.h>
#include <dtHLAGM/rprparametertranslator.h>
#include <dtHLAGM/spatial.h>
#include <dtHLAGM/environmentprocessrecordlist.h>

#include <dtCore/namedgroupparameter.inl>

#include <dtUtil/coordinates.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/stringutils.h>

#include <osg/Endian>

#include <iostream>
#include <vector>
#include <string>

namespace dtHLAGM
{
   /////////////////////////////////////////////////////////////////////////////
   // TEST RPR PARAMETER TRANSLATOR
   /////////////////////////////////////////////////////////////////////////////
   class TestRPRParamTranslator : public RPRParameterTranslator
   {
      public:
         TestRPRParamTranslator(dtUtil::Coordinates& coordinates, ObjectRuntimeMappingInfo& runtimeMappings)
            : RPRParameterTranslator(coordinates,runtimeMappings)
         {
         }

         void SetIntegerValuePublic(unsigned value, dtGame::MessageParameter& parameter,
            const OneToManyMapping& mapping, unsigned parameterDefIndex) const;

         unsigned GetIntegerValuePublic(const dtGame::MessageParameter& parameter,
            const OneToManyMapping& mapping, unsigned parameterDefIndex) const;

      protected:
         virtual ~TestRPRParamTranslator()
         {
         }
   };

   /////////////////////////////////////////////////////////////////////////////
   void TestRPRParamTranslator::SetIntegerValuePublic(
      unsigned value, dtGame::MessageParameter& parameter,
      const OneToManyMapping& mapping, unsigned parameterDefIndex) const
   {
      // This function needs to be exposed for testing
      SetIntegerValue( value, parameter, mapping, parameterDefIndex );
   }

   /////////////////////////////////////////////////////////////////////////////
   unsigned TestRPRParamTranslator::GetIntegerValuePublic(
      const dtGame::MessageParameter& parameter, const OneToManyMapping& mapping,
      unsigned parameterDefIndex) const
   {
      // This function needs to be exposed for testing
      return GetIntegerValue( parameter, mapping, parameterDefIndex );
   }



////////////////////////////////////////////////////////////////////////////////
// TEST OBJECT CODE
////////////////////////////////////////////////////////////////////////////////
class ParameterTranslatorTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ParameterTranslatorTests);

      CPPUNIT_TEST(TestInOutEnviornmentProcessRecordListDataTranslation);
      CPPUNIT_TEST(TestOutgoingSpatialDataTranslation);
      CPPUNIT_TEST(TestOutgoingWorldCoordinateDataTranslation);
      CPPUNIT_TEST(TestOutgoingMarkingTypeDataTranslation);
      CPPUNIT_TEST(TestOutgoingStringDataTranslation);
      CPPUNIT_TEST(TestOutgoingOctetDataTranslation);
      CPPUNIT_TEST(TestOutgoingEulerAngleDataTranslation);
      CPPUNIT_TEST(TestOutgoingVectorDataTranslation);
      CPPUNIT_TEST(TestOutgoingAngularVectorDataTranslation);
      CPPUNIT_TEST(TestOutgoingArticulationDataTranslation);
      CPPUNIT_TEST(TestOutgoingEnumDataTranslation);
      CPPUNIT_TEST(TestOutgoingEntityTypeEnumDataTranslation);
      CPPUNIT_TEST(TestOutgoingIntDataTranslation);
      CPPUNIT_TEST(TestOutgoingFloatDataTranslation);
      CPPUNIT_TEST(TestOutgoingStringToRTIIDStructDataTranslation);
      CPPUNIT_TEST(TestOutgoingActorIdToRTIIDStructDataTranslation);
      CPPUNIT_TEST(TestIncomingDataTranslation);
      CPPUNIT_TEST(TestIncomingSpatialDataTranslation);
      CPPUNIT_TEST(TestIncomingEntityTypeDataTranslation);
      CPPUNIT_TEST(TestIncomingStringToEnumDataTranslation);
      CPPUNIT_TEST(TestIncomingStringDataTranslation);
      CPPUNIT_TEST(TestIncomingOctetDataTranslation);
      CPPUNIT_TEST(TestIncomingVelocityVectorDataTranslation);
      CPPUNIT_TEST(TestIncomingRTIIDStructToActorIdDataTranslation);
      CPPUNIT_TEST(TestIncomingRTIIDStructToStringDataTranslation);
      CPPUNIT_TEST(TestFindTypeByName);
      CPPUNIT_TEST(TestAttributeSupportedQuery);
      CPPUNIT_TEST(TestIncomingArticulation);
      CPPUNIT_TEST(TestGetAndSetInteger);

   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp()
      {
         mBuffer = NULL;
         mLogger = &dtUtil::Log::GetInstance("parametertranslatortests.cpp");
         mParameterTranslator = new dtHLAGM::TestRPRParamTranslator(mCoordinates, mRuntimeMappings);
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

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::VEC3);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::WORLD_COORDINATE_TYPE);
         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         osg::Vec3 testVec(5.0f, 4.3f, 73.9f);
         vec3Param->SetValue(testVec);
         messageParameters.push_back(vec3Param);

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         osg::Vec3d expectedVec = mCoordinates.ConvertToRemoteTranslation(testVec);

         dtHLAGM::WorldCoordinate wc;

         wc.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), wc.GetX(), 1e-6) &&
                        osg::equivalent(expectedVec.y(), wc.GetY(), 1e-6) &&
                        osg::equivalent(expectedVec.z(), wc.GetZ(), 1e-6) );



         messageParameters.clear();

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::VEC3F);
         dtCore::RefPtr<dtGame::Vec3fMessageParameter> vec3fParam = new dtGame::Vec3fMessageParameter("test");
         osg::Vec3f testVecf(5.0f, 4.3f, 73.9f);
         vec3fParam->SetValue(testVecf);

         messageParameters.push_back(vec3fParam);

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);
         wc.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), wc.GetX(), 1e-6) &&
                        osg::equivalent(expectedVec.y(), wc.GetY(), 1e-6) &&
                        osg::equivalent(expectedVec.z(), wc.GetZ(), 1e-6) );

         messageParameters.clear();

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::VEC3D);
         dtCore::RefPtr<dtGame::Vec3dMessageParameter> vec3dParam = new dtGame::Vec3dMessageParameter("test");
         osg::Vec3d testVecd(5.0, 4.3, 73.9);
         vec3dParam->SetValue(testVecd);

         messageParameters.push_back(vec3dParam);

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);
         wc.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), wc.GetX(), 1e-6) &&
                        osg::equivalent(expectedVec.y(), wc.GetY(), 1e-6) &&
                        osg::equivalent(expectedVec.z(), wc.GetZ(), 1e-6) );

      }

      void TestInOutEnviornmentProcessRecordListDataTranslation()
      {
         mMapping.GetParameterDefinitions()[0].SetGameName("Records");
         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::GROUP);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::ENVIRONMENT_RECORD_LIST_TYPE);

         dtCore::RefPtr<dtCore::NamedGroupParameter> groupParam = new dtCore::NamedGroupParameter("Record");

         dtCore::RefPtr<dtCore::NamedGroupParameter> rec1 = new dtCore::NamedGroupParameter("1");
         dtCore::RefPtr<dtCore::NamedGroupParameter> rec2 = new dtCore::NamedGroupParameter("2");

         const osg::Vec3d testPos1(-1000.4, 1.0, 7.3);
         const osg::Vec3d testPos2(11.4, -1.66, -4366.88);
         const osg::Vec3f testVec1(27.4f, -3.7f, 36.22f);
         const osg::Vec3f testVec2(19.22f, 11.6f, -0.0023f);

         const osg::Vec3f testOrient(-1.11, 3.77, 6.28f);

         const float testHeight = 1.88f;
         const float testRate = 8.73f;

         rec1->SetValue("Index", 7U);
         rec1->SetValue("TypeCode", unsigned(EnvironmentProcessRecord::GaussianPuffRecordType));
         rec1->SetValue(EnvironmentProcessRecord::PARAM_LOCATION, testPos1);
         rec1->SetValue(EnvironmentProcessRecord::PARAM_ORIGINATION_LOCATION, testPos2);
         rec1->SetValue(EnvironmentProcessRecord::PARAM_DIMENSION, testVec1);
         rec1->SetValue(EnvironmentProcessRecord::PARAM_DIMENSION_RATE, testVec2);
         rec1->SetValue(EnvironmentProcessRecord::PARAM_ORIENTATION, testOrient);
         rec1->SetValue(EnvironmentProcessRecord::PARAM_VELOCITY, testVec1);
         rec1->SetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, testVec2);
         rec1->SetValue(EnvironmentProcessRecord::PARAM_CENTROID_HEIGHT, testHeight);

         rec2->SetValue("Index", 8U);
         rec2->SetValue("TypeCode", unsigned(EnvironmentProcessRecord::SphereRecord2Type));
         rec2->SetValue(EnvironmentProcessRecord::PARAM_LOCATION, testPos1);
         rec2->SetValue(EnvironmentProcessRecord::PARAM_RADIUS, testHeight);
         rec2->SetValue(EnvironmentProcessRecord::PARAM_RADIUS_RATE, testRate);
         rec2->SetValue(EnvironmentProcessRecord::PARAM_VELOCITY, testVec1);
         rec2->SetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, testVec2);

         groupParam->AddParameter(*rec1);
         groupParam->AddParameter(*rec2);

         char* buffer = NULL;
         size_t bufSize = 0;

         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;
         messageParameters.push_back(groupParam);

         TranslateOutgoingParameter(buffer, bufSize, messageParameters, mMapping);


         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParametersIn;
         dtCore::RefPtr<dtCore::NamedGroupParameter> groupIncoming = new dtCore::NamedGroupParameter("Record");
         messageParametersIn.push_back(groupIncoming);

         mParameterTranslator->MapToMessageParameters(buffer, bufSize, messageParametersIn, mMapping);

         CPPUNIT_ASSERT_EQUAL(unsigned(2U), unsigned(groupIncoming->GetParameterCount()));

         std::vector<const dtGame::MessageParameter* > incomingParameters;
         groupIncoming->GetParameters(incomingParameters);

         for (unsigned i = 0; i < 2; ++i)
         {
            CPPUNIT_ASSERT(incomingParameters[i]->GetDataType() == dtCore::DataType::GROUP);

            const dtCore::NamedGroupParameter& groupParam = static_cast<const dtCore::NamedGroupParameter&>(*incomingParameters[i]);

            unsigned typeCode = groupParam.GetValue("TypeCode", 0U);
            if (typeCode == EnvironmentProcessRecord::SphereRecord2Type)
            {
               CPPUNIT_ASSERT_EQUAL(8U, groupParam.GetValue("Index", 0U));
               CPPUNIT_ASSERT(dtUtil::Equivalent(testPos1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_LOCATION, osg::Vec3d()), 0.01));
               CPPUNIT_ASSERT_EQUAL(testHeight, groupParam.GetValue(EnvironmentProcessRecord::PARAM_RADIUS, float(0.0f)));
               CPPUNIT_ASSERT_EQUAL(testRate, groupParam.GetValue(EnvironmentProcessRecord::PARAM_RADIUS_RATE, float(0.0f)));
               CPPUNIT_ASSERT(dtUtil::Equivalent(testVec1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_VELOCITY, osg::Vec3f()), 0.01f));
               CPPUNIT_ASSERT_EQUAL(testVec2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, osg::Vec3f()));
            }
            else if (typeCode == EnvironmentProcessRecord::GaussianPuffRecordType)
            {
               CPPUNIT_ASSERT_EQUAL(7U, groupParam.GetValue("Index", 0U));
               CPPUNIT_ASSERT(dtUtil::Equivalent(testPos1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_LOCATION, osg::Vec3d()), 0.01));
               CPPUNIT_ASSERT_EQUAL(testVec1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_DIMENSION, osg::Vec3f()));
               CPPUNIT_ASSERT(dtUtil::Equivalent(testOrient, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ORIENTATION, osg::Vec3f()), 0.01f));
               CPPUNIT_ASSERT(dtUtil::Equivalent(testPos2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ORIGINATION_LOCATION, osg::Vec3d()), 0.01));
               CPPUNIT_ASSERT_EQUAL(testHeight, groupParam.GetValue(EnvironmentProcessRecord::PARAM_CENTROID_HEIGHT, float(0.0f)));
               CPPUNIT_ASSERT_EQUAL(testVec2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_DIMENSION_RATE, osg::Vec3f()));
               CPPUNIT_ASSERT(dtUtil::Equivalent(testVec1, groupParam.GetValue(EnvironmentProcessRecord::PARAM_VELOCITY, osg::Vec3f()), 0.01f));
               CPPUNIT_ASSERT_EQUAL(testVec2, groupParam.GetValue(EnvironmentProcessRecord::PARAM_ANGULAR_VELOCITY, osg::Vec3f()));
            }
            else
            {
               CPPUNIT_FAIL("The typecode should have matched the gaussian or the sphere record type.");
            }

         }


      }

      void TestOutgoingSpatialDataTranslation()
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         SetupSpatialMapping();

         dtCore::RefPtr<dtGame::IntMessageParameter> drParam = new dtGame::IntMessageParameter("drCode");
         dtCore::RefPtr<dtGame::BooleanMessageParameter> frozenParam = new dtGame::BooleanMessageParameter("frozen");
         dtCore::RefPtr<dtGame::Vec3MessageParameter> posParam = new dtGame::Vec3MessageParameter("pos");
         dtCore::RefPtr<dtGame::Vec3MessageParameter> rotParam = new dtGame::Vec3MessageParameter("rot");
         dtCore::RefPtr<dtGame::Vec3MessageParameter> velParam = new dtGame::Vec3MessageParameter("vel");
         dtCore::RefPtr<dtGame::Vec3MessageParameter> accelParam = new dtGame::Vec3MessageParameter("accel");
         dtCore::RefPtr<dtGame::Vec3MessageParameter> angVelParam = new dtGame::Vec3MessageParameter("angvel");

         osg::Vec3 testVecPos(5.0f, 4.3f, 73.9f);
         osg::Vec3 testVecRot(6.0f, 3.1f, 88.2f);
         osg::Vec3 testVecVel(0.01f, 13.3f, 1.9f);
         osg::Vec3 testVecAccel(23.1f, 9.0f, 1.1f);
         osg::Vec3 testVecAngVel(9.0f, 14.33f, 7.6f);

         posParam->SetValue(testVecPos);
         rotParam->SetValue(testVecRot);
         velParam->SetValue(testVecVel);
         accelParam->SetValue(testVecAccel);
         angVelParam->SetValue(testVecAngVel);

         messageParameters.push_back(drParam);
         messageParameters.push_back(frozenParam);
         messageParameters.push_back(posParam);
         messageParameters.push_back(rotParam);
         messageParameters.push_back(velParam);
         messageParameters.push_back(accelParam);
         messageParameters.push_back(angVelParam);

         for (unsigned i = 0; i < 10; ++i)
         {
            drParam->SetValue(i);
            //alternate frozen to make it test both possibilities
            frozenParam->SetValue((i % 2) == 1);
            TestSpatialData(messageParameters, drParam->GetValue(), frozenParam->GetValue(),
                     testVecPos, testVecRot, testVecVel, testVecAccel, testVecAngVel);
         }

         //test with null parameters
         messageParameters[4] = NULL;
         messageParameters[5] = NULL;
         messageParameters[6] = NULL;

         osg::Vec3 zeroVec;
         //test with null parameters
         for (unsigned i = 0; i < 10; ++i)
         {
            drParam->SetValue(i);
            //alternate frozen to make it test both possibilities
            frozenParam->SetValue((i % 2) == 0);
            TestSpatialData(messageParameters, drParam->GetValue(), frozenParam->GetValue(),
                     testVecPos, testVecRot, zeroVec, zeroVec, zeroVec);
         }

         messageParameters.clear();
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

      void TestOutgoingStringDataTranslation()
      {
         InternalTestOutgoingStringDataTranslation("01234", false);
         InternalTestOutgoingStringDataTranslation("01234567890", false);
         InternalTestOutgoingStringDataTranslation("012345678903234", false);

         dtCore::UniqueId testId;
         InternalTestOutgoingUniqueIdToStringDataTranslation(testId);
      }

      void TestOutgoingOctetDataTranslation()
      {
         InternalTestOutgoingStringDataTranslation("01234", true);
         InternalTestOutgoingStringDataTranslation("01234567890", true);
         std::string bigString;
         bigString.append(dtHLAGM::RPRAttributeType::OCTET_TYPE.GetEncodedLength(), 'a');
         InternalTestOutgoingStringDataTranslation(bigString, true);
         //Too long
         bigString.append(40, 'a');
         InternalTestOutgoingStringDataTranslation(bigString, true);
      }

      void TestOutgoingEulerAngleDataTranslation()
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         dtCore::RefPtr<dtGame::MessageParameter> msg;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::VEC3);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::EULER_ANGLES_TYPE);
         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         msg = vec3Param;
         osg::Vec3 testVec(1.5f, 3.11f, -2.73f);
         vec3Param->SetValue(testVec);
         messageParameters.push_back(vec3Param);

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

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::VEC3F);
         dtCore::RefPtr<dtGame::Vec3fMessageParameter> vec3fParam = new dtGame::Vec3fMessageParameter("test");
         osg::Vec3f testVecf(1.5f, 3.11f, -2.73f);
         vec3fParam->SetValue(testVecf);
         messageParameters.push_back(vec3fParam);

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         eua.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         //must remember to reorder the vector since actor properties store rotation in a different order.
         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), eua.GetPsi(), 1e-6f) &&
                        osg::equivalent(expectedVec.y(), eua.GetTheta(), 1e-6f) &&
                        osg::equivalent(expectedVec.z(), eua.GetPhi(), 1e-6f) );

         //Now testing Vec3d with the same data.

         messageParameters.clear();

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::VEC3D);
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
         InternalTestOutgoingVectorDataTranslation(dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE);
      }

      void TestOutgoingAngularVectorDataTranslation()
      {
         InternalTestOutgoingVectorDataTranslation(dtHLAGM::RPRAttributeType::ANGULAR_VELOCITY_VECTOR_TYPE);
      }

      void TestOutgoingArticulationDataTranslation()
      {
         // Set the mapping direction
         mMapping.SetHLAType( dtHLAGM::RPRAttributeType::ARTICULATED_PART_TYPE );
         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType( dtCore::DataType::GROUP );

         // NOTE: All values used are arbitrary and have no relevance to the
         //       real world values used in articulations.

         dtHLAGM::ArticulatedParts articParts;
         articParts.SetClass( 99 );
         articParts.SetTypeMetric( 11 );
         articParts.SetValue( 987.654f );
         pd.AddEnumerationMapping("99","testDOF");
         pd.AddEnumerationMapping("11","Azimuth"); // "Azimuth" is 1 of 16 possible Metric names.

         dtHLAGM::EntityType dis( 5, 10, 15, 20, 25, 30, 35 );
         pd.AddEnumerationMapping("5 10 15 20 25 30 35","testDIS");

         dtHLAGM::AttachedParts attachParts;
         attachParts.SetStation( 246 );
         attachParts.SetStoreType( dis );

         dtHLAGM::ParameterValue articValue;
         articValue.SetArticulatedParts( articParts );
         articValue.SetAttachedParts( attachParts );

         dtHLAGM::ArticulatedParameter articParam;
         articParam.SetArticulatedParameterChange( 123 );
         articParam.SetParameterValue( articValue );
         articParam.SetPartAttachedTo( 1 ); // DOF2 @ index 1
         pd.AddEnumerationMapping("2000","Parent_DOF2"); // DOF2

         // NOTE: Internal tests will add 2 extra ArticulatedParameters to the
         //       articulations array while adding this test parameter object to
         //       the end at index 2.
         pd.AddEnumerationMapping("1000","DOF1");
         pd.AddEnumerationMapping("0","Parent_DOF0"); // Base Hull (0)
         pd.AddEnumerationMapping("2000","DOF2");
         pd.AddEnumerationMapping("1000","Parent_DOF1"); // DOF1


         // Test Articulated Parts
         articValue.SetArticulatedParameterType( (dtHLAGM::ArticulatedParameterType) 1 );
         articParam.SetParameterValue( articValue );
         InternalTestOutgoingArticulationDataTranslation( articParam );

         // Test Attached Parts
         articValue.SetArticulatedParameterType( (dtHLAGM::ArticulatedParameterType) 0 );
         articParam.SetParameterValue( articValue );
         InternalTestOutgoingArticulationDataTranslation( articParam );
      }

      void InternalTestOutgoingArticulationDataTranslation(const dtHLAGM::ArticulatedParameter& expectedResult)
      {
         // Create the array for articulated parameters.
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         // The main group parameter "value" to be assigned
         dtCore::RefPtr<dtGame::GroupMessageParameter> mainParam = new dtGame::GroupMessageParameter( "Articulated Parameters Array" );

         // The sub group of parameters pertaining to either an
         // ArticulatedPart or an AttachPart
         dtCore::RefPtr<dtGame::GroupMessageParameter> subParam = NULL;

         // Create other articulated parameters to lengthen the array
         // --- Extra 0
         subParam = new dtGame::GroupMessageParameter( "ArticulatedPartMessageParam0" );
         subParam->AddParameter( *new dtGame::FloatMessageParameter( "Azimuth", 0.0f ) );
         subParam->AddParameter( *new dtGame::StringMessageParameter( "OurName", "DOF1" ) );
         subParam->AddParameter( *new dtGame::StringMessageParameter( "OurParent", "Parent_DOF0" ) );
         subParam->AddParameter( *new dtGame::UnsignedShortIntMessageParameter( "Change", 0 ) );
         mainParam->AddParameter( *subParam );
         // --- Extra 1
         subParam = new dtGame::GroupMessageParameter( "ArticulatedPartMessageParam1" );
         subParam->AddParameter( *new dtGame::FloatMessageParameter( "Azimuth", 0.0f ) );
         subParam->AddParameter( *new dtGame::StringMessageParameter( "OurName", "DOF2" ) );
         subParam->AddParameter( *new dtGame::StringMessageParameter( "OurParent", "Parent_DOF1" ) );
         subParam->AddParameter( *new dtGame::UnsignedShortIntMessageParameter( "Change", 0 ) );
         mainParam->AddParameter( *subParam );

         // Create the test articulated parameter
         const dtHLAGM::ParameterValue& articValue = expectedResult.GetParameterValue();
         if( articValue.GetArticulatedParameterType() == 1 )
         {
            const dtHLAGM::AttachedParts& attachParts = articValue.GetAttachedParts();

            // Attach Parts
            subParam = new dtGame::GroupMessageParameter( "AttachedPartMessageParam0" );

            subParam->AddParameter( *new dtGame::EnumMessageParameter( "DISInfo", "testDIS" ) );
            subParam->AddParameter( *new dtGame::UnsignedIntMessageParameter( "Station", attachParts.GetStation() ) );
         }
         else
         {
            const dtHLAGM::ArticulatedParts& articParts = articValue.GetArticulatedParts();

            // Articulated Parts
            subParam = new dtGame::GroupMessageParameter( "ArticulatedPartMessageParam2" );

            subParam->AddParameter( *new dtGame::FloatMessageParameter( "Azimuth", articParts.GetValue() ) );
            subParam->AddParameter( *new dtGame::StringMessageParameter( "OurName", "testDOF" ) );
         }

         subParam->AddParameter( *new dtGame::StringMessageParameter( "OurParent", "Parent_DOF2" ) );
         subParam->AddParameter( *new dtGame::UnsignedShortIntMessageParameter( "Change", expectedResult.GetArticulatedParameterChange() ) );

         // Assign the group as a value to the main group parameter
         mainParam->AddParameter( *subParam );

         // Assign the parameter to be translated
         messageParameters.push_back(mainParam.get());

         // Encode the message parameter into the buffer.
         char* buffer = NULL;
         size_t size = 0;
         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         // Test expected size of the returned buffer.
         unsigned expectedBufferSize = expectedResult.EncodedLength() * mainParam->GetParameterCount();
         CPPUNIT_ASSERT_EQUAL(unsigned(size), unsigned(expectedBufferSize));

         // Capture the last parameter from the buffer; this should be the translated test parameter.
         dtHLAGM::ArticulatedParameter actualOutgoingParam;
         actualOutgoingParam.Decode(&buffer[expectedBufferSize-expectedResult.EncodedLength()]);

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT( actualOutgoingParam.IsEqual( expectedResult ) );
      }

      void TestOutgoingEnumDataTranslation()
      {
         std::string expectedResult("33");
         int iExpectedResult = 33;

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];

         pd.SetGameType(dtCore::DataType::ENUMERATION);

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
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::STRING_TYPE);
         InternalTestOutgoingEnumToStringDataTranslation(expectedResult);

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
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::STRING_TYPE);
         InternalTestOutgoingEnumToStringDataTranslation("1");
      }

      void TestOutgoingEntityTypeEnumDataTranslation()
      {
         dtHLAGM::EntityType expectedResult(2, 2, 225, 2, 2, 0, 0);
         std::string sExpectedResult("2 2 225 2 2 0 0");

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];

         pd.SetGameType(dtCore::DataType::ENUMERATION);

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
         dtHLAGM::EntityType expectedResult_2(2, 8, 342, 3, 8, 3, 9);
         InternalTestOutgoingEntityTypeDataTranslation(expectedResult_2);

         // Test to be sure that entity type is not outgoing redundantly through
         // a mapping to a game type string parameter.
         InternalTestOutgoingStringToEntityTypeDataTranslation(expectedResult_2,"2 8 342 3 8 3 9");
      }

      void TestOutgoingIntDataTranslation()
      {
         unsigned int expectedResult = 33;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::UINT);

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

      void TestOutgoingActorIdToRTIIDStructDataTranslation()
      {
         std::string rtiId = "RTIObjectIdentifierStruct:TestID";
         dtCore::UniqueId actorId;

         // Map the RTI ID to an Actor ID
         mRuntimeMappings.Put(rtiId,actorId);

         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE);
         InternalTestOutgoingRTIIDTypeDataTranslation(actorId.ToString(), rtiId, dtCore::DataType::ACTOR);
      }

      void TestOutgoingStringToRTIIDStructDataTranslation()
      {
         std::string rtiId = "RTIObjectIdentifierStruct:TestID";

         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE);
         InternalTestOutgoingRTIIDTypeDataTranslation(rtiId, rtiId, dtCore::DataType::STRING);
      }

      void TestIncomingSpatialDataTranslation()
      {
         dtHLAGM::Spatial spatial;
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         SetupSpatialMapping();

         spatial.GetWorldCoordinate().set(3113.4, 532.7, 8333.8);
         spatial.GetOrientation().set(2.717f, 3.33f, 1.078f);
         spatial.GetVelocity().set(1.1f, 3.3f, -13.3f);
         spatial.GetAcceleration().set(0.11f, -3.22f, -1.0f);
         spatial.GetAngularVelocity().set(-2.22f, 1.1f, 0.01f);

         osg::Vec3 expectedVecPos = mCoordinates.ConvertToLocalTranslation(spatial.GetWorldCoordinate());
         osg::Vec3 expectedVecRot = mCoordinates.ConvertToLocalRotation(spatial.GetOrientation());
         osg::Vec3f expectedVecVel = mCoordinates.GetOriginRotationMatrix().preMult(spatial.GetVelocity());
         osg::Vec3f expectedVecAccel = mCoordinates.GetOriginRotationMatrix().preMult(spatial.GetAcceleration());

         for (char i = 0; i < 10; ++i)
         {
            spatial.SetDeadReckoningAlgorithm(i);
            spatial.SetFrozen( (i % 2) == 0 );

            messageParameters.clear();

            dtCore::RefPtr<dtGame::IntMessageParameter> drParam = new dtGame::IntMessageParameter("drCode");
            dtCore::RefPtr<dtGame::BooleanMessageParameter> frozenParam = new dtGame::BooleanMessageParameter("frozen");
            dtCore::RefPtr<dtGame::Vec3MessageParameter> posParam = new dtGame::Vec3MessageParameter("pos");
            dtCore::RefPtr<dtGame::Vec3MessageParameter> rotParam = new dtGame::Vec3MessageParameter("rot");
            dtCore::RefPtr<dtGame::Vec3MessageParameter> velParam = new dtGame::Vec3MessageParameter("vel");
            dtCore::RefPtr<dtGame::Vec3MessageParameter> accelParam = new dtGame::Vec3MessageParameter("accel");
            dtCore::RefPtr<dtGame::Vec3MessageParameter> angVelParam = new dtGame::Vec3MessageParameter("angvel");

            messageParameters.push_back(drParam.get());
            messageParameters.push_back(frozenParam.get());
            messageParameters.push_back(posParam.get());
            messageParameters.push_back(rotParam.get());
            messageParameters.push_back(velParam.get());
            messageParameters.push_back(accelParam.get());
            messageParameters.push_back(angVelParam.get());

            char* buffer = new char[512];
            try
            {

               size_t size = spatial.Encode(buffer, 512);
               mParameterTranslator->MapToMessageParameters(buffer, size, messageParameters, mMapping);

               delete[] buffer;
            }
            catch (...)
            {
               delete[] buffer;
            }

            CPPUNIT_ASSERT_EQUAL(spatial.GetDeadReckoningAlgorithm(), char(drParam->GetValue()));
            CPPUNIT_ASSERT(messageParameters[0].valid());

            CPPUNIT_ASSERT_EQUAL(spatial.IsFrozen(), frozenParam->GetValue());
            CPPUNIT_ASSERT(messageParameters[1].valid());

            CPPUNIT_ASSERT(messageParameters[2].valid());
            CPPUNIT_ASSERT_EQUAL(expectedVecPos, posParam->GetValue());

            CPPUNIT_ASSERT(messageParameters[3].valid());
            CPPUNIT_ASSERT_EQUAL(osg::Vec3(expectedVecRot[1], expectedVecRot[2], expectedVecRot[0]),
                     rotParam->GetValue());

            if (spatial.HasVelocity())
            {
               CPPUNIT_ASSERT(messageParameters[4].valid());
               CPPUNIT_ASSERT_EQUAL(expectedVecVel, velParam->GetValue());
            }
            else
            {
               CPPUNIT_ASSERT(!messageParameters[4].valid());
            }

            if (spatial.HasAcceleration())
            {
               CPPUNIT_ASSERT(messageParameters[5].valid());
               CPPUNIT_ASSERT_EQUAL(expectedVecAccel, accelParam->GetValue());
            }
            else
            {
               CPPUNIT_ASSERT(!messageParameters[5].valid());
            }

            if (spatial.HasAngularVelocity())
            {
               CPPUNIT_ASSERT_EQUAL(osg::Vec3(spatial.GetAngularVelocity()), angVelParam->GetValue());
               CPPUNIT_ASSERT(messageParameters[6].valid());
            }
            else
            {
               CPPUNIT_ASSERT(!messageParameters[6].valid());
            }
         }
      }

      void TestIncomingRTIIDStructToActorIdDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         std::string rtiId = "RTIObjectIdentifierStruct:TestID";
         dtCore::UniqueId actorId;

         // Map the RTI ID to an Actor ID
         mRuntimeMappings.Put(rtiId,actorId);

         dtCore::RefPtr<dtGame::ActorMessageParameter> actorParam =
            new dtGame::ActorMessageParameter("test",actorId);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE);

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType(actorParam->GetDataType());

         messageParameters.push_back(actorParam.get());

         mParameterTranslator->MapToMessageParameters(rtiId.c_str(), rtiId.size(), messageParameters, mMapping);

         std::string paramValue = actorParam->GetValue().ToString();
         CPPUNIT_ASSERT_EQUAL(actorId.ToString(), paramValue);
      }

      void TestIncomingRTIIDStructToStringDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         std::string rtiId = "RTIObjectIdentifierStruct:TestID";

         dtCore::RefPtr<dtGame::StringMessageParameter> stringParam =
            new dtGame::StringMessageParameter("test","");
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE);

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType(stringParam->GetDataType());

         messageParameters.push_back(stringParam.get());

         mParameterTranslator->MapToMessageParameters(rtiId.c_str(), rtiId.size(), messageParameters, mMapping);

         std::string paramValue = stringParam->GetValue();
         CPPUNIT_ASSERT_EQUAL(rtiId, paramValue);
      }

      //////////////////////////////////////////////////////////////////////////
      void TestIncomingEntityTypeDataTranslation()
      {
         // Container for parameters that need to have their values set
         // by incoming HLA parameters.
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::ENTITY_TYPE);

         // pd is contained in the mMapping object and its settings will be carried
         // into the MapToMessageParameters method via mMapping being passed in by
         // reference. This tells the translator how to convert the incoming HLA parameter.
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


         // Get ready for the next test, involving the conversion of the incoming
         // HLA Parameter Type Entity Type to Game Parameter Type STRING.
         messageParameters.clear();

         // The target game type for the incoming Entity Type will be set to
         // Game Type STRING.
         std::string expectedStrValue("2 9 111 1 0 3 9");
         dtCore::RefPtr<dtGame::StringMessageParameter> strParam = new dtGame::StringMessageParameter("Test String Param");
         pd.SetGameType( strParam->GetDataType() );

         // Add the parameter to the messageParameter list so that it can receive
         // the value of the Entity Type encoding, currently encoded in the buffer.
         messageParameters.push_back( strParam.get() );

         // Let the parameter translator perform its work, setting the string value
         // of the incoming Entity Type into the string parameter contained in the
         // container, messageParameters.
         mParameterTranslator->MapToMessageParameters(mBuffer, inputValue2.EncodedLength(), messageParameters, mMapping);

         // Translation of Entity Type to STRING should have been performed successfully.
         CPPUNIT_ASSERT_EQUAL(expectedStrValue, strParam->GetValue());
      }

      //////////////////////////////////////////////////////////////////////////
      void TestIncomingVelocityVectorDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE);

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType(vec3Param->GetDataType());

         messageParameters.push_back(vec3Param.get());

         dtHLAGM::VelocityVector inputValue(1.5f, 2.3f, 3.3f);
         inputValue.Encode(mBuffer);

         mParameterTranslator->MapToMessageParameters(mBuffer, inputValue.EncodedLength(), messageParameters, mMapping);

         osg::Vec3 expectedValue;
         expectedValue =  mCoordinates.GetOriginRotationMatrix().preMult(osg::Vec3(inputValue.GetX(), inputValue.GetY(), inputValue.GetZ()));

         CPPUNIT_ASSERT_EQUAL(expectedValue, vec3Param->GetValue());

         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE);
         mParameterTranslator->MapToMessageParameters(mBuffer, inputValue.EncodedLength(), messageParameters, mMapping);
         //The value should be passed as is.
         expectedValue.set(inputValue.GetX(), inputValue.GetY(), inputValue.GetZ());

         CPPUNIT_ASSERT_EQUAL(expectedValue, vec3Param->GetValue());
      }

      void TestIncomingStringToEnumDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::STRING_TYPE);

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType(enumParam->GetDataType());
         pd.AddEnumerationMapping("horse", "hello");
         pd.AddEnumerationMapping("chicken", "correct");
         pd.SetDefaultValue("hello");

         messageParameters.push_back(enumParam.get());

         std::string value("chicken");
         for (unsigned i = 0; i < value.size(); ++i)
         {
            mBuffer[i] = value[i];
         }
         mBuffer[value.size()] = '\0';

         mParameterTranslator->MapToMessageParameters(mBuffer, value.size(), messageParameters, mMapping);

         std::string expectedValue("correct");

         CPPUNIT_ASSERT_EQUAL(expectedValue, enumParam->GetValue());

         value = "booga";
         for (unsigned i = 0; i < value.size(); ++i)
         {
            mBuffer[i] = value[i];
         }
         mBuffer[value.size()] = '\0';

         expectedValue = "hello";

         mParameterTranslator->MapToMessageParameters(mBuffer, value.size(), messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL(expectedValue, enumParam->GetValue());
      }

      void TestIncomingStringDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtCore::RefPtr<dtGame::StringMessageParameter> stringParam = new dtGame::StringMessageParameter("test");
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::STRING_TYPE);

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType(stringParam->GetDataType());

         messageParameters.push_back(stringParam.get());

         const std::string testValue("bigTest");
         const std::string testValueJunkAfterNull("bigTest\0test");

         mParameterTranslator->MapToMessageParameters(testValueJunkAfterNull.c_str(), testValueJunkAfterNull.size(), messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The string should have been passed correctly, stopping at the NULL",
                  testValue, stringParam->GetValue());

         //clear the string to make sure the call isn't a no-op since the result should be the same
         //as last time.
         stringParam->SetValue("");
         mParameterTranslator->MapToMessageParameters(testValue.c_str(), testValue.size(), messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The string should have been passed correctly, stopping at the end of the buffer",
                  testValue, stringParam->GetValue());

         dtCore::RefPtr<dtGame::ActorMessageParameter> actorParam = new dtGame::ActorMessageParameter("test");
         pd.SetGameType(actorParam->GetDataType());

         messageParameters.clear();
         messageParameters.push_back(actorParam.get());

         mParameterTranslator->MapToMessageParameters(testValue.c_str(), testValue.size(), messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL(testValue, actorParam->GetValue().ToString());
      }

      void TestIncomingOctetDataTranslation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtCore::RefPtr<dtGame::StringMessageParameter> stringParam = new dtGame::StringMessageParameter("test");
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::OCTET_TYPE);

         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         pd.SetGameType(stringParam->GetDataType());

         messageParameters.push_back(stringParam.get());

         const std::string testValue("bigTest\0test\0My Test Is very good \r\t\0 \0 \0");

         mParameterTranslator->MapToMessageParameters(testValue.c_str(), testValue.size(), messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The message should have passed, even with null value in tact.",
                  testValue, stringParam->GetValue());
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
         float testDouble = 344.9f;

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

      void TestIncomingArticulation()
      {
         std::vector<dtCore::RefPtr<dtGame::MessageParameter> > messageParameters;

         dtHLAGM::ArticulatedParts ap(1212, 8, -20.21f);
         dtHLAGM::ArticulatedParameter artParam(0,0, dtHLAGM::ParameterValue(ap));
         artParam.Encode(mBuffer);

         dtHLAGM::ArticulatedParts ap2(2231, 8, -13.61f);
         dtHLAGM::ArticulatedParameter artParam2(1, 2231, dtHLAGM::ParameterValue(ap2));
         artParam2.Encode(mBuffer + artParam.EncodedLength());

         EntityType et(1, 1 , 225, 3, 4, 5, 6);
         dtHLAGM::AttachedParts at(1, et);
         dtHLAGM::ArticulatedParameter artParam3(0, 3334, dtHLAGM::ParameterValue(at));
         artParam3.Encode(mBuffer + 2 * artParam.EncodedLength());

         dtCore::RefPtr<dtGame::GroupMessageParameter> groupParam = new dtGame::GroupMessageParameter("Group Test");
         messageParameters.push_back(groupParam.get());

         dtHLAGM::OneToManyMapping oneToMany("default", dtHLAGM::RPRAttributeType::ARTICULATED_PART_TYPE, false);
         dtHLAGM::OneToManyMapping::ParameterDefinition pd("ArticulatedParameter", dtCore::DataType::GROUP, "", false);
         pd.AddEnumerationMapping("3334", "dof_notadof");
         pd.AddEnumerationMapping("2231", "dof_fakerz");
         oneToMany.GetParameterDefinitions().push_back(pd);

         mParameterTranslator->MapToMessageParameters(mBuffer, artParam.EncodedLength() * 3, messageParameters, oneToMany);

         dtGame::MessageParameter* msgParam = groupParam.get()->GetParameter("AttachedPartMessageParam0");
         CPPUNIT_ASSERT_MESSAGE("Did not find an articulated message param that should have been added", msgParam != NULL);

         msgParam = groupParam.get()->GetParameter("ArticulatedPartMessageParam1");
         CPPUNIT_ASSERT_MESSAGE("Did not find the articulated message param that should have been added", msgParam != NULL);

         msgParam = groupParam.get()->GetParameter("ArticulatedPartMessageParam0");
         CPPUNIT_ASSERT_MESSAGE("Did not find the articulated message param that should have been added", msgParam != NULL);

         if(msgParam->GetDataType() == dtCore::DataType::GROUP)
         {
            dtGame::MessageParameter* Value = (*(dtGame::GroupMessageParameter*)&(*msgParam)).GetParameter("LocationYRate");
            CPPUNIT_ASSERT_MESSAGE("Did not find the locationyRate value that should have been there", Value != NULL);
            float yrate = (*(dtGame::FloatMessageParameter*)&(*Value)).GetValue();
            bool check = false;
            if((yrate -1) < -20.21 && (yrate + 1) > -20.21)
               check = true;
            CPPUNIT_ASSERT_MESSAGE("YRate is not correct!", check );
         }
         else
         {
            CPPUNIT_ASSERT_MESSAGE("Should have been a group msg param...", msgParam->GetDataType() != dtCore::DataType::GROUP);
         }
      }

      void TestFindTypeByName()
      {
         for (unsigned int i = 0; i < dtHLAGM::RPRAttributeType::EnumerateType().size(); ++i)
         {
            //Test all the types to see if it's working properly
            CPPUNIT_ASSERT(mParameterTranslator->GetAttributeTypeForName(dtHLAGM::RPRAttributeType::EnumerateType()[i]->GetName()) ==
               *dtHLAGM::RPRAttributeType::EnumerateType()[i]);
         }
         //Make sure it returns unknown for anything else.
         CPPUNIT_ASSERT(mParameterTranslator->GetAttributeTypeForName("Some Junk") ==
            dtHLAGM::AttributeType::UNKNOWN);
      }

      void TestAttributeSupportedQuery()
      {
         for (unsigned int i = 0; i < dtHLAGM::RPRAttributeType::EnumerateType().size(); ++i)
         {
            //Test all the types to see if it's working properly
            CPPUNIT_ASSERT(mParameterTranslator->TranslatesAttributeType(*dtHLAGM::RPRAttributeType::EnumerateType()[i]));
         }

         //Make sure UNKNOWN is not supported.
         CPPUNIT_ASSERT(!mParameterTranslator->TranslatesAttributeType(dtHLAGM::AttributeType::UNKNOWN));
      }

   private:
      dtUtil::Log* mLogger;
      dtCore::RefPtr<dtHLAGM::TestRPRParamTranslator> mParameterTranslator;
      dtUtil::Coordinates mCoordinates;
      dtHLAGM::ObjectRuntimeMappingInfo mRuntimeMappings;
      osg::Endian mEndian;
      //shared buffer.
      char* mBuffer;
      dtHLAGM::AttributeToPropertyList mMapping;

      void SetupSpatialMapping()
      {
         mMapping.GetParameterDefinitions()[0].SetGameName("drCode");
         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::INT);

         dtHLAGM::OneToManyMapping::ParameterDefinition pdFrozen;
         pdFrozen.SetGameName("frozen");
         pdFrozen.SetGameType(dtCore::DataType::BOOLEAN);
         mMapping.GetParameterDefinitions().push_back(pdFrozen);

         dtHLAGM::OneToManyMapping::ParameterDefinition pdWorldCoord;
         pdFrozen.SetGameName("pos");
         pdFrozen.SetGameType(dtCore::DataType::VEC3);
         mMapping.GetParameterDefinitions().push_back(pdWorldCoord);

         dtHLAGM::OneToManyMapping::ParameterDefinition pdOrient;
         pdFrozen.SetGameName("rot");
         pdFrozen.SetGameType(dtCore::DataType::VEC3);
         mMapping.GetParameterDefinitions().push_back(pdOrient);

         dtHLAGM::OneToManyMapping::ParameterDefinition pdVel;
         pdFrozen.SetGameName("vel");
         pdFrozen.SetGameType(dtCore::DataType::VEC3);
         mMapping.GetParameterDefinitions().push_back(pdVel);

         dtHLAGM::OneToManyMapping::ParameterDefinition pdAccel;
         pdFrozen.SetGameName("accel");
         pdFrozen.SetGameType(dtCore::DataType::VEC3);
         mMapping.GetParameterDefinitions().push_back(pdAccel);

         dtHLAGM::OneToManyMapping::ParameterDefinition pdAngVel;
         pdFrozen.SetGameName("angvel");
         pdFrozen.SetGameType(dtCore::DataType::VEC3);
         mMapping.GetParameterDefinitions().push_back(pdAngVel);

         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::SPATIAL_TYPE);
      }

      void TestSpatialData(
               std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& messageParameters,
               char drValue,
               bool frozen,
               const osg::Vec3& testVecPos,
               const osg::Vec3& testVecRot,
               const osg::Vec3& testVecVel,
               const osg::Vec3& testVecAccel,
               const osg::Vec3& testVecAngVel)
      {
         char* buffer = NULL;
         size_t size = 0;

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         osg::Vec3d expectedVecPos = mCoordinates.ConvertToRemoteTranslation(testVecPos);
         osg::Vec3f expectedVecRot = mCoordinates.ConvertToRemoteRotation(osg::Vec3(testVecRot[2], testVecRot[0], testVecRot[1]));
         osg::Vec3f expectedVecVel = mCoordinates.GetOriginRotationMatrixInverse().preMult(testVecVel);
         osg::Vec3f expectedVecAccel = mCoordinates.GetOriginRotationMatrixInverse().preMult(testVecAccel);
         osg::Vec3f expectedVecAngVel = testVecAngVel;

         dtHLAGM::Spatial spatial;

         spatial.Decode(buffer, size);
         mParameterTranslator->DeallocateBuffer(buffer);

         osg::Vec3 zeroVec;

         CPPUNIT_ASSERT_EQUAL(drValue, spatial.GetDeadReckoningAlgorithm());
         CPPUNIT_ASSERT_EQUAL(frozen, spatial.IsFrozen());
         CPPUNIT_ASSERT(dtUtil::Equivalent(expectedVecPos, osg::Vec3d(spatial.GetWorldCoordinate()), 3, 1e-5));
         CPPUNIT_ASSERT(dtUtil::Equivalent(expectedVecRot, osg::Vec3f(spatial.GetOrientation()), 3, 1e-3f));

         if (!spatial.HasVelocity())
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(zeroVec, osg::Vec3f(spatial.GetVelocity()), 3, 1e-3f));
         }
         else
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(expectedVecVel, osg::Vec3f(spatial.GetVelocity()), 3, 1e-3f));
         }

         if (!spatial.HasAcceleration())
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(zeroVec, osg::Vec3f(spatial.GetAcceleration()), 3, 1e-3f));
         }
         else
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(expectedVecAccel, osg::Vec3f(spatial.GetAcceleration()), 3, 1e-3f));
         }

         if (!spatial.HasAngularVelocity())
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(zeroVec, osg::Vec3f(spatial.GetAngularVelocity()), 3, 1e-3f));
         }
         else
         {
            CPPUNIT_ASSERT(dtUtil::Equivalent(expectedVecAngVel, osg::Vec3f(spatial.GetAngularVelocity()), 3, 1e-3f));
         }

      }

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
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type unsigned int",
            unsigned(expectedResult), uIntParam->GetValue());

         //Int param
         dtCore::RefPtr<dtGame::IntMessageParameter> intParam = new dtGame::IntMessageParameter("test");
         RunIncomingTranslation(*intParam, mapping, bufferSize);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type int",
            int(expectedResult), intParam->GetValue());

         //short param
         dtCore::RefPtr<dtGame::ShortIntMessageParameter> shortParam = new dtGame::ShortIntMessageParameter("test");
         RunIncomingTranslation(*shortParam, mapping, bufferSize);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type short",
            (short int)expectedResult, shortParam->GetValue());

         //unsigned short
         dtCore::RefPtr<dtGame::UnsignedShortIntMessageParameter> uShortParam = new dtGame::UnsignedShortIntMessageParameter("test");
         RunIncomingTranslation(*uShortParam, mapping, bufferSize);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type unsigned short",
            (unsigned short)expectedResult, uShortParam->GetValue());

         //unsigned char
         dtCore::RefPtr<dtGame::UnsignedCharMessageParameter> uCharParam = new dtGame::UnsignedCharMessageParameter("test");
         RunIncomingTranslation(*uCharParam, mapping, bufferSize);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Value for " + mapping.GetHLAType().GetName() + " data should assign a param of type unsigned char",
            (unsigned char)expectedResult, (unsigned char)uCharParam->GetValue());

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

      void InternalTestOutgoingVectorDataTranslation(const dtHLAGM::RPRAttributeType& type)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::VEC3);
         mMapping.SetHLAType(type);
         dtCore::RefPtr<dtGame::Vec3MessageParameter> vec3Param = new dtGame::Vec3MessageParameter("test");
         osg::Vec3 testVec(1.5f, 3.11f, -2.73f);
         vec3Param->SetValue(testVec);
         messageParameters.push_back(vec3Param.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         osg::Vec3 expectedVec;
         if (type == dtHLAGM::RPRAttributeType::VELOCITY_VECTOR_TYPE)
            expectedVec =  mCoordinates.GetOriginRotationMatrix().preMult(testVec);
         else
            expectedVec = testVec;

         dtHLAGM::VelocityVector vv;

         vv.Decode(buffer);
         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT(osg::equivalent(expectedVec.x(), vv.GetX(), 1e-6f) &&
                        osg::equivalent(expectedVec.y(), vv.GetY(), 1e-6f) &&
                        osg::equivalent(expectedVec.z(), vv.GetZ(), 1e-6f) );
      }


      void InternalTestOutgoingMarkingTypeDataTranslation(const std::string& testValue)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::STRING);
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

         CPPUNIT_ASSERT_MESSAGE("The result should have been \"" + expectedValue + "\" but it is \"" + result + "\"",
                  result == expectedValue);
      }

      void InternalTestOutgoingRTIIDTypeDataTranslation(const std::string& testValue,
               const std::string& expectedValue, dtCore::DataType& dataType)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dataType);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::RTI_OBJECT_ID_STRUCT_TYPE);
         dtCore::RefPtr<dtGame::MessageParameter> param = dtCore::NamedParameter::CreateFromType(dataType, "test");
         param->FromString(testValue);
         messageParameters.push_back(param.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         std::string result;
         for (unsigned i = 0; i < size; ++i)
         {
            if (buffer[i] == '\0')
               break;

            result.append(1, buffer[i]);
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT_EQUAL(expectedValue, result);

      }

      void InternalTestOutgoingStringDataTranslation(const std::string& testValue, bool useOctet)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::STRING);
         if (useOctet)
         {
            mMapping.SetHLAType(dtHLAGM::RPRAttributeType::OCTET_TYPE);
         }
         else
         {
            mMapping.SetHLAType(dtHLAGM::RPRAttributeType::STRING_TYPE);
         }

         dtCore::RefPtr<dtGame::StringMessageParameter> stringParam = new dtGame::StringMessageParameter("test");
         stringParam->SetValue(testValue);
         messageParameters.push_back(stringParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         if (useOctet)
         {
            if (testValue.size() > dtHLAGM::RPRAttributeType::OCTET_TYPE.GetEncodedLength())
            {
               CPPUNIT_ASSERT_EQUAL_MESSAGE("The size of the buffer should be capped at the max length.",
                        dtHLAGM::RPRAttributeType::OCTET_TYPE.GetEncodedLength(), size);
            }
            else
            {
               CPPUNIT_ASSERT_EQUAL_MESSAGE("The size of the buffer should match the string length.",
                        testValue.size(), size);
            }
         }
         else
         {
            CPPUNIT_ASSERT_EQUAL_MESSAGE("The size of the buffer should match the string length plus one.",
                     testValue.size() + 1, size);
         }

         std::string result;
         result.reserve(size);
         for (unsigned i = 0; i < size; ++i)
         {
            if (buffer[i] == '\0' && !useOctet)
               break;

            result.append(1, buffer[i]);
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         //This variable takes care of max size caps
         const std::string testValueSub = testValue.substr(0, size);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The result should have been \"" + testValueSub + "\" but it is \"" + result + "\"",
                  testValueSub, result);
      }

      void InternalTestOutgoingUniqueIdToStringDataTranslation(const dtCore::UniqueId& testValue)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::ACTOR);
         mMapping.SetHLAType(dtHLAGM::RPRAttributeType::STRING_TYPE);
         dtCore::RefPtr<dtGame::ActorMessageParameter> actorParam = new dtGame::ActorMessageParameter("test");
         actorParam->SetValue(testValue);
         messageParameters.push_back(actorParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The size of the buffer should match the string length plus one.", testValue.ToString().size() + 1, size);

         std::string result;
         result.reserve(size);
         for (unsigned i = 0; i < size; ++i)
         {
            if (buffer[i] == '\0')
               break;

            result.append(1, buffer[i]);
         }

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT_EQUAL_MESSAGE("The result should have been \"" + testValue.ToString() + "\" but it is \"" + result + "\"", testValue.ToString(), result);
      }

      void TranslateOutgoingParameter(char*& buffer, size_t& size,
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> >& messageParameters, dtHLAGM::AttributeToPropertyList& mapping)
      {
         mParameterTranslator->AllocateBuffer(buffer, size, mapping.GetHLAType());

         CPPUNIT_ASSERT(buffer != NULL);

         if (size != mapping.GetHLAType().GetEncodedLength())
         {
            mParameterTranslator->DeallocateBuffer(buffer);
            std::ostringstream ss;
            ss << "the size of the buffer for a " << mapping.GetHLAType().GetName() << " is not correct.  It was \""
               << size << "\" but it should have been \"" <<  mapping.GetHLAType().GetEncodedLength() << "\".";
            CPPUNIT_FAIL(ss.str());
         }

         mParameterTranslator->MapFromMessageParameters(buffer, size, messageParameters, mapping);

      }

      void InternalTestOutgoingEnumToStringDataTranslation(std::string expectedResult)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         enumParam->SetValue("correct");
         messageParameters.push_back(enumParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);

         std::string actualValue(buffer);

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT_EQUAL(expectedResult, actualValue);
         CPPUNIT_ASSERT_EQUAL_MESSAGE("The size of the outgoing string should be length of the string plus 1", actualValue.size() + 1, size);
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

      //////////////////////////////////////////////////////////////////////////
      void InternalTestOutgoingEntityTypeDataTranslation(const dtHLAGM::EntityType& expectedResult)
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam = new dtGame::EnumMessageParameter("test");
         enumParam->SetValue("correct");
         messageParameters.push_back(enumParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);
         CPPUNIT_ASSERT_EQUAL(unsigned(size), unsigned(expectedResult.EncodedLength()));

         dtHLAGM::EntityType actualValue;
         actualValue.Decode(buffer);

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT_EQUAL(expectedResult, actualValue);
      }

      //////////////////////////////////////////////////////////////////////////
      void InternalTestOutgoingStringToEntityTypeDataTranslation(const dtHLAGM::EntityType& expectedResult, const std::string& entityTypeValue )
      {
         std::vector<dtCore::RefPtr<const dtGame::MessageParameter> > messageParameters;

         char* buffer = NULL;
         size_t size = 0;

         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::STRING);
         dtCore::RefPtr<dtGame::StringMessageParameter> stringParam = new dtGame::StringMessageParameter("test");
         stringParam->SetValue(entityTypeValue);
         messageParameters.push_back(stringParam.get());

         TranslateOutgoingParameter(buffer, size, messageParameters, mMapping);
         CPPUNIT_ASSERT_EQUAL(unsigned(size), unsigned(expectedResult.EncodedLength()));

         dtHLAGM::EntityType actualValue;
         actualValue.Decode(buffer);

         mParameterTranslator->DeallocateBuffer(buffer);

         CPPUNIT_ASSERT_EQUAL(expectedResult, actualValue);
      }

      //////////////////////////////////////////////////////////////////////////
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
         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::DOUBLE);

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
         mMapping.GetParameterDefinitions()[0].SetGameType(dtCore::DataType::FLOAT);

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

      void TestGetAndSetInteger()
      {
         // Setup all non-integer parameter types to be translated to a number.
         // NOTE: The resource parameter can be any resource type. The particle
         //       system data type is used arbitrarily for a simple test on
         //       resource parameter mapping.
         dtCore::RefPtr<dtGame::ResourceMessageParameter> resParam
            = new dtGame::ResourceMessageParameter(dtCore::DataType::PARTICLE_SYSTEM,"testResourceParam");
         dtCore::RefPtr<dtGame::StringMessageParameter> strParam
            = new dtGame::StringMessageParameter("testStringParam");
         dtCore::RefPtr<dtGame::EnumMessageParameter> enumParam
            = new dtGame::EnumMessageParameter("testEnumParam");

         // --- Use a vector to allow looping through the parameters.
         std::vector<dtGame::MessageParameter*> params;
         params.push_back(resParam.get());
         params.push_back(strParam.get());
         params.push_back(enumParam.get());

         // Declare base values. These values will be used and expanded only by
         // resource parameters.
         std::vector<std::string> baseValues;
         baseValues.push_back("Apple");
         baseValues.push_back("Orange");
         baseValues.push_back("Banana");
         baseValues.push_back("Kiwi");
         baseValues.push_back("Grapes");
         baseValues.push_back("Tomato"); // default value, tested last

         // Setup the parameter definitions object
         // --- Prepare the mapping values
         //
         //     NOTE:
         //     Resource parameters format strings a certain way, thus making
         //     the "set" value not equal to the "get" value; this is undesired
         //     for tests. Rather than fighting the formatting, use mapping
         //     values that the resource parameter creates.
         std::vector<std::string> mappingValues;

         resParam->FromString(baseValues[0]);
         mappingValues.push_back(resParam->ToString());

         resParam->FromString(baseValues[1]);
         mappingValues.push_back(resParam->ToString());

         resParam->FromString(baseValues[2]);
         mappingValues.push_back(resParam->ToString());

         resParam->FromString(baseValues[3]);
         mappingValues.push_back(resParam->ToString());

         resParam->FromString(baseValues[4]);
         mappingValues.push_back(resParam->ToString());

         // --- Prepare mapping keys
         std::vector<std::string> mappingKeys;
         mappingKeys.push_back("1");
         mappingKeys.push_back("2");
         mappingKeys.push_back("3");
         mappingKeys.push_back("4");
         mappingKeys.push_back("5");

         // --- Assign the values to the parameter definitions object
         dtHLAGM::OneToManyMapping::ParameterDefinition& pd = mMapping.GetParameterDefinitions()[0];
         resParam->FromString(baseValues[5]);
         pd.SetDefaultValue(resParam->ToString());
         pd.AddEnumerationMapping(mappingKeys[0], mappingValues[0] );
         pd.AddEnumerationMapping(mappingKeys[1], mappingValues[1] );
         pd.AddEnumerationMapping(mappingKeys[2], mappingValues[2] );
         pd.AddEnumerationMapping(mappingKeys[3], mappingValues[3] );
         pd.AddEnumerationMapping(mappingKeys[4], mappingValues[4] );

         std::stringstream assertMessage;
         dtGame::MessageParameter* curParam = NULL;
         bool useBogusValue = false;
         unsigned resultKey = 0;
         unsigned mappingsCount = mappingValues.size() + 1; // # mappings + default mapping
         unsigned limit = params.size();
         for( unsigned i = 0; i < limit; ++i )
         {
            // Access the current parameter.
            curParam = params[i];

            mMapping.GetParameterDefinitions()[0].SetGameType( curParam->GetDataType() );
            mMapping.SetHLAType(dtHLAGM::RPRAttributeType::UNSIGNED_INT_TYPE);

            // Determine if the current parameter is a resource parameter.
            dtGame::ResourceMessageParameter* tmpResParam
               = dynamic_cast<dtGame::ResourceMessageParameter*>(curParam);

            for( unsigned curMapping = 0; curMapping < mappingsCount; ++curMapping )
            {
               // Determine whether to use a bogus value or a valid value.
               useBogusValue = curMapping >= mappingsCount-1;
               const std::string& curValue = useBogusValue ? "bogus value" : mappingValues[curMapping];

               // Ready the parameter for translation.
               if( NULL != tmpResParam )
               {
                  // This is a resource property that format a new value internally.
                  // So, use a base value so it can be formatted like the pre-formatted
                  // mapping values.
                  // Example: "Apple" (base value) -> "Apple/Apple" (internal formatted value like pre-formatted mapping values)
                  curParam->FromString( baseValues[curMapping] );
               }
               else
               {
                  // This is an enum or string parameter, so use pre-formatted mapping value.
                  curParam->FromString( curValue );
               }

               // Run the GET function that is to be tested.
               resultKey = mParameterTranslator->GetIntegerValuePublic( *curParam, mMapping, 0);

               unsigned curKey = 0L;

               // Test the function's results
               assertMessage.clear();
               if( useBogusValue )
               {
                  assertMessage << "HLA Type " << mMapping.GetHLAType().GetName()
                     << " mapping to Unknown Game Type " << curParam->GetDataType()
                     << " should map to HLA Type default value 0, but it is: " << resultKey;
                  CPPUNIT_ASSERT_MESSAGE( assertMessage.str(), 0.0 == resultKey );

                  // Run the SET function that is to be tested.
                  mParameterTranslator->SetIntegerValuePublic( curKey, *curParam, mMapping, 0 );

                  assertMessage.clear();
                  assertMessage << "Unknown HLA Type " << mMapping.GetHLAType().GetName()
                     << " mapping to Game Type " << curParam->GetDataType()
                     << " should map to default Game Type value \""
                     << pd.GetDefaultValue() << "\", but it is: \""
                     << curParam->ToString() << "\"";
                  CPPUNIT_ASSERT_MESSAGE( assertMessage.str(),
                     curParam->ToString() == pd.GetDefaultValue() );
               }
               else // This should be a valid mapping that is about to be tested.
               {
                  // Convert the expected key string to a number value.
                  std::istringstream iss;
                  iss.str(mappingKeys[curMapping]);
                  iss >> curKey;

                  assertMessage << "HLA Type " << mMapping.GetHLAType().GetName()
                     << " mapping to Game Type " << curParam->GetDataType()
                     << " should map to HLA Type value \"" << curKey << "\", but it is: " << resultKey;
                  CPPUNIT_ASSERT_MESSAGE( assertMessage.str(), curKey == resultKey );

                  // Run the SET function that is to be tested.
                  mParameterTranslator->SetIntegerValuePublic( curKey, *curParam, mMapping, 0 );

                  assertMessage.clear();
                  assertMessage << "HLA Type " << mMapping.GetHLAType().GetName()
                     << " mapping to Game Type " << curParam->GetDataType()
                     << " should map to Game Type value \"" << curValue << "\", but it is: \""
                     << curParam->ToString() << "\"";
                  CPPUNIT_ASSERT_MESSAGE( assertMessage.str(), curParam->ToString() == curValue );
               }

            }
         }
      }

};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ParameterTranslatorTests);
}
