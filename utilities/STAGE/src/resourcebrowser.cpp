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

#include <dtEditQt/resourcebrowser.h>
#include <dtEditQt/tabcontainer.h>
#include <dtEditQt/tabwrapper.h>
#include <dtEditQt/staticmeshbrowser.h>
#include <dtEditQt/skeletalmeshbrowser.h>
#include <dtEditQt/particlebrowser.h>
#include <dtEditQt/texturebrowser.h>
#include <dtEditQt/soundbrowser.h>
//#include <dtEditQt/characterbrowser.h>
#include <dtEditQt/terrainbrowser.h>
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

      // tabs
      mTabMesh          = new TabWrapper(this);
      mTabSkeletal      = new TabWrapper(this);
      mTabSound         = new TabWrapper(this);
      mTabParticle      = new TabWrapper(this);
      mTabTexture       = new TabWrapper(this);
      mTabTerrain       = new TabWrapper(this);
      mTabShader        = new TabWrapper(this);
      mTabDirector      = new TabWrapper(this);

      // widgets
      mMeshWidget       = new StaticMeshBrowser(dtCore::DataType::STATIC_MESH, this);
      mSkeletalWidget   = new SkeletalMeshBrowser(dtCore::DataType::SKELETAL_MESH, this);
      mSoundWidget      = new SoundBrowser(dtCore::DataType::SOUND, this);
      mParticleWidget   = new ParticleBrowser(dtCore::DataType::PARTICLE_SYSTEM, this);
      mTextureWidget    = new TextureBrowser(dtCore::DataType::TEXTURE, this);
      mTerrainWidget    = new TerrainBrowser(dtCore::DataType::TERRAIN, this);
      mShaderWidget     = new ShaderBrowser(dtCore::DataType::SHADER, this);
      mDirectorWidget   = new DirectorBrowser(dtCore::DataType::DIRECTOR, this);

      addTabs();

      setWidget(mTabC->getWidget());
   }

   /////////////////////////////////////////////////////////////////////////////////
   ResourceBrowser::~ResourceBrowser() {}

   /////////////////////////////////////////////////////////////////////////////////
   void ResourceBrowser::addTabs()
   {
      // Static Mesh tab
      mTabMesh->setWidget(mMeshWidget);
      mTabMesh->setName("Static Mesh");
      mTabC->addTab(mTabMesh);
      //mTabC->addTab(mTabMesh, UIResources::ICON_STATICMESH_TAB.c_str());

      // Skeletal Mesh tab
      mTabSkeletal->setWidget(mSkeletalWidget);
      mTabSkeletal->setName("Skeletal Mesh");
      mTabC->addTab(mTabSkeletal);

      // Sound tab
      mTabSound->setWidget(mSoundWidget);
      mTabSound->setName("Sound");
      mTabC->addTab(mTabSound);
      //mTabC->addTab(mTabSound, UIResources::ICON_SOUND_TAB.c_str());

      // Particle tab
      mTabParticle->setWidget(mParticleWidget);
      mTabParticle->setName("Particle");
      mTabC->addTab(mTabParticle);
      //mTabC->addTab(mTabParticle, UIResources::ICON_PARTICLE_TAB.c_str());

      // Texture tab
      mTabTexture->setWidget(mTextureWidget);
      mTabTexture->setName("Texture");
      mTabC->addTab(mTabTexture);
      //mTabC->addTab(mTabTexture, UIResources::ICON_TEXTURE_TAB.c_str());

      //// Character tab
      //mTabCharacter->setWidget(mCharacterWidget);
      //mTabCharacter->setName("Characters");
      //mTabC->addTab(mTabCharacter);
      //mTabC->addTab(mTabCharacter, UIResources::ICON_CHARACTER_TAB.c_str());

      //// Terrain tab
      mTabTerrain->setWidget(mTerrainWidget);
      mTabTerrain->setName("Terrain");
      mTabC->addTab(mTabTerrain);
      //mTabC->addTab(mTabTerrain, UIResources::ICON_TERRAIN_TAB.c_str());

      // Shader tab
      mTabShader->setWidget(mShaderWidget);
      mTabShader->setName("Shader");
      mTabC->addTab(mTabShader);

      // Director tab
      mTabDirector->setWidget(mDirectorWidget);
      mTabDirector->setName("Director");
      mTabC->addTab(mTabDirector);
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
