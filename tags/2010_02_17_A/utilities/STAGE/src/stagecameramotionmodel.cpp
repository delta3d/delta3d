// stagecameramotionmodel.cpp: Implementation of the STAGECameraMotionModel class.
//
//////////////////////////////////////////////////////////////////////
#include <dtEditQt/stagecameramotionmodel.h>
#include <dtEditQt/editorviewport.h>

#include <dtCore/system.h>
#include <dtCore/view.h>

namespace dtEditQt
{
   IMPLEMENT_MANAGEMENT_LAYER(STAGECameraMotionModel)

   ////////////////////////////////////////////////////////////////////////////////
   STAGECameraMotionModel::STAGECameraMotionModel(const std::string& name)
      : dtCore::MotionModel(name)
      , mLeftMouse(false)
      , mRightMouse(false)
      , mMiddleMouse(false)
      , mCamera(NULL)
      , mViewport(NULL)
   {
      RegisterInstance(this);

      AddSender(&dtCore::System::GetInstance());
   }

   ////////////////////////////////////////////////////////////////////////////////
   STAGECameraMotionModel::~STAGECameraMotionModel()
   {
      RemoveSender(&dtCore::System::GetInstance());

      DeregisterInstance(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void STAGECameraMotionModel::SetCamera(StageCamera* camera)
   {
      mCamera = camera;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void STAGECameraMotionModel::SetViewport(EditorViewport* viewport)
   {
      mViewport = viewport;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void STAGECameraMotionModel::OnMessage(MessageData *data)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::OnLeftMousePressed(void)
   {
      if (IsEnabled())
      {
         mLeftMouse = true;

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::OnLeftMouseReleased(void)
   {
      if (IsEnabled())
      {
         mLeftMouse = false;

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::OnRightMousePressed(void)
   {
      if (IsEnabled())
      {
         mRightMouse = true;

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::OnRightMouseReleased(void)
   {
      if (IsEnabled())
      {
         mRightMouse = false;

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::OnMiddleMousePressed(void)
   {
      if (IsEnabled())
      {
         mMiddleMouse = true;

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::OnMiddleMouseReleased(void)
   {
      if (IsEnabled())
      {
         mMiddleMouse = false;

         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::BeginCameraMode(QMouseEvent* e)
   {
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::EndCameraMode(QMouseEvent* e)
   {
      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::OnMouseMoved(float dx, float dy)
   {
      if (IsEnabled() && mCamera.valid())
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool STAGECameraMotionModel::WheelEvent(int delta)
   {
      if (IsEnabled() && mCamera.valid())
      {
         return true;
      }

      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
