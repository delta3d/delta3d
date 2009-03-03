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
 
#ifndef objectmanager_h__
#define objectmanager_h__

#include <dtInspectorQt/imanager.h>
#include <QtCore/QObject>
#include <dtCore/observerptr.h>
#include <dtCore/object.h>

namespace Ui
{
   class InspectorWidget;
}

namespace dtInspectorQt
{
   ///Handles the properties of dtCore::Object
   class ObjectManager : public IManager
   {
      Q_OBJECT

   public:
      ObjectManager(Ui::InspectorWidget &ui);
      virtual ~ObjectManager();

      virtual void OperateOn(dtCore::Base* b);

   protected slots:
      void OnScaleChanged(double val);
      void OnOffsetChanged(double val);
      void OnFilenameChanged();
      void OnFindFile();

   private:
      Ui::InspectorWidget* mUI;
      dtCore::ObserverPtr<dtCore::Object> mOperateOn;

      void Update();

   };
}  
#endif //objectmanager_h__
