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

#ifndef DELTA_ANIMATION_CONTROL_DOCK_H
#define DELTA_ANIMATION_CONTROL_DOCK_H

#include "ui_AnimationControlPanel.h"
#include <QtGui/QDockWidget>
// DELTA3D
#include <dtAnim/animclippath.h>
#include <dtCore/observerptr.h>
#include <dtCore/object.h>



/////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
/////////////////////////////////////////////////////////////////////////////////
class QTableWidgetItem;

namespace Ui
{
   class AnimationControlPanel;
}

namespace dtAnim
{
    class AnimCallbackVisitor;
}



/////////////////////////////////////////////////////////////////////////////////
// ANIMATION PARAMS STRUCT CODE
/////////////////////////////////////////////////////////////////////////////////
struct AnimParams
{
   std::string mName;
   double mBeginFrame;
   double mEndFrame;
   double mBeginFrameOffset;
   double mSpeed;
   int mLoopLimit;
   int mPlayMode;

   AnimParams();
};



/////////////////////////////////////////////////////////////////////////////////
// ANIMATION PARAMS TABLE ROW CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class AnimationParamsTableRow : public osg::Referenced
{
public:
   enum Column{NAME, BEGIN, END, OFFSET, SPEED, PLAYMODE, LIMIT, MAX_COLUMNS};

   AnimationParamsTableRow();

   void InsertValuesToTable(QTableWidget& table, int rowIndex);

   void SetRowValues(QTableWidget& table, int rowIndex);

   AnimParams& GetParams();
   const AnimParams& GetParams() const;

protected:
   virtual ~AnimationParamsTableRow() {}

private:
   AnimParams mParams;
};



/////////////////////////////////////////////////////////////////////////////////
// ANIMATION CONTROL DOCK CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class AnimationControlDock : public QDockWidget
{
   Q_OBJECT

public:
   typedef QDockWidget BaseClass;

   AnimationControlDock(QWidget* parent = 0);

   void UpdateUI();

   void SetPlayMode(int playMode) const;
   osg::AnimationPath::LoopMode GetPlayMode() const;

   osg::AnimationPath::LoopMode ConvertToPlayMode(int playMode) const;

public slots:
   void OnPause();
   void OnPlay();
   void OnReset();
   void OnAdd();
   void OnRemove();
   void OnSpeedChanged();
   void OnBeginFrameOffsetChanged();
   void OnLoopLimitChanged();
   void OnPlayModeChanged(bool checked);
   void OnItemSelect(int row, int column);
   void OnGeometryLoaded(dtCore::Object* object);

protected:
   void CreateConnections();
   void ResetUI();

   void ApplyAnimationParameters(dtAnim::AnimCallbackVisitor& visitor);

   void UpdateAnimParamsFromUI(AnimParams& outParams);

   void UpdateUIFromAnimParams(const AnimParams& params);

   osg::Node* GetRootNode();

   void InternalPlay(bool reset);
   void InternalPause();

   std::string GetRowID(int rowIndex) const;
   int GetRowIndex(const std::string& animName) const;

private:
   Ui::AnimationControlPanel mUI;
   dtCore::ObserverPtr<dtCore::Object> mObject;

   typedef std::map<std::string, dtCore::RefPtr<AnimationParamsTableRow> > AnimParamRows;
   AnimParamRows mAnimParams;

   bool mPaused;
};


#endif
