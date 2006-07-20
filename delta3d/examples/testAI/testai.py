from PyDtUtil import *
from PyDtCore import *
from PyDtAI import *
from PyDtABC import *
from PyDtDAL import *
from PyDtChar import *

import sys
import math
import random

class AICharacter:
   
   def __init__(self, scene, camera, waypoint, filename, speed):
      self.mCharacter = Character("AICharacter")
      self.mCharacter.LoadFile(filename)
      self.mCurrentWaypoint = waypoint
      self.SetPosition(self.mCurrentWaypoint)
      scene.AddDrawable(self.mCharacter)
      self.mCharacter.AddChild(camera)
      self.mAStar = WaypointAStar()
      self.mSpeed = speed

   def SetPosition(self, waypoint):
      trans = Transform()
      trans.SetTranslation(waypoint.GetPosition())
      self.mCharacter.SetTransform(trans)      

   def GetCurrentWaypoint(self):
      return self.mCurrentWaypoint
   
   def FindPathAndGoToWaypoint(self, waypoint):
      self.mAStar.Reset(self.mCurrentWaypoint, waypoint)
      result = self.mAStar.FindPath()
      self.mWaypointPath = self.mAStar.GetPath()
      #for waypoint in self.mWaypointPath:
      #   waypoint.SetRenderFlag(Waypoint.RENDER_GREEN) 
      self.mWaypointPath[-1].SetRenderFlag(Waypoint.RENDER_RED)

   def GoToWaypoint(self, dt, waypoint):
      self.mCharacter.RotateCharacterToPoint(waypoint.GetPosition(), dt)
      self.mCharacter.SetVelocity(self.mSpeed)

   def GetPosition(self):
      trans = Transform()
      self.mCharacter.GetTransform(trans)
      pos = Vec3()
      trans.GetTranslation(pos)
      return pos

   def AmAtWaypoint(self, waypoint):
      pos = self.GetPosition()
      wayPos = waypoint.GetPosition()
      distToX = math.fabs(pos[0] - wayPos[0])
      distToY = math.fabs(pos[1] - wayPos[1])
      return (distToX < 1.0) and ( distToY < 1.0)   

   def Update(self, dt):
      if (len (self.mWaypointPath) != 0):
         if(self.AmAtWaypoint(self.mWaypointPath[0])):
            self.mCurrentWaypoint = self.mWaypointPath[0]
            del self.mWaypointPath[0]
            self.mCurrentWaypoint.SetRenderFlag(Waypoint.RENDER_BLUE)
         if (len (self.mWaypointPath) != 0):
            self.GoToWaypoint(dt, self.mWaypointPath[0])
         else:
            self.mCharacter.SetVelocity(0)
         
class TestAI(Application):

   def Config(self) :
      
      contextName = GetDeltaDataPathList() + "/demoMap"
      
      Project.GetInstance().SetContext(contextName)
      myMap = Project.GetInstance().GetMap("TesttownLt")
      self.LoadMap(myMap)

      WaypointManager.GetInstance().CreateNavMesh(self.GetScene())
      WaypointManager.GetInstance().SetDrawWaypoints(1)
      WaypointManager.GetInstance().SetDrawNavMesh(0)
      self.mDrawNavMesh = 0
      
      self.AddDrawable(WaypointManager.GetInstance())    
      
      trans = Transform()
      trans.SetTranslation(-1.0, 5.5, 1.5)
      trans.SetRotation(180.0, -2.0, 0.0)
      self.GetCamera().SetTransform(trans)

      #create overhead camera
      self.mOverheadCamera = Camera()
      self.mOverheadCamera.SetScene(self.GetScene())
      self.mOverheadCamera.SetWindow(self.GetWindow())
      self.mOverheadCamera.SetEnabled(0)
      trans.SetTranslation(-1.0, 20.0, 100.0)
      trans.SetRotation(90.0, 270.0, 0.0)
      self.mOverheadCamera.SetTransform(trans)
      self.GetCamera().AddChild(self.mOverheadCamera)
      
      self.mWaypointList = WaypointManager.GetInstance().GetWaypoints()
      self.character = AICharacter(self.GetScene(), self.GetCamera(), self.mWaypointList[0], str("marine/marine.rbody"), 10)                 

      self.mWaypoint = self.mWaypointList[1]
      self.character.FindPathAndGoToWaypoint(self.mWaypoint)

   def PreFrame(self, deltaFrameTime) :      
      if self.character.GetCurrentWaypoint().GetPosition() == self.mWaypoint.GetPosition():
         self.mWaypoint = random.choice(self.mWaypointList)
         self.character.FindPathAndGoToWaypoint(self.mWaypoint)
      self.character.Update(deltaFrameTime)

   def KeyPressed( self, keyboard, key, character ) :
      verdict = 0
      if key is KeyboardKey.Key_space :            
         self.mOverheadCamera.SetEnabled(not self.mOverheadCamera.GetEnabled())
         self.GetCamera().SetEnabled(not self.GetCamera().GetEnabled())               
         verdict = 1
      elif key is KeyboardKey.Key_N :
         WaypointManager.GetInstance().SetDrawNavMesh(not self.mDrawNavMesh)
         self.mDrawNavMesh = not self.mDrawNavMesh
         verdict = 1
      if key is KeyboardKey.Key_Escape :
         self.Quit()
         verdict = 1
      return verdict     

SetDataFilePathList (   GetDeltaRootPath() + "/examples/testMap/;" +
                        GetDeltaDataPathList() + ";" +
                        GetDeltaDataPathList() + "/demoMap/Particles" )

app = TestAI('testMap.xml')

app.Config()
app.Run()
