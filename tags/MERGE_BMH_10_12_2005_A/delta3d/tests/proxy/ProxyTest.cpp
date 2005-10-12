#include "ProxyTest.h"

#if defined (WIN32) || defined (_Win32) || defined (__WIN32__)
//    #include "dtActors/dtedterrainactorproxy.h"
#endif
#include "dtActors/infinitelightactorproxy.h"


CPPUNIT_TEST_SUITE_REGISTRATION(ProxyTest);

ProxyTest::ProxyTest() :
libMgr(LibraryManager::GetInstance())
{
}

ProxyTest::~ProxyTest()
{
}

void ProxyTest::setUp()
{
    //dtDAL::Project::GetInstance();
    SetDataFilePathList(dtCore::GetDeltaDataPathList());
    libMgr.GetActorTypes(actors);
    //dtAudio::AudioManager::Instantiate();
    //dtAudio::AudioManager::GetManager()->Config();
    CPPUNIT_ASSERT(actors.size());
}

void ProxyTest::tearDown()
{
    //dtAudio::AudioManager::Destroy();
}

void ProxyTest::testProps(ActorProxy& proxy)
{
    std::string proxyTypeName = proxy.GetActorType().GetName();
    std::vector<ActorProperty*> props;
    proxy.GetPropertyList(props);
    const float epsilon = 0.01f;

    for (unsigned int i = 0; i < props.size(); i++)
    {
        std::string name = props[i]->GetName();
        
        if(props[i]->IsReadOnly())
        {
            // Test and make sure you can't set the property
            ActorProperty *p = props[i];
            const std::string &str = p->GetStringValue();
            bool shouldBeFalse = p->SetStringValue(str);
            CPPUNIT_ASSERT_MESSAGE("Should not have been able to set the string value on an read only property", !shouldBeFalse);
            continue;
        }

        if (props[i]->GetPropertyType() == DataType::FLOAT)
        {
            FloatActorProperty* prop1 = ((FloatActorProperty*)props[i]);
            prop1->SetValue(1.3f);
            CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                + " should have value 1.3, but it is: " + props[i]->GetStringValue(),
                osg::equivalent(prop1->GetValue(), 1.3f, (float)epsilon));

            std::string stringValue = prop1->GetStringValue();
            //set some other value so we can test the string can set it back.
            prop1->SetValue(17.28f);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                + " should have value 1.3, but it is: " + props[i]->GetStringValue(),
                osg::equivalent(prop1->GetValue(), 1.3f, (float)epsilon));
        }
        else if (props[i]->GetPropertyType() == DataType::DOUBLE)
        {
            DoubleActorProperty* prop1 = ((DoubleActorProperty*)props[i]);
            prop1->SetValue(1.3);
            CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                + " should have value 1.3, but it is: " + props[i]->GetStringValue(),
                osg::equivalent(prop1->GetValue(), 1.3, (double)epsilon));

            std::string stringValue = prop1->GetStringValue();
            //set some other value so we can test the string can set it back.
            prop1->SetValue(17.28);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                + " should have value 1.3, but it is: " + props[i]->GetStringValue(),
                osg::equivalent(prop1->GetValue(), 1.3, (double)epsilon));
        }
        else if (props[i]->GetPropertyType() == DataType::INT)
        {
            ((IntActorProperty*)props[i])->SetValue(3);
            CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                + " should have value 3, but it is: " + props[i]->GetStringValue(),
                ((IntActorProperty*)props[i])->GetValue() == 3);
        }
        else if (props[i]->GetPropertyType() == DataType::LONGINT)
        {
            ((LongActorProperty*)props[i])->SetValue(4);
            CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                + " should have value 4, but it is: " + props[i]->GetStringValue(),
                ((LongActorProperty*)props[i])->GetValue() == 4);
        }
        else if (props[i]->GetPropertyType() == DataType::STRING)
        {
            ((StringActorProperty*)props[i])->SetValue("cache");
            CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxyTypeName
                + " should have value \"cache\", but it is: " + props[i]->GetStringValue(),
                ((StringActorProperty*)props[i])->GetValue() == "cache");
        }
        else if (props[i]->GetPropertyType() == DataType::BOOLEAN)
        {
            //there have been some problems with Normal Rescaling
            if (name != "Normal Rescaling" && name != "Collision Geometry")
            {

                ((BooleanActorProperty*)props[i])->SetValue(false);
                CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxy.GetName() + " should be false, but it's not." ,
                    !((BooleanActorProperty*)props[i])->GetValue());
                ((BooleanActorProperty*)props[i])->SetValue(true);
                CPPUNIT_ASSERT_MESSAGE(name + " property on " + proxy.GetName() + " should be true, but it's not." ,
                    ((BooleanActorProperty*)props[i])->GetValue());
            }
            else
            {
                std::cout << "Skipping property " << name << std::endl;
            }
        }
        else if (props[i]->GetPropertyType() == DataType::ENUMERATION)
        {

            dtDAL::AbstractEnumActorProperty* eap = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(props[i]);
            eap->SetEnumValue(const_cast<dtUtil::Enumeration&>(**(eap->GetList().begin()+1)));

            CPPUNIT_ASSERT_MESSAGE(std::string("Value should be ") + (*(eap->GetList().begin()+1))->GetName()
                + " but it is " + eap->GetEnumValue().GetName(),
                eap->GetEnumValue() == **(eap->GetList().begin()+1));
        }
        else if (props[i]->GetPropertyType() == DataType::VEC3)
        {
            Vec3ActorProperty* prop1 = ((Vec3ActorProperty*)props[i]);
            osg::Vec3 test(9.0f, 2.0f, 7.34f);
            if (name == "Direction")
                //Direction ignores the y rotation because you can't roll a vector.
                test.y() = 0.0f;

            prop1->SetValue(test);
            osg::Vec3 result = prop1->GetValue();

            for (int x = 0; x < 3; x++)
            {
                if (x == 1 && name == "Direction")
                    continue;
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
            osg::Vec3 test2(7.0f, 3.0f, -9.25f);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 3; x++)
            {
                if (x == 1 && name == "Direction")
                    continue;
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
        }

        else if (props[i]->GetPropertyType() == DataType::VEC3F)
        {
            Vec3fActorProperty* prop1 = ((Vec3fActorProperty*)props[i]);
            osg::Vec3f test(9.0f, 2.0f, 7.34f);
            if (name == "Direction")
                //Direction ignores the y rotation because you can't roll a vector.
                test.y() = 0.0f;

            prop1->SetValue(test);
            osg::Vec3f result = prop1->GetValue();

            for (int x = 0; x < 3; x++)
            {
                if (x == 1 && name == "Direction")
                    continue;
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "f] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent(result[x], test[x], epsilon));
            }
            osg::Vec3f test2(7.0f, 3.0f, -9.25f);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 3; x++)
            {
                if (x == 1 && name == "Direction")
                    continue;
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "f] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent(result[x], test[x], epsilon));
            }
        }
        else if (props[i]->GetPropertyType() == DataType::VEC3D)
        {
            Vec3dActorProperty* prop1 = ((Vec3dActorProperty*)props[i]);
            osg::Vec3d test(9.0, 2.0, 7.34);
            if (name == "Direction")
                //Direction ignores the y rotation because you can't roll a vector.
                test.y() = 0.0;

            prop1->SetValue(test);
            osg::Vec3d result = prop1->GetValue();

            for (int x = 0; x < 3; x++)
            {
                if (x == 1 && name == "Direction")
                    continue;
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "d] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent((double)result[x], (double)test[x], (double)epsilon));
            }
            osg::Vec3d test2(7.0, 3.0, -9.25);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 3; x++)
            {
                if (x == 1 && name == "Direction")
                    continue;
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "d] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent((double)result[x], (double)test[x], (double)epsilon));
            }
        }
        else if (props[i]->GetPropertyType() == DataType::VEC4)
        {
            Vec4ActorProperty* prop1 = ((Vec4ActorProperty*)props[i]);
            osg::Vec4 test(6.0f, 6.0f, 5.0f, 7.3f);
            prop1->SetValue(test);
            osg::Vec4 result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
            osg::Vec4 test2(7.0f, 3.0f, 8.0f, 2.1f);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }

        }
        else if (props[i]->GetPropertyType() == DataType::VEC4F)
        {
            Vec4fActorProperty* prop1 = ((Vec4fActorProperty*)props[i]);
            osg::Vec4f test(6.0f, 6.0f, 5.0f, 7.3f);
            prop1->SetValue(test);
            osg::Vec4f result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "f] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent(result[x], test[x], epsilon));
            }
            osg::Vec4f test2(7.0f, 3.0f, 8.0f, 2.1f);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "f] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent(result[x], test[x], epsilon));
            }

        }
        else if (props[i]->GetPropertyType() == DataType::VEC4D)
        {
            Vec4dActorProperty* prop1 = ((Vec4dActorProperty*)props[i]);
            osg::Vec4d test(6.0, 6.0, 5.0, 7.3);
            prop1->SetValue(test);
            osg::Vec4d result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "d] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent((double)result[x], (double)test[x], (double)epsilon));
            }
            osg::Vec4d test2(7.0, 3.0, 8.0, 2.1);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "d] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent((double)result[x], (double)test[x], (double)epsilon));
            }

        }
        else if (props[i]->GetPropertyType() == DataType::VEC2)
        {
            Vec2ActorProperty* prop1 = ((Vec2ActorProperty*)props[i]);
            osg::Vec2 test(9.0f, 2.0f);
            prop1->SetValue(test);
            osg::Vec2 result = prop1->GetValue();

            for (int x = 0; x < 2; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
            osg::Vec2 test2(7.0f, 3.0f);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 2; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
        }
        else if (props[i]->GetPropertyType() == DataType::VEC2F)
        {
            Vec2fActorProperty* prop1 = ((Vec2fActorProperty*)props[i]);
            osg::Vec2f test(9.0f, 2.0f);
            prop1->SetValue(test);
            osg::Vec2f result = prop1->GetValue();

            for (int x = 0; x < 2; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "f] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent(result[x], test[x], epsilon));
            }
            osg::Vec2f test2(7.0f, 3.0f);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 2; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "f] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent(result[x], test[x], epsilon));
            }
        }
        else if (props[i]->GetPropertyType() == DataType::VEC2D)
        {
            Vec2dActorProperty* prop1 = ((Vec2dActorProperty*)props[i]);
            osg::Vec2d test(9.0, 2.0);
            prop1->SetValue(test);
            osg::Vec2d result = prop1->GetValue();

            for (int x = 0; x < 2; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "d] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent((double)result[x], (double)test[x], (double)epsilon));
            }
            osg::Vec2d test2(7.0, 3.0);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 2; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "d] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                    osg::equivalent((double)result[x], (double)test[x], (double)epsilon));
            }
        }
        else if (props[i]->GetPropertyType() == DataType::RGBACOLOR)
        {
            ColorRgbaActorProperty* prop1 = ((ColorRgbaActorProperty*)props[i]);
            osg::Vec4 test(6.0f, 6.0f, 5.0f, 7.3f);
            prop1->SetValue(test);
            osg::Vec4 result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
            osg::Vec4 test2(7.0f, 3.0f, 8.0f, 2.1f);
            std::string stringValue = prop1->GetStringValue();
            //change the value so we can change it back.
            prop1->SetValue(test2);
            CPPUNIT_ASSERT(prop1->SetStringValue(stringValue));

            result = prop1->GetValue();

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy string TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
        }
    }
}


