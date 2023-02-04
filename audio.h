#ifndef AUDIO_H
#define AUDIO_H

#include <QSoundEffect>

class Audio
{

public:
    Audio();
    void play();

private:
    QSoundEffect        effect;
};

#endif // AUDIO_H
