//-----------------------------------------------------------------------------
// Material Class
//-----------------------------------------------------------------------------

#ifndef MATERIAL_H
#define MATERIAL_H


class Material : public Resource< Material >
{
private:
	IDirect3DTexture9* m_texture;
	D3DMATERIAL9	   m_lighting;
	unsigned long	   m_width;
	unsigned long	   m_height;
	bool		   m_ignoreFace;
	bool		   m_ignoreFog;
	bool		   m_ignoreRay;

public:
	Material();
	Material(char* name, char* path = "./");
	virtual ~Material();

	IDirect3DTexture9* GetTexture();
	D3DMATERIAL9*	   GetLighting();
	unsigned long	   GetWidth();
	unsigned long	   GetHeight();
	bool		   GetIgnoreFace();
	bool		   GetIgnoreFog();
	bool		   GetIgnoreRay();
};

#endif
