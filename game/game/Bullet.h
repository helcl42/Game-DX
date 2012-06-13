#ifndef BULLET_H
#define BULLET_H


//-----------------------------------
// Bullet
//-----------------------------------
class Bullet
{
private:
	SceneObject*				m_owner;
	RayIntersectionResult*	m_hitResult;
	float					m_totalDistance;
	bool						m_expired;

	D3DXVECTOR3				m_translation;
	D3DXVECTOR3				m_direction;
	float					m_velocity;
	float					m_range;
	float					m_damage;

public:
	Bullet(SceneObject* owner, D3DXVECTOR3 translation, D3DXVECTOR3 direction, float velocity, float range, float damage);
	virtual ~Bullet();

	void Update(float elapsed);
	bool IsExpired();
};



//-----------------------------------
//  Bullet Manager
//-----------------------------------
class BulletManager
{
private:
	LinkedList<Bullet>* m_bullets;

public:
	BulletManager();
	virtual ~BulletManager();

	void Update(float elapsed);
	void AddBullet(SceneObject* owner, D3DXVECTOR3 translation, D3DXVECTOR3 direction, float velocity, float range, float damage);
};

#endif

