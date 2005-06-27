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
#ifndef DELTA_DYNAMICNAMECONTROL
#define DELTA_DYNAMICNAMECONTROL

#include "dtEditQt/dynamicabstractcontrol.h"


namespace dtEditQt 
{

    /**
    * @class DynamicNameControl
    * @brief This is not really a dynamic control at all.  It is used to edit the proxies name in the tree.
    */
    class DynamicNameControl : public DynamicAbstractControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicNameControl();

        /**
         * Destructor
         */
        virtual ~DynamicNameControl();

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
         * @see DynamicAbstractControl#isEditable
         */
        virtual bool isEditable();

    public slots: 

        /**
         * @see DynamicAbstractControl#updateData
         */
        virtual bool updateData(QWidget *widget);

    protected:

    private:
    };

}

#endif
