/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamiccolorelementcontrol (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_DYNAMICCOLORELEMENTCONTROL
#define DELTA_DYNAMICCOLORELEMENTCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamicsubwidgets.h>

namespace dtCore
{
    //class Vec3ActorProperty;
    class ColorRgbaActorProperty;
}

namespace dtQt
{

    /**
     * @class DynamicColorElementControl
     * @brief This is the a sub control used by the various color property classes.
     * In order to draw a vector 3 in the property tree, you actually have
     * an X, Y, and Z entry.  This class is for that.  And, this class actually supports
     * the ColorRgbaActorProperty, and ColorRgbActorProperty with separate contructors.
     * It provides a get and set method to get at the data.
     */
    class DT_QT_EXPORT DynamicColorElementControl : public DynamicAbstractControl
    {
        Q_OBJECT

    public:
        /**
         * Constructor - For the ColorRgbaActorProperty property
         *
         * @note - We can put data in the constructor because aren't using the factory for this.
         */
        DynamicColorElementControl(dtCore::ColorRgbaActorProperty* colorRGBA, int whichIndex,
            const std::string& newLabel);

        /**
         * Constructor - For the ColorRgbAActorProperty property
         *
         * @note - We can put data in the constructor because aren't using the factory for this.
         */
        //DynamicColorElementControl(dtCore::Vec4ActorProperty* newVectorProp, int whichIndex,
        //    const std::string& newLabel);

        /**
         * Destructor
         */
        virtual ~DynamicColorElementControl();

        /**
         * @see DynamicAbstractControl#InitializeData
         */
        virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
            dtCore::PropertyContainer* pc, dtCore::ActorProperty* property);

        /**
         * @see DynamicAbstractControl#updateEditorFromModel
         */
        virtual void updateEditorFromModel(QWidget* widget);

        /**
         * @see DynamicAbstractControl#updateModelFromEditor
         */
        virtual bool updateModelFromEditor(QWidget* widget);

        /**
         * @see DynamicAbstractControl#createEditor
         */
        virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
            const QModelIndex& index);

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
        *	Returns true if the base property as well as all linked properties match.
        */
       virtual bool doPropertiesMatch();

       /**
         * A convenience method to get the value from the associated
         * vector.  Which element is the 0 based index into the vector.
         */
        int getValue();

        /**
         * Puts the passed in value into the appropriate vector at whichElement index
         */
        bool setValue(int value);

        /**
         * Convert the 0.0 to 1.0 float color value to the 0 to 255 int display range.
         */
        static int convertColorFloatToInt(float value);

        /**
         * Convert the 0 to 255 int display range to 0.0 to a 1.0 float value
         */
        static float convertColorIntToFloat(int value);


    public slots:
        virtual bool updateData(QWidget* widget);

        /**
         * @see DynamicAbstractControl#handleSubEditDestroy
         */
        void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

    protected:

    private:
        enum WHICHTYPE {RGBA, RGB} mWhichType;

        std::string mLabel;

        //dtCore::ColorRgbActorProperty *mColorRGB;
        dtCore::ColorRgbaActorProperty *mColorRGBA;
        int mElementIndex;

        // This pointer is not really in our control.  It is constructed in the createEditor()
        // method and destroyed whenever QT feels like it (mostly when the control looses focus).
        // We work around this by trapping the destruction of this object, it should
        // call our handleSubEditDestroy() method so we know to not hold this anymore
        SubQSpinBox *mTemporaryEditControl;
    };

} // namespace dtQt

#endif // DELTA_DYNAMICCOLORELEMENTCONTROL
