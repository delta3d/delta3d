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

//message parts
var _senderID = "";
var _messageType = "";
var _messageValue = "";
var _objectiveID = "";

function DisplayMessage(message)
{
     //name table cells & text area
     var simulationStatusCell = document.getElementById('SimulationTable').rows[0].cells[2];
     var objectiveStatusCell = document.getElementById('SimulationTable').rows[1].cells[0];

     //extract message into its various parts
     ParseMessage(message);
     
     if (_senderID == "ERROR")
     {
          simulationStatusCell.innerHTML = "Message Error";
          objectiveStatusCell.innerHTML = "&nbsp";

          document.DebugTextForm.DebugMessageTextArea.value = document.DebugTextForm.DebugMessageTextArea.value + message + "\n";
          return;
     }
     
     if (_messageType == "SIMULATION" && _messageValue == "RUNNING")
     {
          Clear();
          simulationStatusCell.innerHTML = "Simulation Started...";
		    objectiveStatusCell.innerHTML = "&nbsp";

		    var objectiveHtmlTable = WriteObjectiveCheckBoxes();
          document.getElementById('SimulationTable').rows[2].cells[0].innerHTML = objectiveHtmlTable;
          //UpdateObjectivesDisplay();
		
          //document.DebugTextForm.DebugMessageTextArea.value = document.DebugTextForm.DebugMessageTextArea.value + message + "\n";
     }
       
     if (_messageType == "SIMULATION" &&
        (_messageValue == "STOPPED_NORMAL" || _messageValue == "STOPPED_ABNORMAL"))
     {
          //the simulation has stopped; display message to that effect
          simulationStatusCell.innerHTML = "Simulation Stopped...";
		    objectiveStatusCell.innerHTML = "&nbsp";

          //document.DebugTextForm.DebugMessageTextArea.value = document.DebugTextForm.DebugMessageTextArea.value + message + "\n";
     }

     if (_messageType == "OBJECTIVE_COMPLETION")
     {
          var checkBoxNode = document.getElementById(_objectiveID);
          if (checkBoxNode != null)
          {
               if (_messageValue == "COMPLETE")
               {
                    checkBoxNode.checked = true;
                    SetObjectiveSuccess(_objectiveID, "passed");
                    objectiveStatusCell.innerHTML = checkBoxNode.name + ":     Objective Completed"
               }
          }
          
          //document.DebugTextForm.DebugMessageTextArea.value = document.DebugTextForm.DebugMessageTextArea.value + message + "\n";
     }
     
     if (_messageType == "OBJECTIVE_SCORE")
     {
         SetObjectiveScoreScaled(_objectiveID, _messageValue);
     }
     
     document.DebugTextForm.DebugMessageTextArea.value = document.DebugTextForm.DebugMessageTextArea.value + message + "\n";

	  //scroll debug textarea to bottom to put latest message into view
	  document.DebugTextForm.DebugMessageTextArea.scrollTop = document.DebugTextForm.DebugMessageTextArea.scrollHeight;

     return;
}

function ParseMessage(message)
{
     _senderID = "";
     _messageType = "";
     _messageValue = "";
     _objectiveID = "";

     var messageParts = message.split(":");
     
     //get _senderID
     if (messageParts.length > 0)
     {
          _senderID = messageParts[0];
     }
     
     //get _messageType
     if (messageParts.length > 1)
     {
          _messageType = messageParts[1];
     }
     
     //get _messageValue
     if (messageParts.length > 2)
     {
          _messageValue = messageParts[2];
     }

     //get _objectiveID
     if (messageParts.length > 3)
     {
          _objectiveID = messageParts[3];
     }
}

function Clear()
{
     //_currentObjectiveID = "";
     //_objectiveQue = [];
     document.DebugTextForm.DebugMessageTextArea.value = "";
     document.getElementById('SimulationTable').rows[2].cells[0].innerHTML = "<em>Required Tasks:<br><br></em>";
}