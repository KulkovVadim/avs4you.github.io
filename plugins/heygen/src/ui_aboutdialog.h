#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include "uibutton.h"
#include "uilabel.h"
#include "uiboxlayout.h"
#include "uimetrics.h"

#define SetMetrics(role, value) metrics()->setMetrics(role, value)

class Ui_AboutDialog
{
public:
    UILabel *label;
    UIButton *btnOk;

    void setupUi(UIWidget *w)
    {
        UIBoxLayout *cenVlut = new UIBoxLayout(UIBoxLayout::Vertical, UIBoxLayout::AlignCenter);
        cenVlut->setContentMargins(16, 16, 16, 24);
        cenVlut->setSpacing(12);
        w->setLayout(cenVlut);

        label = new UILabel(w);
        label->setObjectGroupId(_T("PanelLabel"));
        label->setSizePolicy(SizePolicy::VSizeBehavior, SizePolicy::Expanding);
        label->SetMetrics(Metrics::TextAlignment, Metrics::AlignHCenter | Metrics::AlignVCenter);
        label->SetMetrics(Metrics::TextMarginLeft, 0);
        FontInfo fi;
        fi.name = DEFAULT_FONT_NAME;
        fi.pointSize = 10;
        fi.weight = 600;
        label->setFont(fi);
        label->setBaseSize(50, 28);
        cenVlut->addWidget(label);

        btnOk = new UIButton(w);
        btnOk->setObjectGroupId(_T("PushButton"));
        btnOk->SetMetrics(Metrics::TextAlignment, Metrics::AlignHCenter | Metrics::AlignVCenter);
        cenVlut->addWidget(btnOk);

        retranslateUi(w);
    }

    void retranslateUi(UIWidget *w)
    {
        btnOk->setText(_T("Ok"));
    }

private:

};

namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
}

#endif // UI_ABOUTDIALOG_H
