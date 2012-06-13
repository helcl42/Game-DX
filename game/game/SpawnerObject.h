#ifndef SPAWNER_OBJECT_H
#define SPAWNER_OBJECT_H

#define TYPE_SPAWNER_OBJECT 2

class SpawnerObject : public SceneObject
{
private:
	char*		 m_name;
	float		 m_frequency;
	float		 m_spawnTimer;
	Sound*		 m_sound;
	AudioPath3D* 	 m_audioPath;
	Script*		 m_objectScript;

public:
	SpawnerObject(char* name, char* path = "./", unsigned long type = TYPE_SPAWNER_OBJECT);
	virtual ~SpawnerObject();

	virtual void Update(float elapsed, bool addVelovity = true);

	virtual void CollisionOccourred(SceneObject* object, unsigned long collisionStamp);

	Script* GetObjectScript();
};


#endif