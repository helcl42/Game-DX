#ifndef VIEW_FRUSTRUM_H
#define VIEW_FRUSTRUM_H


class ViewFrustum
{
private:
	D3DXMATRIX	m_projection;
	D3DXPLANE	m_planes[5];

public:
	void Update(D3DXMATRIX* view);

	void SetProjectionMatrix(D3DXMATRIX projection);

	bool IsBoxInside(D3DXVECTOR3 min, D3DXVECTOR3 max);
	bool IsBoxInside(D3DXVECTOR3 translation, D3DXVECTOR3 min, D3DXVECTOR3 max);
	bool IsSphereInside(D3DXVECTOR3 translation, float radius);
};

#endif