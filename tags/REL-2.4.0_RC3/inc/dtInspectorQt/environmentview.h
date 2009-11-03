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
 
#ifndef environmentview_h__
#define environmentview_h__

#include <dtInspectorQt/iview.h>
#include <QtCore/QObject>
#include <QtGui/QDateTimeEdit>
#include <dtCore/observerptr.h>
#include <dtCore/environment.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Environment
   class EnvironmentView : public IView
   {
      Q_OBJECT

   public:
      EnvironmentView(Ui::InspectorWidget &ui);
      virtual ~EnvironmentView();

      virtual void OperateOn(dtCore::Base* b);
      bool IsOfType(QString name, dtCore::Base* object);

   protected slots:
      void OnFogToggled(int checked);
      void OnVisibilityChanged(double newValue);
      void OnFogColorChanged(double newValue);
      void OnFogColorPickerClicked();
      void OnSkyColorChanged(double newValue);
      void OnSkyColorPickerClicked();
      void OnDateTimeChanged(QDateTime newDateTime);
      void OnRefPositionChanged(double newValue);

   private:
      void Update();

      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::Environment> mOperateOn;

   };
}  
#endif //environmentview_h__
