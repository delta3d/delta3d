from PyDtUtil import *
from PyDtCore import *
from PyDtABC import *

from math import *
from time import *

#
# An individual waypoint.
#
class Waypoint:

    #
    # Constructor.
    #
    # timeCode: The waypoint's time code, in seconds
    # position: The position of the waypoint
    # lookAt: The look-at position of the waypoint
    # upVector: The waypoint's up vector
    #
    def __init__(self, timeCode=0.0, position=(0.0, 0.0, 0.0), lookAt=(0.0, 1.0, 0.0), upVector=(0.0, 0.0, 1.0)):
    
        self.timeCode = timeCode
        self.position = position
        self.lookAt = lookAt
        self.upVector = upVector
    
    
#
# Computes an interpolation parameter given two times and a wraparound
# time.
#
# t0: the first time
# t1: the second time
# t: the time between the two boundaries
#        
def ComputeAlpha(t0, t1, t):
    if t0 == t1:
        return 0.0
    else:
        return (t-t0)/(t1-t0)
       
        
#
# Linearly interpolates between two waypoints, returning a new waypoint.
#
# w0: The first waypoint
# w1: The second waypoint
# a: The interpolation parameter
#
def Lerp(w0, w1, a):

    t = w0.timeCode + a*(w1.timeCode-w0.timeCode)
    
    position = ()
    lookAt = ()
    upVector = ()
    
    for i in range(3):
        position = position + (w0.position[i] + a*(w1.position[i]-w0.position[i]),)
        lookAt = lookAt + (w0.lookAt[i] + a*(w1.lookAt[i]-w0.lookAt[i]),)
        upVector = upVector + (w0.upVector[i] + a*(w1.upVector[i]-w0.upVector[i]),)
        
    return Waypoint(t, position, lookAt, upVector)
        
        
#
# Interpolates between four adjacent waypoints along a spline, returning
# a new waypoint.
#
# w0: The first waypoint
# w1: The second waypoint
# w2: The third waypoint
# w3: The fourth waypoint
# t: The time value
# wt: The wraparound time
#
def Splerp(w0, w1, w2, w3, t, wt):

    w0t = w0.timeCode
    w1t = w1.timeCode
    w2t = w2.timeCode
    w3t = w3.timeCode
    
    nt = t
    
    if w1t < w0t:
        w1t = w1t + wt
    
    if w2t < w1t:
        w2t = w2t + wt
        
    if w3t < w2t:
        w3t = w3t + wt
        
    if nt < w1t:
        nt = nt + wt
    
    a0 = ComputeAlpha(w0t, w1t, nt)
    a1 = ComputeAlpha(w1t, w2t, nt)
    a2 = ComputeAlpha(w2t, w3t, nt)
    
    x0 = Lerp(w0, w1, a0)
    x1 = Lerp(w1, w2, a1)
    x2 = Lerp(w2, w3, a2)
    
    b0 = ComputeAlpha(w0t, w2t, nt)
    b1 = ComputeAlpha(w1t, w3t, nt)
    
    y0 = Lerp(x0, x1, b0)
    y1 = Lerp(x1, x2, b1)
    
    c = ComputeAlpha(w0t, w3t, nt)
    
    return Lerp(y0, y1, c)
        
        
