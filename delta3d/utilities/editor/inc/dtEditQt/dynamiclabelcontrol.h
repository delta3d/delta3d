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
#ifndef DELTA_DYNAMICLABELCONTROL
#define DELTA_DYNAMICLABELCONTROL

#include "dtEditQt/dynamicabstractcontrol.h"


namespace dtDAL 
{
    //class StringActorProperty;
}

namespace dtEditQt
{

    /**
    * @class DynamicLabelControl
    * @brief This is the dynamic control that is a bit odd.  It serves a dual purpose. 
    * The first purpose is to work with a label data type (once it exists).  The second
    * is just a placeholder to add non-editable strings with no underlying data type.
    */
    class DynamicLabelControl : public DynamicAbstractControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicLabelControl();

        /**
         * Destructor
         */
        virtual ~DynamicLabelControl();

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
         * Use this for the 2nd use of this control, when you don't have a real 
         * property, but instead have some static strings you are working with.
         * If you call this AFTER initializeData(), these values will override the 
         * property values.
         * @note It'd be nice if this could be put in the constructor, however we 
         * have to be careful because the factory uses the constructor.  Also, using 
         * the constructor would prevent overriding the values for a real property
         */
        void setDisplayValues(QString name, QString description, QString valueAsString);

    public slots: 

        /**
         * @see DynamicAbstractControl#updateData
         */
        virtual bool updateData(QWidget *widget);

    protected:

    private:
        // eventually, this should be a label property
        //dtDAL::StringActorProperty *myProperty;

        // whether it has a property or not, this holds the display values
        // if you have a property, it originally comes from the property, if not,
        // then it is sent 
        QString valueAsString;
        QString description;
        QString name;
    };

}

#endif
