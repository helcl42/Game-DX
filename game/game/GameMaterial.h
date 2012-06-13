#ifndef GAME_METERIAL_H
#define GAME_MATERIAL_H



class GameMaterial : public Material
{
private:
	LinkedList<Sound>* m_stepSounds;

public:
	GameMaterial();
	GameMaterial(char* name, char* path = "./");
	virtual ~GameMaterial();

	Sound* GetStepSound();
};

#endif