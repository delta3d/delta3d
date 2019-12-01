/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Christopher DuBuc
 */
#ifndef DELTA_LMS_TASK_STATUS
#define DELTA_LMS_TASK_STATUS

#include <dtLMS/export.h>

namespace dtLMS
{
   /*
    * This class is meant to hold a task's previous values within the mPreviousTaskStatus
    * map variable in the LmsComponent class. The purpose of storing the previous values
    * of a task are so that we can know what has changed when the LmsComponent receives a
    * task update message (and therefore we can only send changed data to the lms).
    */
   class DT_LMS_EXPORT LmsTaskStatus
   {
      public:
         /**
          * Constructs the LmsTaskStatus object and initializes
          * the member variables.
          */
         LmsTaskStatus()
            : mCompleted(false)
            , mScore(0.0f)
         {}

         /**
          * Destucts the LmsTaskStatus object.
          */
         ~LmsTaskStatus() {}

         bool GetCompleted() { return mCompleted; }
         void SetCompleted(bool completed) { mCompleted = completed; }

         float GetScore() { return mScore; }
         void SetScore(float score) { mScore = score; }

      private:
         bool mCompleted;
         float mScore;
   };

} // namespace dtLMS

#endif // DELTA_LMS_TASK_STATUS
