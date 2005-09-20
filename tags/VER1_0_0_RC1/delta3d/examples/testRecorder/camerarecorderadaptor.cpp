/** \author John K. Grant
  * \date August 26, 2005
  * \file CameraRecorderAdaptor.h
  */
#include "camerarecorderadaptor.h"
#include "osgserializer.h"

#include <dtCore/camera.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/stringutils.h>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMElement.hpp>

#include <functional>

XERCES_CPP_NAMESPACE_USE

// -- CameraRecorderAdaptor implementation -- //
CameraRecorderAdaptor::CameraRecorderAdaptor(dtCore::Camera* cam): BaseClass("CameraRecorderAdaptor"), mCamera(cam)
{
   Serializer::intialize();
}

CameraRecorderAdaptor::~CameraRecorderAdaptor()
{
}

CameraRecorderAdaptor::FrameData* CameraRecorderAdaptor::CreateFrameData() const
{
   dtCore::Transform xform;
   mCamera->GetTransform( &xform );

   FrameData* fd = new FrameData();
   xform.Get( fd->mMatrix );
   return fd;
}

void CameraRecorderAdaptor::UseFrameData(const FrameData* d)
{
   dtCore::Transform xform;
   mCamera->GetTransform(&xform);
   xform.Set( d->mMatrix );
   mCamera->SetTransform( &xform );
}

CameraRecorderAdaptor::FrameData* CameraRecorderAdaptor::Deserialize(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,
                                                                     XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* adaptor) const
{
   char* nodename = XMLString::transcode( adaptor->getNodeName() );
   std::string nname( nodename );
   XMLString::release( &nodename );
   LOG_INFO("Deserializing Node: " + nname)

   // what to do with this data?  pack it into the FrameData packet? probably yes.
   std::string cameraname = dtUtil::FindAttributeValueFor("Name",adaptor->getAttributes());

   XERCES_CPP_NAMESPACE_QUALIFIER DOMTreeWalker* adaptorwalker = doc->createTreeWalker(adaptor,
                                                                                       DOMNodeFilter::SHOW_ELEMENT,
                                                                                       0, true);

   FrameData* fd(0);
   for(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* child=adaptorwalker->firstChild();
       child != 0;
       child=adaptorwalker->nextSibling())
   {
      DOMElement* element = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>( child );
      char* childname = XMLString::transcode( child->getNodeName() );
      std::string cname( childname );
      XMLString::release( &childname );

      if( cname == "OSGMatrix" )
      {
         LOG_INFO("Attempting to decode: " + cname)
         fd = new FrameData( Serializer::OSGMatrix( element ) );
         break;
      }
   }

   //DOMNodeList* children = adaptor->getChildNodes();
   //unsigned int n = children->getLength();

   //if( n > 0 )
   //{
   //   DOMNode* child = children->item(0);
   //   DOMElement* element = static_cast<DOMElement*>( child );
   //   fd = new FrameData( Serializer::OSGMatrix( element ) );
   //}

   return fd;
}

DOMElement* CameraRecorderAdaptor::Serialize(const FrameData* d,XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc) const
{
   XMLCh* CAMERA_RECORDER_ADAPTOR_STRING = XMLString::transcode("CameraRecorderAdaptor");
   XMLCh* NAME = XMLString::transcode("Name");
   DOMElement* element = doc->createElement( CAMERA_RECORDER_ADAPTOR_STRING );

   ///\warning does this leak the transcode?
   element->setAttribute( NAME , XMLString::transcode(mCamera->GetName().c_str()) );

   DOMElement* matrixelement = Serializer::OSGMatrix(d->mMatrix,doc);
   element->appendChild( matrixelement );

   XMLString::release( &NAME );
   XMLString::release( &CAMERA_RECORDER_ADAPTOR_STRING );
   return element;
}

// -- FrameData implementation -- //
typedef CameraRecorderAdaptor CRA;
CRA::FrameData::FrameData() : mMatrix()
{
}

CRA::FrameData::FrameData(const osg::Matrix& m): mMatrix(m)
{
}

CRA::FrameData::~FrameData()
{
}
