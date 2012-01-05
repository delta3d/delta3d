from PyDtUtil import *
from PyDtCore import *
from PyDtAI import *
from PyDtABC import *
from PyDtAnim import *

import sys
import math
import random

class AICharacter:
   
   def __init__(self, scene, camera, waypoint, filename, speed): 
      self.mSpeed = speed
      self.mCharacter = CharacterWrapper("demoMap/SkeletalMeshes/marine.xml")
      self.mCurrentWaypoint = waypoint
      self.mAStar = WaypointAStar()
      self.mScene = scene
      self.mCharacter.SetGroundClamp(scene, 0.0)
      self.SetPosition(self.mCurrentWaypoint)
      scene.AddDrawable(self.mCharacter)
      
      self.mCharacter.AddChild(camera)

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
      if not self.mWaypointPath:
          return False
      #for waypoint in self.mWaypointPath:
      #   waypoint.SetRenderFlag(Waypoint.RENDER_GREEN)
      for i in range(len(self.mWaypointPath)):
          self.mWaypointPath[i].SetRenderFlag(Waypoint.RENDER_GREEN) 
      self.mWaypointPath[-1].SetRenderFlag(Waypoint.RENDER_RED)
      return True

   def GoToWaypoint(self, dt, waypoint):
      self.mCharacter.RotateToPoint(waypoint.GetPosition(), dt * 3.0)
      self.mCharacter.SetSpeed(-self.mSpeed)
      if not self.mCharacter.IsAnimationPlaying("Walk"):
        self.mCharacter.ClearAllAnimations(0.5)
        self.mCharacter.PlayAnimation("Walk")

   def GetPosition(self):
      trans = Transform()
      self.mCharacter.GetTransform(trans)
      pos = Vec3()
      trans.GetTranslation(pos)
      return pos

   def AmAtWaypoint(self, waypoint):
      pos = self.GetPosition()
      wayPos = waypoint.GetPosition()
      distToX = abs(pos[0] - wayPos[0])
      distToY = abs(pos[1] - wayPos[1])
      return (distToX < 0.1) and ( distToY < 0.1)   

   def ApplyStringPulling(self):
      if len(self.mWaypointPath) >= 2:
         isector = Isector(self.mScene, self.GetPosition(), self.mWaypointPath[1].GetPosition())
         if not isector.Update():
            self.mWaypointPath[0].SetRenderFlag(Waypoint.RENDER_BLUE)
            del self.mWaypointPath[0]
      

   def Update(self, dt):
      if self.mWaypointPath:
         if(self.AmAtWaypoint(self.mWaypointPath[0])):
            self.mCurrentWaypoint = self.mWaypointPath[0]
            del self.mWaypointPath[0]
            self.mCurrentWaypoint.SetRenderFlag(Waypoint.RENDER_BLUE)
         if self.mWaypointPath:
            self.ApplyStringPulling()
            self.GoToWaypoint(dt, self.mWaypointPath[0])
         else:
            self.mCharacter.SetSpeed(0)
      self.mCharacter.Update(dt)
         
class TestAI(Application):

   def Config(self) :
      contextName = GetDeltaRootPath()+"/examples/data/demoMap";
      Project.GetInstance().SetContext(contextName)
      myMap = Project.GetInstance().GetMap("TesttownLt")
      self.LoadMap(myMap)

      WaypointManager.GetInstance().SetDrawWaypoints(1)
      WaypointManager.GetInstance().SetDrawNavMesh(0, 0)
      self.mDrawNavMesh = 0
      
      self.AddDrawable(WaypointManager.GetInstance())    
      
      trans = Transform()

      trans.SetTranslation(-1.0, 5.5, 1.5)
      trans.SetRotation(180.0, -2.0, 0.0)
      self.GetCamera().SetTransform(trans)

      #create overhead camera
      overheadView = View("overhead view")
      overheadView.SetScene( self.GetScene() )
      overheadView.SetKeyboard( self.GetView().GetKeyboard() )
      overheadView.SetMouse( self.GetView().GetMouse() )

      self.AddView( overheadView )
  
      self.mOverheadCamera = Camera()
      self.mOverheadCamera.SetWindow(self.GetWindow())
      self.mOverheadCamera.SetEnabled(0)
      overheadView.SetCamera( self.mOverheadCamera )
      trans.SetTranslation(-1.0, 5.0, 100.0)
      trans.SetRotation(90.0, 270.0, 0.0)
      
      self.mOverheadCamera.SetTransform(trans)
      self.GetCamera().AddChild(self.mOverheadCamera)
      
      self.mWaypointList = WaypointManager.GetInstance().GetWaypoints()
      self.character = AICharacter(self.GetScene(), self.GetCamera(), self.mWaypointList[0], str("marine/marine.rbody"), 10)                 

      self.mWaypoint = self.mWaypointList[1]
      self.character.FindPathAndGoToWaypoint(self.mWaypoint)

   def PreFrame(self, deltaFrameTime) :      
      if self.character.GetCurrentWaypoint().GetID() == self.mWaypoint.GetID():
          foundPathB = False
          while not foundPathB:
             self.mWaypoint = random.choice(self.mWaypointList)
             foundPathB = self.character.FindPathAndGoToWaypoint(self.mWaypoint)
      self.character.Update(deltaFrameTime)
        
   def KeyPressed( self, keyboard, key ) :
      verdict = 0
      if key == int(KeyboardKey.KEY_Space) :            
         self.mOverheadCamera.SetEnabled(not self.mOverheadCamera.GetEnabled())
         self.GetCamera().SetEnabled(not self.GetCamera().GetEnabled())               
         verdict = 1
      elif key == ord('N'):
         WaypointManager.GetInstance().SetDrawNavMesh(not self.mDrawNavMesh, 1)
         self.mDrawNavMesh = not self.mDrawNavMesh
         verdict = 1
      elif key == int(KeyboardKey.KEY_Escape): 
         self.Quit()
         verdict = 1
      elif key == ord('A'): 
         if self.mOverheadCamera.GetEnabled():
            trans = Transform()
            self.mOverheadCamera.GetTransform(trans)
            vec = Vec3()            
            trans.GetTranslation(vec)
            #vec[1] -= (15.5 / 100.0)
            vec[2] -= 1.0
            trans.SetTranslation(vec)
            self.mOverheadCamera.SetTransform(trans)
            verdict = 1
      elif key == ord('Z'): 
         if self.mOverheadCamera.GetEnabled():
            trans = Transform()
            self.mOverheadCamera.GetTransform(trans)
            vec = Vec3()            
            trans.GetTranslation(vec)
            #vec[1] += (15.5 / 100.0)
            vec[2] += 1.0
            trans.SetTranslation(vec)
            self.mOverheadCamera.SetTransform(trans)
            verdict = 1
      return verdict     
                        
SetDataFilePathList (   GetDeltaDataPathList() + ";" +
                        GetDeltaRootPath() + "/examples/data/;" )

app = TestAI('testMap.xml')

app.Config()
app.Run()
