#include "Engine.h"


SpawnerObject::SpawnerObject(char* name, char* path, unsigned long type) : SceneObject(type)
{
	SetGhost(true);
	Script* script = new Script(name, path);

	m_frequency = *script->GetFloatData("frequency");

	m_spawnTimer = 0.0f;

	if(script->GetStringData("sound") != NULL)
	{
		m_sound = new Sound(script->GetStringData("sound"));
		m_audioPath = new AudioPath3D();
	}
	else
	{
		m_sound = NULL;
		m_audioPath = NULL;
	}

	m_objectScript = Engine::GetInstance()->GetScriptManager()->Add(script->GetStringData("object"), script->GetStringData("object_path"));

	m_name = new char[strlen(m_objectScript->GetStringData("name")) + 1];
	strcpy(m_name, m_objectScript->GetStringData("name"));	

	SetMesh(m_objectScript->GetStringData("mesh"), m_objectScript->GetStringData("mesh_path"));
	SetSpin(0.0f, 1.0f, 0.0f);

	if(*script->GetFloatData("radius") != 0.0f)
		SetBoundingSphere(D3DXVECTOR3(0.0f, 0.0f, 0.0f), *script->GetFloatData("radius"));
	else
		SetEllipsoidRadius(*m_objectScript->GetVectorData("ellipse_radius"));

	SAFE_DELETE(script);
}


SpawnerObject::~SpawnerObject()
{
	SAFE_DELETE_ARRAY(m_name);

	SAFE_DELETE(m_sound);
	SAFE_DELETE(m_audioPath);

	Engine::GetInstance()->GetScriptManager()->Remove(&m_objectScript);
}


void SpawnerObject::Update(float elapsed, bool addVelocity)
{
	SceneObject::Update(elapsed, addVelocity);

	if(!GetVisible())
	{
		m_spawnTimer += elapsed;
		if(m_spawnTimer >= m_frequency)
		{
			SetVisible(true);
			SetIgnoreCollisions(false);
			m_spawnTimer = 0.0f;
		}
	}

	if(m_audioPath)
	{
		m_audioPath->SetPosition(GetTranslation());
		m_audioPath->SetVelocity(GetVelocity());
	}
}


void SpawnerObject::CollisionOccourred(SceneObject* object, unsigned long collisionStamp)
{
	SceneObject::CollisionOccurred(object, collisionStamp);

	if(object->GetType() == TYPE_SCENE_OBJECT || object->GetType() == TYPE_ANIMATED_OBJECT || object->GetType() == TYPE_SPAWNER_OBJECT)
		return;

	SetVisible(true);
	SetIgnoreCollisions(true);

	if(m_audioPath != NULL && m_sound != NULL)
		m_audioPath->Play(m_sound->GetSegment());
}


Script* SpawnerObject::GetObjectScript()
{
	return m_objectScript;
}

