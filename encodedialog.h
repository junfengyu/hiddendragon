

#ifndef ENCODEDIALOG_H_
#define ENCODEDIALOG_H_

#include "optiondialog.h"

class EncodeDialog : public OptionDialog
{
    Q_OBJECT;

    public:
        EncodeDialog(QWidget* parent=0);

    protected:
       

        private slots:
        void ok();
        void showEvent(QShowEvent*);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);

    signals:
        void encodedImage(QString);
};


#endif
