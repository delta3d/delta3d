/* -*-c++-*-
* testRecorder - camerarecorderadaptor (.h & .cpp) - Using 'The MIT License'
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
* John K. Grant
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

//////////////////////////////////////////////////////////////////////////////
// NOTE - This test is provided for historical reference. The ability to 
// record and playback is provided via the AAR Record and Playback 
// components in dtGame. See testAAR for a example of using this behavior.
//////////////////////////////////////////////////////////////////////////////

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
      virtual ~FrameData();

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
