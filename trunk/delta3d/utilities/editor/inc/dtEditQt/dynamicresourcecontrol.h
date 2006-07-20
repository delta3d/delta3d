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
* @author Curtiss Murphy
*/
#ifndef DELTA_DYNAMICRESOURCECONTROL
#define DELTA_DYNAMICRESOURCECONTROL

#include "dtEditQt/dynamicabstractparentcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"
#include "dtEditQt/dynamiclabelcontrol.h"

namespace dtDAL 
{
    class ResourceActorProperty;
    class ResourceDescriptor;
}

class QLabel;

namespace dtEditQt 
{

    class SubQLabel;
    class SubQPushButton;

    /**
    * @class DynamicResourceControl
    * @brief This is the resource actor property.  It knows how to work with the various 
    * resource data types (Terrain, Character, Mesh, Texture, sound, ...) from DataTypes.h
    * This control is not editable, but has several child controls and some of them 
    * are editable.
    */
    class DynamicResourceControl : public DynamicAbstractParentControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicResourceControl();

        /**
         * Destructor
         */
        virtual ~DynamicResourceControl();

        /**
         * @see DynamicAbstractControl#initializeData
         */
        virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
            dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);

        /**
         * @see DynamicAbstractControl#getDisplayName
         */
        virtual const QString getDisplayName();

         /**
         * @see DynamicAbstractControl#getDescription
         */
        virtual const QString getDescription();

         /**
         * @see DynamicAbstractControl#getValueAsString
         */
        virtual const QString getValueAsString();

        /**
         * @see DynamicAbstractControl#updateEditorFromModel
         */
        virtual void updateEditorFromModel(QWidget *widget);

        /**
         * @see DynamicAbstractControl#updateModelFromEditor
         */
        virtual bool updateModelFromEditor(QWidget *widget);

        /**
         * @see DynamicAbstractControl#createEditor
         */
        virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
            const QModelIndex &index);

         /**
          * @see DynamicAbstractControl#isEditable
          */
        virtual bool isEditable();

         /**
          * @see DynamicAbstractControl#handleSubEditDestroy
          */
        virtual void handleSubEditDestroy(QWidget *widget);

        /** 
          * @see DynamicAbstractControl#installEventFilterOnControl
         */
        virtual void installEventFilterOnControl(QObject *filterObj);

    public slots: 

        virtual bool updateData(QWidget *widget);

        /**
         * The user pressed the 'Use Current' Button.  Grab the current resource.
         */
        void useCurrentPressed();

        /**
         * The user pressed the 'Clear' Button.  Clear out the resource.
         */
        void clearPressed();

        void actorPropertyChanged(ActorProxyRefPtr proxy,
            ActorPropertyRefPtr property);

    protected:

    private: 
        dtDAL::ResourceActorProperty *myProperty;

        // This pointer is not really in our control.  It is constructed in the createEditor() 
        // method and destroyed whenever QT feels like it (mostly when the control looses focus). 
        // We work around this by trapping the destruction of this object, it should
        // call our handleSubEditDestroy() method so we know to not hold this anymore
        SubQLabel *temporaryEditOnlyTextLabel;
        SubQPushButton *temporaryUseCurrentBtn;
        SubQPushButton *temporaryClearBtn;

        /**
         * Figure out which resource descriptor  to get from EditorData and get it.
         * @return the current resource descriptor for our type, else an empty one of if type is invalid.
         */
        dtDAL::ResourceDescriptor getCurrentResource();

    };


}

#endif
