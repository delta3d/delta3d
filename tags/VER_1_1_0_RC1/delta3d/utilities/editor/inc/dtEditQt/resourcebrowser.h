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
#ifndef __ResourceBrowser__h
#define __ResourceBrowser__h

#include <QWidget>
#include <QDockWidget>
#include <QMainWindow>
#include <QObject>

namespace dtEditQt 
{

    class TabWrapper;
    class TabContainer;
    class StaticMeshBrowser;
    class SoundBrowser;
    class ParticleBrowser;
    class TextureBrowser;
    //class CharacterBrowser;
    class TerrainBrowser;

    /**
    * @class ResourceBrowser
    * @brief This class holds all project resource tabs.
    */
    class ResourceBrowser : public QDockWidget
    {
        Q_OBJECT
    public:
        /**
        * Constructor
        */
        ResourceBrowser(QMainWindow * parent = 0);
        /**
        * Destructor
        */
        virtual ~ResourceBrowser();
        /**
        * Adds browser widgets to the tab container
        * @param void
        */
        void addTabs();
        /**
        * gets the resource browser widget
        * @return QWidget
        */
        QWidget *getWidget();

    private:
        TabWrapper *tabMesh;
        TabWrapper *tabSound;
        TabWrapper *tabParticle;
        TabWrapper *tabTexture;
        //TabWrapper *tabCharacter;
        TabWrapper *tabTerrain;
        TabContainer *tabC;

        StaticMeshBrowser *meshWidget;
        SoundBrowser      *soundWidget;
        ParticleBrowser   *particleWidget;
        TextureBrowser    *textureWidget;
        //CharacterBrowser  *characterWidget;
        TerrainBrowser    *terrainWidget;

        void closeEvent(QCloseEvent *e);
    };
}

#endif
