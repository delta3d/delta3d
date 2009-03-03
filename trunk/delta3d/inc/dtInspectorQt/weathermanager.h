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
 
#ifndef weathermanager_h__
#define weathermanager_h__

#include <dtInspectorQt/imanager.h>
#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtABC/weather.h>

namespace Ui
{
   class InspectorWidget;
}

namespace dtInspectorQt
{
   ///Handles the properties of dtABC::Weather
   class WeatherManager : public IManager
   {
      Q_OBJECT

   public:
      WeatherManager(Ui::InspectorWidget &ui);
      virtual ~WeatherManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:

      void OnThemeChanged(int index);
      void OnCloudTypeChanged(int index);
      void OnWindTypeChanged(int index);
      void OnVisibilityTypeChanged(int index);
      void OnTimeSeasonChanged(int);

   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtABC::Weather> mOperateOn;
      void Update();
   };
}  
#endif //weathermanager_h__
