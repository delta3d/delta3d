from dtCore import *
from dtABC import *
from dtChar import *

from math import *
from time import *

def dotProduct(a, b):
    return sum([x*y for (x,y) in zip(a,b)])

def compareFuzzy(a,b,fuzz):
    if( abs(a[0] - b[0]) > fuzz or abs(a[1] - b[1]) > fuzz ):
        return 0
    return 1
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
    def __init__(self, position=(0.0, 0.0, 0.0), lookAt=(0.0, 1.0, 0.0), upVector=(0.0, 0.0, 1.0)):
    
        self.position = position
        self.lookAt = lookAt
        self.upVector = upVector
    
    
        
#
# The LinearPathController moves its target between a series of
# waypoints along linear paths.
#
class LinearPathController:
    
    #
    # Constructor.
    #
    # box1: box for curernt waypoint
    # box2: box for next waypoint
    # target: The Transformable target to move
    # waypoints: The list of waypoints to move between
    # loop: Whether or not to loop to the beginning after the last waypoint
    #
    def __init__(self, box1=None, box2=None, target=None, waypoints=[], loop=True):
    
        self.target = target
        self.box1 = box1
        self.box2 = box2
        self.waypoints = waypoints
        self.loop = loop
        self.transform = Transform()
        self.Reset()
        
        
    
    #
    # Resets the path controller to its initial state.
    #
    def Reset(self):
        self.currentIndex = 0
        
        
    #
    # Updates the transform of the target.  Should be called once per frame.
    #
    # deltaFrameTime: The amount of time elapsed since the last update, in seconds
    #
    def Update(self, deltaFrameTime):
    
        if self.target is not None and self.currentIndex < len(self.waypoints):
                 
            currentWaypoint = self.waypoints[self.currentIndex]
            
            if self.loop and self.currentIndex == (len(self.waypoints) -1):
                nextWaypoint = self.waypoints[0]
            else:
                nextWaypoint = self.waypoints[self.currentIndex + 1]
                
            #move boxes to current and next waypoints
            trans1 = Transform(currentWaypoint.position[0],currentWaypoint.position[1],currentWaypoint.position[2])
            self.box1.SetTransform(trans1)
                
            trans2 = Transform(nextWaypoint.position[0],nextWaypoint.position[1],nextWaypoint.position[2])
            self.box2.SetTransform(trans2)
                
            # calculate angle for rotation of character
            vec1 = [nextWaypoint.position[0] - currentWaypoint.position[0], nextWaypoint.position[1] - currentWaypoint.position[1]]
            vec2 = [ 0, -1 ]
                
            vec1Mag = sqrt( vec1[0]*vec1[0] + vec1[1]*vec1[1] )
            vec2Mag = sqrt( vec2[0]*vec2[0] + vec2[1]*vec2[1] )
               
            dotProd = dotProduct( vec1, vec2 )
                
            theta = degrees( acos( dotProd / (vec1Mag * vec2Mag) ) )
                
            if vec1[0] <= 0:
                newRotation = theta * -1
            elif vec1[0] >= 0:
                newRotation = theta
            else:
                newRotation = 180
            
            self.target.SetRotation( newRotation )
            self.target.SetVelocity(2.0)
                
            #find current location of target
            currentTransform = Transform()
            self.target.GetTransform(currentTransform)
            
            x = currentTransform.GetTranslationX()
            y = currentTransform.GetTranslationY()
            z = currentTransform.GetTranslationY()
        
            currentPosition = (x,y,z)
            nextPosition = nextWaypoint.position
            
            if compareFuzzy(currentPosition,nextPosition,0.1):
                
                if self.loop and self.currentIndex == (len(self.waypoints)-1):
                    self.currentIndex = 0
                else:
                    self.currentIndex = self.currentIndex + 1
                    
                

      
class TestCharWaypointsApp(Application):

    mTerrain   = Object("Terrain")
    mCharacter = Character("Bob")
    
    mOrbit     = OrbitMotionModel()
    
    mBox1      = Object("Box1")
    mBox2      = Object("Box2")
     
    def Config(self):
        Application.Config(self)
        SetDataFilePathList('../../data');
        
        self.GetWindow().SetWindowTitle("testCharWaypoints")
        
        camTrans = Transform(0.0,-20.0,20.0,0.0,-45.0,0.0)
        self.GetCamera().SetTransform(camTrans)
        
        self.mOrbit.SetDistance(10.0)
                
        self.mOrbit.SetDefaultMappings(self.GetKeyboard(),self.GetMouse())
        self.mOrbit.SetTarget(self.GetCamera())

        self.mTerrain.LoadFile("dirt.ive")
        self.mCharacter.LoadFile("marine/marine.rbody")
        
        self.mBox1.LoadFile("box.flt")
        self.mBox2.LoadFile("box.flt")

        self.AddDrawable(self.mTerrain)
        self.AddDrawable(self.mCharacter)
        self.AddDrawable(self.mBox1)
        self.AddDrawable(self.mBox2)
    
        waypoints = [ Waypoint((0.0, 0.0, 0.0), (0.0, 0.0, 0.0)),
              Waypoint((-20.0, 0.0, 0.0), (0.0, 0.0, 0.0)),
              Waypoint((0.0, 20.0, 0.0), (0.0, 0.0, 0.0)),
              Waypoint((20.0, 0.0, 0.0), (0.0, 0.0, 0.0)),
              Waypoint((0.0, -10.0, 0.0), (0.0, 0.0, 0.0)),
              Waypoint((-10.0, 0.0, 0.0), (0.0, 0.0, 0.0)),
              Waypoint((0.0, 10.0, 0.0), (0.0, 0.0, 0.0)),
              Waypoint((10.0, 0.0, 0.0), (0.0, 0.0, 0.0)) ]
  
        self.lpc = LinearPathController(self.mBox1,self.mBox2,self.mCharacter,waypoints)
        
    def PreFrame(self, deltaFrameTime):
        self.lpc.Update(deltaFrameTime)

app = TestCharWaypointsApp()

app.Config()
app.Run()
