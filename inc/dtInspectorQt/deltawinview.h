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
 
#ifndef deltawinview_h__
#define deltawinview_h__

#include <dtInspectorQt/iview.h>
#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtCore/deltawin.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class InspectorWidget;
}
/// @endcond

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::DeltaWin
   class DeltaWinView : public IView
   {
      Q_OBJECT

   public:
      DeltaWinView(Ui::InspectorWidget &ui);
      virtual ~DeltaWinView();

      virtual void OperateOn(dtCore::Base* b);
      bool IsOfType(QString name, dtCore::Base* object);

   protected slots:
      void OnFullScreenToggle(int checked);
      void OnCursorToggle(int checked);
      void OnTitleChanged(const QString& title);
      void OnPositionChanged();

   private:
      void Update();

      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::DeltaWin> mOperateOn;

   };
}  
#endif //deltawinview_h__
