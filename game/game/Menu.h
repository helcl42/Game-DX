#ifndef MENU_H
#define MENU_H

#define STATE_MENU 0

class Menu : public State
{
private:
	static Menu* m_instance;

public:
	Menu();

	static Menu* GetInstance();

	virtual void Update(float elapsed);
};


#endif