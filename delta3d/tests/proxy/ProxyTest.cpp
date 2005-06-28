#include "ProxyTest.h"
#include "dtActors/particlesystemactorproxy.h"
#include "dtActors/characteractorproxy.h"
#include "dtActors/soundactorproxy.h"
#include "dtActors/infiniteterrainactorproxy.h"
#include "dtActors/staticmeshactorproxy.h"
#include "dtActors/spotlightactorproxy.h"
#include "dtActors/terrainactorproxy.h"
#include "dtActors/environmentactorproxy.h"
#include "dtActors/cameraactorproxy.h"
#include "dtActors/meshterrainactorproxy.h"

#if defined (WIN32) || defined (_Win32) || defined (__WIN32__)
    #include "dtActors/dtedterrainactorproxy.h"
#endif
#include "dtActors/infinitelightactorproxy.h"

using namespace dtActors;
using namespace dtChar;

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
    SetDataFilePathList("../data;" + GetDeltaDataPathList());
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
    vector<ActorProperty*> props;
    proxy.GetPropertyList(props);
    const float epsilon = 0.01f;

    for (unsigned int i = 0; i < props.size(); i++)
    {
        string name = props[i]->GetName();
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

            for (int x = 0; x < 3; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
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

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
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

            for (int x = 0; x < 2; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
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

            for (int x = 0; x < 4; x++)
            {
                std::ostringstream ss;
                ss << proxyTypeName << " proxy TEST FAILED: Vec[" << x
                    << "] property: " << name << " - Value: " << result;
                CPPUNIT_ASSERT_MESSAGE(ss.str(),
                                       osg::equivalent(result[x], test[x], epsilon));
            }
        }
    }
}


void ProxyTest::compareProxies(ActorProxy& ap1, ActorProxy& ap2)
{
    vector<ActorProperty*> props;
    ap1.GetPropertyList(props);
    const float epsilon = 0.01f;

    for(unsigned int i = 0; i < props.size(); i++)
    {
        string str = props[i]->GetName();
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
        proxy = libMgr.CreateActorProxy(actors[i]).get();
        CPPUNIT_ASSERT(proxy != NULL);
        testProps(*proxy);
        osg::ref_ptr<ActorProxy> proxy2 = proxy->Clone();
        compareProxies(*proxy, *proxy2);
        compareProxies(*proxy2, *proxy);
    }

}

