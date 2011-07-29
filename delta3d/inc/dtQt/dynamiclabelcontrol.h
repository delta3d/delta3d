/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamiclabelcontrol (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_DYNAMICLABELCONTROL
#define DELTA_DYNAMICLABELCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>


namespace dtCore
{
    //class StringActorProperty;
}

namespace dtQt
{

    /**
    * @class DynamicLabelControl
    * @brief This is the dynamic control that is a bit odd.  It serves a dual purpose.
    * The first purpose is to work with a label data type (once it exists).  The second
    * is just a placeholder to add non-editable strings with no underlying data type.
    */
    class DT_QT_EXPORT DynamicLabelControl : public DynamicAbstractControl
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
         * @see DynamicAbstractControl#InitializeData
         */
        virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
            dtCore::PropertyContainer* newPC, dtCore::ActorProperty* property);

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
         * If you call this AFTER InitializeData(), these values will override the
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
        virtual bool updateData(QWidget* widget);

    protected:

    private:
        // eventually, this should be a label property
        //dtCore::StringActorProperty *myProperty;

        // whether it has a property or not, this holds the display values
        // if you have a property, it originally comes from the property, if not,
        // then it is sent
        QString mValueAsString;
        QString mDescription;
        QString mName;
    };

} // namespace dtQt

#endif // DELTA_DYNAMICLABELCONTROL
