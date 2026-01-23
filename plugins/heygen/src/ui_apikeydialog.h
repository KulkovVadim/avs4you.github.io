#ifndef UI_APIKEYDIALOG_H
#define UI_APIKEYDIALOG_H

#include "uibutton.h"
#include "uilabel.h"
#include "uilineedit.h"
#include "uiboxlayout.h"
#include "uispacer.h"
#include "uimetrics.h"

#define SetMetrics(role, value) metrics()->setMetrics(role, value)

class Ui_ApiKeyDialog
{
public:
    UILineEdit *editKey;
    UIButton *linkKey;
    UIButton *btnOk;

    void setupUi(UIWidget *w)
    {
        UIBoxLayout *cenVlut = new UIBoxLayout(UIBoxLayout::Vertical, UIBoxLayout::AlignCenter);
        cenVlut->setContentMargins(16, 16, 16, 24);
        cenVlut->setSpacing(12);
        w->setLayout(cenVlut);

        label = new UILabel(w);
        label->setObjectGroupId(_T("PanelLabel"));
        label->SetMetrics(Metrics::TextAlignment, Metrics::AlignHCenter | Metrics::AlignVCenter);
        label->SetMetrics(Metrics::TextMarginLeft, 0);
        FontInfo fi;
        fi.name = DEFAULT_FONT_NAME;
        fi.pointSize = 11;
        fi.weight = 600;
        label->setFont(fi);
        label->setBaseSize(50, 28);
        cenVlut->addWidget(label);

        editKey = new UILineEdit(w);
        editKey->setObjectGroupId(_T("LineEdit"));
        editKey->setSizePolicy(SizePolicy::VSizeBehavior, SizePolicy::Fixed);
        cenVlut->addWidget(editKey);

        linkKey = new UIButton(w);
        linkKey->setObjectGroupId(_T("HyperLink"));
        fi.pointSize = 10;
        fi.weight = 400;
        fi.underline = true;
        linkKey->setFont(fi);
        linkKey->setHandCursorOnHover();
        cenVlut->addWidget(linkKey);

        UISpacer *spacer1 = new UISpacer(5, 5, SizePolicy::Fixed, SizePolicy::Expanding);
        cenVlut->addSpacer(spacer1);

        btnOk = new UIButton(w);
        btnOk->setObjectGroupId(_T("PushButton"));
        btnOk->SetMetrics(Metrics::TextAlignment, Metrics::AlignHCenter | Metrics::AlignVCenter);
        cenVlut->addWidget(btnOk);

        retranslateUi(w);
    }

    void retranslateUi(UIWidget *w)
    {
        label->setText(_T("Set api key for Heygen"));
        linkKey->setText(_T("Heygen key instructions"));
        linkKey->adjustSizeBasedOnContent();
        btnOk->setText(_T("Ok"));
    }

private:
    UILabel *label;
};

namespace Ui {
    class ApiKeyDialog: public Ui_ApiKeyDialog {};
}

#endif // UI_APIKEYDIALOG_H
