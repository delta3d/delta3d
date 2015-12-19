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

#ifndef DELTA_MOTION_MODEL_TOOLBAR_H
#define DELTA_MOTION_MODEL_TOOLBAR_H

#include "ui_MotionModelToolbar.h"
#include <dtUtil/enumeration.h>
#include <QtWidgets/QToolBar>



/////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
/////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class MotionModelToolbar;
}



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
enum MotionModelTypeE
{
   MM_FLY,
   MM_ORBIT,
   MM_UFO
};

class MotionModelType : public dtUtil::Enumeration
{
   DECLARE_ENUM(MotionModelType);

public:
   typedef dtUtil::Enumeration BaseClass;

   static const MotionModelType FLY;
   static const MotionModelType ORBIT;
   static const MotionModelType UFO;

   MotionModelTypeE GetValue() const;

   const std::string& GetIconPath() const;

protected:
   MotionModelType(const std::string& name, MotionModelTypeE value, const std::string& iconPath);
   virtual ~MotionModelType();

   MotionModelTypeE mValue;
   std::string mIconPath;
};



////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
Q_DECLARE_METATYPE(MotionModelTypeE);
static const int typePhysObjArray = qRegisterMetaType<MotionModelTypeE>();



/////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class MotionModelToolbar;
class MotionModelPropertiesWidget : public QWidget
{
   Q_OBJECT
public:
   typedef QWidget BaseClass;

   MotionModelPropertiesWidget(QWidget* parent = 0);

private:
   friend class MotionModelToolbar;

   Ui::MotionModelToolbar mUI;
};



/////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class MotionModelToolbar : public QToolBar
{
   Q_OBJECT

public:
   typedef QToolBar BaseClass;

   MotionModelToolbar(QWidget* parent = 0);

   virtual ~MotionModelToolbar();

   void UpdateUI();

   const MotionModelType& GetCurrentMotioModel() const;

public slots:
   void OnButtonClick(QAction* action);
   void OnSpeedChanged(double speed);

signals:
   void SignalMotionModelSelected(MotionModelTypeE motionModelType);
   void SignalMotionModelSpeedChanged(MotionModelTypeE motionModelType, float);

protected:
   void CreateConnections();

private:
   MotionModelPropertiesWidget* mPropWidget;
   const MotionModelType* mCurrentMotionModel;

   typedef std::map<QAction*, const MotionModelType*> ActionMotionModelTypeMap;
   ActionMotionModelTypeMap mActionMap;
};


#endif
