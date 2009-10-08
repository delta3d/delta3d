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

var _xmlHttp;
var _xmlDoc;

function LoadXMLDoc(url, asynch)
{
     //this method creates and sends the request to download an xml file
     CreateXmlHttpRequest();
     _xmlHttp.onreadystatechange = XmlHttpCallback;
     _xmlHttp.open("GET", url, asynch);
     _xmlHttp.send(null);
}

function CreateXmlHttpRequest()
{
     //create _xmlHttpRequest object depending on browser type

     if (window.ActiveXObject)
     {
          //Internet Explorer browser
          _xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
     }
     else if (window.XMLHttpRequest)
     {
          //other browsers
          _xmlHttp = new XMLHttpRequest();
     }
}

function XmlHttpCallback()
{
     //this method is called when the server returns the xml file and stores the result
     //in _xmlDoc

     if (_xmlHttp.readyState == 4)
     {
          if (_xmlHttp.status == 200)
          {
               _xmlDoc = _xmlHttp.responseXML;
          }
     }
}