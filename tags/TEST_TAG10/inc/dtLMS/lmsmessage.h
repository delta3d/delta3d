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

#ifndef DELTA_LMS_MESSAGE
#define DELTA_LMS_MESSAGE

#include <dtLMS/export.h>
#include <string>

namespace dtLMS
{
   class LmsMessageType;

   /**
    * This class is used to encapsulate an LMS message that is sent to or received from
    * an LMS. It provides several overloaded constructors to make it easy to create the
    * message along with utility methods that can import or export the message to the
    * delimited string format specified by the LMS messaging protocol.
    */
   class DT_LMS_EXPORT LmsMessage
   {
   public:

      /**
       * Default constructor.
       */
      LmsMessage();

      /**
       * Constructor used to create status messages, where only the ID, message type, and
       * message value are required (i.e. id123:SIMULATION:RUNNING).
       * @param senderID The ID value of the component sending the messages. For messages
       *      sent by the LmsClientSocket, this can be retrieved via GetClientID().
       * @param type The enumerated type of this message (i.e. SIMULATION, LAUNCH_PAGE,
       *      (OBJECTIVE_COMPLETION, OBJECTIVE_SCORE)
       * @param value The content of the message. In most cases, one should use the
       *      LmsMessageValue enumeration's GetName() method; however, in the case of
       *      OBJECTIVE_SCORE type messages, this will be the string form of a float
       *      value.
       */
      LmsMessage(const std::string& senderID,
                 const LmsMessageType& type,
                 const std::string& value);

      /**
       * Constructor used to create objective messages, where an ID, message type,
       * message value, and objective ID are required
       *      (i.e. id123:OBJECTIVE_COMPLETION:COMPLETE:ObjID123).
       * @param senderID The ID value of the component sending the messages. For messages
       *      sent by the LmsClientSocket, this can be retrieved via GetClientID().
       * @param type The enumerated type of this message (i.e. SIMULATION, LAUNCH_PAGE,
       *      (OBJECTIVE_COMPLETION, OBJECTIVE_SCORE)
       * @param value The content of the message. In most cases, one should use the
       *      LmsMessageValue enumeration's GetName() method; however, in the case of
       *      OBJECTIVE_SCORE type messages, this will be the string form of a float
       *      value.
       * @param taskID The ID value of the LMS objective being updated. This generally
       *      should be the ID value of the TaskActor sending the update message.
       */
      LmsMessage(const std::string& senderID,
                 const LmsMessageType& type,
                 const std::string& value,
                 const std::string& objectiveID);

      /**
       * Constructor used to create an lms message from an appropriately delimited string.
       * This constructor is generally used to create an LmsMessage object from a string
       * message received from the LMS.
       * @ param messageString The delimited string to create the LmsMessage from. Must be
       *      in the proper delimited format per the LMS messaging protocol.
       */
      LmsMessage(const std::string& messageString);

      /**
       * Destructor
       */
      virtual ~LmsMessage();

      /**
       * This public method populates this LmsMessage with the values parsed from
       * an appropriately delimited string. It is called by one of the constructors
       * (see LmsMessage(string messageString), but can also be called externally.
       * @ param messageString The delimited string to create the LmsMessage from. Must be
       *      in the proper delimited format per the LMS messaging protocol.
       */
      void BuildFromString(const std::string& messageString);

      /**
       * This public method will create and return an appropriately delimited string
       * suitable for sending the LmsMessage to the LMS via the LmsClientSocket's
       * SendLmsMessage() method.
       * @ return Returns a delimited string that can be used by the LmsClientSocket's
       *      SendLmsMessage() method.
       */
      std::string ToString() const;

      const std::string& GetSenderID() const { return mSenderID; }
      std::string& GetSenderID() { return mSenderID; }
      void SetSenderID(const std::string& senderID) { mSenderID = senderID; }

      const std::string& GetValue() const { return mValue; }
      std::string& GetValue()  { return mValue; }
      void SetValue(const std::string& value) { mValue = value; }

      const std::string& GetObjectiveID() const { return mObjectiveID; }
      std::string& GetObjectiveID() { return mObjectiveID; }
      void SetObjectiveID(std::string objectiveID) { mObjectiveID = objectiveID; }

      const std::string& GetDelimiter() const { return mDelimiter; }
      std::string& GetDelimiter() { return mDelimiter; }
      void SetDelimiter(const std::string& delimiter) { mDelimiter = delimiter; }

      const LmsMessageType& GetMessageType() const { return *mType; }
      void SetMessageType(const LmsMessageType& type) { mType = &type; }

   // member variables
   private:
      std::string mSenderID;       // ID of the LMS message.
      std::string mValue;          // value of the LMS message.
      std::string mObjectiveID;    // objective ID of the LMS message.
      std::string mDelimiter;      // delimiter character used in string form of LMS messages. Defaults to ":".
      const LmsMessageType* mType; // type of the LMS message
   };
} // namespace dtLMS

#endif // DELTA_LMS_MESSAGE
