/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - actorsearcher (.h & .cpp) - Using 'The MIT License'
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

#ifndef DELTA_ACTORSEARCHER
#define DELTA_ACTORSEARCHER

#include <QtWidgets/QWidget>
#include <vector>
#include <dtCore/refptr.h>
#include <dtCore/actorproxy.h>
#include <dtQt/typedefs.h>
#include <dtCore/actorproperty.h>

class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;

namespace dtEditQt
{

   // forward declare
   class ActorResultsTable;

   /**
    * @class ActorSearcher
    * @brief This class allows the user to search for actors in the map
    */
   class ActorSearcher : public QWidget
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      ActorSearcher(QWidget* parent = 0);

      /**
       * Destructor
       */
      ~ActorSearcher();

   public slots:
      /**
       * Slot - handles the event when the search button is pressed
       */
      void searchPressed();

      /**
       * Called when maps, projects, or libraries change.  It should clear
       * any results and the various search combo boxes.
       */
      void refreshAll();

      /**
       * When a new actor is created, we have to add it's classes to our class list.
       */
      void onActorCreated(dtCore::ActorPtr actor, bool forceNoAdjustments);

   private:
      QGroupBox* resultsGroup();
      QGroupBox* searchGroup();

      QComboBox*   mCategoryBox;
      QComboBox*   mTypeBox;
      QComboBox*   mClassBox;
      QPushButton* mSearchBtn;
      QPushButton* mClearBtn;
      QLineEdit*   mActorNameEdit;
      QLineEdit*   mActorCategoryEdit;

      ActorResultsTable* mResultsTable;
      QString            mAnyValue;
      QStringList        mClassList;
   };

} // namespace dtEditQt

#endif // DELTA_ACTORSEARCHER
