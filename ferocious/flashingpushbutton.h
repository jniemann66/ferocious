#ifndef FLASHINGPUSHBUTTON_H
#define FLASHINGPUSHBUTTON_H

#include <QPushButton>
#include <QTimer>

// class flashingPushButton
// Description: a QPushButton which flashes on and off whenever it is Disabled (to indicate that the app is busy processing ...)

class flashingPushbutton : public QPushButton{
public:
    flashingPushbutton(QWidget* parent=0) : QPushButton(parent), flashState(0)
    {
        connect(&timer, &QTimer::timeout,this, &flashingPushbutton::flashWhenDisabled);
        timer.start(500);
    }
    ~flashingPushbutton()
    {

    }
private slots:
    void flashWhenDisabled()
    {
        if (!this->isEnabled()) // flash when disabled
        {
            if (flashState == 0){
                flashState = 1;
                this->setProperty("flashing", true);
            }
            else if (flashState == 1){
                flashState = 0;
                this->setProperty("flashing", false);
            }
            style()->unpolish(this);
            style()->polish(this);
        }
        else { // don't flash
            if (flashState != 0) {
                flashState = 0;
                this->setProperty("flashing", false);
                style()->unpolish(this);
                style()->polish(this);
            }
        }
    }

private:
    int flashState;
    QTimer timer;

};

#endif // FLASHINGPUSHBUTTON_H
