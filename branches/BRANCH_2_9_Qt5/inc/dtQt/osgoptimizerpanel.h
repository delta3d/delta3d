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
*/

#ifndef DELTA_OSG_OPTIMIZER_PANEL_H
#define DELTA_OSG_OPTIMIZER_PANEL_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <QtGui/qcheckbox.h>
#include <osgUtil/Optimizer>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class OsgOptimizerPanel;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT OsgOptimizerPanel : public QWidget
   {
      Q_OBJECT
   public:
      typedef QWidget BaseClass;

      typedef osgUtil::Optimizer::OptimizationOptions OptimizerOptions;

      OsgOptimizerPanel(QWidget* parent = NULL);

      virtual ~OsgOptimizerPanel();

      OptimizerOptions GetOptions() const;

      void UpdateUI();

   public slots:
      void OnCheckboxChanged(int state);
      void OnSelectAllClicked();
      void OnSelectDefaultsClicked();

   protected:
      void CreateOptionMapping();

      void CreateConnections();

      Ui::OsgOptimizerPanel* mUI;

      OptimizerOptions mOptions;

      typedef std::map<OptimizerOptions, QCheckBox*> OptionCheckboxMap;
      typedef std::map<QCheckBox*, OptimizerOptions> CheckboxOptionMap;
      OptionCheckboxMap mCheckboxMap;
      CheckboxOptionMap mOptionMap;
   };
}

#endif
