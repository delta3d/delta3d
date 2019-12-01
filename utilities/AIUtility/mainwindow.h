/* -*-c++-*-
 * Delta3D
 * Copyright 2009, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef AIUTILITY_MAIN_WINDOW
#define AIUTILITY_MAIN_WINDOW

#include <QtGui/QMainWindow>
#include <dtAI/primitives.h>
#include <dtCore/transform.h>

#include <vector>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class MainWindow;
}

namespace dtAI
{
   class AIPluginInterface;
   class WaypointInterface;
}

namespace dtCore
{
   class PropertyContainer;
   class ObjectType;
   class ActorProperty;
}

class QCloseEvent;
class QUndoStack;
class QUndoCommand;
class AIPropertyEditor;
class WaypointBrowser;
/// @endcond

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   static const std::string ORG_NAME;
   static const std::string APP_NAME;
   static const std::string PROJECT_CONTEXT_SETTING;
   static const std::string CURRENT_MAP_SETTING;
   static const std::string WINDOW_SETTINGS;
   static const std::string PREFERENCES_SETTINGS;
   static const std::string SELECTION_RENDERING_SETTING;
   static const std::string RENDER_BACKFACES_SETTING;

   MainWindow(QWidget& centralWidget);
   ~MainWindow();

   void showEvent(QShowEvent* e);
   void closeEvent(QCloseEvent* e);

   dtAI::AIPluginInterface* GetAIPluginInterface();

signals:
   void ProjectContextChanged(const std::string& path);
   void MapSelected(const std::string& path);
   void CloseMapSelected();
   void RequestCameraTransformChange(const dtCore::Transform& xform);
   void AddAIInterfaceToMap(const std::string& map);
   void WaypointBrushSelectMode(bool enabled);
   void WaypointBrushSizeChanged(double value);
   void GroundClampSelectedWaypoints();
   void RenderOnSelection(bool enabled);
   void RenderBackfaces(bool shouldRender);
   void WaypointPropertyBaseChanged();

public slots:
   void OnError(const std::string& message);
   void ChangeProjectContext();
   void OnOpenMap();
   void OnCloseMap();
   void OnSave();
   void OnPreferences();
   void EnableOrDisableControls();
   void SetAIPluginInterface(dtAI::AIPluginInterface* interface, bool selectionBasedRenderingHint);
   void SelectRenderingOptions();
   void OnAddEdge();
   void OnRemoveEdge();
   void OnAddBiDirectionalEdge();
   void OnRemoveBiDirectionalEdge();
   void OnConvertWaypointTypes();
   void PropertyChangedFromControl(dtCore::PropertyContainer&, dtCore::ActorProperty&);
   void OnCameraTransformChanged(const dtCore::Transform& xform);

   void OnPropertyEditorShowHide(bool);
   void OnWaypointBrowserShowHide(bool);
   void OnChildRequestCameraTransformChange(const dtCore::Transform& xform);

   void OnWaypointSelectionChanged(std::vector<dtAI::WaypointInterface*>& selectedWaypoints);
   void OnSelectAllWaypoints();
   void OnDeselectAllWaypoints();
   void OnSelectInverseWaypoints();
   void OnUndoCommandCreated(QUndoCommand* undoCommand);

private slots:
   void OnModifiedChanged();
   void OnSelectWaypointPointMode();
   void OnSelectWaypontBrushMode();
   void OnWaypointBrushSizeChanged(double value);
   void OnWaypointTypeSelectionChanged(const dtCore::ObjectType* type);
   void OnGroundClampSelectedWaypoints();

private:
   void ChangeMap(const QString& newMap);

   bool DoesEdgeExistBetweenWaypoints(dtAI::WaypointInterface* waypointStart,
                                      dtAI::WaypointInterface* waypointEnd);

   void RefreshPropertyEditor(std::vector<dtAI::WaypointInterface*>& selectedWaypoints);

   bool MaybeSave();

   Ui::MainWindow* mUi;
   QWidget& mCentralWidget;
   AIPropertyEditor& mPropertyEditor;
   WaypointBrowser* mWaypointBrowser;
   dtAI::AIPluginInterface* mPluginInterface;

   QString mCurrentMapName;
   dtCore::Transform mCurrentCameraTransform;

   QUndoStack* mUndoStack;
   bool mSelectionBasedRendering; ///Do we only render expensive graphics for selected waypoints
   bool mRenderBackfaces;

   void RemoveDegenerateEdges(const dtAI::WaypointID sourcePoint,
      std::vector<const dtAI::WaypointInterface*>& connections);
};

#endif /*AIUTILITY_MAIN_WINDOW*/
