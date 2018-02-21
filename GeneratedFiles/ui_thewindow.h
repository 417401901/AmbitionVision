/********************************************************************************
** Form generated from reading UI file 'thewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_THEWINDOW_H
#define UI_THEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TheWindowClass
{
public:

    void setupUi(QWidget *TheWindowClass)
    {
        if (TheWindowClass->objectName().isEmpty())
            TheWindowClass->setObjectName(QStringLiteral("TheWindowClass"));
        TheWindowClass->resize(600, 400);

        retranslateUi(TheWindowClass);

        QMetaObject::connectSlotsByName(TheWindowClass);
    } // setupUi

    void retranslateUi(QWidget *TheWindowClass)
    {
        TheWindowClass->setWindowTitle(QApplication::translate("TheWindowClass", "TheWindow", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TheWindowClass: public Ui_TheWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_THEWINDOW_H
