/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef SOUNDFILEDIALOG_H
#define SOUNDFILEDIALOG_H

#include <QFileDialog>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include <QLabel>
#include <QPlainTextEdit>

class SoundFileDialog : public QFileDialog
{
    Q_OBJECT

public:
    explicit SoundFileDialog(QWidget *parent = nullptr,
                             const QString &converterPath = {},
                             bool hasInfoOption = false);
    ~SoundFileDialog() override;

private slots:
    void onCurrentFileChanged(const QString &path);
    void onPlayButtonClicked();
    void onInfoButtonClicked();
    void onHideInfoButtonClicked();
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState state);
    void onPlayerErrorOccurred(QMediaPlayer::Error error, const QString &errorString);

private:
    void updateInfoPanel();
    QPushButton *playButton;
    QPushButton *infoButton;
    QPushButton *hideInfoButton;
    QPlainTextEdit *infoTextEdit;
    QLabel *statusLabel;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QString currentPath;
    QString converterPath;
    int normalWidth{0};  // dialog width before the info panel is shown
};

#endif // SOUNDFILEDIALOG_H
