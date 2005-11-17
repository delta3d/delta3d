/*
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc.
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* @author Teague Coonan
*/

#include <QAction>

#include "dtEditQt/resourcebrowser.h"
#include "dtEditQt/tabcontainer.h"
#include "dtEditQt/tabwrapper.h"
#include "dtEditQt/staticmeshbrowser.h"
#include "dtEditQt/particlebrowser.h"
#include "dtEditQt/texturebrowser.h"
#include "dtEditQt/soundbrowser.h"
//#include "dtEditQt/characterbrowser.h"
#include "dtEditQt/terrainbrowser.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    ResourceBrowser::ResourceBrowser(QMainWindow * parent): QDockWidget(tr("Resources"), parent, 0)
    {
        setWindowTitle(tr("Resources"));
        // container
        tabC              = new TabContainer();

        // tabs
        tabMesh           = new TabWrapper();
        tabSound          = new TabWrapper();
        tabParticle       = new TabWrapper();
        tabTexture        = new TabWrapper();
        //tabCharacter      = new TabWrapper();
        tabTerrain        = new TabWrapper();

        // widgets
        meshWidget        = new StaticMeshBrowser(dtDAL::DataType::STATIC_MESH);
        soundWidget       = new SoundBrowser(dtDAL::DataType::SOUND);
        particleWidget    = new ParticleBrowser(dtDAL::DataType::PARTICLE_SYSTEM);
        textureWidget     = new TextureBrowser(dtDAL::DataType::TEXTURE);
        //characterWidget   = new CharacterBrowser(dtDAL::DataType::CHARACTER);
        terrainWidget     = new TerrainBrowser(dtDAL::DataType::TERRAIN);

        addTabs();

        setWidget(tabC->getWidget());
    }
    /////////////////////////////////////////////////////////////////////////////////
    ResourceBrowser::~ResourceBrowser(){}
    /////////////////////////////////////////////////////////////////////////////////
    void ResourceBrowser::addTabs()
    {
        // Static Mesh tab
        tabMesh->setWidget(meshWidget);
        tabMesh->setName("Static Mesh");
        tabC->addTab(tabMesh);
        //tabC->addTab(tabMesh, UIResources::ICON_STATICMESH_TAB.c_str());

        // Sound tab
        tabSound->setWidget(soundWidget);
        tabSound->setName("Sound");
        tabC->addTab(tabSound);
        //tabC->addTab(tabSound, UIResources::ICON_SOUND_TAB.c_str());

        // Particle tab
        tabParticle->setWidget(particleWidget);
        tabParticle->setName("Particle");
        tabC->addTab(tabParticle);
        //tabC->addTab(tabParticle, UIResources::ICON_PARTICLE_TAB.c_str());

        // Texture tab
        tabTexture->setWidget(textureWidget);
        tabTexture->setName("Texture");
        tabC->addTab(tabTexture);
        //tabC->addTab(tabTexture, UIResources::ICON_TEXTURE_TAB.c_str());

        //// Character tab
        //tabCharacter->setWidget(characterWidget);
        //tabCharacter->setName("Characters");
        //tabC->addTab(tabCharacter);
        //tabC->addTab(tabCharacter, UIResources::ICON_CHARACTER_TAB.c_str());

        //// Terrain tab
        tabTerrain->setWidget(terrainWidget);
        tabTerrain->setName("Terrain");
        tabC->addTab(tabTerrain);
        //tabC->addTab(tabTerrain, UIResources::ICON_TERRAIN_TAB.c_str());
    }
    /////////////////////////////////////////////////////////////////////////////////
    QWidget *ResourceBrowser::getWidget()
    {
        return tabC->getWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////
    void ResourceBrowser::closeEvent(QCloseEvent *e)
    {
        EditorActions::getInstance().actionWindowsResourceBrowser->setChecked(false);
    }
}
