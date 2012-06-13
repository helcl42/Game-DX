#include "Main.h"

GameMaterial::GameMaterial() : Material()
{
	m_stepSounds = NULL;
}


GameMaterial::GameMaterial(char* name, char* path) : Material(name, path)
{
	Script* script = new Script(name, path);

	m_stepSounds = new LinkedList<Sound>();
	char stepSound[16] = { "step_sound0" };

	while(script->GetStringData(stepSound) != NULL)
	{
		m_stepSounds->Add(new Sound(script->GetStringData(stepSound)));
		sprintf(stepSound, "step_sound%d", m_stepSounds->GetTotalElements());
	}

	SAFE_DELETE(script);
}


GameMaterial::~GameMaterial()
{
	SAFE_DELETE(m_stepSounds);
}


Sound* GameMaterial::GetStepSound()
{
	if(m_stepSounds == NULL)
		return NULL;

	return m_stepSounds->GetRandom();
}