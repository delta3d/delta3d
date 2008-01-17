var xmlHttp;
var xmlDoc;
var MAXNEST = 5;
var nestingLevel = 0;
var objectiveCheckBoxes;

function CreateXMLHttpRequest()
{
     //create XMLHttpRequest object depending on browser type

     if (window.ActiveXObject)
     {
          //Internet Explorer browser
          xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
     }
     else if (window.XMLHttpRequest)
     {
          //other browsers
          xmlHttp = new XMLHttpRequest();
     }
}

function LoadObjectiveXML(appUrl)
{
     //this method creates and sends the request to download the AppConfig.xml file

     CreateXMLHttpRequest();
     xmlHttp.onreadystatechange = XMLHttpCallback;
     xmlHttp.open("GET", assessmentXmlUrl, true); //assessmentXmlUrl defined on launch page
     xmlHttp.send(null);
}

function XMLHttpCallback()
{
     //this method is called when the server returns the xml file and stores the result
     //in xmlDoc

     if (xmlHttp.readyState == 4)
     {
          if (xmlHttp.status == 200)
          {
               xmlDoc = xmlHttp.responseXML;
          }
     }
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

     var objectiveNodes = xmlDoc.getElementsByTagName("objective");

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
               WriteChildObjectiveCheckBoxes(objectiveNode); //recursively calls itself
          }
     }

     objectiveCheckBoxes += "</table>";

     document.getElementById('SimulationTable').rows[2].cells[0].innerHTML = objectiveCheckBoxes;
}

function WriteChildObjectiveCheckBoxes(parentObjectiveNode)
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
               WriteChildObjectiveCheckBoxes(childNode);
               nestingLevel -= 1;
          }
     }
}

function GetObjectiveNodeById(id)
{
     //helper method to return a specific objective node given its ID

     var objectiveNodes = xmlDoc.getElementsByTagName("objective");
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