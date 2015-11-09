/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicabstractparentcontrol (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_DYNAMICABSTRACTPARENTCONTROL
#define DELTA_DYNAMICABSTRACTPARENTCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <vector>

namespace dtQt
{

    //class DynamicVectorElementControl;

    /**
     * @class DynamicAbstractParentControl
     * @brief This is a base class for any dynamic control that has children.
     * It keeps a vector of DynamicAbstractControl *'s and knows how to work
     * with it.  This is common behavior in the dynamic controls that was pulled
     * to this class.
     */
    class DT_QT_EXPORT DynamicAbstractParentControl : public DynamicAbstractControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicAbstractParentControl();

        /**
         * Destructor
         */
        virtual ~DynamicAbstractParentControl();


        /**
         * A clean up method you should use when you are planning to reuse this control.
         */
        void removeAllChildren(PropertyEditorModel* model);

        /**
         * @see DynamicAbstractControl#getChildIndex
         */
        virtual int getChildIndex(DynamicAbstractControl* child);

        /**
         * @see DynamicAbstractControl#getChild
         */
        virtual DynamicAbstractControl* getChild(int index);

        /**
         * @see DynamicAbstractControl#getChildCount
         */
        virtual int getChildCount();

    protected:
        std::vector<DynamicAbstractControl*> mChildren;
    };

} // namespace dtEditQt

#endif // DELTA_DYNAMICABSTRACTPARENTCONTROL
