#include "Engine.h"


SoundSystem::SoundSystem(float scale)
{
	CoCreateInstance(CLSID_DirectMusicLoader, NULL , CLSCTX_INPROC, IID_IDirectMusicLoader8, (void**)&m_loader);
	CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&m_performance);
	m_performance->InitAudio(NULL, NULL, NULL, DMUS_APATH_SHARED_STEREOPLUSREVERB, 8, DMUS_AUDIOF_ALL, NULL);

	IDirectMusicAudioPath8* audioPath3D = NULL;
	m_performance->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D, 1, true, &audioPath3D);

	if(audioPath3D != NULL)
	{
		audioPath3D->GetObjectInPath(0, DMUS_PATH_PRIMARY_BUFFER, 0, GUID_All_Objects, 0, IID_IDirectSound3DListener, (void**)&m_listener);
		SAFE_RELEASE(audioPath3D);
		
		m_scale = scale;		
		m_listener->SetDistanceFactor(m_scale, DS3D_IMMEDIATE);
	}
	else
	{
		m_listener = NULL;
	}
}


SoundSystem::~SoundSystem()
{
	m_performance->CloseDown();
	SAFE_RELEASE(m_performance);
	SAFE_RELEASE(m_loader);
}


void SoundSystem::UpdateListener(D3DXVECTOR3 forward, D3DXVECTOR3 position, D3DXVECTOR3 velocity)
{
	if(m_listener == NULL)
		return;

	m_listener->SetOrientation(forward.x, forward.y, forward.z, 0.0f, 1.0f, 0.0f, DS3D_DEFERRED);

	position *= m_scale;
	m_listener->SetPosition(position.x, position.y, position.z, DS3D_DEFERRED);

	velocity *= m_scale;
	m_listener->SetVelocity(velocity.x, velocity.y, velocity.z, DS3D_DEFERRED);

	m_listener->CommitDeferredSettings();
}


void SoundSystem::GarbageCollection()
{
	m_loader->CollectGarbage();
}


void SoundSystem::SetVolume(long volume)
{
	m_performance->SetGlobalParam(GUID_PerfMasterVolume, &volume, sizeof(long));
}


IDirectMusicLoader8* SoundSystem::GetLoader()
{
	return m_loader;
}


IDirectMusicPerformance8* SoundSystem::GetPerformance()
{
	return m_performance;
}


//Sound class
Sound::Sound(char* filename)
{
	WCHAR* wideFilename = new WCHAR[strlen(filename) + 1];
	MultiByteToWideChar(CP_ACP, 0, filename, -1, wideFilename, strlen(filename) + 1);
	wideFilename[strlen(filename)] = 0;

	Engine::GetInstance()->GetSoundSystem()->GetLoader()->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, wideFilename, (void**)&m_segment);
	SAFE_DELETE_ARRAY(wideFilename);

	m_segment->Download(Engine::GetInstance()->GetSoundSystem()->GetPerformance());
}


Sound::~Sound()
{
	m_segment->Unload(Engine::GetInstance()->GetSoundSystem()->GetPerformance());
	Engine::GetInstance()->GetSoundSystem()->GetLoader()->ReleaseObjectByUnknown(m_segment);
	SAFE_RELEASE(m_segment);
}


void Sound::Play(bool loop, unsigned long flags)
{
	if(loop == true)
		m_segment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
	else
		m_segment->SetRepeats(0);

	Engine::GetInstance()->GetSoundSystem()->GetPerformance()->PlaySegment(m_segment, flags, 0, NULL);
}


IDirectMusicSegment8* Sound::GetSegment()
{
	return m_segment;
}


//AudioPath3D class
AudioPath3D::AudioPath3D()
{
	m_soundBuffer = NULL;
	m_audioPath = NULL;

	Engine::GetInstance()->GetSoundSystem()->GetPerformance()->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D, 1, true, &m_audioPath);

	if(m_audioPath != NULL)
		m_audioPath->GetObjectInPath(DMUS_PCHANNEL_ALL, DMUS_PATH_BUFFER, 0, GUID_NULL, 0, IID_IDirectSound3DBuffer, (void**)&m_soundBuffer);
}


AudioPath3D::~AudioPath3D()
{
	SAFE_RELEASE(m_soundBuffer);
	SAFE_RELEASE(m_audioPath);
}


void AudioPath3D::SetPosition(D3DXVECTOR3 position)
{
	if(m_soundBuffer == NULL)
		return;

	position *= Engine::GetInstance()->GetScale();
	m_soundBuffer->SetPosition(position.x, position.y, position.z, DS3D_IMMEDIATE);
}


void AudioPath3D::SetVelocity(D3DXVECTOR3 velocity)
{
	if(m_soundBuffer == NULL)
		return;

	velocity *= Engine::GetInstance()->GetScale();
	m_soundBuffer->SetVelocity(velocity.x, velocity.y, velocity.z, DS3D_IMMEDIATE);
}


void AudioPath3D::SetMode(unsigned long mode)
{
	if(m_soundBuffer == NULL)
		return;

	m_soundBuffer->SetMode(mode, DS3D_IMMEDIATE);
}


void AudioPath3D::Play(IDirectMusicSegment8* segment, bool loop, unsigned long flags)
{
	if(m_audioPath == NULL)
		return;

	if(loop == true)
		segment->SetRepeats( DMUS_SEG_REPEAT_INFINITE );
	else
		segment->SetRepeats( 0 );

	Engine::GetInstance()->GetSoundSystem()->GetPerformance()->PlaySegmentEx( segment, NULL, NULL, flags, 0, NULL, NULL, m_audioPath );
}

