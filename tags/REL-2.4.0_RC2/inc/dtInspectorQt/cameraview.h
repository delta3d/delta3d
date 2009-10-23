/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */
 
#ifndef cameraview_h__
#define cameraview_h__

#include <dtInspectorQt/iview.h>
#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtCore/camera.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

namespace dtInspectorQt
{
   ///operates on Camera
   class CameraView : public IView
   {
      Q_OBJECT

   public:
      CameraView(Ui::InspectorWidget &ui);
      virtual ~CameraView();

      virtual void OperateOn(dtCore::Base* b);
      bool IsOfType(QString name, dtCore::Base* object);

   protected slots:
      void OnEnabled(int state);
      void OnPerspectiveChanged(double value);
      void OnClearColorChanged(double value);

   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::Camera> mOperateOn;
      void Update();

   };
}  
#endif //cameraview_h__
