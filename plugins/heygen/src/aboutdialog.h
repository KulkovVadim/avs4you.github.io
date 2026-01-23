#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "uidialog.h"

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public UIDialog
{
public:
    AboutDialog(UIWidget *parent = nullptr, const Rect &rc = DEFAULT_DLG_RECT);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
