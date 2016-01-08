/* -*-c++-*-
* Copyright (C) 2015, Caper Holdings LLC
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
* Chris Rodgers
*/

#ifndef DELTA_STATESETPANEL_H
#define DELTA_STATESETPANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtCore/refptr.h>
// Qt
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QWidget>
// OSG
#include <osg/StateSet>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class StateSetPanel;
}



namespace dtQt
{
   class TextureListItem;

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT StateSetPanel : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

	  StateSetPanel(QWidget* parent = nullptr);

	  virtual ~StateSetPanel();

	  void SetStateSet(osg::StateSet* stateset);
	  osg::StateSet* GetStateSet() const;

      /**
       * Update the UI values from the referenced data.
       */
      virtual void UpdateUI();

      /**
       * Update the referenced data with values from the UI.
       */
      virtual void UpdateData();

      int Clear();

      void dragEnterEvent(QDragEnterEvent* event);
      void dragMoveEvent(QDragMoveEvent* event);
      void dropEvent(QDropEvent* event);

   signals:
      void SignalUIChanged(); // Signal to emit if user changes value.
      void SignalDataChanged(); // Signal to emit if underlying property value changed.

      void SignalUIUpdated(); // Signal to emit when UI value matches the data.
      void SignalDataUpdated(); // Signal to emit when data has been updated with values from UI.
      void SignalDataAboutToUpdate(); // Signal to emit before data is modified with values from UI.

   public slots:
      void OnUIChanged();
      void OnDataChanged();
      void OnItemClicked(QTreeWidgetItem* item, int column);
      void OnTextureUnitChanged(int unit);

   protected:
      virtual void CreateConnections();

      void UpdateListLabels();

      int FindFreeTextureUnit() const;

      TextureListItem* mCurrentItem;
      dtCore::RefPtr<osg::StateSet> mStateSet;
	   Ui::StateSetPanel* mUI;
   };
}

#endif
