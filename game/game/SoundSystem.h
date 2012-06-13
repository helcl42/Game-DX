#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H


class SoundSystem
{
private:
	float			  m_scale;
	IDirectMusicLoader8*	  m_loader;
	IDirectMusicPerformance8* m_performance;
	IDirectSound3DListener8*  m_listener;

public:
	SoundSystem(float scale = 1.0f);
	virtual ~SoundSystem();
	
	void UpdateListener(D3DXVECTOR3 forward, D3DXVECTOR3 position, D3DXVECTOR3 velocity);

	void			  GarbageCollection();
	void			  SetVolume(long volume);
	IDirectMusicLoader8*	  GetLoader();
	IDirectMusicPerformance8* GetPerformance();
};


class Sound
{
private:
	IDirectMusicSegment8* m_segment;

public:
	Sound(char* filename);
	virtual ~Sound();

	void Play(bool loop = false, unsigned long flags = DMUS_SEGF_AUTOTRANSITION);

	IDirectMusicSegment8* GetSegment();
};


class AudioPath3D
{
private:
	IDirectMusicAudioPath8* m_audioPath;
	IDirectSound3DBuffer8*  m_soundBuffer;

public:
	AudioPath3D();
	virtual ~AudioPath3D();

	void SetPosition(D3DXVECTOR3 position);
	void SetVelocity(D3DXVECTOR3 velocity);
	void SetMode(unsigned long mode);
	void Play( IDirectMusicSegment8 *segment, bool loop = false, unsigned long flags = DMUS_SEGF_SECONDARY );
};

#endif