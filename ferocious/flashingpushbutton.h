#ifndef FLASHINGPUSHBUTTON_H
#define FLASHINGPUSHBUTTON_H

// class flashingPushButton
// Description: a QPushButton which flashes on and off whenever it is Active (to indicate that the app is busy processing ...)

#include <QPushButton>
#include <QTimer>
#include <QStyle>
#include <QMouseEvent>

class flashingPushbutton : public QPushButton{

    Q_OBJECT

public:
    flashingPushbutton(QWidget* parent=0) : QPushButton(parent), isActive(false), flashState(0)
    {
        connect(&timer, &QTimer::timeout,this, &flashingPushbutton::flashWhenActive);
        timer.start(500);
    }

    ~flashingPushbutton(){}

    bool getIsActive() const;
    void setIsActive(bool value);

protected:
    void mousePressEvent(QMouseEvent* mouseEvent) override;

signals:
    void stopRequested();
    void rightClicked();

private slots:
    void flashWhenActive()
    {
        if (isActive) // flash when active
        {
            if (flashState == 0){
                flashState = 1;
                setProperty("flashing", true);
            }
            else if (flashState == 1){
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
    bool isActive;
    int flashState;
    QTimer timer;

};


inline void flashingPushbutton::mousePressEvent(QMouseEvent *mouseEvent)
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

inline bool flashingPushbutton::getIsActive() const
{
    return isActive;
}

inline void flashingPushbutton::setIsActive(bool value)
{
    isActive = value;
}

#endif // FLASHINGPUSHBUTTON_H
