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
 * @author Chris DuBuc
 */

#include <dtLMS/lmsmessagevalue.h>

namespace dtLMS
{
   IMPLEMENT_ENUM(LmsMessageValue::SimulationValue);

   const LmsMessageValue::SimulationValue LmsMessageValue::SimulationValue::RUNNING("RUNNING");
   const LmsMessageValue::SimulationValue LmsMessageValue::SimulationValue::STOPPED_NORMAL("STOPPED_NORMAL");
   const LmsMessageValue::SimulationValue LmsMessageValue::SimulationValue::STOPPED_ABNORMAL("STOPPED_ABNORMAL");

   IMPLEMENT_ENUM(LmsMessageValue::LaunchPageValue);

   const LmsMessageValue::LaunchPageValue LmsMessageValue::LaunchPageValue::LISTENING("LISTENING");
   const LmsMessageValue::LaunchPageValue LmsMessageValue::LaunchPageValue::STOPPED("STOPPED");

   IMPLEMENT_ENUM(LmsMessageValue::ObjectiveCompletionValue);

   const LmsMessageValue::ObjectiveCompletionValue LmsMessageValue::ObjectiveCompletionValue::COMPLETE("COMPLETE");
   const LmsMessageValue::ObjectiveCompletionValue LmsMessageValue::ObjectiveCompletionValue::INCOMPLETE("INCOMPLETE");
}
