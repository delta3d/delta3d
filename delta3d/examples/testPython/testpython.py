from dtCore import *
from dtABC import *

from math import *
from time import *

def radians(v):
   return v * pi/180
   
class TestPythonApplication(Application):
    def Config(self):
        Application.Config(self)
        SetDataFilePathList('../../data');
        self.plane = Object('cessna')
        self.plane.LoadFile('cessna.osg')
        self.GetScene().AddDrawable(self.plane)
        self.transform = Transform()
        self.angle = 0.0
        
    def PreFrame(self, deltaFrameTime):
        self.transform.Set(40*cos(radians(self.angle)),
                           100 + 40*sin(radians(self.angle)), 
                           0, self.angle, 0, -45)
        self.plane.SetTransform(self.transform)
        self.angle -= 45*deltaFrameTime

class MouseHandler(MouseListener):
    def MouseMoved(self, mouse, x, y):
        print 'you moved the mouse!'
        
testPythonApp = TestPythonApplication('config.xml')

mouseHandler = MouseHandler()

testPythonApp.GetMouse().AddMouseListener(mouseHandler)

testPythonApp.Config()
testPythonApp.Run()
