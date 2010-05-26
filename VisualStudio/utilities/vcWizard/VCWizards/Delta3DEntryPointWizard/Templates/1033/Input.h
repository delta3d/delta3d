#ifndef [!output PROJECT_NAME]Input_h__
#define [!output PROJECT_NAME]Input_h__

#include "EntryPointExport.h"

#include <dtGame/baseinputcomponent.h>

class ENTRY_POINT_EXPORT [!output PROJECT_NAME]Input : public dtGame::BaseInputComponent
{
public:
	[!output PROJECT_NAME]Input();

   /**
   * KeyboardListener call back- Called when a key is pressed.
   * Override this if you want to handle this listener event.
   * Default handles the Escape key to quit.
   *
   * @param keyboard the source of the event
   * @param key the key pressed
   * @param character the corresponding character
   */
   virtual bool HandleKeyPressed(const dtCore::Keyboard* keyboard, int key);

   /**
   * KeyboardListener call back - Called when a key is released.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param keyboard the source of the event
   * @param key the key released
   * @param character the corresponding character
   * @return true if this KeyboardListener handled the event. The
   * Keyboard calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleKeyReleased(const dtCore::Keyboard* keyboard, int key);

   /**
   * KeyboardListener call back - Called when a key is typed.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param keyboard the source of the event
   * @param key the key typed
   * @param character the corresponding character
   * @return true if this KeyboardListener handled the event. The
   * Keyboard calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleKeyTyped(const dtCore::Keyboard* keyboard, int key);

   /**
   * MouseListener call back - Called when a button is pressed.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param mouse the source of the event
   * @param button the button pressed
   * @return true if this MouseListener handled the event. The
   * Mouse calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);

   /**
   * MouseListener call back - Called when a button is released.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param mouse the source of the event
   * @param button the button released
   * @return true if this MouseListener handled the event. The
   * Mouse calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleButtonReleased(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button);

   /**
   * MouseListener call back - Called when a button is clicked.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param mouse the source of the event
   * @param button the button clicked
   * @param clickCount the click count
   * @return true if this MouseListener handled the event. The
   * Mouse calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleButtonClicked(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button, int clickCount);

   /**
   * MouseListener call back - Called when the mouse pointer is moved.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param mouse the source of the event
   * @param x the x coordinate
   * @param y the y coordinate
   * @return true if this MouseListener handled the event. The
   * Mouse calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleMouseMoved(const dtCore::Mouse* mouse, float x, float y);

   /**
   * MouseListener call back - Called when the mouse pointer is dragged.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param mouse the source of the event
   * @param x the x coordinate
   * @param y the y coordinate
   * @return true if this MouseListener handled the event. The
   * Mouse calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleMouseDragged(const dtCore::Mouse* mouse, float x, float y);

   /**
   * MouseListener call back - Called when the mouse pointer is scrolled.
   * Override this if you want to handle this listener event.
   * Default does nothing.
   *
   * @param mouse the source of the event
   * @param delta the scroll delta (+1 for up one, -1 for down one)
   * @return true if this MouseListener handled the event. The
   * Mouse calling this function is responsible for using this
   * return value or not.
   */
   virtual bool HandleMouseScrolled(const dtCore::Mouse* mouse, int delta);

protected:
   virtual ~[!output PROJECT_NAME]Input();
	
private:
};
#endif // [!output PROJECT_NAME]Input_h__