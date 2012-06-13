#ifndef MAIN_H
#define MAIN_H


#include <windows.h>
#include "Engine.h"


#include "GameMaterial.h"
#include "Bullet.h"
#include "Weapon.h"
#include "PlayerObject.h"
#include "PlayerManager.h"
#include "Menu.h"
#include "Game.h"


struct PlayerData
{
	char character[MAX_PATH];
	char map[MAX_PATH];
};

#endif