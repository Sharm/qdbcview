#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qdbcinstance.h"
//QT
#include <QtGui/QMainWindow>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QItemSelection>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void fileClosed();
    void fileOpened(QDBCFile* f);
    void fileOpenFailed(QDBCFile* f, qint32 errorCorrection = 0);
    void fileDataDetectionBegin(QDBCFile* f);
    void fileDataDetectionFailed(QDBCFile* f);
    void fileDataDetectionProgress(quint32 step);
    void fileDataDetectionEnd(QDBCFile* f);
    void fileDataTableModelReady(QDBCTableModel* m);
    void widgetDataQueryResponse(quint32 row, quint32 column, DBCFieldType type, QString dec, QString hex, QString bin, QString mask, QString f, QString b, QString s);

signals:
    void beginFileDataDetection(QDBCFile* f);
    void queryDataForWidget(QItemSelection selected);
    void openFile(const QString &f, qint32 errorCorrection = 0);
    void closeFile();

private:
    Ui::MainWindow *ui;

private slots:
    void fileOpen();
    void fileQuit();
    void about();
    void tableSelectionChanged(QItemSelection selected, QItemSelection deselected);
};

#endif // MAINWINDOW_H
