#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressDialog>
#include <QtGui/QStatusBar>
#include <QtGui/QTreeView>
#include <QtGui/QTableView>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
//another UI
#include "ui_dockwidget.h"

const QString rsrcPath = ":/images";

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QMenuBar *menuBar;
    QMenu *menuBarFileMenu;
    QMenu *menuBarHelpMenu;
    QMenu *menuBarEditMenu;
    QTableView *tableView;
    QDockWidget *dockWidget;
    QToolBar *toolBar;
    QStatusBar *statusBar;
    QAction *actionFileOpen;
    QAction *actionFileQuit;
    QAction *actionAbout;
    QAction *actionAboutQt;
    QProgressDialog *progressDialogDetection;
    Ui::dockWidgetCellData *dockWidgetUi;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));

        MainWindow->resize(800, 600);

        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBarFileMenu = menuBar->addMenu(MainWindow->tr("&File"));
        menuBarEditMenu = menuBar->addMenu(MainWindow->tr("&Edit"));
        menuBarHelpMenu = menuBar->addMenu(MainWindow->tr("&Help"));
        MainWindow->setMenuBar(menuBar);

        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        toolBar->setWindowTitle(MainWindow->tr("Toolbar"));
        toolBar->setFloatable(false);
        toolBar->setMovable(false);
        MainWindow->addToolBar(Qt::TopToolBarArea,toolBar);

        dockWidget = new QDockWidget(MainWindow);
        dockWidgetUi = new Ui::dockWidgetCellData();
        dockWidgetUi->setupUi(dockWidget);
        MainWindow->addDockWidget(Qt::BottomDockWidgetArea,dockWidget);
        dockWidget->setWindowTitle(MainWindow->tr("Cell data"));
        dockWidget->hide();

        tableView = new QTableView(MainWindow);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setSortingEnabled(true);
        tableView->setCornerButtonEnabled(false);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
        MainWindow->setCentralWidget(tableView);

        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        progressDialogDetection = new QProgressDialog(MainWindow->tr("Performing automatic analysis of data types in the table..."),QString(),0,0,MainWindow);
        progressDialogDetection->setObjectName(QString::fromUtf8("progressDialogDetection"));
        progressDialogDetection->setWindowTitle(MainWindow->tr("DBC data analysis"));
        progressDialogDetection->setMinimumDuration(2000);
        progressDialogDetection->setAutoClose(true);
        progressDialogDetection->setAutoReset(true);

        actionFileOpen = new QAction(QIcon(rsrcPath+"/fileopen.png"),MainWindow->tr("&Open DBC..."),MainWindow);
        actionFileOpen->setStatusTip(MainWindow->tr("Open an existing DBC file"));
        actionFileOpen->setShortcut(QKeySequence::Open);
        MainWindow->connect(actionFileOpen,SIGNAL(triggered()),MainWindow,SLOT(fileOpen()));

        actionFileQuit = new QAction(MainWindow->tr("E&xit"),MainWindow);
        actionFileQuit->setStatusTip(MainWindow->tr("Exit the application"));
        actionFileQuit->setShortcut(QKeySequence::Quit);
        actionFileQuit->setMenuRole(QAction::QuitRole);
        MainWindow->connect(actionFileQuit,SIGNAL(triggered()),MainWindow,SLOT(fileQuit()));

        actionAbout = new QAction(MainWindow->tr("&About"),MainWindow);
        actionAbout->setStatusTip(MainWindow->tr("Show the application's About box"));
        MainWindow->connect(actionAbout,SIGNAL(triggered()),MainWindow,SLOT(about()));

        actionAboutQt = new QAction(MainWindow->tr("About &Qt"),MainWindow);
        actionAboutQt->setStatusTip(MainWindow->tr("Show the Qt library's About box"));
        MainWindow->connect(actionAboutQt,SIGNAL(triggered()),qApp,SLOT(aboutQt()));

        menuBarFileMenu->addAction(actionFileOpen);
        menuBarFileMenu->addSeparator();
        menuBarFileMenu->addAction(actionFileQuit);
        menuBarHelpMenu->addAction(actionAbout);
        menuBarHelpMenu->addAction(actionAboutQt);
        toolBar->addAction(actionFileOpen);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi
};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
