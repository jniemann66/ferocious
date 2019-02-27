#include "fancylineedit.h"

#include <QToolButton>
#include <QStyle>
#include <QInputDialog>
#include <QDir>

FancyLineEdit::FancyLineEdit(QWidget *parent) : QLineEdit(parent)
{
    clearButton = new QToolButton(this);
    editButton = new QToolButton(this);
    fileDropDialog = new FileDropDialog(this);

    clearButton->resize(24, 18);
    clearButton->setText("x");
    clearButton->setCursor(Qt::ArrowCursor);

    editButton->resize(24, 18);
    editButton->setText("...");
    editButton->setCursor(Qt::ArrowCursor);

    fileDropDialog->hide();
    setAcceptDrops(true);

    connect(clearButton, &QAbstractButton::clicked, this, &QLineEdit::clear);
    connect(editButton, &QAbstractButton::clicked,this, &FancyLineEdit::on_editButton_Clicked);
}

FancyLineEdit::~FancyLineEdit(){}

void FancyLineEdit::hideEditButton()
{
    editButton->hide();
}

void FancyLineEdit::hideClearButton()
{
    clearButton->hide();
}

void FancyLineEdit::showEditButton()
{
    editButton->show();
}

void FancyLineEdit::showClearButton()
{
    clearButton->show();
}

void FancyLineEdit::resizeEvent(QResizeEvent *)
{
    // put the Clear button on the far-right of the Line Edit, and position 2 pixels down from Line Edit;
    clearButton->move(QPoint(this->width()-clearButton->sizeHint().width(), 2));
    clearButton->resize(QSize(clearButton->sizeHint().width() - 2, this->height() - 4));

    // put the Edit button to the left of the Clear button, and position 2 pixels down from Line Edit;
    editButton->move(QPoint(this->width() - 2 * editButton->sizeHint().width() + 3, 2));
    editButton->resize(QSize(editButton->sizeHint().width() - 2, this->height() - 4));
}

void FancyLineEdit::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasText()) {
        e->acceptProposedAction();
    }
}

void FancyLineEdit::dropEvent(QDropEvent *e)
{
    QStringList urlStrings = e->mimeData()->text().split("\n");
    QUrl url;
    QStringList paths;
    for(const auto& urlString : urlStrings) {
        if(urlString.isEmpty())
            continue;
        url = urlString;
        QString path = QDir::toNativeSeparators(url.path());

#ifdef Q_OS_WIN
        if(path.startsWith('\\')) {
            path.remove(0, 1);
        }
#endif
        // if path is a directory, append a wilcard(*) to it.
        QFileInfo x(path);
        if(x.isDir()) {
            path.append("/*");
        }

        paths.append(QDir::toNativeSeparators(path));

    }

    if(!paths.isEmpty()) {
        setText(paths.join("\n"));
        emit QLineEdit::editingFinished();
    }
}

void FancyLineEdit::on_editButton_Clicked()
{
    fileDropDialog->setText(tr("Edit multiple filenames"), tr("Edit filenames below, and click 'OK' when done."), this->text());
    fileDropDialog->show();

    connect(fileDropDialog, &FileDropDialog::accepted, this, [this]{
        setText(fileDropDialog->getText());
        fileDropDialog->hide();
    });

    connect(fileDropDialog, &FileDropDialog::rejected, this, [this]{
        fileDropDialog->close();
    });
}