#
# The SplinePathController moves its target between a series of
# waypoints along an interpolating spline.
#
class SplinePathController:
    
    #
    # Constructor.
    #
    # target: The Transformable target to move
    # waypoints: The list of waypoints to move between
    # loop: Whether or not to loop to the beginning after the last waypoint
    #
    def __init__(self, target=None, waypoints=[], loop=True):
    
        self.target = target
        self.waypoints = waypoints
        self.loop = loop
        self.transform = Transform()
        self.Reset()
        
    
    #
    # Resets the path controller to its initial state.
    #
    def Reset(self):
        self.currentIndex = 0
        if len(self.waypoints) > 0:
            self.currentTime = self.waypoints[0].timeCode
        
        
    #
    # Updates the transform of the target.  Should be called once per frame.
    #
    # deltaFrameTime: The amount of time elapsed since the last update, in seconds
    #
    def Update(self, deltaFrameTime):
    
        if self.target is not None and self.currentIndex < len(self.waypoints):
            
            while (self.currentIndex < len(self.waypoints) and 
                   self.currentTime > self.waypoints[self.currentIndex].timeCode):
                if self.loop and self.currentIndex == len(self.waypoints) - 1:
                    self.currentTime = self.currentTime - self.waypoints[self.currentIndex].timeCode
                    self.currentIndex = 0
                else:
                    self.currentIndex = self.currentIndex + 1
            
            if self.currentIndex < len(self.waypoints):
            
                if self.currentIndex > 0:
                    i1 = self.currentIndex - 1
                elif self.loop:
                    i1 = len(self.waypoints) - 1
                else:
                    i1 = self.currentIndex
                    
                if i1 > 0:
                    i0 = i1 - 1
                elif self.loop:
                    i0 = len(self.waypoints)-1
                else:
                    i0 = i1
                
                if self.currentIndex < len(self.waypoints)-1:
                    i3 = self.currentIndex + 1
                elif self.loop:
                    i3 = 0
                else:
                    i3 = self.currentIndex
                
                w = Splerp(
                    self.waypoints[i0],
                    self.waypoints[i1],
                    self.waypoints[self.currentIndex],
                    self.waypoints[i3],
                    self.currentTime,
                    self.waypoints[len(self.waypoints)-1].timeCode
                )
                
                self.transform.Set(
                    w.position[0], w.position[1], w.position[2],
                    w.lookAt[0], w.lookAt[1], w.lookAt[2],
                    w.upVector[0], w.upVector[1], w.upVector[2]
                )
                
                self.target.SetTransform(self.transform)
             
        self.currentTime += deltaFrameTime
        

#
# An application to test the SplinePathController with a camera.
#        
class TestPathedCameraApplication(Application):

    infiniteTerrain = InfiniteTerrain()
    helo = Object()
    
    def Config(self):
        Application.Config(self)
        self.AddDrawable(self.infiniteTerrain)
        self.helo.LoadFile('models/uh-1n.ive')
        self.AddDrawable(self.helo)

        hot = self.GetScene().GetHeightOfTerrain(0.0,0.0)
        transform = Transform()
        transform.SetTranslation(0.0,0.0,hot+10.0)
        self.helo.SetTransform(transform)
        
        self.spc = SplinePathController(
            self.GetCamera(),
            [ Waypoint(5.0, (0.0, -200.0, 50.0), (0.0, 0.0, 0.0)),
              Waypoint(10.0, (-200.0, 0.0, 50.0), (0.0, 0.0, 0.0)),
              Waypoint(15.0, (0.0, 200.0, 50.0), (0.0, 0.0, 0.0)),
              Waypoint(20.0, (200.0, 0.0, 50.0), (0.0, 0.0, 0.0)),
              Waypoint(30.0, (0.0, -1000.0, 500.0), (0.0, 0.0, 0.0)),
              Waypoint(35.0, (-1000.0, 0.0, 500.0), (0.0, 0.0, 0.0)),
              Waypoint(40.0, (0.0, 1000.0, 500.0), (0.0, 0.0, 0.0)),
              Waypoint(45.0, (1000.0, 0.0, 500.0), (0.0, 0.0, 0.0)) ]
        )
        
    def PreFrame(self, deltaFrameTime):
        self.spc.Update(deltaFrameTime)

SetDataFilePathList( GetDeltaRootPath() + '/examples/testPython/;' +
                               GetDeltaRootPath() + '/examples/data/;' +
                     GetDeltaDataPathList() )

testPathedCameraApp = TestPathedCameraApplication('config.xml')

testPathedCameraApp.Config()
testPathedCameraApp.Run()
