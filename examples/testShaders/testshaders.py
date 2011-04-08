from PyDtUtil import *
from PyDtCore import *
from PyDtABC import *
from PyDtDAL import *


class TestShadersApp( Application ):
    def __init__(self, configFilename):
        Application.__init__(self, configFilename)
        self.mTotalTime = 0.0
        contextName = GetDeltaRootPath() + "/examples/data/demoMap"
        
        Project.GetInstance().SetContext( contextName )
        sm = ShaderManager.GetInstance()
        sm.LoadShaderDefinitions("Shaders/ShaderDefs.xml")
        self.LoadGeometry()
        self.EnableShaders()
        
        xform = Transform(0.0, -3.0, 0.0, 0.0, 0.0, 0.0)
        self.GetCamera().SetTransform(xform)

        self.GetWindow().SetWindowTitle("testShaders")
        
    def LoadGeometry(self):
        self.mObject = Object("Happy Sphere")
        self.mObject.LoadFile("models/physics_happy_sphere.ive")
        self.AddDrawable( self.mObject )  
   
    def EnableShaders(self):
        sm = ShaderManager.GetInstance()
        sp = sm.FindShaderPrototype("TestShader", "DemoShaders")
        if sp:
            osgn = self.mObject.GetOSGNode()
            sm.AssignShaderFromPrototype( sp, osgn )
            self.mEnabled = True
            
    def DisableShaders(self):
        sm = ShaderManager.GetInstance()
        osgn = self.mObject.GetOSGNode()
        sm.UnassignShaderFromNode( osgn )
        self.mEnabled = False
        
    def KeyPressed( self, keyboard, key ) :
        if key == int(KeyboardKey.KEY_Space):
            if self.mEnabled:
                self.DisableShaders()
            else:
                self.EnableShaders()
            return 1
        elif key == int(KeyboardKey.KEY_Escape):
            self.Quit()
            return 1
        return 0
        

dataPath = GetDeltaDataPathList()
SetDataFilePathList(dataPath + ";" + GetDeltaRootPath() + 
    "/examples/data" + ";" + GetDeltaRootPath() + "/examples/testShaders")

app = TestShadersApp( "testshadersconfig.xml" )
app.Config()
app.Run()
