#ifndef FANCYLINEEDIT_H
#define FANCYLINEEDIT_H

#include <QLineEdit>

// fancy line edit: a QLineEdit with an Edit (...) button and clear (x) button:
// edit button opens a multi-line edit box

class QToolButton;

class FancyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    FancyLineEdit(QWidget *parent = 0);
   ~FancyLineEdit();

    void hideEditButton();
    void hideClearButton();
    void showEditButton();
    void showClearButton();

protected:
  void resizeEvent(QResizeEvent *) override;

private:
    QToolButton *editButton;
    QToolButton *clearButton;

private slots:
    void on_editButton_Clicked();
};

#endif // FANCYLINEEDIT_H
