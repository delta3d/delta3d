/* -*-c++-*-
* testRecorder - osgserializer (.h & .cpp) - Using 'The MIT License'
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
* author John K. Grant
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

//////////////////////////////////////////////////////////////////////////////
// NOTE - This test is provided for historical reference. The ability to 
// record and playback is provided via the AAR Record and Playback 
// components in dtGame. See testAAR for a example of using this behavior.
//////////////////////////////////////////////////////////////////////////////

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

   static void Initialize();
   

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
