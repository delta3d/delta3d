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
 
#ifndef systemview_h__
#define systemview_h__

#include <dtInspectorQt/iview.h>
#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtCore/system.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::System
   class SystemView : public IView
   {
      Q_OBJECT

   public:
      SystemView(Ui::InspectorWidget &ui);
      virtual ~SystemView();

      virtual void OperateOn(dtCore::Base* b);
      bool IsOfType(QString name, dtCore::Base* object);

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
#endif //systemview_h__
