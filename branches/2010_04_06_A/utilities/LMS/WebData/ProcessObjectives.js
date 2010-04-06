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

var MAXNEST = 5; //the maximum that child objectives will be nested
var nestingLevel = 0;
var objectiveCheckBoxes;

function WriteObjectivesTable()
{
     //This method will dynamically create html checkboxes for each objective in the AppConfig.xml file. The
     //checkboxes will be contained within a table so that child objectives can be indented respective to
     //their parent objectives. The WriteChildObjectiveCheckBoxes() method will be called recursively
     //in order to process all child objectives.
     
     var objectiveNodes = _xmlDoc.getElementsByTagName("objective");

     objectiveCheckBoxes = "<table id=\"main\" border=0 cellpadding=0 cellspacing=1 width = 400 style=\"color:FFFD86\">";
     objectiveCheckBoxes += "<tr>";
     
     //write the list of objectives on the left side of the main table
     objectiveCheckBoxes += "<td>";
     objectiveCheckBoxes += WriteObjectiveListHtmlTable(objectiveNodes);
     objectiveCheckBoxes += "</td>";

     //write the result values for the objectives on the right side of the main table
     objectiveCheckBoxes += "<td>";
     objectiveCheckBoxes += WriteObjectiveResultsHtmlTable(objectiveNodes);
     objectiveCheckBoxes += "</td>";
     
     objectiveCheckBoxes += "</tr>";
     objectiveCheckBoxes += "</table>";

     return objectiveCheckBoxes;
}

function WriteObjectiveListHtmlTable(objectiveNodes)
{
     var objectiveList = "<table id=\"objectiveCheckboxTable\" border=0 cellpadding=0 cellspacing=1 width=220 style=\"color:FFFD86\">";
     for (var i=0; i<MAXNEST; i++)
     {
          objectiveList += "<col>";
     }
     objectiveList += "<tr><td colspan=" + MAXNEST +"><em><u>Simulation Objectives</u></em></td></tr>";

     //var objectiveNodes = _xmlDoc.getElementsByTagName("objective");

     for (var i=0; i<objectiveNodes.length; i++)
     {
          //first select only parent objective nodes
          if (objectiveNodes[i].parentNode.tagName != "objective")
          {
               var objectiveNode = objectiveNodes[i];
               var objectiveID = objectiveNode.getAttribute("id");
               var objectiveName = objectiveNode.getAttribute("name");
               
               objectiveList += "<tr><td colspan=" + MAXNEST +">";
               objectiveList += objectiveName;
               objectiveList += "</td></tr>";

               nestingLevel = 1;
               objectiveList += WriteObjectiveListHtmlTableChildren(objectiveNode); //recursive
          }
     }

     objectiveList += "</table>";
     
     return objectiveList;
}

function WriteObjectiveListHtmlTableChildren(parentObjectiveNode)
{
     //this method is called recursively in order to process all child nodes
     var objectiveList = "";

     for (var i=0; i<parentObjectiveNode.childNodes.length; i++)
     {
          var childNode = parentObjectiveNode.childNodes[i];
          if (childNode.tagName == "objective")
          {
               var childID = childNode.getAttribute("id");
               var childName = childNode.getAttribute("name");

               objectiveList += "<tr>";
               for (var j=0; j<nestingLevel; j++)
               {
                    objectiveList += "<td>&nbsp</td>";
               }
               var colSpan = MAXNEST - nestingLevel;
               objectiveList += "<td colspan=" + colSpan + ">";

               objectiveList += childName;

               objectiveList += "</td></tr>";

               nestingLevel += 1;
               objectiveList += WriteObjectiveListHtmlTableChildren(childNode);
               nestingLevel -= 1;
          }
     }
     return objectiveList;
}

