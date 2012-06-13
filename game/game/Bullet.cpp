#include "Main.h"


Bullet::Bullet(SceneObject* owner, D3DXVECTOR3 translation, D3DXVECTOR3 direction, float velocity, float range, float damage)
{
	m_owner = owner;
	m_hitResult = new RayIntersectionResult();
	m_totalDistance = 0.0f;
	m_expired = false;

	m_translation = translation;
	m_direction = direction;
	m_velocity = velocity;
	m_range = range;
	m_damage = damage;
}


Bullet::~Bullet()
{
	SAFE_DELETE(m_hitResult);
}


void Bullet::Update(float elapsed)
{
	m_hitResult->material = NULL;
	m_hitResult->distance = 0.0f;
	m_hitResult->hitObject = NULL;

	if(Engine::GetInstance()->GetSceneManager()->RayIntersectScene(m_hitResult, m_translation, m_direction, true, m_owner, true))
	{
		if(m_hitResult->distance < m_velocity * elapsed)
		{
			m_totalDistance += m_hitResult->distance;
			if(m_totalDistance > m_range)
			{
				m_hitResult->material = NULL;
				m_hitResult->distance = 0.0f;
				m_hitResult->hitObject = NULL;

				m_expired = true;
				return;
			}

			m_expired = true;

			if(! Engine::GetInstance()->GetNetwork()->IsHost())
				return;

			if(m_hitResult->hitObject == NULL)
				return;

			if(m_hitResult->hitObject->GetType() == TYPE_PLAYER_OBJECT)
				((PlayerObject*)m_hitResult->hitObject)->Hurt(m_damage, (PlayerObject*)m_owner);

			return;
		}
		else
		{
			m_hitResult->material = NULL;
			m_hitResult->distance = 0.0f;
			m_hitResult->hitObject = NULL;
		}
	}

	m_totalDistance += m_velocity * elapsed;
	if(m_totalDistance > m_range)
	{
		m_expired = true;
		return;
	}
	m_translation += m_direction * (m_velocity * elapsed);
}


bool Bullet::IsExpired()
{
	return m_expired;
}


//-------------------------------------------------
//	BulletManager Class
//-------------------------------------------------

BulletManager::BulletManager()
{
	m_bullets = new LinkedList<Bullet>();
}


BulletManager::~BulletManager()
{
	SAFE_DELETE(m_bullets);
}


void BulletManager::Update(float elapsed)
{
	Bullet* remove = NULL;
	Bullet* bullet = m_bullets->GetFirst();

	while(bullet != NULL)
	{
		if(bullet->IsExpired())
			remove = bullet;
		else
			bullet->Update(elapsed);

		bullet = m_bullets->GetNext(bullet);

		if(remove != NULL)
			m_bullets->Remove(&remove);
	}
}


void BulletManager::AddBullet(SceneObject* owner, D3DXVECTOR3 translation, D3DXVECTOR3 direction, float velocity, float range, float damage)
{
	m_bullets->Add(new Bullet(owner, translation, direction, velocity, range, damage));
}

