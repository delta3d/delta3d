/** \author John K. Grant
  * \date August 26, 2005
  * \file OSGSerializer.cpp
  */
#include "osgserializer.h"

#include <algorithm>
#include <osg/io_utils>

#include <dtUtil/stringutils.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <cstdio>   // for sscanf

XERCES_CPP_NAMESPACE_USE

// initialize static variables
OSGSerializer::XMLChRep OSGSerializer::mRep;
bool OSGSerializer::mInitialized = false;

// implementation
OSGSerializer::OSGSerializer() {}

OSGSerializer::~OSGSerializer()
{
   // clean up the caught XMLCh strings
   std::for_each( mRep.begin(), mRep.end(), ReleaseSecond() );
}

void OSGSerializer::Initialize()
{
   if( mInitialized )
      return;

   try  // to initialize the xml tools
   {
      XMLPlatformUtils::Initialize();
   }
   catch(const XMLException& e)
   {
      char* message = XMLString::transcode( e.getMessage() );
      std::string msg(message);
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize() with message: " + msg);
      XMLString::release( &message );
   }
   catch(...)
   {
      LOG_ERROR("An exception occurred during XMLPlatformUtils::Initialize()");
   }

   // general attribute names
   mRep.insert( XMLChRep::value_type( "X", XMLString::transcode("X") ) );
   mRep.insert( XMLChRep::value_type( "Y", XMLString::transcode("Y") ) );
   mRep.insert( XMLChRep::value_type( "Z", XMLString::transcode("Z") ) );
   mRep.insert( XMLChRep::value_type( "W", XMLString::transcode("W") ) );
   mRep.insert( XMLChRep::value_type( "Matrix", XMLString::transcode("Matrix") ) );
   //mRep.insert( XMLChRep::value_type( "ForwardVec", XMLString::transcode("ForwardVec") ) );
   //mRep.insert( XMLChRep::value_type( "RightVec", XMLString::transcode("RightVec") ) );
   //mRep.insert( XMLChRep::value_type( "UpVec", XMLString::transcode("UpVec") ) );
   //mRep.insert( XMLChRep::value_type( "TransVec", XMLString::transcode("TransVec") ) );

   // OSG specific attribute names
   mRep.insert( XMLChRep::value_type( "OSGVec3" , XMLString::transcode("OSGVec3") ) );
   mRep.insert( XMLChRep::value_type( "OSGVec4" , XMLString::transcode("OSGVec4") ) );
   mRep.insert( XMLChRep::value_type( "OSGQuat" , XMLString::transcode("OSGQuat") ) );
   mRep.insert( XMLChRep::value_type( "OSGMatrix" , XMLString::transcode("OSGMatrix") ) );
}

XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* OSGSerializer::OSGVec3(const osg::Vec3& vec, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   DOMElement* element = doc->createElement( mRep["OSGVec3"] );
   ///\warning does this leak the XMLString::transcode strings?
   element->setAttribute( mRep["X"] , XMLString::transcode( dtUtil::ToString<osg::Vec3::value_type>(vec[0]).c_str() ) );
   element->setAttribute( mRep["Y"] , XMLString::transcode( dtUtil::ToString<osg::Vec3::value_type>(vec[1]).c_str() ) );
   element->setAttribute( mRep["Z"] , XMLString::transcode( dtUtil::ToString<osg::Vec3::value_type>(vec[2]).c_str() ) );
   return element;
}

DOMElement* OSGSerializer::OSGVec4(const osg::Vec4& vec, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   DOMElement* element = doc->createElement( mRep["OSGVec4"] );
   ///\warning does this leak the XMLString::transcode strings?
   element->setAttribute( mRep["X"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(vec[0]).c_str()) );
   element->setAttribute( mRep["Y"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(vec[1]).c_str()) );
   element->setAttribute( mRep["Z"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(vec[2]).c_str()) );
   element->setAttribute( mRep["W"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(vec[3]).c_str()) );
   return element;
}

DOMElement* OSGSerializer::OSGQuat(const osg::Quat& quat, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* element = doc->createElement( mRep["OSGQuat"] );
   ///\warning does this leak the XMLString::transcode strings?
   element->setAttribute( mRep["X"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(quat.x()).c_str()) );
   element->setAttribute( mRep["Y"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(quat.y()).c_str()) );
   element->setAttribute( mRep["Z"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(quat.z()).c_str()) );
   element->setAttribute( mRep["W"] , XMLString::transcode(dtUtil::ToString<osg::Vec4::value_type>(quat.w()).c_str()) );
   return element;
}

DOMElement* OSGSerializer::OSGMatrix(const osg::Matrix& mat, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
{
   XMLCh* OSGMATRIX = XMLString::transcode("OSGMatrix");
   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* element = doc->createElement( OSGMATRIX );
   XMLString::release( &OSGMATRIX );

   std::string valuestring = dtUtil::ToString<osg::Matrix>( mat );
   XMLCh* VALUES = XMLString::transcode( valuestring.c_str() );

   XMLCh* MATRIX = XMLString::transcode("Matrix");
   element->setAttribute( MATRIX , VALUES );
   XMLString::release( &MATRIX );

   XMLString::release( &VALUES );
   return element;
}

osg::Matrix OSGSerializer::OSGMatrix(DOMElement* e)
{
   char* nodename = XMLString::transcode( e->getNodeName() );
   std::string nname( nodename );
   XMLString::release( &nodename );
   LOG_INFO("Deserializing Node: " + nname )

   std::string matrixstring = dtUtil::FindAttributeValueFor("Matrix",e->getAttributes());
   float mt[16];
   sscanf(matrixstring.c_str(), "{ %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f }",
          &mt[0], &mt[1], &mt[2], &mt[3],
          &mt[4], &mt[5], &mt[6], &mt[7],
          &mt[8], &mt[9], &mt[10], &mt[11],
          &mt[12], &mt[13], &mt[14], &mt[15]);
   osg::Matrix mat(mt);
   return mat;
}

