from PyDtCore import *
from PyDtABC import *
from PyDtAudio import *

class TestSoundApp( Application ):

   def __init__( self, configFile ) :
      Application.__init__( self, configFile )
      
      self.kSoundFile1 = 'sounds/exp57.wav'
      self.kSoundFile2 = 'sounds/exp35.wav'
      AudioManager.Instantiate()
      AudioManager.GetManager().Config()
      AudioManager.GetManager().LoadFile( self.kSoundFile1 )
      AudioManager.GetManager().LoadFile( self.kSoundFile2 )
      self.mSound = AudioManager.GetManager().NewSound()
      self.mSound.LoadFile( self.kSoundFile1 )

   def __del__( self ) :
      AudioManager.GetManager().FreeSound( self.mSound )
      AudioManager.GetManager().Destroy()
         
   def KeyPressed( self, keyboard, key ) :
      if key == KeyboardKey.KEY_Space :
         self.mSound.Play()

SetDataFilePathList( GetDeltaRootPath() + '/examples/testSound/;' +
                     GetDeltaDataPathList() )     
     
app = TestSoundApp( 'config.xml' )

app.Config()
app.Run()
