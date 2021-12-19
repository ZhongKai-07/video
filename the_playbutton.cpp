#include "the_playbutton.h"

QPushButton *playButton;
QPushButton *stopButton;

void PlayButton::setState(QMediaPlayer::State state){

        switch (state) { //allows the icon to change depending on whether the video is paused or playing
            case QMediaPlayer::PlayingState:
                setEnabled(true);
                setIcon(style()->standardIcon(QStyle::SP_MediaPause));
                break;
            case QMediaPlayer::PausedState:
                setEnabled(true);
                setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            }
}
