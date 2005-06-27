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
#ifndef DELTA_DYNAMICABSTRACTCONTROL
#define DELTA_DYNAMICABSTRACTCONTROL

#include <QObject>
#include <osg/ref_ptr>
#include "dtDAL/log.h"

class QWidget;
class QGridLayout;
class QStyleOptionViewItem;
class QModelIndex;
class QLineEdit;
class QPainter;
class QSize;
class QColor;

namespace dtDAL 
{
    class ActorProxy;
    class ActorProperty;
}

namespace dtEditQt 
{

// The number of digits to the right of a decimal point for floats/doubles
#define NUM_DECIMAL_DIGITS 5

    class PropertyEditorModel;
    class PropertyEditorTreeView;

    /**
    * @class DynamicAbstractControl
    * @brief This is the base class for all the dynamic controls that are used in the property editor
    */
    class DynamicAbstractControl : public QObject
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicAbstractControl();

        /**
         * Destructor
         */
        virtual ~DynamicAbstractControl();

        /**
         * Initialize the object.  We don't do this in the constructor because it's created via the
         * ObjectFactory which primarily works with parameterless constructors at the moment.
         * @note - The base class does not store the property.  Each subclass is responsible for doing
         * that.
         */
        virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
            dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);

        /**
         * This method is called by the delegate between the model and the view when data should
         * be pushed to the editor from the model.  I'm not exactly sure when that happens, but
         * I assume that's at construction time of the editor.  The default does nothing.
         *
         * @NOTE - In all likelihood, this method won't need the widget that's passed in,
         * since it's tracked by the individual subclass.
         */
        virtual void updateEditorFromModel(QWidget *widget);

        /**
         * This method is called by the delegate between the model and the view when data should
         * be pushed into the model from the editor.  This is called when the control thinks
         * it's done editing.  Perhaps the user pressed a key, or tabbed away, or something
         * like that.  The default does nothing and returns false.
         *
         * @return Returns true if the model was successfully changed.  ie, the data was different
         * and there were no errors. 
         *
         * @Note It would be nice if the caller of this method would generate the event message.  
         * However, since it can be called from a variety of places, we have to generate the 
         * property changed event from inside here.  So, don't forget :)
         */
        virtual bool updateModelFromEditor(QWidget *widget);

        /**
         * This control should add itself to the passed in parent and layout in whatever fashion
         * necessary.  Complex controls probably have several nested pieces.  For a grid layout,
         * the label is the first column and the data goes into the 2nd column.
         */
        //virtual void addSelfToParentWidget(QWidget &parent, QGridLayout &layout, int row) = 0;

        /**
         * Create the editor widget to be used when editing this control.  It can be complex
         * or simple.   The default method does nothing.
         */
        virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
            const QModelIndex &index);

        /**
         * Returns the parent of this control.  NULL if there is no parent.  All controls have
         * a parent except for those at the root.
         */
        DynamicAbstractControl *getParent();

        /**
         * This retuns the index of the child so that the model can construct a QModelIndex.
         * Note that not all dynamic controls can have children.  However, there are several
         * that can (such as Groups and complex controls like the Vec3).  Therefore, we
         * put the behavior on the base.  If a control doesn't support children, just returns 0.
         * Default is 0.
         */
        virtual int getChildIndex(DynamicAbstractControl *child);

        /**
         * This is the reverse of getChildIndex.  It gets the child from the index.  Note again that
         * not all controls can have children.  If you don't have children, then return NULL.
         * Default is NULL
         */
        virtual DynamicAbstractControl *getChild(int index);

        /**
         * This tells you how many children you have.  Note again that not all controls have children.
         * Default is 0.
         */
        virtual int getChildCount();

        /**
         * Essentially returns a displayable string.  Each control needs to handle this specially, since
         * some controls have properties and some don't.  Ie, a group control doesn't have a real
         * property under it, nor does the X and Y values of Vec3. Default is "".
         */
        virtual const QString getDisplayName();

        /**
         * Returns a helpful description.  Usually a tooltip string.  Each control needs to handle
         * this, since some controls have properties and some don't.  Ie, a group control doesn't
         * have a real property under it, nor does the X and Y values of Vec3. Default is "".
         */
        virtual const QString getDescription();

        /**
         * Returns a string representation of the value.  This is typically only useful for
         * controls that are not editable.  Default is "".
         */
        virtual const QString getValueAsString();

        /**
         * Returns true if this control is typically editable.  For instance, a float control is editable,
         * but a group isn't.  Note that the subelements of a vector are editor whereas the vector control
         * itself isn't editable.  If you return true, then you need to be able to delegate a control
         * to edit the data.  Default is false.
         */
        virtual bool isEditable();

        /**
         * Returns true if this control knows how to do custom painting for a specific column.  This is 
         * necessary for some controls that have non string elements like a button or a color swatch. 
         * If this method returns true, you MUST override the paint and sizeHint methods or you won't 
         * see anything for your row. This method is based on the column, so you will only get 
         * paint and sizehint calls for a column that returns true.  Default is false.
         * @note Be careful using this on editable columns as you may get yourself in trouble with 
         * nested and competing paints.
         * @return True if your control does custom paint and sizeHint() for this column.
         */
        virtual bool isControlDoesCustomPainting(int column);

        /**
         * This method allows you to do a custom paint on a specific column for a control. 
         * It works with isControlDoesCustomPainting and sizeHint().
         * For instance, the color control might put up a specific picker button or maybe a little
         * color swatch of the selected color.  The only way to do that would be in here. 
         */
        virtual void paintColumn(int column, QPainter *painter, const QStyleOptionViewItem &opt);

        /**
         * This method works with isControlDoesCustomPainting() and paintcolumn.  You should
         * return the size hint for your column.  Default does nothing.
         * @return the QSize sizehint for this column in the tree.
         */
        virtual QSize sizeHint(int column, const QStyleOptionViewItem &opt);

        /**
         * Gets the editor model from the control.  It should be rare that you ever need this.
         */
        inline PropertyEditorModel *getModel()
        { return model; }

        /**
         * This was a temp hack used to set a parent widget for creating sub widgets. 
         * This shouldn't be needed and can be deleted once the final tree is settled on
         */
        void setTreeView(PropertyEditorTreeView *newPropertyTree);

        /** 
         * This method is called by one of the Sub Widgets from DynamicSubWidgets.h from the 
         * destructor of the passed in widget.  The intent is to work around the way editors 
         * work in QT trees.  Since the editors are created only when the user should be editing the
         * field and then destroyed as soon as they lose focus, it is important that any Dynamic
         * controls not be holding on to bogus pointers.  This method gives you a chance to set
         * a pointer back to NULL or whatever.
         */
        virtual void handleSubEditDestroy(QWidget *widget);

        /**
         * Called when the abstract control is constructed to determine whether or not we 
         * need a persistent editor.  Persistent editors are created after the control is 
         * created and closed when the control is removed from the tree. This is used 
         * for some controls that need to ALWAYS show a button or edit box in order to 
         * prevent user confusion.  Default is false;
         * 
         * @note This doesn't work.  Well, the method works, but there is currently no way 
         * to make an editor persistent as I'd like.  By persistent, I mean, actually make
         * the control show up permanently, so that the user doesn't have to click anything
         * to make it show up. 
         */
        virtual bool isNeedsPersistentEditor();

        /**
         * A simple utility method to set the background color for all modes for a widget. 
         * This is typically used when we have wrapper widgets with children in the edit 
         * portion of a dynamic control to make the control blend in better...  
         */
       void setBackgroundColor(QWidget *widget, QColor color);

        /** 
         * Allows the abstract control to work with the delegate event filter.  If you
         * have any nested controls that are created as part of the editor, you need to 
         * use one of the sub controls and make sure you handle NULL'ing your pointer and 
         * then override this method to install an event filter on your non-null editor objects.
         * This is called by the PropertyEditorDelegate after the editor is created.
         * The default calls QObject::installEventFilter()
         */
        virtual void installEventFilterOnControl(QObject *filterObj);

    public slots:

        /**
         * Called when we should take the data out of the controls and put it into the
         * actor.  This is typically trapped to a lost focus or return pressed or similar
         * user event behavior.  This can also be called by the parent control at the
         * moment we change selection.
         * @return Returns true if any data was actually changed and sucessfully set on the control
         * @Note - This is purely virtual
         */
        virtual bool updateData(QWidget *widget) = 0;

    protected:
        // indicates whether the object has been initialized
        bool initialized;

        osg::ref_ptr<dtDAL::ActorProxy> proxy;
        //osg::ref_ptr<dtDAL::ActorProperty> property;

        // The parent control of this control.  All controls have a parent except root level
        // controls which are likely to be groups
        DynamicAbstractControl *parent;
        PropertyEditorModel *model;

        PropertyEditorTreeView *propertyTree;
    };

}

#endif
