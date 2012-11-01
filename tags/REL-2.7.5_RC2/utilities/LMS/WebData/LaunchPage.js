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
   Clear();
   
   varResult = doInitialize(); //initialize LMS API (from APIWrapper.js)
   InitializeObjectivesScore(); //set all un-initialized scores to 0
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

function WriteStartJnlpButton()
{
     //this method writes the html code for the 'Start' button that launches the simulation via the
     //specified jnlp file;

     var writeString = "";
     writeString = "<form action=\"" + jnlpFile + "\" id=\"Form1\">";
     writeString = writeString + "<input type=submit value=\"   Start   \" ID=\"Submit1\" NAME=\"Submit1\">";
     writeString = writeString + "</form>";

     document.write(writeString);
}

function ToggleDebug()
{
     //this method is used to show/hide the debug textarea control
     
     var button = document.getElementById("DebugButton");
     var textAreaRow = document.getElementById('DebugTable').rows[1];

     if (button.innerHTML == "Hide")
     {
          button.innerHTML = "Show";
          textAreaRow.style.display = "none";
     }
     else
     {
          button.innerHTML = "Hide";
          textAreaRow.style.display = "";
     }
}

//May need this code in future if we encounter problems with FireFox support for the innerHTML property
//
//function FFinnerHTML(replaceElement, content)
//{
//     if (replaceElement && !document.all)
//     {
//          range = document.createRange();
//          range.setStartBefore(replaceElement);
//
//          htmlFragment = range.createContextualFragment(content);
//
//          while (replaceElement.hasChildNodes())
//          {
//               replaceElement.removeChild(replaceElement.lastChild);
//          }
//
//          replaceElement.appendChild(htmlFragment);
//     }
//}