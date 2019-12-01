// keyboardbindings.cpp: Keyboard binding implementations.
//
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtCore/keyboard.h>
#include <dtCore/refptr.h>

using namespace boost::python;
using namespace dtCore;
using namespace osgGA;

class KeyboardListenerWrap : public KeyboardListener, public wrapper<KeyboardListener>
{
   public:
   KeyboardListenerWrap()
   {
   }
   protected:
   virtual ~KeyboardListenerWrap()
   {
   }
   public:
   virtual bool HandleKeyPressed(const Keyboard* keyboard, 
                                 int kc )
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleKeyPressed" ).ptr(), boost::ref(keyboard), kc );
      #else
      return this->get_override( "HandleKeyPressed" )( boost::ref(keyboard), kc );
      #endif
   }
   
   virtual bool HandleKeyReleased(const Keyboard* keyboard, 
                                  int kc )
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleKeyReleased" ).ptr(), boost::ref(keyboard), kc );
      #else
      return this->get_override( "HandleKeyReleased" )( boost::ref(keyboard), kc );
      #endif
   }
   
   virtual bool HandleKeyTyped(const Keyboard* keyboard, 
                               int kc )
   {
      #if defined( _MSC_VER ) && ( _MSC_VER == 1400 ) // MSVC 8.0
      return call<bool>( this->get_override( "HandleKeyTyped" ).ptr(), boost::ref(keyboard), kc );
      #else
      return this->get_override( "HandleKeyTyped" )( boost::ref(keyboard), kc );
      #endif
   }
};

