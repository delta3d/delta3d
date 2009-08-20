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

class QCloseEvent;
class AIPropertyEditor;

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace Ui
{
   class MainWindow;
}

namespace dtAI
{
   class AIPluginInterface;
}

namespace dtDAL
{
   class PropertyContainer;
   class ActorProperty;
}
/// @endcond

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget& centralWidget);
   ~MainWindow();

   void showEvent(QShowEvent* e);
   void closeEvent(QCloseEvent* e);

   dtAI::AIPluginInterface* GetAIPluginInterface();

signals:
   void ProjectContextChanged(const std::string& path);
   void MapSelected(const std::string& path);
   void CloseMapSelected();

public slots:
   void ChangeProjectContext();
   void OnOpenMap();
   void OnCloseMap();
   void EnableOrDisableControls();
   void SetAIPluginInterface(dtAI::AIPluginInterface* interface);
   void SelectRenderingOptions();
   void PropertyChangedFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&);
private:
   void ChangeMap(const QString& newMap);

   Ui::MainWindow* mUi;
   QWidget& mCentralWidget;
   AIPropertyEditor& mPropertyEditor;
   dtAI::AIPluginInterface* mPluginInterface;

   QString mCurrentMapName;
};

#endif /*AIUTILITY_MAIN_WINDOW*/