void ProxyTest::compareProxies(ActorProxy& ap1, ActorProxy& ap2)
{
    std::vector<ActorProperty*> props;
    ap1.GetPropertyList(props);
    const float epsilon = 0.01f;

    for(unsigned int i = 0; i < props.size(); i++)
    {
        std::string str = props[i]->GetName();
        ActorProperty* prop2 = ap2.GetProperty(str);

        CPPUNIT_ASSERT(prop2 != NULL);

        if(props[i]->GetPropertyType() == DataType::FLOAT)
        {
            CPPUNIT_ASSERT(osg::equivalent(((FloatActorProperty*)props[i])->GetValue(),
                ((FloatActorProperty*)prop2)->GetValue(), (float)epsilon));
        }
        else if(props[i]->GetPropertyType() == DataType::DOUBLE)
        {
            CPPUNIT_ASSERT(osg::equivalent(((DoubleActorProperty*)props[i])->GetValue(),
                ((DoubleActorProperty*)prop2)->GetValue(), (double)epsilon));
        }
        else if(props[i]->GetPropertyType() == DataType::INT)
        {
            CPPUNIT_ASSERT(((IntActorProperty*)props[i])->GetValue() ==
                ((IntActorProperty*)prop2)->GetValue());
            props[i]->GetStringValue() == prop2->GetStringValue();
        }
        else if(props[i]->GetPropertyType() == DataType::LONGINT)
        {
            CPPUNIT_ASSERT(((LongActorProperty*)props[i])->GetValue() ==
                ((LongActorProperty*)prop2)->GetValue());
            props[i]->GetStringValue() == prop2->GetStringValue();
        }
        else if(props[i]->GetPropertyType() == DataType::STRING)
        {
            CPPUNIT_ASSERT(((StringActorProperty*)props[i])->GetValue() ==
                ((StringActorProperty*)prop2)->GetValue());
            props[i]->GetStringValue() == prop2->GetStringValue();
        }
        else if(props[i]->GetPropertyType() == DataType::BOOLEAN)
        {
            CPPUNIT_ASSERT(((BooleanActorProperty*)props[i])->GetValue() ==
                ((BooleanActorProperty*)prop2)->GetValue());
            props[i]->GetStringValue() == prop2->GetStringValue();
        }
        else if(props[i]->GetPropertyType() == DataType::ENUMERATION)
        {
            CPPUNIT_ASSERT(props[i]->GetStringValue() == prop2->GetStringValue());
            props[i]->GetStringValue() == prop2->GetStringValue();
        }
        else if(props[i]->GetPropertyType() == DataType::VEC3)
        {
            std::ostringstream ss;
            ss << ((dtDAL::Vec3ActorProperty*)props[i])->GetValue() << " vs " << ((dtDAL::Vec3ActorProperty*)prop2)->GetValue();
            CPPUNIT_ASSERT_MESSAGE(props[i]->GetName() + " value should be the same: " + ss.str(),
                osg::equivalent(((dtDAL::Vec3ActorProperty*)props[i])->GetValue()[0],
                    ((dtDAL::Vec3ActorProperty*)prop2)->GetValue()[0], epsilon)
                && osg::equivalent(((dtDAL::Vec3ActorProperty*)props[i])->GetValue()[1],
                    ((dtDAL::Vec3ActorProperty*)prop2)->GetValue()[1], epsilon)
                && osg::equivalent(((dtDAL::Vec3ActorProperty*)props[i])->GetValue()[2],
                    ((dtDAL::Vec3ActorProperty*)prop2)->GetValue()[2], epsilon)
                );
        }
        else if(props[i]->GetPropertyType() == DataType::VEC4)
        {
            std::ostringstream ss;
            ss << ((dtDAL::Vec4ActorProperty*)props[i])->GetValue() << " vs " << ((dtDAL::Vec4ActorProperty*)prop2)->GetValue();
            CPPUNIT_ASSERT_MESSAGE(props[i]->GetName() + " value should be the same: " + ss.str(),
                osg::equivalent(((dtDAL::Vec4ActorProperty*)props[i])->GetValue()[0],
                    ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[0], epsilon)
                && osg::equivalent(((dtDAL::Vec4ActorProperty*)props[i])->GetValue()[1],
                    ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[1], epsilon)
                && osg::equivalent(((dtDAL::Vec4ActorProperty*)props[i])->GetValue()[2],
                    ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[2], epsilon)
                && osg::equivalent(((dtDAL::Vec4ActorProperty*)props[i])->GetValue()[3],
                    ((dtDAL::Vec4ActorProperty*)prop2)->GetValue()[3], epsilon)
                );
        }
        else if(props[i]->GetPropertyType() == DataType::VEC2)
        {
            std::ostringstream ss;
            ss << ((dtDAL::Vec2ActorProperty*)props[i])->GetValue() << " vs " << ((dtDAL::Vec2ActorProperty*)prop2)->GetValue();
            CPPUNIT_ASSERT_MESSAGE(props[i]->GetName() + " value should be the same: " + ss.str(),
                osg::equivalent(((dtDAL::Vec2ActorProperty*)props[i])->GetValue()[0],
                    ((dtDAL::Vec2ActorProperty*)prop2)->GetValue()[0], epsilon)
                && osg::equivalent(((dtDAL::Vec2ActorProperty*)props[i])->GetValue()[1],
                    ((dtDAL::Vec2ActorProperty*)prop2)->GetValue()[1], epsilon)
                );
        }
        else if(props[i]->GetPropertyType() == DataType::RGBACOLOR)
        {
            std::ostringstream ss;
            ss << ((dtDAL::ColorRgbaActorProperty*)props[i])->GetValue() << " vs "
                << ((dtDAL::ColorRgbaActorProperty*)prop2)->GetValue();
             CPPUNIT_ASSERT_MESSAGE(props[i]->GetName() + " value should be the same: " + ss.str(),
                osg::equivalent(((dtDAL::ColorRgbaActorProperty*)props[i])->GetValue()[0],
                    ((dtDAL::ColorRgbaActorProperty*)prop2)->GetValue()[0], epsilon)
                && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)props[i])->GetValue()[1],
                    ((dtDAL::ColorRgbaActorProperty*)prop2)->GetValue()[1], epsilon)
                && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)props[i])->GetValue()[2],
                    ((dtDAL::ColorRgbaActorProperty*)prop2)->GetValue()[2], epsilon)
                && osg::equivalent(((dtDAL::ColorRgbaActorProperty*)props[i])->GetValue()[3],
                    ((dtDAL::ColorRgbaActorProperty*)prop2)->GetValue()[3], epsilon)
                );
       }
    }

}

void ProxyTest::testProxies()
{
    osg::ref_ptr<ActorProxy> proxy;

    for(unsigned int i = 0; i < actors.size(); i++)
    {
        proxy = libMgr.CreateActorProxy(*actors[i]).get();
        CPPUNIT_ASSERT(proxy != NULL);
        testProps(*proxy);
        osg::ref_ptr<ActorProxy> proxy2 = proxy->Clone();
        compareProxies(*proxy, *proxy2);
        compareProxies(*proxy2, *proxy);
    }

}

