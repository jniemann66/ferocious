/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "soundfiledialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QUrl>
#include <QFileInfo>
#include <QMediaDevices>
#include <QAudioDevice>

SoundFileDialog::SoundFileDialog(QWidget *parent)
    : QFileDialog(parent)
{
    setOption(QFileDialog::DontUseNativeDialog, true);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(QMediaDevices::defaultAudioOutput(), this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0f);

    playButton = new QPushButton(tr("Play"), this);
    playButton->setEnabled(false);

    statusLabel = new QLabel(this);

    auto *row = new QHBoxLayout;
    row->addWidget(statusLabel, 1);
    row->addWidget(playButton);

    auto *layout = qobject_cast<QGridLayout *>(this->layout());
    if (layout) {
        layout->addLayout(row, layout->rowCount(), 0, 1, layout->columnCount());
    }

    connect(this, &QFileDialog::currentChanged, this, &SoundFileDialog::onCurrentFileChanged);
    connect(playButton, &QPushButton::clicked, this, &SoundFileDialog::onPlayButtonClicked);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &SoundFileDialog::onPlaybackStateChanged);
    connect(player, &QMediaPlayer::errorOccurred, this, &SoundFileDialog::onPlayerErrorOccurred);
}

SoundFileDialog::~SoundFileDialog()
{
    player->stop();
}

void SoundFileDialog::onCurrentFileChanged(const QString &path)
{
    currentPath = path;
    player->stop();
    statusLabel->clear();
    playButton->setEnabled(QFileInfo(path).isFile());
}

void SoundFileDialog::onPlayButtonClicked()
{
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->stop();
    } else if (!currentPath.isEmpty()) {
        player->setSource(QUrl::fromLocalFile(currentPath));
        player->play();
    }
}

void SoundFileDialog::onPlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    playButton->setText(state == QMediaPlayer::PlayingState ? tr("Stop") : tr("Play"));
}

void SoundFileDialog::onPlayerErrorOccurred(QMediaPlayer::Error /*error*/, const QString &errorString)
{
    statusLabel->setText(errorString);
}
