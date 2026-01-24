#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "isltranslator.h"

AboutDialog::AboutDialog(UIWidget *parent, const Rect &rc) :
    UIDialog(parent, rc),
    ui(new Ui::AboutDialog)
{
    setObjectGroupId(_T("PanelWidget"));
    setWindowTitle(_TR(ABOUT_CAPTION));
    ui->setupUi(this);

    ui->btnOk->clickSignal.connect([this]() {
        accept();
    });

    ui->label->setText(_T("Heygen plugin ver 0.0.3"));
}

AboutDialog::~AboutDialog()
{
    delete ui, ui = nullptr;
}
