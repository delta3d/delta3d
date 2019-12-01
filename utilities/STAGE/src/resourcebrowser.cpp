/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Teague Coonan
 */
#include <prefix/stageprefix.h>
#include <QtGui/QAction>
#include <QtCore/QString>

#include <dtEditQt/resourcebrowser.h>
#include <dtEditQt/tabcontainer.h>
#include <dtEditQt/tabwrapper.h>
#include <dtEditQt/staticmeshbrowser.h>
#include <dtEditQt/texturebrowser.h>
#include <dtEditQt/soundbrowser.h>
#include <dtEditQt/shaderbrowser.h>
#include <dtEditQt/directorbrowser.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/uiresources.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ResourceBrowser::ResourceBrowser(QMainWindow* parent)
      : QDockWidget(tr("Resources"), parent, 0)
   {
      setWindowTitle(tr("Resources"));

      // container
      mTabC              = new TabContainer(this);

      bool enablePreview = true;
      bool enableCreateActor = true;
      // widgets
      addTab(new StaticMeshBrowser(dtCore::DataType::STATIC_MESH, this, enablePreview, enableCreateActor));
      addTab(new StaticMeshBrowser(dtCore::DataType::SKELETAL_MESH, this, enablePreview, enableCreateActor));
      addTab(new SoundBrowser(dtCore::DataType::SOUND, this));
      addTab(new StaticMeshBrowser(dtCore::DataType::PARTICLE_SYSTEM, this, enablePreview, enableCreateActor));
      addTab(new TextureBrowser(dtCore::DataType::TEXTURE, this));
      addTab(new StaticMeshBrowser(dtCore::DataType::VOLUME, this, false, false));
      addTab(new StaticMeshBrowser(dtCore::DataType::TERRAIN, this, false, false));
      addTab(new ShaderBrowser(dtCore::DataType::SHADER, this));
      addTab(new DirectorBrowser(dtCore::DataType::DIRECTOR, this));

      setWidget(mTabC->getWidget());
   }

   /////////////////////////////////////////////////////////////////////////////////
   ResourceBrowser::~ResourceBrowser() {}

   /////////////////////////////////////////////////////////////////////////////////
   void ResourceBrowser::addTab(ResourceAbstractBrowser* rab)
   {
      TabWrapper* tabW = new TabWrapper();
      // Static Mesh tab
      tabW->setWidget(rab);
      QString name = tr(rab->GetResourceType().GetDisplayName().c_str());
      tabW->setName(name);
      mTabC->addTab(tabW);
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* ResourceBrowser::getWidget()
   {
      return mTabC->getWidget();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void ResourceBrowser::closeEvent(QCloseEvent* e)
   {
      EditorActions::GetInstance().mActionWindowsResourceBrowser->setChecked(false);
   }

} // namespace dtEditQt
