/********************************************************************************
** Form generated from reading UI file 'rtview.ui'
**
** Created: Tue Oct 2 11:38:46 2012
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RTVIEW_H
#define UI_RTVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RtView
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *topLayout;
    QSpacerItem *verticalSpacer_2;
    QHBoxLayout *bottomLayout;
    QSpacerItem *verticalSpacer;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RtView)
    {
        if (RtView->objectName().isEmpty())
            RtView->setObjectName(QString::fromUtf8("RtView"));
        RtView->resize(600, 400);
        centralWidget = new QWidget(RtView);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, -1, 0, -1);
        topLayout = new QHBoxLayout();
        topLayout->setSpacing(6);
        topLayout->setObjectName(QString::fromUtf8("topLayout"));
        topLayout->setContentsMargins(-1, 10, -1, -1);

        verticalLayout->addLayout(topLayout);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        bottomLayout = new QHBoxLayout();
        bottomLayout->setSpacing(6);
        bottomLayout->setObjectName(QString::fromUtf8("bottomLayout"));
        bottomLayout->setContentsMargins(-1, 10, -1, -1);

        verticalLayout->addLayout(bottomLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        RtView->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(RtView);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        RtView->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(RtView);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        RtView->setStatusBar(statusBar);

        retranslateUi(RtView);

        QMetaObject::connectSlotsByName(RtView);
    } // setupUi

    void retranslateUi(QMainWindow *RtView)
    {
        RtView->setWindowTitle(QApplication::translate("RtView", "PowerDEVS RTView", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RtView: public Ui_RtView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RTVIEW_H
