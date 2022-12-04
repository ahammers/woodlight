#pragma once

#include <Windows.h>


// SoundEffect Library von Brian Gradin (siehe https://stackoverflow.com/questions/19894384/simple-sounds-in-c)
class SoundEffect
{
public:
    SoundEffect();
    SoundEffect(const int freq, int milliseconds);
    SoundEffect(const int noteInfo[], const int arraySize);
    SoundEffect(SoundEffect& otherInstance);
    ~SoundEffect();

    SoundEffect& operator=(SoundEffect& otherInstance);

    void Play();

private:
    HWAVEOUT m_waveOut; // Handle to sound card output
    WAVEFORMATEX m_waveFormat; // The sound format
    WAVEHDR m_waveHeader; // WAVE header for our sound data
    HANDLE m_done; // Event Handle that tells us the sound has finished being played.
    // This is a very efficient way to put the program to sleep
    // while the sound card is processing the sound buffer

    char* m_data; // Sound data buffer
    int m_bufferSize; // Size of sound data buffer
};