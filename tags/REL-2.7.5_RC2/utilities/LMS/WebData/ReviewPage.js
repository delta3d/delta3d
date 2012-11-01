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

var exitPageStatus;

function LoadPage()
{
  varResult = doInitialize();
  exitPageStatus = false;
}

function UnloadPage()
{

	if (exitPageStatus != true)
	{
		doTerminate();
	}

	// NOTE:  don't return anything that resembles a javascript
	//		  string from this function or IE will take the
	//		  liberty of displaying a confirm message box.

}

function WriteObjectives()
{
  var assessmentXmlUrl = "JavaLaunch.xml";
  
  //request JavaLaunch.xml from server (and wait for response)
   LoadXMLDoc(assessmentXmlUrl, false);

   var objectiveHtmlTable = WriteObjectivesTable();

   //replace div element ObjectiveTable with new HTML containing objective checkboxes
   document.getElementById('ObjectiveTable').innerHTML = objectiveHtmlTable;
   
   //update objective status
   //UpdateObjectivesDisplay();
}