function WriteObjectiveResultsHtmlTable(objectiveNodes)
{
     var objectiveResults = "<table id=\"rightSide\" border=0 cellpadding=0 cellspacing=1 width = 180 style=\"color:FFFD86\">";
     objectiveResults += "<tr>";
     //objectiveResults += "<td align=center><em><u>Completion</u></em></td><td align=center><em><u>Success</u></em></td><td align=center><em><u>Scaled Score</u></em></td><td align=center><em><u>Raw Score</u></em></td>";
     objectiveResults += "<td align=center><em><u>Success</u></em></td><td align=center><em><u>Score</u></em></td>";
     objectiveResults += "</tr>";

     for (var i=0; i<objectiveNodes.length; i++)
     {
          //first select only parent objective nodes
          if (objectiveNodes[i].parentNode.tagName != "objective")
          {
               var objectiveNode = objectiveNodes[i];
               var objectiveID = objectiveNode.getAttribute("id");
               var objectiveName = objectiveNode.getAttribute("name");
               var objectiveIndex = GetObjectiveIndex(objectiveID);
               
               objectiveResults += "<tr>";
         
               //write completion value
               //objectiveResults += "<td align=center width=90>" + doGetValue("cmi.objectives." + objectiveIndex + ".completion_status") + "</td>";

               //write success value
               objectiveResults += "<td align=center width=90>" + doGetValue("cmi.objectives." + objectiveIndex + ".success_status") + "</td>";

               //write scaled score value
               var scaledScore = doGetValue("cmi.objectives." + objectiveIndex + ".score.scaled");
               if (scaledScore == "Data Model Element Value Not Initialized") scaledScore = "NA";
               objectiveResults += "<td align=center width=90>" + scaledScore + "</td>";

               //write raw score value
               //var rawScore = doGetValue("cmi.objectives." + objectiveIndex + ".score.raw");
               //if (rawScore == "Data Model Element Value Not Initialized") rawScore = "NA";
               //objectiveResults += "<td align=center width=90>" + rawScore + "</td>";
               
               objectiveResults += "</tr>";

               objectiveResults += WriteObjectiveResultsHtmlTableChildren(objectiveNode); //recursive
          }
     }

     objectiveResults += "</table>";
     
     return objectiveResults;
}

function WriteObjectiveResultsHtmlTableChildren(parentObjectiveNode)
{
   var objectiveResults = "";

   for (var i=0; i<parentObjectiveNode.childNodes.length; i++)
   {
      var childNode =parentObjectiveNode.childNodes[i];
      if (childNode.tagName == "objective")
      {
         var childID = childNode.getAttribute("id");
         var childName = childNode.getAttribute("name");
         var objectiveIndex = GetObjectiveIndex(childID);

         objectiveResults += "<tr>";
         
         //write completion value
         //objectiveResults += "<td align=center width=90>" + doGetValue("cmi.objectives." + objectiveIndex + ".completion_status") + "</td>";

         //write success value
         objectiveResults += "<td align=center width=90>" + doGetValue("cmi.objectives." + objectiveIndex + ".success_status") + "</td>";

         //write scaled score value
         var scaledScore = doGetValue("cmi.objectives." + objectiveIndex + ".score.scaled");
         if (scaledScore == "Data Model Element Value Not Initialized") scaledScore = "NA";
         objectiveResults += "<td align=center width=90>" + scaledScore + "</td>";

         //write raw score value
         //var rawScore = doGetValue("cmi.objectives." + objectiveIndex + ".score.raw");
         //if (rawScore == "Data Model Element Value Not Initialized") rawScore = "NA";
         //objectiveResults += "<td align=center width=90>" + rawScore + "</td>";
               
         objectiveResults += "</tr>";

         objectiveResults += WriteObjectiveResultsHtmlTableChildren(childNode);
      }
   }
   return objectiveResults;
}

function WriteObjectiveCheckBoxes()
{
     //This method will dynamically create html checkboxes for each objective in the AppConfig.xml file. The
     //checkboxes will be contained within a table so that child objectives can be indented respective to
     //their parent objectives. The WriteChildObjectiveCheckBoxes() method will be called recursively
     //in order to process all child objectives.
     objectiveCheckBoxes = "<table id=\"objectiveCheckboxTable\" border=0 cellpadding=0 cellspacing=5 width=505 style=\"color:FFFD86\">";
     for (var i=0; i<MAXNEST; i++)
     {
          objectiveCheckBoxes += "<col>";
     }
     objectiveCheckBoxes += "<tr><td colspan=" + MAXNEST +"><em> Simulation Objectives:</em></td></tr>";

     var objectiveNodes = _xmlDoc.getElementsByTagName("objective");

     for (var i=0; i<objectiveNodes.length; i++)
     {
          //first select only parent objective nodes
          if (objectiveNodes[i].parentNode.tagName != "objective")
          {
               var objectiveNode = objectiveNodes[i];
               var objectiveID = objectiveNode.getAttribute("id");
               var objectiveName = objectiveNode.getAttribute("name");
               
               objectiveCheckBoxes += "<tr><td colspan=" + MAXNEST +">";
               objectiveCheckBoxes += "<input type=checkbox name=\"" + objectiveName + "\" id=\"" + objectiveID + "\"> " + objectiveName + "</input>";
               objectiveCheckBoxes += "</td></tr>";

               nestingLevel = 1;
               WriteChildCheckBoxes(objectiveNode); //recursively calls itself
          }
     }

     objectiveCheckBoxes += "</table>";

     //document.getElementById('SimulationTable').rows[2].cells[0].innerHTML = objectiveCheckBoxes;
     return objectiveCheckBoxes;
}

