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
 * Matthew W. Campbell
 */

#include <prefix/dtgameprefix.h>
#include <dtGame/logstream.h>

namespace dtGame
{
   ////////////////////////////////////////////////////////////////////////////////
   LogStreamIOException::LogStreamIOException(const std::string& message, const std::string& filename, unsigned int linenum)
      :dtUtil::Exception(message, filename, linenum)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool LogStream::IsEndOfStream() const
   {
      return mEndOfStream;
   }

   ////////////////////////////////////////////////////////////////////////////////
   double LogStream::GetRecordDuration() const
   {
      return mRecordDuration;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void LogStream::SetRecordDuration(double value)
   {
      mRecordDuration = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   MessageFactory& LogStream::GetMessageFactory()
   {
      return *mMessageFactory;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const MessageFactory& LogStream::GetMessageFactory() const
   {
      return *mMessageFactory;
   }
}
