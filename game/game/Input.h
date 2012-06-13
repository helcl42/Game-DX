#ifndef INPUT_H
#define INPUT_H

//-----------------------------------------------------------------------------
// Input Class
//-----------------------------------------------------------------------------
class Input
{
public:
	Input( HWND window );
	virtual ~Input();

	void Update();

	bool GetKeyPress( char key, bool ignorePressStamp = false );
	bool GetButtonPress( char button, bool ignorePressStamp = false );

	long GetPosX();
	long GetPosY();
	long GetDeltaX();
	long GetDeltaY();
	long GetDeltaWheel();

private:
	HWND	       m_window;		   // Handle of the parent window.
	IDirectInput8* m_di;			   // DirectInput object.
	unsigned long  m_pressStamp; 		   // Current press stamp (incremented every frame).

	IDirectInputDevice8* m_keyboard;	   // DirectInput keyboard device.
	char		     m_keyState[256];	   // Stores the state of the keyboard keys.
	unsigned long	     m_keyPressStamp[256]; // Stamps the last frame each key was preseed.

	IDirectInputDevice8* m_mouse;		    // DirectInput mouse device.
	DIMOUSESTATE	     m_mouseState;	    // Stores the state of the mouse buttons.
	unsigned long	     m_buttonPressStamp[3]; // Stamps the last frame each button was preseed.
	POINT		     m_position;	    // Stores the position of the mouse cursor on the screen.
};

#endif