function WriteChildCheckBoxes(parentObjectiveNode)
{
     //this method is called recursively in order to process all child nodes

     for (var i=0; i<parentObjectiveNode.childNodes.length; i++)
     {
          var childNode = parentObjectiveNode.childNodes[i];
          if (childNode.tagName == "objective")
          {
               var childID = childNode.getAttribute("id");
               var childName = childNode.getAttribute("name");

               objectiveCheckBoxes += "<tr>";
               for (var j=0; j<nestingLevel; j++)
               {
                    objectiveCheckBoxes += "<td>&nbsp</td>";
               }
               var colSpan = MAXNEST - nestingLevel;
               objectiveCheckBoxes += "<td colspan=" + colSpan + ">";

               objectiveCheckBoxes += "<input type=checkbox name=\"" + childName + "\" id=\"" + childID + "\"> " + childName + "</input>";

               objectiveCheckBoxes += "</td></tr>";

               nestingLevel += 1;
               WriteChildCheckBoxes(childNode);
               nestingLevel -= 1;
          }
     }
}

function GetObjectiveNodeById(id)
{
     //helper method to return a specific objective node given its ID

     var objectiveNodes = _xmlDoc.getElementsByTagName("objective");
     var returnNode;
     
     for (var i = 0; i < objectiveNodes.length; i++)
     {
          var objNode = objectiveNodes[i];
          if (objNode.getAttribute("id") == id)
          {
               return objNode;
          }
     }
}

function SetObjectiveSuccess(objectiveID, successValue)
{
   var index = GetObjectiveIndex(objectiveID);

   if (index != -1)
   {
      doSetValue("cmi.objectives." + index + ".success_status", successValue);
   }
}

function SetObjectiveScoreScaled(objectiveID, score)
{
   var index = GetObjectiveIndex(objectiveID);
   if (index != -1)
   {
      doSetValue("cmi.objectives." + index + ".score.scaled", score);
   }
}

//this function will set all un-initialized score values to 0
function InitializeObjectivesScore()
{
   var count = doGetValue("cmi.objectives._count");
   for (i=0; i<count; i++)
   {
      var scaledScore = doGetValue("cmi.objectives." + i + ".score.scaled");
      if (scaledScore == "Data Model Element Value Not Initialized")
      {
         doSetValue("cmi.objectives." + i + ".score.scaled", 0);
      }
   }
}

function GetObjectiveIndex(objectiveID)
{
   var count = doGetValue("cmi.objectives._count");

	var index = -1;
	for (i=0; i<count; i++)
	{
		if (doGetValue("cmi.objectives." + i + ".id") == objectiveID)
		{
		   index = i;
		   break;
		}
	}
	
	return index;
}

function UpdateObjectivesDisplay()
{
  var count = doGetValue("cmi.objectives._count");
  
  for (i=0; i<count; i++)
  {
    var id = doGetValue("cmi.objectives." + i + ".id");
    //var completionStatus = doGetValue("cmi.objectives." + i + ".completion_status");
    var successStatus = doGetValue("cmi.objectives." + i + ".success_status");
    //var scoreRaw = doGetValue("cmi.objectives." + i + ".score.raw");
    //var scoreScaled = doGetValue("cmi.objectives." + i + ".score_scaled");

    if (successStatus == "passed")
    {
      var checkBoxNode = document.getElementById(id);

      if (checkBoxNode != null)
      {
        checkBoxNode.checked = true;
      }
    }
  }
}