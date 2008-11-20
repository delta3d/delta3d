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
* Curtiss Murphy
*/
#ifndef DELTA_DYNAMICVEC4CONTROL
#define DELTA_DYNAMICVEC4CONTROL

#include <dtEditQt/dynamicabstractparentcontrol.h>

namespace dtDAL 
{
    class Vec4ActorProperty;
    class Vec4fActorProperty;
    class Vec4dActorProperty;
}

namespace dtEditQt
{

    class DynamicVectorElementControl;

    /**
    * @class DynamicVec4Control
    * @brief This is the dynamic control for a vect 4 - used in the property editor
    * @Note It adds a group of child elements to the tree, since you can't edit 4 things
    * in one control easily.
    */
    class DynamicVec4Control : public DynamicAbstractParentControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicVec4Control();

        /**
         * Destructor
         */
        virtual ~DynamicVec4Control();


        /**
         * @see DynamicAbstractControl#initializeData
         */
        virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
            dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);

        /**
         * @see DynamicAbstractControl#addSelfToParentWidget
         */
        void addSelfToParentWidget(QWidget &parent, QGridLayout &layout, int row);

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

        virtual bool updateData(QWidget *widget);

    protected:

    private: 
 
        DynamicVectorElementControl *wElement;
        DynamicVectorElementControl *xElement;
        DynamicVectorElementControl *yElement;
        DynamicVectorElementControl *zElement;

        bool isVecFloat;
        // the tool tip type label indicates that the vector is a float or a double
        std::string toolTipTypeLabel;

        dtCore::RefPtr<dtDAL::Vec4ActorProperty> myVec4Property;
        dtCore::RefPtr<dtDAL::Vec4fActorProperty> myVec4fProperty;
        dtCore::RefPtr<dtDAL::Vec4dActorProperty> myVec4dProperty;
    };

}

#endif
