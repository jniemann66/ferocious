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
#include <QProcess>

SoundFileDialog::SoundFileDialog(QWidget *parent, const QString &converterPath, bool hasInfoOption)
    : QFileDialog(parent), converterPath(converterPath)
{
    setOption(QFileDialog::DontUseNativeDialog, true);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(QMediaDevices::defaultAudioOutput(), this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0f);

    playButton = new QPushButton(tr("Play"), this);
    playButton->setEnabled(false);

    infoButton = new QPushButton(tr("Info >>"), this);
    infoButton->setEnabled(false);
    infoButton->setVisible(hasInfoOption);

    hideInfoButton = new QPushButton(tr("<< Hide Info"), this);
    hideInfoButton->setVisible(false);

    statusLabel = new QLabel(this);

    infoTextEdit = new QPlainTextEdit(this);
    infoTextEdit->setReadOnly(true);
    infoTextEdit->setVisible(false);

    auto *layout = qobject_cast<QGridLayout *>(this->layout());
    if (layout) {
        auto *buttonRow = new QHBoxLayout;
        buttonRow->addWidget(statusLabel, 1);
        buttonRow->addWidget(infoButton);
        buttonRow->addWidget(hideInfoButton);
        buttonRow->addWidget(playButton);
        const int cols = layout->columnCount();
        layout->addLayout(buttonRow, layout->rowCount(), 0, 1, cols);
        // place infoTextEdit to the right of all existing content, spanning every row
        infoTextEdit->setMinimumWidth(300);
        layout->addWidget(infoTextEdit, 0, cols, layout->rowCount(), 1);
    }

    connect(this, &QFileDialog::currentChanged, this, &SoundFileDialog::onCurrentFileChanged);
    connect(playButton, &QPushButton::clicked, this, &SoundFileDialog::onPlayButtonClicked);
    connect(infoButton, &QPushButton::clicked, this, &SoundFileDialog::onInfoButtonClicked);
    connect(hideInfoButton, &QPushButton::clicked, this, &SoundFileDialog::onHideInfoButtonClicked);
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
    const bool isFile = QFileInfo(path).isFile();
    playButton->setEnabled(isFile);
    infoButton->setEnabled(isFile);
    if (infoTextEdit->isVisible()) {
        if (isFile) {
            updateInfoPanel();
        } else {
            onHideInfoButtonClicked();
        }
    }
}

void SoundFileDialog::updateInfoPanel()
{
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start(converterPath, {"-i", currentPath, "--info"});
    if (!proc.waitForFinished(5000)) {
        return;
    }
    infoTextEdit->setPlainText(QString::fromLocal8Bit(proc.readAll()).trimmed());
}

void SoundFileDialog::onInfoButtonClicked()
{
    if (currentPath.isEmpty() || converterPath.isEmpty()) {
        return;
    }

    updateInfoPanel();
    normalWidth = width();
    infoTextEdit->setVisible(true);
    infoButton->setVisible(false);
    hideInfoButton->setVisible(true);
    auto *grid = qobject_cast<QGridLayout *>(this->layout());
    const int spacing = grid ? grid->horizontalSpacing() : 6;
    resize(normalWidth + infoTextEdit->minimumWidth() + spacing, height());
}

void SoundFileDialog::onHideInfoButtonClicked()
{
    infoTextEdit->setVisible(false);
    hideInfoButton->setVisible(false);
    infoButton->setVisible(true);
    resize(normalWidth, height());
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
