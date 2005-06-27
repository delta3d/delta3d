/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* @author Curtiss Murphy
*/
#ifndef __PropertyEditor__h
#define __PropertyEditor__h

#include <QDockWidget>
#include <osg/ref_ptr>
#include <vector>
#include <tree.h>
#include <dtUtil/objectfactory.h>
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/actortype.h"

class QMainWindow;
class QTreeWidget;
class QLabel;
class QGridLayout;
class QScrollView;
class QGroupBox;

namespace dtDAL
{
    class DataType;
}

namespace dtEditQt
{

    class DynamicAbstractControl;
    class DynamicAbstractParentControl;
    class DynamicGroupControl;
    class PropertyEditorTreeView;
    class PropertyEditorModel;

    /**
    * This class is the property editor for displaying and editing properties of selected objects.  It shows the
    * information about the selected actor(s) including type, name, location, rotation, etc...  It is a
    * dockable window.
    */
    class PropertyEditor : public QDockWidget
    {
        Q_OBJECT

    public:
        /**
        * Constructor
        */
        PropertyEditor(QMainWindow * parent);

    public slots:

        /**
        * Handles the actor selection changed event message from EditorEvents
        */
        void handleActorsSelected(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > &actors);

        void actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property);

        void proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy, std::string oldName);

    private:

        // list of what the editor thinks is the last known selected actors
        std::vector <osg::ref_ptr<dtDAL::ActorProxy> > selectedActors;

        //QGroupBox *actorInfoBox;
        QString baseGroupBoxName;
        QGroupBox *actorPropBox;
        QGridLayout *mainGridLayout;
        QWidget *mainAreaWidget;
        QGridLayout *dynamicControlLayout;
        PropertyEditorTreeView *propertyTree;
        PropertyEditorModel *propertyModel;
        DynamicGroupControl *rootProperty;

        // the dynamic control factory can create objects for each type.
        osg::ref_ptr<dtUtil::ObjectFactory<dtDAL::DataType *, DynamicAbstractControl> > controlFactory;

        // this is a tree of property group names which were expanded.  It is used
        // when we change selected actors.  We walk the property tree and look for
        // expanded items.  For each one, we add it to the tree.  Then, we walk
        // back through this tree to reexpand items.
        core::tree<QString> expandedTreeNames;
        int lastScrollBarLocation;

        /**
         * Creates the main User Interface for the Property Editor.
         * @note The property editor is mostly driven by dynamic controls so the look and
         *    feel can completely change depending on what you have selected.
         */
        void setupUI();

        /**
         * Clear out and rebuild the property editor dialog.  This should be called when
         * you know or suspect that the selected actor has changed.  It will recreate all the
         * appropriate controls.
         */
        void refreshSelectedActors();

        /**
         * Add all the dynamic controls for this proxy object.
         */
        void buildDynamicControls(osg::ref_ptr<dtDAL::ActorProxy> proxy);

        /**
         * Indicate selection information in the group box title
         */
        void resetGroupBoxLabel();

        /**
         * Looks at the current property tree and tries to mark which group controls are
         * currently expanded.  This is then used to re-expand them with restorePreviousExpansion()
         * after the tree is rebuilt.
         */
        void markCurrentExpansion();

        /**
         * recursive method to support markCurrentExpansion().
         */
        void recurseMarkCurrentExpansion(DynamicAbstractControl *parent,
            core::tree<QString> &currentTree);

        /**
         * Attempts to re-expand previously expanded tree nodes.  This is a nicity for the user
         * for when they switch selected objects.  If they were toggling between 2 different
         * objects, it would be extremely annoying that you had to re-expand the controls each time.
         */
        void restorePreviousExpansion();

        /**
         * Recursive method to support restorePreviousExpansion().
         */
        void recurseRestorePreviousExpansion(DynamicAbstractControl *parent, core::tree<QString> &currentTree);

        /// Called on the close event
        void closeEvent(QCloseEvent *e);
    };

}

#endif
