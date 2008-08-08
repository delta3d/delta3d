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
#include <prefix/dtstageprefix-src.h>
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
#include <dtEditQt/editoractions.h>
#include <dtEditQt/uiresources.h>

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
        tabSkeletal       = new TabWrapper();
        tabSound          = new TabWrapper();
        tabParticle       = new TabWrapper();
        tabTexture        = new TabWrapper();
        tabTerrain        = new TabWrapper();

        // widgets
        meshWidget        = new StaticMeshBrowser(dtDAL::DataType::STATIC_MESH);
        skeletalWidget    = new SkeletalMeshBrowser(dtDAL::DataType::SKELETAL_MESH);
        soundWidget       = new SoundBrowser(dtDAL::DataType::SOUND);
        particleWidget    = new ParticleBrowser(dtDAL::DataType::PARTICLE_SYSTEM);
        textureWidget     = new TextureBrowser(dtDAL::DataType::TEXTURE);
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

        // Skeletal Mesh tab
        tabSkeletal->setWidget(skeletalWidget);
        tabSkeletal->setName("Skeletal Mesh");
        tabC->addTab(tabSkeletal);

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
        EditorActions::GetInstance().actionWindowsResourceBrowser->setChecked(false);
    }
}
