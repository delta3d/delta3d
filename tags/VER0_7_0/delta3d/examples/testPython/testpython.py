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
        self.helo = Object('UH-1N')
        self.helo.LoadFile('UH-1N/UH-1N.ive')
        self.GetScene().AddDrawable(self.helo)
        self.transform = Transform()
        self.angle = 0.0
        
    def PreFrame(self, deltaFrameTime):
        self.transform.Set(40*cos(radians(self.angle)),
                           100 + 40*sin(radians(self.angle)), 
                           0, self.angle, 0, -45)
        self.helo.SetTransform(self.transform)
        self.angle += 45*deltaFrameTime

testPythonApp = TestPythonApplication('config.xml')

testPythonApp.Config()
testPythonApp.Run()
