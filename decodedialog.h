
#ifndef DECODEDIALOG_H_
#define DECODEDIALOG_H_

#include <QPointer>

#include <data.h>
#include "optiondialog.h"


class DecodeDialog : public OptionDialog
{
        Q_OBJECT;

    public:
        DecodeDialog(QWidget* parent=0);
        ~DecodeDialog();

    protected:
     
        private slots:
        void ok();
        void showEvent(QShowEvent*);
        void releaseFileButton();
};



#endif
