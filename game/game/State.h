#ifndef STATE_H
#define STATE_H


struct ViewerSetup
{
	SceneObject*  viewer; // Current viewing object.
	unsigned long viewClearFlags; // Flags used for clearing the view.

	ViewerSetup()
	{
		viewer = NULL;
		viewClearFlags = 0;
	};
};


class State
{

private:
	unsigned long m_id; // Application defined ID (must be unique for state switching).

public:
	State( unsigned long id = 0 );

	virtual void Load();
	virtual void Close();

	virtual void RequestViewer( ViewerSetup *viewer );
	virtual void Update( float elapsed );
	virtual void Render();

	unsigned long GetID();
};

#endif