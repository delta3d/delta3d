on error resume next
If isIE = "true" Then
  	If Not(IsObject(CreateObject("JavaWebStart.isInstalled"))) Then
     	javawsInstalled = 0
  	Else
     	javawsInstalled = 1
     	JREVersion = "Pre 1.4.2"
  	End If
  	If Not(IsObject(CreateObject("JavaWebStart.isInstalled.1.4.2.0"))) Then
     	javaws142Installed = 0
  	Else
     	javaws142Installed = 1
     	JREVersion = "1.4.2"
  	End If 
  	If Not(IsObject(CreateObject("JavaWebStart.isInstalled.1.5.0.0"))) Then
     	javaws150Installed = 0
  	Else
     	javaws150Installed = 1
     	JREVersion = "1.5"
  	End If  
End If