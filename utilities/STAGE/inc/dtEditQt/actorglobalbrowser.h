/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - actorglobalbrowser (.h & .cpp) - Using 'The MIT License'
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

#ifndef DELTA_ACTORGLOBALBROWSER
#define DELTA_ACTORGLOBALBROWSER

#include <QtWidgets/QWidget>
#include <vector>
#include <dtCore/actorproxy.h>
#include <dtCore/actorproperty.h>
#include <dtQt/typedefs.h>

class QComboBox;
class QGroupBox;
class QLineEdit;
class QPushButton;

namespace dtEditQt
{
   // forward declare
   class ActorResultsTable;

   /**
    * @class ActorGlobalBrowser
    * @brief This class allows the user to browse through the global actors.
    */
   class ActorGlobalBrowser: public QWidget
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      ActorGlobalBrowser(QWidget* parent = NULL);

      /**
       * Destructor
       */
      ~ActorGlobalBrowser();

   public slots:
      /**
       *  Reloads the global browser data
       */
      void refreshAll();

      /**
       * When a new actor is created, we may have to add to the global list
       */
      void onActorCreated(dtCore::ActorPtr proxy, bool forceNoAdjustments);

      /**
       * Called when an actor proxy is deleted
       */
      void onActorDestroyed(dtCore::ActorPtr proxy);

      /**
       * Called when a property changes to update the list
       */
      void onActorPropertyChanged(dtCore::ActorPtr proxy, ActorPropertyRefPtr property);

   private:
      QGroupBox* resultsGroup();
      ActorResultsTable* mResultsTable;
   };

} // namespace dtEditQt

#endif // DELTA_ACTORGLOBALBROWSER
