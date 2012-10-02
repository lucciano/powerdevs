/********************************************************************************
** Form generated from reading UI file 'knob.ui'
**
** Created: Tue Oct 2 10:31:05 2012
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KNOB_H
#define UI_KNOB_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDial>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_knob
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    QLabel *name;
    QDial *dial;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *txtVal;

    void setupUi(QWidget *knob)
    {
        if (knob->objectName().isEmpty())
            knob->setObjectName(QString::fromUtf8("knob"));
        knob->resize(400, 300);
        horizontalLayout = new QHBoxLayout(knob);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        name = new QLabel(knob);
        name->setObjectName(QString::fromUtf8("name"));
        name->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(name);

        dial = new QDial(knob);
        dial->setObjectName(QString::fromUtf8("dial"));

        verticalLayout_3->addWidget(dial);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(knob);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_3->addWidget(label_2);

        txtVal = new QLineEdit(knob);
        txtVal->setObjectName(QString::fromUtf8("txtVal"));

        horizontalLayout_3->addWidget(txtVal);


        verticalLayout_3->addLayout(horizontalLayout_3);


        horizontalLayout->addLayout(verticalLayout_3);


        retranslateUi(knob);

        QMetaObject::connectSlotsByName(knob);
    } // setupUi

    void retranslateUi(QWidget *knob)
    {
        knob->setWindowTitle(QApplication::translate("knob", "Form", 0, QApplication::UnicodeUTF8));
        name->setText(QApplication::translate("knob", "Text", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("knob", "Value", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class knob: public Ui_knob {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KNOB_H
