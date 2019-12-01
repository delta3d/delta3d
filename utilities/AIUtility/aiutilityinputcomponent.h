/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 * Eric Heine
 */
#ifndef AIUTILITY_INPUT_COMPONENT
#define AIUTILITY_INPUT_COMPONENT

#include <QtCore/QObject>
#include <dtGame/baseinputcomponent.h>
#include <dtAI/aiplugininterface.h>

#include "waypointmotionmodel.h"

namespace dtCore
{
   class FlyMotionModel;
}

class AIUtilityInputComponent : public QObject, public dtGame::BaseInputComponent
{
   Q_OBJECT
public:
   static const dtCore::RefPtr<dtCore::SystemComponentType> TYPE;

   AIUtilityInputComponent();

   /**
    * MouseListener call back - Called when a button is pressed.
    * Override this if you want to handle this listener event.
    * Default does nothing.
    *
    * @param mouse the source of the event
    * @param button the button pressed
    * @return true if this MouseListener handled the event. The
    * Mouse calling this function is responsbile for using this
    * return value or not.
    */
   virtual bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);

   virtual bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y);

   void SetWaypointMotionModel(WaypointMotionModel* waypointMotionModel);

   void SetAIPluginInterface(dtAI::AIPluginInterface* aiInterface);

   float GetPickDistanceBuffer() const;
   void SetPickDistanceBuffer(float val);

   void OnSelectWaypontBrushMode(bool enable);

   void OnSelectBrushSizeChanged(double size);

   /**
   *  Set this to enable controlling the camera speed with -/=
   */
   void SetCameraMotionModel(dtCore::FlyMotionModel& fm);


   virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

private slots:
   void OnWaypointTranslationBeginning();

protected:
   virtual ~AIUtilityInputComponent();

private:
   dtCore::RefPtr<dtAI::AIPluginInterface> mpAIInterface;
   bool IsShiftHeld();
   bool IsCtrlHeld();
   double GetSelectionBrushSize() const;
   bool IsOkToSelect() const;

   float mCameraSpeed;
   dtCore::ObserverPtr<dtCore::FlyMotionModel> mCameraMotionModel;

   float mPickDistanceBuffer; //used to "pad" the mouse picked position to make it easier to select

   bool mSelectBrushMode;
   double mSelectBrushSize; ///<Size (meters) of the selection brush
   dtCore::ObserverPtr<WaypointMotionModel> mWaypointMotionModel;
};

#endif //AIUTILITY_INPUT_COMPONENT
