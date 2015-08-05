#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include <QtGui>
#include "ui_aboutdialog.h"



class AboutDialog : public QDialog, public Ui::AboutDialog
{
    Q_OBJECT;

    public:
    AboutDialog(QWidget* parent=0) : QDialog(parent){ setupUi(this); }

};


#endif
