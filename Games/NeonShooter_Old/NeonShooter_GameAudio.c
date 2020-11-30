#include "NeonShooter_Assets.h"
#include "NeonShooter_GameAudio.h"

#include <Array.h>
#include <raylib.h>

static Music        music;
static Array(Sound) shootSounds = 0;
static Array(Sound) spawnSounds = 0;
static Array(Sound) explosionSounds = 0;

void GameAudioInit(void)
{
    InitAudioDevice();

    //const char* musicStreamPath = GetAssetPath("Audios/Music.ogg");
    //music = LoadMusicStream(musicStreamPath);
    //StopMusicStream(music);

    ArrayPush(shootSounds, LoadSound(GetAssetPath("Audios/shoot-01.wav")));
    ArrayPush(shootSounds, LoadSound(GetAssetPath("Audios/shoot-02.wav")));
    ArrayPush(shootSounds, LoadSound(GetAssetPath("Audios/shoot-03.wav")));
    ArrayPush(shootSounds, LoadSound(GetAssetPath("Audios/shoot-04.wav")));

    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-01.wav")));
    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-02.wav")));
    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-03.wav")));
    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-04.wav")));
    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-05.wav")));
    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-06.wav")));
    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-07.wav")));
    ArrayPush(spawnSounds, LoadSound(GetAssetPath("Audios/spawn-08.wav")));

    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-01.wav")));
    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-02.wav")));
    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-03.wav")));
    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-04.wav")));
    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-05.wav")));
    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-06.wav")));
    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-07.wav")));
    ArrayPush(explosionSounds, LoadSound(GetAssetPath("Audios/explosion-08.wav")));
}

void GameAudioRelease(void)
{
    for (int i = 0, n = ArrayCount(shootSounds); i < n; i++)
    {
        UnloadSound(shootSounds[i]);
    }
    ArrayFree(shootSounds);

    for (int i = 0, n = ArrayCount(spawnSounds); i < n; i++)
    {
        UnloadSound(spawnSounds[i]);
    }
    ArrayFree(spawnSounds);

    for (int i = 0, n = ArrayCount(explosionSounds); i < n; i++)
    {
        UnloadSound(explosionSounds[i]);
    }
    ArrayFree(explosionSounds);

    //UnloadMusicStream(music);
    //music = (Music){ 0 };

    CloseAudioDevice();     // Close audio device
}

void GameAudioUpdate(void)
{
    //UpdateMusicStream(music);
}

void GameAudioPlayMusic(void)
{
    //SetMusicVolume(music, 0.5f);
    //SetMusicLoopCount(music, -1);

    //PlayMusicStream(music);
}

void GameAudioStopMusic(void)
{
    //StopMusicStream(music);
}

void GameAudioPlayShoot(void)
{
    int count = ArrayCount(shootSounds);
    if (count > 0)
    {
        Sound sound = shootSounds[GetRandomValue(0, count - 1)];
        SetSoundVolume(sound, 0.2f);
        PlaySound(sound);
    }
}

void GameAudioStopShoot(void)
{
    for (int i = 0, n = ArrayCount(shootSounds); i < n; i++)
    {
        StopSound(shootSounds[i]);
    }
}

void GameAudioPlayExplosion(void)
{
    int count = ArrayCount(explosionSounds);
    if (count > 0)
    {
        Sound sound = explosionSounds[GetRandomValue(0, count - 1)];
        SetSoundVolume(sound, 0.4f);
        PlaySound(sound);
    }
}

void GameAudioStopExplosion(void)
{
    for (int i = 0, n = ArrayCount(explosionSounds); i < n; i++)
    {
        StopSound(explosionSounds[i]);
    }
}

void GameAudioPlaySpawn(void)
{
    int count = ArrayCount(spawnSounds);
    if (count > 0)
    {
        Sound sound = spawnSounds[GetRandomValue(0, count - 1)];
        SetSoundVolume(sound, 0.3f);
        PlaySound(sound);
    }
}

void GameAudioStopSpawn(void)
{
    for (int i = 0, n = ArrayCount(spawnSounds); i < n; i++)
    {
        StopSound(spawnSounds[i]);
    }
}
