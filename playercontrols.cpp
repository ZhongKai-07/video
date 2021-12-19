#include "playercontrols.h"
#include "absolutesetstyle.h"

#include <QBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QStyle>
#include <QToolButton>

PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent), playerState(QMediaPlayer::StoppedState), playerMuted(false) {

    playButton = new QToolButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    playButton->setIconSize(QSize(40,40));
    connect(playButton, SIGNAL(clicked()), this, SLOT(playClicked()));

    stopButton = new QToolButton(this);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    stopButton->setIconSize(QSize(40,40));
    stopButton->setEnabled(false);
    connect(stopButton, SIGNAL(clicked()), this, SIGNAL(stop()));

    nextButton = new QToolButton(this);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    nextButton->setIconSize(QSize(40,40));
    connect(nextButton, SIGNAL(clicked()), this, SIGNAL(next()));

    previousButton = new QToolButton(this);
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    previousButton->setIconSize(QSize(40,40));
    connect(previousButton, SIGNAL(clicked()), this, SIGNAL(previous()));

    muteButton = new QToolButton(this);
    muteButton->setIcon(QIcon(":/volume.png"));
    muteButton->setIconSize(QSize(40,40));
    connect(muteButton, SIGNAL(clicked()), this, SLOT(muteClicked()));

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setStyle(new AbsoluteSetStyle(volumeSlider->style()));
    connect(volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onVolumeSliderValueChanged()));

    forwardButton = new QToolButton(this);
    //forwardButton->setIcon(QIcon(":/forward.png"));
    forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    forwardButton->setIconSize(QSize(40,40));
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(forwardClicked()));

    backButton = new QToolButton(this);
    backButton->setIconSize(QSize(40,40));
    //backButton->setIcon(QIcon(":/backward.png"));
    backButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(backButton, SIGNAL(clicked()), this, SLOT(backClicked()));

    rateBox = new QComboBox(this);
    rateBox->addItem("0.5x", QVariant(0.5));
    rateBox->addItem("1.0x", QVariant(1.0));
    rateBox->addItem("1.5x", QVariant(1.5));
    rateBox->addItem("2.0x", QVariant(2.0));
    rateBox->setCurrentIndex(1);
    connect(rateBox, SIGNAL(activated(int)), SLOT(updateRate()));

    initLayout();
}

void PlayerControls::initLayout() {
    QBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(stopButton);
    layout->addWidget(previousButton);
    layout->addWidget(backButton);
    layout->addWidget(playButton);
    layout->addWidget(forwardButton);
    layout->addWidget(nextButton);
    layout->addWidget(muteButton);
    layout->addWidget(volumeSlider);
    layout->addWidget(rateBox);
    setLayout(layout);
}

QMediaPlayer::State PlayerControls::state() const {
    return playerState;
}

void PlayerControls::setState(QMediaPlayer::State state) {
    if (state != playerState) {
        playerState = state;
        switch (state) {
            case QMediaPlayer::StoppedState:
                stopButton->setEnabled(false);
                playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
            case QMediaPlayer::PlayingState:
                stopButton->setEnabled(true);
                playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
                break;
            case QMediaPlayer::PausedState:
                stopButton->setEnabled(true);
                playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                break;
        }
    }
}

int PlayerControls::volume() const {
    qreal linearVolume =
        QAudio::convertVolume(volumeSlider->value() / qreal(100), QAudio::LogarithmicVolumeScale,
                              QAudio::LinearVolumeScale);
    return qRound(linearVolume * 100);
}

void PlayerControls::setVolume(int volume) {
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100), QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);
    volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

bool PlayerControls::isMuted() const {
    return playerMuted;
}

void PlayerControls::setMuted(bool muted) {
    if (muted != playerMuted) {
        playerMuted = muted;
        if(muted){
            muteButton->setIcon(QIcon(":/mute.png"));
        }
        else{
            muteButton->setIcon(QIcon(":/volume.png"));
        }
    }
}

void PlayerControls::playClicked() {
    switch (playerState) {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            emit play();
            break;
        case QMediaPlayer::PlayingState:
            emit pause();
            break;
    }
}

void PlayerControls::muteClicked() {
    emit changeMuting(!playerMuted);
}

void PlayerControls::backClicked() {
    emit back();
}

void PlayerControls::forwardClicked() {
    emit forward();
}

qreal PlayerControls::playbackRate() const {
    return rateBox->itemData(rateBox->currentIndex()).toDouble();
}

void PlayerControls::setPlaybackRate(float rate) {
    for (int i = 0; i < rateBox->count(); ++i) {
        if (qFuzzyCompare(rate, float(rateBox->itemData(i).toDouble()))) {
            rateBox->setCurrentIndex(i);
            return;
        }
    }

    rateBox->addItem(QString("%1x").arg(rate), QVariant(rate));
    rateBox->setCurrentIndex(rateBox->count() - 1);
}

void PlayerControls::updateRate() {
    emit changeRate(playbackRate());
}

void PlayerControls::onVolumeSliderValueChanged() {
    emit changeVolume(volume());
}