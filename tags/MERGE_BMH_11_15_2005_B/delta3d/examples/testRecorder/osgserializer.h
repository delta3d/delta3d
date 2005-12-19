/** \author John K. Grant
  * \date August 26, 2005
  * \file OSGSerializer.cpp
  */
#ifndef DELTA_OSG_SERIALIZER
#define DELTA_OSG_SERIALIZER

#include <map>     // for std::map
#include <string>  // for std::string
#include <sstream> // for std::ostringstream

#include <osg/Quat>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOMElement.hpp>

/** \brief Helps to convert some OSG types into XML.
  * It converts OSG types into XML representation.
  * It maintains a vector of XMLCh* strings created
  * by Xerces, that needs to be cleaned up.
  *
  * \todo rename functions to be overrides of the operator() function.
  */
class OSGSerializer
{
public:
   typedef std::map<std::string,XMLCh*> XMLChRep;  /// a mapping of XMLCh* representation of a char*

   struct ReleaseSecond : public std::unary_function< XMLChRep::value_type, void >
   {
      void operator ()(XMLChRep::value_type& kv)
      {
         XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release( &kv.second );
      }
   };

   OSGSerializer();
   ~OSGSerializer();

   static void OSGSerializer::intialize();

   // serializing
   static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* OSGVec3(const osg::Vec3& vec, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);
   static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* OSGVec4(const osg::Vec4& vec, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);
   static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* OSGQuat(const osg::Quat& quat, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);
   static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* OSGMatrix(const osg::Matrix& mat, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);

   // deserialing
   static osg::Matrix OSGMatrix(XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e);

private:
   static bool mInitialized;
   static XMLChRep mRep;

   typedef XMLCh* (*Convert2XMLCh)(const char*);   /// a convenience typedef for short function usage
};

#endif // DELTA_OSG_SERIALIZER
