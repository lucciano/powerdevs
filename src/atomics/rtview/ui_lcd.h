/********************************************************************************
** Form generated from reading UI file 'lcd.ui'
**
** Created: Tue Oct 2 11:38:46 2012
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LCD_H
#define UI_LCD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLCDNumber>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_lcd
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_4;
    QLabel *name;
    QLCDNumber *value;

    void setupUi(QWidget *lcd)
    {
        if (lcd->objectName().isEmpty())
            lcd->setObjectName(QString::fromUtf8("lcd"));
        lcd->resize(400, 300);
        horizontalLayout = new QHBoxLayout(lcd);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        name = new QLabel(lcd);
        name->setObjectName(QString::fromUtf8("name"));
        name->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(name);

        value = new QLCDNumber(lcd);
        value->setObjectName(QString::fromUtf8("value"));
        value->setMinimumSize(QSize(50, 50));

        verticalLayout_4->addWidget(value);


        horizontalLayout->addLayout(verticalLayout_4);


        retranslateUi(lcd);

        QMetaObject::connectSlotsByName(lcd);
    } // setupUi

    void retranslateUi(QWidget *lcd)
    {
        lcd->setWindowTitle(QApplication::translate("lcd", "Form", 0, QApplication::UnicodeUTF8));
        name->setText(QApplication::translate("lcd", "Text", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class lcd: public Ui_lcd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LCD_H
