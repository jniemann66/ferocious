/*
* Copyright (C) 2016 - 2023 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef FLASHINGPUSHBUTTON_H
#define FLASHINGPUSHBUTTON_H

// class flashingPushButton
// Description: a QPushButton which flashes on and off whenever it is Active (to indicate that the app is busy processing ...)

#include <QPushButton>
#include <QTimer>
#include <QStyle>
#include <QMouseEvent>

class FlashingPushButton : public QPushButton
{
    Q_OBJECT

public:
	FlashingPushButton(QWidget* parent = nullptr) : QPushButton(parent)
    {
        connect(&timer, &QTimer::timeout,this, &FlashingPushButton::flashWhenActive);
        timer.start(500);
    }

    bool getIsActive() const;
    void setIsActive(bool value);

protected:
    void mousePressEvent(QMouseEvent* mouseEvent) override;

signals:
    void stopRequested();
    void rightClicked();

private slots:
    void flashWhenActive() {
        if (isActive) { // flash when active
            if (flashState == 0) {
                flashState = 1;
                setProperty("flashing", true);
            }
            else if (flashState == 1) {
                flashState = 0;
                setProperty("flashing", false);
            }
            style()->unpolish(this);
            style()->polish(this);
        }
        else { // don't flash
            if (flashState != 0) {
                flashState = 0;
                setProperty("flashing", false);
                style()->unpolish(this);
                style()->polish(this);
            }
        }
    }

private:
    bool isActive{false};
    int flashState{0};
    QTimer timer;

};

inline void FlashingPushButton::mousePressEvent(QMouseEvent *mouseEvent)
{

    if(isActive) {
        emit stopRequested();
    } else {
        if(mouseEvent->button() == Qt::RightButton) {
            emit rightClicked();
        } else { // default behavior
            QPushButton::mousePressEvent(mouseEvent);
        }
    }
}

inline bool FlashingPushButton::getIsActive() const
{
    return isActive;
}

inline void FlashingPushButton::setIsActive(bool value)
{
    isActive = value;
}

#endif // FLASHINGPUSHBUTTON_H
