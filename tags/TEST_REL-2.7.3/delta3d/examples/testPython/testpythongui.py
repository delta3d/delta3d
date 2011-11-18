from Tkinter import *

from PyDtUtil import *
from PyDtCore import *
from PyDtABC import *

from math import *
from time import *

def radians(v):
   return v * pi/180

class DeltaFrame:
    
    configured = False

    buttonStates = {1: False, 2: False, 3: False}
    
    keyMappings = {
        'Unknown' : 0x0000,
        'space' : 0x020,
        'exclam' : 0x021,
        'quotedbl' : 0x022,
        'numbersign' : 0x023,
        'dollar' : 0x024,
        'percent' : 0x025,
        'ampersand' : 0x026,
        'apostrophe' : 0x027,
        'quoteright' : 0x027,
        'parenleft' : 0x028,
        'parenright' : 0x029,
        'asterisk' : 0x02a,
        'plus' : 0x02b,
        'comma' : 0x02c,
        'minus' : 0x02d,
        'period' : 0x02e,
        'slash' : 0x02f,
        '0' : 0x030,
        '1' : 0x031,
        '2' : 0x032,
        '3' : 0x033,
        '4' : 0x034,
        '5' : 0x035,
        '6' : 0x036,
        '7' : 0x037,
        '8' : 0x038,
        '9' : 0x039,
        'colon' : 0x03a,
        'semicolon' : 0x03b,
        'less' : 0x03c,
        'equal' : 0x03d,
        'greater' : 0x03e,
        'question' : 0x03f,
        'at' : 0x040,
        'A' : 0x041,
        'B' : 0x042,
        'C' : 0x043,
        'D' : 0x044,
        'E' : 0x045,
        'F' : 0x046,
        'G' : 0x047,
        'H' : 0x048,
        'I' : 0x049,
        'J' : 0x04a,
        'K' : 0x04b,
        'L' : 0x04c,
        'M' : 0x04d,
        'N' : 0x04e,
        'O' : 0x04f,
        'P' : 0x050,
        'Q' : 0x051,
        'R' : 0x052,
        'S' : 0x053,
        'T' : 0x054,
        'U' : 0x055,
        'V' : 0x056,
        'W' : 0x057,
        'X' : 0x058,
        'Y' : 0x059,
        'Z' : 0x05a,
        'bracketleft' : 0x05b,
        'backslash' : 0x05c,
        'bracketright' : 0x05d,
        'asciicircum' : 0x05e,
        'underscore' : 0x05f,
        'grave' : 0x060,
        'quoteleft' : 0x060,
        'a' : 0x061,
        'b' : 0x062,
        'c' : 0x063,
        'd' : 0x064,
        'e' : 0x065,
        'f' : 0x066,
        'g' : 0x067,
        'h' : 0x068,
        'i' : 0x069,
        'j' : 0x06a,
        'k' : 0x06b,
        'l' : 0x06c,
        'm' : 0x06d,
        'n' : 0x06e,
        'o' : 0x06f,
        'p' : 0x070,
        'q' : 0x071,
        'r' : 0x072,
        's' : 0x073,
        't' : 0x074,
        'u' : 0x075,
        'v' : 0x076,
        'w' : 0x077,
        'x' : 0x078,
        'y' : 0x079,
        'z' : 0x07a,
        'braceleft' : 0x07b,
        'bar' : 0x07c,
        'braceright' : 0x07d,
        'asciitilde' : 0x07e,
        'nobreakspace' : 0x0a0,
        'exclamdown' : 0x0a1,
        'cent' : 0x0a2,
        'sterling' : 0x0a3,
        'currency' : 0x0a4,
        'yen' : 0x0a5,
        'brokenbar' : 0x0a6,
        'section' : 0x0a7,
        'diaeresis' : 0x0a8,
        'copyright' : 0x0a9,
        'ordfeminine' : 0x0aa,
        'guillemotleft' : 0x0ab,
        'notsign' : 0x0ac,
        'hyphen' : 0x0ad,
        'registered' : 0x0ae,
        'macron' : 0x0af,
        'degree' : 0x0b0,
        'plusminus' : 0x0b1,
        'twosuperior' : 0x0b2,
        'threesuperior' : 0x0b3,
        'acute' : 0x0b4,
        'mu' : 0x0b5,
        'paragraph' : 0x0b6,
        'periodcentered' : 0x0b7,
        'cedilla' : 0x0b8,
        'onesuperior' : 0x0b9,
        'masculine' : 0x0ba,
        'guillemotright' : 0x0bb,
        'onequarter' : 0x0bc,
        'onehalf' : 0x0bd,
        'threequarters' : 0x0be,
        'questiondown' : 0x0bf,
        'Agrave' : 0x0c0,
        'Aacute' : 0x0c1,
        'Acircumflex' : 0x0c2,
        'Atilde' : 0x0c3,
        'Adiaeresis' : 0x0c4,
        'Aring' : 0x0c5,
        'AE' : 0x0c6,
        'Ccedilla' : 0x0c7,
        'Egrave' : 0x0c8,
        'Eacute' : 0x0c9,
        'Ecircumflex' : 0x0ca,
        'Ediaeresis' : 0x0cb,
        'Igrave' : 0x0cc,
        'Iacute' : 0x0cd,
        'Icircumflex' : 0x0ce,
        'Idiaeresis' : 0x0cf,
        'ETH' : 0x0d0,
        'Eth' : 0x0d0,
        'Ntilde' : 0x0d1,
        'Ograve' : 0x0d2,
        'Oacute' : 0x0d3,
        'Ocircumflex' : 0x0d4,
        'Otilde' : 0x0d5,
        'Odiaeresis' : 0x0d6,
        'multiply' : 0x0d7,
        'Ooblique' : 0x0d8,
        'Ugrave' : 0x0d9,
        'Uacute' : 0x0da,
        'Ucircumflex' : 0x0db,
        'Udiaeresis' : 0x0dc,
        'Yacute' : 0x0dd,
        'THORN' : 0x0de,
        'Thorn' : 0x0de,
        'ssharp' : 0x0df,
        'agrave' : 0x0e0,
        'aacute' : 0x0e1,
        'acircumflex' : 0x0e2,
        'atilde' : 0x0e3,
        'adiaeresis' : 0x0e4,
        'aring' : 0x0e5,
        'ae' : 0x0e6,
        'ccedilla' : 0x0e7,
        'egrave' : 0x0e8,
        'eacute' : 0x0e9,
        'ecircumflex' : 0x0ea,
        'ediaeresis' : 0x0eb,
        'igrave' : 0x0ec,
        'iacute' : 0x0ed,
        'icircumflex' : 0x0ee,
        'idiaeresis' : 0x0ef,
        'eth' : 0x0f0,
        'ntilde' : 0x0f1,
        'ograve' : 0x0f2,
        'oacute' : 0x0f3,
        'ocircumflex' : 0x0f4,
        'otilde' : 0x0f5,
        'odiaeresis' : 0x0f6,
        'division' : 0x0f7,
        'oslash' : 0x0f8,
        'ugrave' : 0x0f9,
        'uacute' : 0x0fa,
        'ucircumflex' : 0x0fb,
        'udiaeresis' : 0x0fc,
        'yacute' : 0x0fd,
        'thorn' : 0x0fe,
        'ydiaeresis' : 0x0ff,
        'BackSpace' : 0xFF08,
        'Tab' : 0xFF09,
        'Linefeed' : 0xFF0A,
        'Clear' : 0xFF0B,
        'Return' : 0xFF0D,
        'Pause' : 0xFF13,
        'Scroll_Lock' : 0xFF14,
        'Sys_Req' : 0xFF15,
        'Escape' : 0xFF1B,
        'Multi_key' : 0xFF20,
        'Kanji' : 0xFF21,
        'Muhenkan' : 0xFF22,
        'Henkan_Mode' : 0xFF23,
        'Henkan' : 0xFF23,
        'Romaji' : 0xFF24,
        'Hiragana' : 0xFF25,
        'Katakana' : 0xFF26,
        'Hiragana_Katakana' : 0xFF27,
        'Zenkaku' : 0xFF28,
        'Hankaku' : 0xFF29,
        'Zenkaku_Hankaku' : 0xFF2A,
        'Touroku' : 0xFF2B,
        'Massyo' : 0xFF2C,
        'Kana_Lock' : 0xFF2D,
        'Kana_Shift' : 0xFF2E,
        'Eisu_Shift' : 0xFF2F,
        'Eisu_toggle' : 0xFF30,
        'Codeinput' : 0xFF37,
        'Kanji_Bangou' : 0xFF37,
        'SingleCandidate' : 0xFF3C,
        'MultipleCandidate' : 0xFF3D,
        'Zen_Koho' : 0xFF3D,
        'PreviousCandidate' : 0xFF3E,
        'Mae_Koho' : 0xFF3E,
        'Home' : 0xFF50,
        'Left' : 0xFF51,
        'Up' : 0xFF52,
        'Right' : 0xFF53,
        'Down' : 0xFF54,
        'Prior' : 0xFF55,
        'Page_Up' : 0xFF55,
        'Next' : 0xFF56,
        'Page_Down' : 0xFF56,
        'End' : 0xFF57,
        'Begin' : 0xFF58,
        'Select' : 0xFF60,
        'Print' : 0xFF61,
        'Execute' : 0xFF62,
        'Insert' : 0xFF63,
        'Undo' : 0xFF65,
        'Redo' : 0xFF66,
        'Menu' : 0xFF67,
        'Find' : 0xFF68,
        'Cancel' : 0xFF69,
        'Help' : 0xFF6A,
        'Break' : 0xFF6B,
        'Mode_switch' : 0xFF7E,
        'script_switch' : 0xFF7E,
        'kana_switch' : 0xFF7E,
        'Arabic_switch' : 0xFF7E,
        'Greek_switch' : 0xFF7E,
        'Hebrew_switch' : 0xFF7E,
        'Hangul_switch' : 0xFF7E,
        'Num_Lock' : 0xFF7F,
        'KP_Space' : 0xFF80,
        'KP_Tab' : 0xFF89,
        'KP_Enter' : 0xFF8D,
        'KP_F1' : 0xFF91,
        'KP_F2' : 0xFF92,
        'KP_F3' : 0xFF93,
        'KP_F4' : 0xFF94,
        'KP_Home' : 0xFF95,
        'KP_Left' : 0xFF96,
        'KP_Up' : 0xFF97,
        'KP_Right' : 0xFF98,
        'KP_Down' : 0xFF99,
        'KP_Prior' : 0xFF9A,
        'KP_Page_Up' : 0xFF9A,
        'KP_Next' : 0xFF9B,
        'KP_Page_Down' : 0xFF9B,
        'KP_End' : 0xFF9C,
        'KP_Begin' : 0xFF9D,
        'KP_Insert' : 0xFF9E,
        'KP_Delete' : 0xFF9F,
        'KP_Multiply' : 0xFFAA,
        'KP_Add' : 0xFFAB,
        'KP_Separator' : 0xFFAC,
        'KP_Subtract' : 0xFFAD,
        'KP_Decimal' : 0xFFAE,
        'KP_Divide' : 0xFFAF,
        'KP_0' : 0xFFB0,
        'KP_1' : 0xFFB1,
        'KP_2' : 0xFFB2,
        'KP_3' : 0xFFB3,
        'KP_4' : 0xFFB4,
        'KP_5' : 0xFFB5,
        'KP_6' : 0xFFB6,
        'KP_7' : 0xFFB7,
        'KP_8' : 0xFFB8,
        'KP_9' : 0xFFB9,
        'KP_Equal' : 0xFFBD,
        'F1' : 0xFFBE,
        'F2' : 0xFFBF,
        'F3' : 0xFFC0,
        'F4' : 0xFFC1,
        'F5' : 0xFFC2,
        'F6' : 0xFFC3,
        'F7' : 0xFFC4,
        'F8' : 0xFFC5,
        'F9' : 0xFFC6,
        'F10' : 0xFFC7,
        'F11' : 0xFFC8,
        'L1' : 0xFFC8,
        'F12' : 0xFFC9,
        'L2' : 0xFFC9,
        'F13' : 0xFFCA,
        'L3' : 0xFFCA,
        'F14' : 0xFFCB,
        'L4' : 0xFFCB,
        'F15' : 0xFFCC,
        'L5' : 0xFFCC,
        'F16' : 0xFFCD,
        'L6' : 0xFFCD,
        'F17' : 0xFFCE,
        'L7' : 0xFFCE,
        'F18' : 0xFFCF,
        'L8' : 0xFFCF,
        'F19' : 0xFFD0,
        'L9' : 0xFFD0,
        'F20' : 0xFFD1,
        'L10' : 0xFFD1,
        'F21' : 0xFFD2,
        'R1' : 0xFFD2,
        'F22' : 0xFFD3,
        'R2' : 0xFFD3,
        'F23' : 0xFFD4,
        'R3' : 0xFFD4,
        'F24' : 0xFFD5,
        'R4' : 0xFFD5,
        'F25' : 0xFFD6,
        'R5' : 0xFFD6,
        'F26' : 0xFFD7,
        'R6' : 0xFFD7,
        'F27' : 0xFFD8,
        'R7' : 0xFFD8,
        'F28' : 0xFFD9,
        'R8' : 0xFFD9,
        'F29' : 0xFFDA,
        'R9' : 0xFFDA,
        'F30' : 0xFFDB,
        'R10' : 0xFFDB,
        'F31' : 0xFFDC,
        'R11' : 0xFFDC,
        'F32' : 0xFFDD,
        'R12' : 0xFFDD,
        'F33' : 0xFFDE,
        'R13' : 0xFFDE,
        'F34' : 0xFFDF,
        'R14' : 0xFFDF,
        'F35' : 0xFFE0,
        'R15' : 0xFFE0,
        'Shift_L' : 0xFFE1,
        'Shift_R' : 0xFFE2,
        'Control_L' : 0xFFE3,
        'Control_R' : 0xFFE4,
        'Caps_Lock' : 0xFFE5,
        'Shift_Lock' : 0xFFE6,
        'Meta_L' : 0xFFE7,
        'Meta_R' : 0xFFE8,
        'Alt_L' : 0xFFE9,
        'Alt_R' : 0xFFEA,
        'Super_L' : 0xFFEB,
        'Super_R' : 0xFFEC,
        'Hyper_L' : 0xFFED,
        'Hyper_R' : 0xFFEE,
        'Delete' : 0xFFFF}
        
    def __init__(self, widget, master):
    
        self.master = master
        
        self.frame = Frame(master, width=640, height=480, bg="")
        self.frame.pack(fill=BOTH, expand=1)
        
        self.frame.bind("<Configure>", self.resize)
        self.frame.bind("<ButtonPress>", self.onButtonPress)
        self.frame.bind("<ButtonRelease>", self.onButtonRelease)
        self.frame.bind("<Double-Button>", self.onDoubleButton)
        self.frame.bind("<Motion>", self.onMotion)
        self.frame.bind("<KeyPress>", self.onKeyPress)
        self.frame.bind("<KeyRelease>", self.onKeyRelease)
        
        self.frame.focus_set()
        
        self.widget = widget
        
        self.widget.AddSender(self.widget)
        
    def resize(self, event):
    
        winData = WinData()
        
        winData.pos_x = self.frame.winfo_x()
        winData.pos_y = self.frame.winfo_y()
        winData.width = self.frame.winfo_width()
        winData.height = self.frame.winfo_height()
        if self.configured:
            self.widget.SendMessage(Widget.msgResize, winData)
        else:
            winData.hwnd = self.frame.winfo_id()
            self.widget.SendMessage(Widget.msgWindowData, winData)
            self.configured = True
            self.frame.after(1, self.step)
            
    def onButtonPress(self, event):

        mouseEvent = MouseEvent()

        mouseEvent.event = MouseEvent.PUSH
        mouseEvent.pos_x = event.x*2.0/self.frame.winfo_width()-1.0
        mouseEvent.pos_y = event.y*-2.0/self.frame.winfo_height()+1.0
        mouseEvent.button = event.num

        self.buttonStates[mouseEvent.button] = True
        
        self.widget.SendMessage(Widget.msgMouseEvent, mouseEvent)
        
    def onButtonRelease(self, event):

        mouseEvent = MouseEvent()

        mouseEvent.event = MouseEvent.RELEASE
        mouseEvent.pos_x = event.x*2.0/self.frame.winfo_width()-1.0
        mouseEvent.pos_y = event.y*-2.0/self.frame.winfo_height()+1.0
        mouseEvent.button = event.num

        self.buttonStates[mouseEvent.button] = False
        
        self.widget.SendMessage(Widget.msgMouseEvent, mouseEvent)
        
    def onDoubleButton(self, event):

        mouseEvent = MouseEvent()

        mouseEvent.event = MouseEvent.DOUBLE
        mouseEvent.pos_x = event.x*2.0/self.frame.winfo_width()-1.0
        mouseEvent.pos_y = event.y*-2.0/self.frame.winfo_height()+1.0
        mouseEvent.button = event.num
        
        self.widget.SendMessage(Widget.msgMouseEvent, mouseEvent)
        
    def onMotion(self, event):

        mouseEvent = MouseEvent()

        if self.buttonStates[1] or self.buttonStates[2] or self.buttonStates[3]:
            mouseEvent.event = MouseEvent.DRAG
        else:
            mouseEvent.event = MouseEvent.MOVE

        mouseEvent.pos_x = event.x*2.0/self.frame.winfo_width()-1.0
        mouseEvent.pos_y = event.y*-2.0/self.frame.winfo_height()+1.0
        
        self.widget.SendMessage(Widget.msgMouseEvent, mouseEvent)
        
    def onKeyPress(self, event):

        keyboardEvent = KeyboardEvent()

        keyboardEvent.event = KeyboardEvent.KEYDOWN
        keyboardEvent.key = self.keyMappings[event.keysym]
        
        self.widget.SendMessage(Widget.msgKeyboardEvent, keyboardEvent)
    
    def onKeyRelease(self, event):

        keyboardEvent = KeyboardEvent()

        keyboardEvent.event = KeyboardEvent.KEYUP
        keyboardEvent.key = self.keyMappings[event.keysym]
        
        self.widget.SendMessage(Widget.msgKeyboardEvent, keyboardEvent)
    
    def step(self):

        self.widget.SendMessage(Widget.msgStep)
        self.frame.after(1, self.step)
        
                
class TestPythonGUIApplication(Widget):

    transform = Transform()
    angle = 0.0
    
    def Config(self, data):
        Widget.Config(self, data)
        self.helo = Object('UH-1N')
        self.helo.LoadFile('models/uh-1n.ive')
        self.AddDrawable(self.helo)
        self.omm = OrbitMotionModel(self.GetKeyboard(), self.GetMouse())
        self.omm.SetTarget(self.GetCamera())
        
    def Quit(self):
        root.quit()
        
    def PreFrame(self, deltaFrameTime):
        self.transform.Set(40*cos(radians(self.angle)),
                           100 + 40*sin(radians(self.angle)), 
                           0, self.angle, 0, -45)
        self.helo.SetTransform(self.transform)
        self.angle += 45*deltaFrameTime
        
root = Tk()

menu = Menu(root)

root.config(menu=menu)

filemenu = Menu(menu, tearoff=False)
menu.add_cascade(label="File", menu=filemenu)
filemenu.add_command(label="Quit", command=root.quit)

SetDataFilePathList( GetDeltaDataPathList() )   
                     
testPythonGUIApp = TestPythonGUIApplication()

deltaFrame = DeltaFrame(testPythonGUIApp, root)

root.mainloop()
