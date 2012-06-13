#include "Engine.h"


Material::Material() : Resource<Material>("noname", "nopath")
{
	m_texture = NULL;
	m_width = 0;
	m_height = 0;

	m_lighting.Diffuse = D3DXCOLOR(1.0f, 1.01f, 1.0f, 1.0f);
	m_lighting.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_lighting.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_lighting.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_lighting.Power = 0;

	m_ignoreFace = false;
	m_ignoreFog = false;
	m_ignoreRay = false;
}


Material::Material(char* name, char* path) : Resource<Material>(name, path)
{
	D3DXIMAGE_INFO info;
	Script* script = new Script(name, path);

	if( script->GetColourData( "transparency" )->a == 0.0f )
	{
		// Load the texture without transparency.
		D3DXCreateTextureFromFileEx( Engine::GetInstance()->GetDevice(), script->GetStringData( "texture" ), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE, 0, &info, NULL, &m_texture);
	}
	else
	{
		// Load the texture using a transparency colour value.
		D3DCOLORVALUE *colour = script->GetColourData( "transparency" );
		D3DCOLOR transparency = D3DCOLOR_COLORVALUE( colour->r, colour->g, colour->b, colour->a );
		D3DXCreateTextureFromFileEx( Engine::GetInstance()->GetDevice(), script->GetStringData( "texture" ), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE, transparency, &info, NULL, &m_texture);
	}

	m_width = info.Width;
	m_height = info.Height;

	m_lighting.Diffuse = *script->GetColourData("diffuse");
	m_lighting.Ambient = *script->GetColourData("ambient");
	m_lighting.Specular = *script->GetColourData("specular");
	m_lighting.Power = *script->GetFloatData("power");

	m_ignoreFace = *script->GetBoolData("ignore_face");
	m_ignoreFog = *script->GetBoolData("ignore_fog");
	m_ignoreRay = *script->GetBoolData("ignore_ray");

	SAFE_DELETE(script);
}


Material::~Material()
{
	SAFE_RELEASE(m_texture);
}


IDirect3DTexture9* Material::GetTexture()
{
	return m_texture;
}


D3DMATERIAL9* Material::GetLighting()
{
	return &m_lighting;
}


unsigned long Material::GetWidth()
{
	return m_width;
}


unsigned long Material::GetHeight()
{
	return m_height;
}


bool Material::GetIgnoreFace()
{
	return m_ignoreFace;
}


bool Material::GetIgnoreFog()
{
	return m_ignoreFog;
}


bool Material::GetIgnoreRay()
{
	return m_ignoreRay;
}

