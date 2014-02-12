/********************************************************************************
** Form generated from reading UI file 'convex.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONVEX_H
#define UI_CONVEX_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>
#include "glviewer.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionClear;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    GlViewer *viewer;
    QStatusBar *statusbar;
    QMenuBar *menubar;
    QMenu *menuData;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(798, 629);
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionClear = new QAction(MainWindow);
        actionClear->setObjectName(QString::fromUtf8("actionClear"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("icons/fileNew.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClear->setIcon(icon);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        viewer = new GlViewer(centralwidget);
        viewer->setObjectName(QString::fromUtf8("viewer"));
        viewer->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));

        gridLayout->addWidget(viewer, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 798, 21));
        menuData = new QMenu(menubar);
        menuData->setObjectName(QString::fromUtf8("menuData"));
        MainWindow->setMenuBar(menubar);

        menubar->addAction(menuData->menuAction());
        menuData->addAction(actionClear);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "CGAL - 2D Convex hull", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
        actionQuit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionClear->setText(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionClear->setStatusTip(QApplication::translate("MainWindow", "Clear", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionClear->setShortcut(QApplication::translate("MainWindow", "Space", 0, QApplication::UnicodeUTF8));
        menuData->setTitle(QApplication::translate("MainWindow", "Data", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONVEX_H
