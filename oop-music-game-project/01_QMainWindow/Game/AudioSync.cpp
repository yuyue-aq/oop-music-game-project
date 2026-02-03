#include "AudioSync.h"

AudioSync::AudioSync(QMediaPlayer* mediaPlayer, GameConfig* config)
    : mediaPlayer(mediaPlayer), config(config), startTime(0.0), isPaused(false)
{
}

double AudioSync::getCurrentMusicTime() const {
    if (!mediaPlayer) return 0.0;
    
    double rawTime = getRawPosition();
    double adjustedTime = applyAudioOffset(rawTime);
    
    return adjustedTime;
}

double AudioSync::getRawPosition() const {
    if (!mediaPlayer) return 0.0;
    return static_cast<double>(mediaPlayer->position());
}

double AudioSync::applyAudioOffset(double rawTime) const {
    return rawTime + config->getAudioOffset();
}

void AudioSync::play() {
    if (mediaPlayer) {
        mediaPlayer->play();
        isPaused = false;
    }
}