from PyDtUtil import *
from PyDtCore import *
from PyDtABC import *

from math import *
import time

def radians(v):
   return v * pi/180.0
   
class TestPythonApplication(Application):
    def Config(self):
        Application.Config(self)
        self.helo = Object('UH-1N')
        self.helo.LoadFile('models/uh-1n.ive')
        self.AddDrawable(self.helo)
        self.transform = Transform()
        self.angle = 0.0
        
    def PreFrame(self, deltaFrameTime):
        noise = Noise1f()
        translation = Vec3(40.0*cos(radians(self.angle)),
                           100.0 + 40.0*sin(radians(self.angle)), 
                           0.0)
        rotation = Vec3(self.angle, 15.0 * noise.GetNoise(time.clock() * 0.70332423), -45.0 + 35.0 * noise.GetNoise(time.clock() * 0.5958992))
        self.transform.SetTranslation(translation)
        self.transform.SetRotation(rotation)
        self.helo.SetTransform(self.transform)
        self.angle += 45.0*deltaFrameTime

SetDataFilePathList( GetDeltaRootPath() + '/examples/testPython/;' +
                               GetDeltaRootPath() + '/examples/data/;' +
                     GetDeltaDataPathList() )   
testPythonApp = TestPythonApplication('config.xml')

testPythonApp.Config()
testPythonApp.Run()

