/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - ActorSearchDockWidget (.h & .cpp) - Using 'The MIT License'
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
 * Curtiss Murphy
 */
#ifndef DELTA_STAGE_ACTORSEACHTAB__H_
#define DELTA_STAGE_ACTORSEACHTAB__H_

#include <QtCore/QObject>
#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>

class QWidget;

namespace dtEditQt
{

    class TabWrapper;
    class TabContainer;
    class ActorBrowser;
    class ActorSearcher;
    class ActorGlobalBrowser;
    class PrefabBrowser;

    /**
    * @class ActorSearchDockWidget
    * @brief This class holds all actor related tabs.
    */
    class ActorSearchDockWidget : public QDockWidget
    {
    public:
        /**
        * Constructor
        */
        ActorSearchDockWidget(QWidget* parent = 0);

        /**
        * Destructor
        */
        ~ActorSearchDockWidget();

        /**
        * Adds tab widgets to the tab container
        */
        void addTabs();

        /**
        * gets the actor tab widget
        * @return The main QWidget control for the tab
        */
        QWidget* getWidget();

    private:
        TabContainer* mTabC;        
        TabWrapper*   mTabActorSearch;
        TabWrapper*   mTabGlobalActor;        
        
        ActorSearcher*      mActorSearchWidget;
        ActorGlobalBrowser* mActorGlobalWidget;        

        void closeEvent(QCloseEvent* e);
    };

} // namespace dtEditQt

#endif // DELTA_STAGE_ACTORSEACHTAB__H_