void initKeyboardBindings()
{
   #define ENUM_KEYBOARD_KEY(VALUE) keyboard_key.value(#VALUE, GUIEventAdapter::VALUE)

   enum_<GUIEventAdapter::KeySymbol> keyboard_key("KeyboardKey");
   ENUM_KEYBOARD_KEY( KEY_Space );
   ENUM_KEYBOARD_KEY( KEY_BackSpace );
   ENUM_KEYBOARD_KEY( KEY_Tab );
   ENUM_KEYBOARD_KEY( KEY_Linefeed );
   ENUM_KEYBOARD_KEY( KEY_Clear );
   ENUM_KEYBOARD_KEY( KEY_Return );
   ENUM_KEYBOARD_KEY( KEY_Pause );
   ENUM_KEYBOARD_KEY( KEY_Scroll_Lock );
   ENUM_KEYBOARD_KEY( KEY_Sys_Req );
   ENUM_KEYBOARD_KEY( KEY_Escape );
   ENUM_KEYBOARD_KEY( KEY_Delete );
   ENUM_KEYBOARD_KEY( KEY_Home );
   ENUM_KEYBOARD_KEY( KEY_Left );
   ENUM_KEYBOARD_KEY( KEY_Up );
   ENUM_KEYBOARD_KEY( KEY_Right );
   ENUM_KEYBOARD_KEY( KEY_Down );
   ENUM_KEYBOARD_KEY( KEY_Prior );
   ENUM_KEYBOARD_KEY( KEY_Page_Up );
   ENUM_KEYBOARD_KEY( KEY_Next );
   ENUM_KEYBOARD_KEY( KEY_Page_Down );
   ENUM_KEYBOARD_KEY( KEY_End );
   ENUM_KEYBOARD_KEY( KEY_Begin );
   ENUM_KEYBOARD_KEY( KEY_Select );
   ENUM_KEYBOARD_KEY( KEY_Print );
   ENUM_KEYBOARD_KEY( KEY_Execute );
   ENUM_KEYBOARD_KEY( KEY_Insert );
   ENUM_KEYBOARD_KEY( KEY_Undo );
   ENUM_KEYBOARD_KEY( KEY_Redo );
   ENUM_KEYBOARD_KEY( KEY_Menu );
   ENUM_KEYBOARD_KEY( KEY_Find );
   ENUM_KEYBOARD_KEY( KEY_Cancel );
   ENUM_KEYBOARD_KEY( KEY_Help );
   ENUM_KEYBOARD_KEY( KEY_Break );
   ENUM_KEYBOARD_KEY( KEY_Mode_switch );
   ENUM_KEYBOARD_KEY( KEY_Script_switch );
   ENUM_KEYBOARD_KEY( KEY_Num_Lock );
   ENUM_KEYBOARD_KEY( KEY_KP_Space );
   ENUM_KEYBOARD_KEY( KEY_KP_Tab );
   ENUM_KEYBOARD_KEY( KEY_KP_Enter );
   ENUM_KEYBOARD_KEY( KEY_KP_F1 );
   ENUM_KEYBOARD_KEY( KEY_KP_F2 );
   ENUM_KEYBOARD_KEY( KEY_KP_F3 );
   ENUM_KEYBOARD_KEY( KEY_KP_F4 );
   ENUM_KEYBOARD_KEY( KEY_KP_Home );
   ENUM_KEYBOARD_KEY( KEY_KP_Left );
   ENUM_KEYBOARD_KEY( KEY_KP_Up );
   ENUM_KEYBOARD_KEY( KEY_KP_Right );
   ENUM_KEYBOARD_KEY( KEY_KP_Down );
   ENUM_KEYBOARD_KEY( KEY_KP_Prior );
   ENUM_KEYBOARD_KEY( KEY_KP_Page_Up );
   ENUM_KEYBOARD_KEY( KEY_KP_Next );
   ENUM_KEYBOARD_KEY( KEY_KP_Page_Down );
   ENUM_KEYBOARD_KEY( KEY_KP_End );
   ENUM_KEYBOARD_KEY( KEY_KP_Begin );
   ENUM_KEYBOARD_KEY( KEY_KP_Insert );
   ENUM_KEYBOARD_KEY( KEY_KP_Delete );
   ENUM_KEYBOARD_KEY( KEY_KP_Equal );
   ENUM_KEYBOARD_KEY( KEY_KP_Multiply );
   ENUM_KEYBOARD_KEY( KEY_KP_Add );
   ENUM_KEYBOARD_KEY( KEY_KP_Separator );
   ENUM_KEYBOARD_KEY( KEY_KP_Subtract );
   ENUM_KEYBOARD_KEY( KEY_KP_Decimal );
   ENUM_KEYBOARD_KEY( KEY_KP_Divide );
   ENUM_KEYBOARD_KEY( KEY_KP_0 );
   ENUM_KEYBOARD_KEY( KEY_KP_1 );
   ENUM_KEYBOARD_KEY( KEY_KP_2 );
   ENUM_KEYBOARD_KEY( KEY_KP_3 );
   ENUM_KEYBOARD_KEY( KEY_KP_4 );
   ENUM_KEYBOARD_KEY( KEY_KP_5 );
   ENUM_KEYBOARD_KEY( KEY_KP_6 );
   ENUM_KEYBOARD_KEY( KEY_KP_7 );
   ENUM_KEYBOARD_KEY( KEY_KP_8 );
   ENUM_KEYBOARD_KEY( KEY_KP_9 );
   ENUM_KEYBOARD_KEY( KEY_F1 );
   ENUM_KEYBOARD_KEY( KEY_F2 );
   ENUM_KEYBOARD_KEY( KEY_F3 );
   ENUM_KEYBOARD_KEY( KEY_F4 );
   ENUM_KEYBOARD_KEY( KEY_F5 );
   ENUM_KEYBOARD_KEY( KEY_F6 );
   ENUM_KEYBOARD_KEY( KEY_F7 );
   ENUM_KEYBOARD_KEY( KEY_F8 );
   ENUM_KEYBOARD_KEY( KEY_F9 );
   ENUM_KEYBOARD_KEY( KEY_F10 );
   ENUM_KEYBOARD_KEY( KEY_F11 );
   ENUM_KEYBOARD_KEY( KEY_F12 );
   ENUM_KEYBOARD_KEY( KEY_F13 );
   ENUM_KEYBOARD_KEY( KEY_F14 );
   ENUM_KEYBOARD_KEY( KEY_F15 );
   ENUM_KEYBOARD_KEY( KEY_F16 );
   ENUM_KEYBOARD_KEY( KEY_F17 );
   ENUM_KEYBOARD_KEY( KEY_F18 );
   ENUM_KEYBOARD_KEY( KEY_F19 );
   ENUM_KEYBOARD_KEY( KEY_F20 );
   ENUM_KEYBOARD_KEY( KEY_F21 );
   ENUM_KEYBOARD_KEY( KEY_F22 );
   ENUM_KEYBOARD_KEY( KEY_F23 );
   ENUM_KEYBOARD_KEY( KEY_F24 );
   ENUM_KEYBOARD_KEY( KEY_F25 );
   ENUM_KEYBOARD_KEY( KEY_F26 );
   ENUM_KEYBOARD_KEY( KEY_F27 );
   ENUM_KEYBOARD_KEY( KEY_F28 );
   ENUM_KEYBOARD_KEY( KEY_F29 );
   ENUM_KEYBOARD_KEY( KEY_F30 );
   ENUM_KEYBOARD_KEY( KEY_F31 );
   ENUM_KEYBOARD_KEY( KEY_F32 );
   ENUM_KEYBOARD_KEY( KEY_F33 );
   ENUM_KEYBOARD_KEY( KEY_F34 );
   ENUM_KEYBOARD_KEY( KEY_F35 );
   ENUM_KEYBOARD_KEY( KEY_Shift_L );
   ENUM_KEYBOARD_KEY( KEY_Shift_R );
   ENUM_KEYBOARD_KEY( KEY_Control_L );
   ENUM_KEYBOARD_KEY( KEY_Control_R );
   ENUM_KEYBOARD_KEY( KEY_Caps_Lock );
   ENUM_KEYBOARD_KEY( KEY_Shift_Lock );
   ENUM_KEYBOARD_KEY( KEY_Meta_L );
   ENUM_KEYBOARD_KEY( KEY_Meta_R );
   ENUM_KEYBOARD_KEY( KEY_Alt_L );
   ENUM_KEYBOARD_KEY( KEY_Alt_R );
   ENUM_KEYBOARD_KEY( KEY_Super_L );
   ENUM_KEYBOARD_KEY( KEY_Super_R );
   ENUM_KEYBOARD_KEY( KEY_Hyper_L );
   ENUM_KEYBOARD_KEY( KEY_Hyper_R );
   keyboard_key.export_values();
/*
   #define ENUM_KEY_CHARACTER(VALUE) key_character.value(#VALUE, Producer::VALUE)
   enum_<Producer::KeyCharacter> key_character("KeyCharacter");
   ENUM_KEY_CHARACTER(KeyChar_Unknown);
   ENUM_KEY_CHARACTER(KeyChar_space);
   ENUM_KEY_CHARACTER(KeyChar_exclam);
   ENUM_KEY_CHARACTER(KeyChar_quotedbl);
   ENUM_KEY_CHARACTER(KeyChar_numbersign);
   ENUM_KEY_CHARACTER(KeyChar_dollar);
   ENUM_KEY_CHARACTER(KeyChar_percent);
   ENUM_KEY_CHARACTER(KeyChar_ampersand);
   ENUM_KEY_CHARACTER(KeyChar_apostrophe);
   ENUM_KEY_CHARACTER(KeyChar_quoteright);
   ENUM_KEY_CHARACTER(KeyChar_parenleft);
   ENUM_KEY_CHARACTER(KeyChar_parenright);
   ENUM_KEY_CHARACTER(KeyChar_asterisk);
   ENUM_KEY_CHARACTER(KeyChar_plus);
   ENUM_KEY_CHARACTER(KeyChar_comma);
   ENUM_KEY_CHARACTER(KeyChar_minus);
   ENUM_KEY_CHARACTER(KeyChar_period);
   ENUM_KEY_CHARACTER(KeyChar_slash);
   ENUM_KEY_CHARACTER(KeyChar_0);
   ENUM_KEY_CHARACTER(KeyChar_1);
   ENUM_KEY_CHARACTER(KeyChar_2);
   ENUM_KEY_CHARACTER(KeyChar_3);
   ENUM_KEY_CHARACTER(KeyChar_4);
   ENUM_KEY_CHARACTER(KeyChar_5);
   ENUM_KEY_CHARACTER(KeyChar_6);
   ENUM_KEY_CHARACTER(KeyChar_7);
   ENUM_KEY_CHARACTER(KeyChar_8);
   ENUM_KEY_CHARACTER(KeyChar_9);
   ENUM_KEY_CHARACTER(KeyChar_colon);
   ENUM_KEY_CHARACTER(KeyChar_semicolon);
   ENUM_KEY_CHARACTER(KeyChar_less);
   ENUM_KEY_CHARACTER(KeyChar_equal);
   ENUM_KEY_CHARACTER(KeyChar_greater);
   ENUM_KEY_CHARACTER(KeyChar_question);
   ENUM_KEY_CHARACTER(KeyChar_at);
   ENUM_KEY_CHARACTER(KeyChar_A);
   ENUM_KEY_CHARACTER(KeyChar_B);
   ENUM_KEY_CHARACTER(KeyChar_C);
   ENUM_KEY_CHARACTER(KeyChar_D);
   ENUM_KEY_CHARACTER(KeyChar_E);
   ENUM_KEY_CHARACTER(KeyChar_F);
   ENUM_KEY_CHARACTER(KeyChar_G);
   ENUM_KEY_CHARACTER(KeyChar_H);
   ENUM_KEY_CHARACTER(KeyChar_I);
   ENUM_KEY_CHARACTER(KeyChar_J);
   ENUM_KEY_CHARACTER(KeyChar_K);
   ENUM_KEY_CHARACTER(KeyChar_L);
   ENUM_KEY_CHARACTER(KeyChar_M);
   ENUM_KEY_CHARACTER(KeyChar_N);
   ENUM_KEY_CHARACTER(KeyChar_O);
   ENUM_KEY_CHARACTER(KeyChar_P);
   ENUM_KEY_CHARACTER(KeyChar_Q);
   ENUM_KEY_CHARACTER(KeyChar_R);
   ENUM_KEY_CHARACTER(KeyChar_S);
   ENUM_KEY_CHARACTER(KeyChar_T);
   ENUM_KEY_CHARACTER(KeyChar_U);
   ENUM_KEY_CHARACTER(KeyChar_V);
   ENUM_KEY_CHARACTER(KeyChar_W);
   ENUM_KEY_CHARACTER(KeyChar_X);
   ENUM_KEY_CHARACTER(KeyChar_Y);
   ENUM_KEY_CHARACTER(KeyChar_Z);
   ENUM_KEY_CHARACTER(KeyChar_bracketleft);
   ENUM_KEY_CHARACTER(KeyChar_backslash);
   ENUM_KEY_CHARACTER(KeyChar_bracketright);
   ENUM_KEY_CHARACTER(KeyChar_asciicircum);
   ENUM_KEY_CHARACTER(KeyChar_underscore);
   ENUM_KEY_CHARACTER(KeyChar_grave);
   ENUM_KEY_CHARACTER(KeyChar_quoteleft);
   ENUM_KEY_CHARACTER(KeyChar_a);
   ENUM_KEY_CHARACTER(KeyChar_b);
   ENUM_KEY_CHARACTER(KeyChar_c);
   ENUM_KEY_CHARACTER(KeyChar_d);
   ENUM_KEY_CHARACTER(KeyChar_e);
   ENUM_KEY_CHARACTER(KeyChar_f);
   ENUM_KEY_CHARACTER(KeyChar_g);
   ENUM_KEY_CHARACTER(KeyChar_h);
   ENUM_KEY_CHARACTER(KeyChar_i);
   ENUM_KEY_CHARACTER(KeyChar_j);
   ENUM_KEY_CHARACTER(KeyChar_k);
   ENUM_KEY_CHARACTER(KeyChar_l);
   ENUM_KEY_CHARACTER(KeyChar_m);
   ENUM_KEY_CHARACTER(KeyChar_n);
   ENUM_KEY_CHARACTER(KeyChar_o);
   ENUM_KEY_CHARACTER(KeyChar_p);
   ENUM_KEY_CHARACTER(KeyChar_q);
   ENUM_KEY_CHARACTER(KeyChar_r);
   ENUM_KEY_CHARACTER(KeyChar_s);
   ENUM_KEY_CHARACTER(KeyChar_t);
   ENUM_KEY_CHARACTER(KeyChar_u);
   ENUM_KEY_CHARACTER(KeyChar_v);
   ENUM_KEY_CHARACTER(KeyChar_w);
   ENUM_KEY_CHARACTER(KeyChar_x);
   ENUM_KEY_CHARACTER(KeyChar_y);
   ENUM_KEY_CHARACTER(KeyChar_z);
   ENUM_KEY_CHARACTER(KeyChar_braceleft);
   ENUM_KEY_CHARACTER(KeyChar_bar);
   ENUM_KEY_CHARACTER(KeyChar_braceright);
   ENUM_KEY_CHARACTER(KeyChar_asciitilde);
   ENUM_KEY_CHARACTER(KeyChar_nobreakspace);
   ENUM_KEY_CHARACTER(KeyChar_exclamdown);
   ENUM_KEY_CHARACTER(KeyChar_cent);
   ENUM_KEY_CHARACTER(KeyChar_sterling);
   ENUM_KEY_CHARACTER(KeyChar_currency);
   ENUM_KEY_CHARACTER(KeyChar_yen);
   ENUM_KEY_CHARACTER(KeyChar_brokenbar);
   ENUM_KEY_CHARACTER(KeyChar_section);
   ENUM_KEY_CHARACTER(KeyChar_diaeresis);
   ENUM_KEY_CHARACTER(KeyChar_copyright);
   ENUM_KEY_CHARACTER(KeyChar_ordfeminine);
   ENUM_KEY_CHARACTER(KeyChar_guillemotleft);
   ENUM_KEY_CHARACTER(KeyChar_notsign);
   ENUM_KEY_CHARACTER(KeyChar_hyphen);
   ENUM_KEY_CHARACTER(KeyChar_registered);
   ENUM_KEY_CHARACTER(KeyChar_macron);
   ENUM_KEY_CHARACTER(KeyChar_degree);
   ENUM_KEY_CHARACTER(KeyChar_plusminus);
   ENUM_KEY_CHARACTER(KeyChar_twosuperior);
   ENUM_KEY_CHARACTER(KeyChar_threesuperior);
   ENUM_KEY_CHARACTER(KeyChar_acute);
   ENUM_KEY_CHARACTER(KeyChar_mu);
   ENUM_KEY_CHARACTER(KeyChar_paragraph);
   ENUM_KEY_CHARACTER(KeyChar_periodcentered);
   ENUM_KEY_CHARACTER(KeyChar_cedilla);
   ENUM_KEY_CHARACTER(KeyChar_onesuperior);
   ENUM_KEY_CHARACTER(KeyChar_masculine);
   ENUM_KEY_CHARACTER(KeyChar_guillemotright);
   ENUM_KEY_CHARACTER(KeyChar_onequarter);
   ENUM_KEY_CHARACTER(KeyChar_onehalf);
   ENUM_KEY_CHARACTER(KeyChar_threequarters);
   ENUM_KEY_CHARACTER(KeyChar_questiondown);
   ENUM_KEY_CHARACTER(KeyChar_Agrave);
   ENUM_KEY_CHARACTER(KeyChar_Aacute);
   ENUM_KEY_CHARACTER(KeyChar_Acircumflex);
   ENUM_KEY_CHARACTER(KeyChar_Atilde);
   ENUM_KEY_CHARACTER(KeyChar_Adiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_Aring);
   ENUM_KEY_CHARACTER(KeyChar_AE);
   ENUM_KEY_CHARACTER(KeyChar_Ccedilla);
   ENUM_KEY_CHARACTER(KeyChar_Egrave);
   ENUM_KEY_CHARACTER(KeyChar_Eacute);
   ENUM_KEY_CHARACTER(KeyChar_Ecircumflex);
   ENUM_KEY_CHARACTER(KeyChar_Ediaeresis);
   ENUM_KEY_CHARACTER(KeyChar_Igrave);
   ENUM_KEY_CHARACTER(KeyChar_Iacute);
   ENUM_KEY_CHARACTER(KeyChar_Icircumflex);
   ENUM_KEY_CHARACTER(KeyChar_Idiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_ETH);
   ENUM_KEY_CHARACTER(KeyChar_Eth);
   ENUM_KEY_CHARACTER(KeyChar_Ntilde);
   ENUM_KEY_CHARACTER(KeyChar_Ograve);
   ENUM_KEY_CHARACTER(KeyChar_Oacute);
   ENUM_KEY_CHARACTER(KeyChar_Ocircumflex);
   ENUM_KEY_CHARACTER(KeyChar_Otilde);
   ENUM_KEY_CHARACTER(KeyChar_Odiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_multiply);
   ENUM_KEY_CHARACTER(KeyChar_Ooblique);
   ENUM_KEY_CHARACTER(KeyChar_Ugrave);
   ENUM_KEY_CHARACTER(KeyChar_Uacute);
   ENUM_KEY_CHARACTER(KeyChar_Ucircumflex);
   ENUM_KEY_CHARACTER(KeyChar_Udiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_Yacute);
   ENUM_KEY_CHARACTER(KeyChar_THORN);
   ENUM_KEY_CHARACTER(KeyChar_Thorn);
   ENUM_KEY_CHARACTER(KeyChar_ssharp);
   ENUM_KEY_CHARACTER(KeyChar_agrave);
   ENUM_KEY_CHARACTER(KeyChar_aacute);
   ENUM_KEY_CHARACTER(KeyChar_acircumflex);
   ENUM_KEY_CHARACTER(KeyChar_atilde);
   ENUM_KEY_CHARACTER(KeyChar_adiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_aring);
   ENUM_KEY_CHARACTER(KeyChar_ae);
   ENUM_KEY_CHARACTER(KeyChar_ccedilla);
   ENUM_KEY_CHARACTER(KeyChar_egrave);
   ENUM_KEY_CHARACTER(KeyChar_eacute);
   ENUM_KEY_CHARACTER(KeyChar_ecircumflex);
   ENUM_KEY_CHARACTER(KeyChar_ediaeresis);
   ENUM_KEY_CHARACTER(KeyChar_igrave);
   ENUM_KEY_CHARACTER(KeyChar_iacute);
   ENUM_KEY_CHARACTER(KeyChar_icircumflex);
   ENUM_KEY_CHARACTER(KeyChar_idiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_eth);
   ENUM_KEY_CHARACTER(KeyChar_ntilde);
   ENUM_KEY_CHARACTER(KeyChar_ograve);
   ENUM_KEY_CHARACTER(KeyChar_oacute);
   ENUM_KEY_CHARACTER(KeyChar_ocircumflex);
   ENUM_KEY_CHARACTER(KeyChar_otilde);
   ENUM_KEY_CHARACTER(KeyChar_odiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_division);
   ENUM_KEY_CHARACTER(KeyChar_oslash);
   ENUM_KEY_CHARACTER(KeyChar_ugrave);
   ENUM_KEY_CHARACTER(KeyChar_uacute);
   ENUM_KEY_CHARACTER(KeyChar_ucircumflex);
   ENUM_KEY_CHARACTER(KeyChar_udiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_yacute);
   ENUM_KEY_CHARACTER(KeyChar_thorn);
   ENUM_KEY_CHARACTER(KeyChar_ydiaeresis);
   ENUM_KEY_CHARACTER(KeyChar_BackSpace);
   ENUM_KEY_CHARACTER(KeyChar_Tab);
   ENUM_KEY_CHARACTER(KeyChar_Linefeed);
   ENUM_KEY_CHARACTER(KeyChar_Clear);
   ENUM_KEY_CHARACTER(KeyChar_Return);
   ENUM_KEY_CHARACTER(KeyChar_Pause);
   ENUM_KEY_CHARACTER(KeyChar_Scroll_Lock);
   ENUM_KEY_CHARACTER(KeyChar_Sys_Req);
   ENUM_KEY_CHARACTER(KeyChar_Escape);
   ENUM_KEY_CHARACTER(KeyChar_Multi_key);
   ENUM_KEY_CHARACTER(KeyChar_Kanji);
   ENUM_KEY_CHARACTER(KeyChar_Muhenkan);
   ENUM_KEY_CHARACTER(KeyChar_Henkan_Mode);
   ENUM_KEY_CHARACTER(KeyChar_Henkan);
   ENUM_KEY_CHARACTER(KeyChar_Romaji);
   ENUM_KEY_CHARACTER(KeyChar_Hiragana);
   ENUM_KEY_CHARACTER(KeyChar_Katakana);
   ENUM_KEY_CHARACTER(KeyChar_Hiragana_Katakana);
   ENUM_KEY_CHARACTER(KeyChar_Zenkaku);
   ENUM_KEY_CHARACTER(KeyChar_Hankaku);
   ENUM_KEY_CHARACTER(KeyChar_Zenkaku_Hankaku);
   ENUM_KEY_CHARACTER(KeyChar_Touroku);
   ENUM_KEY_CHARACTER(KeyChar_Massyo);
   ENUM_KEY_CHARACTER(KeyChar_Kana_Lock);
   ENUM_KEY_CHARACTER(KeyChar_Kana_Shift);
   ENUM_KEY_CHARACTER(KeyChar_Eisu_Shift);
   ENUM_KEY_CHARACTER(KeyChar_Eisu_toggle);
   ENUM_KEY_CHARACTER(KeyChar_Codeinput);
   ENUM_KEY_CHARACTER(KeyChar_Kanji_Bangou);
   ENUM_KEY_CHARACTER(KeyChar_SingleCandidate);
   ENUM_KEY_CHARACTER(KeyChar_MultipleCandidate);
   ENUM_KEY_CHARACTER(KeyChar_Zen_Koho);
   ENUM_KEY_CHARACTER(KeyChar_PreviousCandidate);
   ENUM_KEY_CHARACTER(KeyChar_Mae_Koho);
   ENUM_KEY_CHARACTER(KeyChar_Home);
   ENUM_KEY_CHARACTER(KeyChar_Left);
   ENUM_KEY_CHARACTER(KeyChar_Up);
   ENUM_KEY_CHARACTER(KeyChar_Right);
   ENUM_KEY_CHARACTER(KeyChar_Down);
   ENUM_KEY_CHARACTER(KeyChar_Prior);
   ENUM_KEY_CHARACTER(KeyChar_Page_Up);
   ENUM_KEY_CHARACTER(KeyChar_Next);
   ENUM_KEY_CHARACTER(KeyChar_Page_Down);
   ENUM_KEY_CHARACTER(KeyChar_End);
   ENUM_KEY_CHARACTER(KeyChar_Begin);
   ENUM_KEY_CHARACTER(KeyChar_Select);
   ENUM_KEY_CHARACTER(KeyChar_Print);
   ENUM_KEY_CHARACTER(KeyChar_Execute);
   ENUM_KEY_CHARACTER(KeyChar_Insert);
   ENUM_KEY_CHARACTER(KeyChar_Undo);
   ENUM_KEY_CHARACTER(KeyChar_Redo);
   ENUM_KEY_CHARACTER(KeyChar_Menu);
   ENUM_KEY_CHARACTER(KeyChar_Find);
   ENUM_KEY_CHARACTER(KeyChar_Cancel);
   ENUM_KEY_CHARACTER(KeyChar_Help);
   ENUM_KEY_CHARACTER(KeyChar_Break);
   ENUM_KEY_CHARACTER(KeyChar_Mode_switch);
   ENUM_KEY_CHARACTER(KeyChar_script_switch);
   ENUM_KEY_CHARACTER(KeyChar_kana_switch);
   ENUM_KEY_CHARACTER(KeyChar_Arabic_switch);
   ENUM_KEY_CHARACTER(KeyChar_Greek_switch);
   ENUM_KEY_CHARACTER(KeyChar_Hebrew_switch);
   ENUM_KEY_CHARACTER(KeyChar_Hangul_switch);
   ENUM_KEY_CHARACTER(KeyChar_Num_Lock);
   ENUM_KEY_CHARACTER(KeyChar_KP_Space);
   ENUM_KEY_CHARACTER(KeyChar_KP_Tab);
   ENUM_KEY_CHARACTER(KeyChar_KP_Enter);
   ENUM_KEY_CHARACTER(KeyChar_KP_F1);
   ENUM_KEY_CHARACTER(KeyChar_KP_F2);
   ENUM_KEY_CHARACTER(KeyChar_KP_F3);
   ENUM_KEY_CHARACTER(KeyChar_KP_F4);
   ENUM_KEY_CHARACTER(KeyChar_KP_Home);
   ENUM_KEY_CHARACTER(KeyChar_KP_Left);
   ENUM_KEY_CHARACTER(KeyChar_KP_Up);
   ENUM_KEY_CHARACTER(KeyChar_KP_Right);
   ENUM_KEY_CHARACTER(KeyChar_KP_Down);
   ENUM_KEY_CHARACTER(KeyChar_KP_Prior);
   ENUM_KEY_CHARACTER(KeyChar_KP_Page_Up);
   ENUM_KEY_CHARACTER(KeyChar_KP_Next);
   ENUM_KEY_CHARACTER(KeyChar_KP_Page_Down);
   ENUM_KEY_CHARACTER(KeyChar_KP_End);
   ENUM_KEY_CHARACTER(KeyChar_KP_Begin);
   ENUM_KEY_CHARACTER(KeyChar_KP_Insert);
   ENUM_KEY_CHARACTER(KeyChar_KP_Delete);
   ENUM_KEY_CHARACTER(KeyChar_KP_Multiply);
   ENUM_KEY_CHARACTER(KeyChar_KP_Add);
   ENUM_KEY_CHARACTER(KeyChar_KP_Separator);
   ENUM_KEY_CHARACTER(KeyChar_KP_Subtract);
   ENUM_KEY_CHARACTER(KeyChar_KP_Decimal);
   ENUM_KEY_CHARACTER(KeyChar_KP_Divide);
   ENUM_KEY_CHARACTER(KeyChar_KP_0);
   ENUM_KEY_CHARACTER(KeyChar_KP_1);
   ENUM_KEY_CHARACTER(KeyChar_KP_2);
   ENUM_KEY_CHARACTER(KeyChar_KP_3);
   ENUM_KEY_CHARACTER(KeyChar_KP_4);
   ENUM_KEY_CHARACTER(KeyChar_KP_5);
   ENUM_KEY_CHARACTER(KeyChar_KP_6);
   ENUM_KEY_CHARACTER(KeyChar_KP_7);
   ENUM_KEY_CHARACTER(KeyChar_KP_8);
   ENUM_KEY_CHARACTER(KeyChar_KP_9);
   ENUM_KEY_CHARACTER(KeyChar_KP_Equal);
   ENUM_KEY_CHARACTER(KeyChar_F1);
   ENUM_KEY_CHARACTER(KeyChar_F2);
   ENUM_KEY_CHARACTER(KeyChar_F3);
   ENUM_KEY_CHARACTER(KeyChar_F4);
   ENUM_KEY_CHARACTER(KeyChar_F5);
   ENUM_KEY_CHARACTER(KeyChar_F6);
   ENUM_KEY_CHARACTER(KeyChar_F7);
   ENUM_KEY_CHARACTER(KeyChar_F8);
   ENUM_KEY_CHARACTER(KeyChar_F9);
   ENUM_KEY_CHARACTER(KeyChar_F10);
   ENUM_KEY_CHARACTER(KeyChar_F11);
   ENUM_KEY_CHARACTER(KeyChar_L1);
   ENUM_KEY_CHARACTER(KeyChar_F12);
   ENUM_KEY_CHARACTER(KeyChar_L2);
   ENUM_KEY_CHARACTER(KeyChar_F13);
   ENUM_KEY_CHARACTER(KeyChar_L3);
   ENUM_KEY_CHARACTER(KeyChar_F14);
   ENUM_KEY_CHARACTER(KeyChar_L4);
   ENUM_KEY_CHARACTER(KeyChar_F15);
   ENUM_KEY_CHARACTER(KeyChar_L5);
   ENUM_KEY_CHARACTER(KeyChar_F16);
   ENUM_KEY_CHARACTER(KeyChar_L6);
   ENUM_KEY_CHARACTER(KeyChar_F17);
   ENUM_KEY_CHARACTER(KeyChar_L7);
   ENUM_KEY_CHARACTER(KeyChar_F18);
   ENUM_KEY_CHARACTER(KeyChar_L8);
   ENUM_KEY_CHARACTER(KeyChar_F19);
   ENUM_KEY_CHARACTER(KeyChar_L9);
   ENUM_KEY_CHARACTER(KeyChar_F20);
   ENUM_KEY_CHARACTER(KeyChar_L10);
   ENUM_KEY_CHARACTER(KeyChar_F21);
   ENUM_KEY_CHARACTER(KeyChar_R1);
   ENUM_KEY_CHARACTER(KeyChar_F22);
   ENUM_KEY_CHARACTER(KeyChar_R2);
   ENUM_KEY_CHARACTER(KeyChar_F23);
   ENUM_KEY_CHARACTER(KeyChar_R3);
   ENUM_KEY_CHARACTER(KeyChar_F24);
   ENUM_KEY_CHARACTER(KeyChar_R4);
   ENUM_KEY_CHARACTER(KeyChar_F25);
   ENUM_KEY_CHARACTER(KeyChar_R5);
   ENUM_KEY_CHARACTER(KeyChar_F26);
   ENUM_KEY_CHARACTER(KeyChar_R6);
   ENUM_KEY_CHARACTER(KeyChar_F27);
   ENUM_KEY_CHARACTER(KeyChar_R7);
   ENUM_KEY_CHARACTER(KeyChar_F28);
   ENUM_KEY_CHARACTER(KeyChar_R8);
   ENUM_KEY_CHARACTER(KeyChar_F29);
   ENUM_KEY_CHARACTER(KeyChar_R9);
   ENUM_KEY_CHARACTER(KeyChar_F30);
   ENUM_KEY_CHARACTER(KeyChar_R10);
   ENUM_KEY_CHARACTER(KeyChar_F31);
   ENUM_KEY_CHARACTER(KeyChar_R11);
   ENUM_KEY_CHARACTER(KeyChar_F32);
   ENUM_KEY_CHARACTER(KeyChar_R12);
   ENUM_KEY_CHARACTER(KeyChar_F33);
   ENUM_KEY_CHARACTER(KeyChar_R13);
   ENUM_KEY_CHARACTER(KeyChar_F34);
   ENUM_KEY_CHARACTER(KeyChar_R14);
   ENUM_KEY_CHARACTER(KeyChar_F35);
   ENUM_KEY_CHARACTER(KeyChar_R15);
   ENUM_KEY_CHARACTER(KeyChar_Shift_L);
   ENUM_KEY_CHARACTER(KeyChar_Shift_R);
   ENUM_KEY_CHARACTER(KeyChar_Control_L);
   ENUM_KEY_CHARACTER(KeyChar_Control_R);
   ENUM_KEY_CHARACTER(KeyChar_Caps_Lock);
   ENUM_KEY_CHARACTER(KeyChar_Shift_Lock);
   ENUM_KEY_CHARACTER(KeyChar_Meta_L);
   ENUM_KEY_CHARACTER(KeyChar_Meta_R);
   ENUM_KEY_CHARACTER(KeyChar_Alt_L);
   ENUM_KEY_CHARACTER(KeyChar_Alt_R);
   ENUM_KEY_CHARACTER(KeyChar_Super_L);
   ENUM_KEY_CHARACTER(KeyChar_Super_R);
   ENUM_KEY_CHARACTER(KeyChar_Hyper_L);
   ENUM_KEY_CHARACTER(KeyChar_Hyper_R);
   ENUM_KEY_CHARACTER(KeyChar_Delete);
   key_character.export_values();

   #define ENUM_KEY_MODIFIER(VALUE) key_modifier.value(#VALUE, GUIEventAdapter::VALUE)
   enum_<Producer::KeyModifier> key_modifier("KeyModifier");
   ENUM_KEY_MODIFIER(KeyMod_NoModifier);
   ENUM_KEY_MODIFIER(KeyMod_Shift);
   ENUM_KEY_MODIFIER(KeyMod_CapsLock);
   ENUM_KEY_MODIFIER(KeyMod_Control);
   ENUM_KEY_MODIFIER(KeyMod_NumLock);
   ENUM_KEY_MODIFIER(KeyMod_Alt);
   ENUM_KEY_MODIFIER(KeyMod_Super);
   key_modifier.export_values();
*/   
   Keyboard* (*KeyboardGI1)(int) = &Keyboard::GetInstance;
   Keyboard* (*KeyboardGI2)(std::string) = &Keyboard::GetInstance;

   class_<KeyboardListenerWrap, dtCore::RefPtr<KeyboardListenerWrap>, boost::noncopyable>("KeyboardListener")
       .def("HandleKeyPressed", pure_virtual(&KeyboardListener::HandleKeyPressed))
       .def("HandleKeyReleased", pure_virtual(&KeyboardListener::HandleKeyReleased))
       .def("HandleKeyTyped", pure_virtual(&KeyboardListener::HandleKeyTyped))
       ;

   class_<Keyboard, bases<InputDevice>, dtCore::RefPtr<Keyboard>, boost::noncopyable >("Keyboard", init< optional<const std::string&> >())
      .def("GetInstanceCount", &Keyboard::GetInstanceCount)
      .staticmethod("GetInstanceCount")
      .def("GetInstance", KeyboardGI1, return_internal_reference<>())
      .def("GetInstance", KeyboardGI2, return_internal_reference<>())
      .staticmethod("GetInstance")
      .def("GetKeyState", &Keyboard::GetKeyState)
      .def("AddKeyboardListener", &Keyboard::AddKeyboardListener, with_custodian_and_ward<1,2>() )
      .def("RemoveKeyboardListener", &Keyboard::RemoveKeyboardListener);
}
