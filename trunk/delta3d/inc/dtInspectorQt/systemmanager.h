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
 
#ifndef systemmanager_h__
#define systemmanager_h__

#include <dtInspectorQt/imanager.h>
#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtCore/system.h>

namespace Ui
{
   class InspectorWidget;
}

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::System
   class SystemManager : public IManager
   {
      Q_OBJECT

   public:
      SystemManager(Ui::InspectorWidget &ui);
      virtual ~SystemManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:
      void OnPaused(int checked);
      void OnTimeScaleChanged(double newValue);
      void OnFixedTimeStepsToggled(int checked);
      void OnSetFrameRateChanged(double newValue);
      void OnMaxBetweenDrawsChanged(double newValue);

   private:
      void Update();

      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::System> mOperateOn;

   };
}  
#endif //systemmanager_h__
