#include "audio.h"

Audio::Audio()
{

}

void Audio::play()
{
    effect.setSource(QUrl::fromLocalFile(":/sounds/sound/fimdovoto.wav"));
    effect.setLoopCount(QSoundEffect::Null);
    effect.play();
}
