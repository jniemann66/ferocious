#ifndef FANCYLINEEDIT_H
#define FANCYLINEEDIT_H

// FancyLineEdit: a QLineEdit with added Edit (...) and clear (x) buttons.
// The edit button opens a multi-line edit box.
// The line edit and the "expanded" multi-edit both accept the filenames of
// dragged-and-dropped files

#include "filedropdialog.h"

#include <QLineEdit>
#include <QDropEvent>
#include <QMimeData>

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
  void dragEnterEvent(QDragEnterEvent* e) override;
  void dropEvent(QDropEvent* e) override;

private:
    QToolButton *editButton;
    QToolButton *clearButton;
    FileDropDialog* fileDropDialog;

private slots:
    void on_editButton_Clicked();
};

#endif // FANCYLINEEDIT_H
