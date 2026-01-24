#ifndef DSAUDIOPLAYER_H
#define DSAUDIOPLAYER_H


class DSAudioPlayer
{
public:
    DSAudioPlayer();
    ~DSAudioPlayer();

    bool LoadFile(const wchar_t *filePath);
    bool GetDuration(long long *pDuration);
    bool Play();
    bool Pause();
    bool Stop();

private:
    class DSAudioPlayerPrivate;
    DSAudioPlayerPrivate *pimpl;
};

#endif // DSAUDIOPLAYER_H

