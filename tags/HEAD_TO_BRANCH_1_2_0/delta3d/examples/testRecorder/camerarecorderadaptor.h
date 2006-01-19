/** \author John K. Grant
  * \date August 26, 2005
  * \file CameraRecorderAdaptor.h
  */
#ifndef DELTA_CAMERA_RECORDER_ADAPTOR_INCLUDE
#define DELTA_CAMERA_RECORDER_ADAPTOR_INCLUDE

#include <xercesc/dom/DOM.hpp>
#include <osg/Matrix>

#include <dtCore/base.h>   // for base class, reference counting desired

namespace dtCore
{
   class Camera;
};

/** \brief A class that interprets Camera data for the Recorder.
  */
class CameraRecorderAdaptor : public dtCore::Base
{
public:
   typedef dtCore::Base BaseClass;
   CameraRecorderAdaptor(dtCore::Camera* cam);

   /** \brief The data of interest for the Camera.*/
   class FrameData : public osg::Referenced
   {
   public:
      FrameData();
      FrameData(const osg::Matrix& m);

   protected:
      ~FrameData();

   private:
      FrameData(const FrameData& f);             /// not implemented by design
      FrameData& operator =(const FrameData& f); /// not implemented by design

      friend class CameraRecorderAdaptor;
      osg::Matrix mMatrix;
   };

   FrameData* CreateFrameData() const;
   void UseFrameData(const FrameData* d);

   XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* Serialize(const FrameData* d,
                                                        XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc) const;

   FrameData* Deserialize(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,
                          XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* elem) const;

protected:
   virtual ~CameraRecorderAdaptor();

private:
   CameraRecorderAdaptor();  /// not implemented by design
   dtCore::Camera* mCamera;  /// class of interest for recording data
};

#endif // DELTA_CAMERA_RECORDER_ADAPTOR_INCLUDE
