#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fileOpen()
{
    QString fn = QFileDialog::getOpenFileName(this,tr("Open DBC"),QString(),tr("WoW Client Database (*.dbc);;All Files (*)"));
    if (!fn.isEmpty())
        emit openFile(fn);
}

void MainWindow::fileQuit()
{
    this->close();
}

void MainWindow::about()
{
    QMessageBox::about(this,tr("About QDBCView"),tr("<b>QDBCView</b> is a simple DBC viewing tool based on Qt library."));
}

void MainWindow::tableSelectionChanged(QItemSelection selected, QItemSelection deselected)
{
    emit queryDataForWidget(selected);
}

void MainWindow::widgetDataQueryResponse(quint32 row, quint32 column, DBCFieldType type, QString dec, QString hex, QString bin, QString mask, QString f, QString b, QString s)
{
    ui->dockWidgetUi->groupBox->setTitle(tr("DBC Cell").append(" X: %1, Y: %2, ").arg(row).arg(column).append(tr("DBC data type").append(": (%3)").arg(type)));
    ui->dockWidgetUi->lineEditDec->setText(dec);
    ui->dockWidgetUi->lineEditHex->setText(hex);
    ui->dockWidgetUi->lineEditBin->setText(bin);
    ui->dockWidgetUi->lineEditMask->setText(mask);
    ui->dockWidgetUi->lineEditFloat->setText(f);
    ui->dockWidgetUi->lineEditBool->setText(b);
    ui->dockWidgetUi->text->setPlainText(s);
}

void MainWindow::fileDataDetectionBegin(QDBCFile* f)
{
    ui->progressDialogDetection->setMaximum(f->fieldCount());
}

void MainWindow::fileDataDetectionFailed(QDBCFile* f)
{
    ui->progressDialogDetection->setValue(f->fieldCount());
    switch(f->errorCode()) {
        case DBCFILE_ERROR_AUTO_DETECT_FAILED_INVALID_FIELD:
            QMessageBox::critical(this,tr("Fatal Error"),tr("Failed to perform automatic analysis of data types in the table due to invalid field numbers."));
            break;
        case DBCFILE_ERROR_AUTO_DETECT_FAILED_UNK_FIELD_SIZE:
            QMessageBox::critical(this,tr("Fatal Error"),tr("Failed to perform automatic analysis of data types in the table due to unsupported field size."));
            break;
        default:
            QMessageBox::critical(this,tr("Fatal Error"),tr("Failed to perform automatic analysis of data types in the table due to unknown issue."));
    }
    emit closeFile();
}

void MainWindow::fileDataDetectionProgress(quint32 step)
{
    ui->progressDialogDetection->setValue(step);
}

void MainWindow::fileDataDetectionEnd(QDBCFile* f)
{
    ui->progressDialogDetection->setValue(f->fieldCount());
}

void MainWindow::fileDataTableModelReady(QDBCTableModel *m)
{
    ui->tableView->setModel(m->getProxy());
    ui->tableView->resizeRowsToContents();
    ui->tableView->resizeColumnsToContents();
    ui->dockWidget->show();
    this->connect(ui->tableView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                  this,SLOT(tableSelectionChanged(QItemSelection,QItemSelection)));
}

void MainWindow::fileOpened(QDBCFile* f)
{
    this->setWindowTitle(tr("QDBCView").append(" - %1").arg(f->file()->fileName().split("/").last()));
    emit beginFileDataDetection(f);
}

void MainWindow::fileOpenFailed(QDBCFile *f, qint32 errorCorrection)
{
    QString fn = f->file()->fileName();
    switch(f->errorCode()) {
        case DBCFILE_ERROR_FILE_DOES_NOT_EXIST:
            QMessageBox::critical(this,tr("Fatal Error"),tr("This file does not exist."));
            break;
        case DBCFILE_ERROR_FAILED_TO_OPEN_A_FILE:
            QMessageBox::critical(this,tr("Fatal Error"),tr("Failed to access the file, code %1").arg(f->file()->error()));
            break;
        case DBCFILE_ERROR_FILE_IS_EMPTY:
            QMessageBox::critical(this,tr("Fatal Error"),tr("This file is empty."));
            break;
        case DBCFILE_ERROR_FILE_IS_TOO_SMALL:
            QMessageBox::critical(this,tr("Fatal Error"),tr("This file is too small to be a DBC file. If it really is, then it may be corrupted or invalid."));
            break;
        case DBCFILE_ERROR_FILE_IS_NOT_A_DBC_FILE:
            QMessageBox::critical(this,tr("Fatal Error"),tr("This file is not a DBC file."));
            break;
        case DBCFILE_ERROR_INVALID_DBC_TABLE_PARAMETERS:
            QMessageBox::critical(this,tr("Fatal Error"),tr("This file has invalid DBC table parameters. Probably, it is corrupted."));
            break;
        case DBCFILE_ERROR_UNUSUAL_RECORD_SIZE:
        {
            switch(QMessageBox::warning(this,tr("Warning"),tr("This file has unusual DBC structure. Average field size is less or greater than default size (4 bytes). This file may be corrupted.\n\nDo you really want to try open this file with given field sizes?"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No)) {
                case QMessageBox::Yes:
                    errorCorrection = (errorCorrection | DBCFILE_IGNORE_FIELD_SIZE);
                    break;
                default:
                    break;
            }
            break;
        }
        case DBCFILE_ERROR_RECORD_DATA_IS_EMPTY:
            QMessageBox::critical(this,tr("Fatal Error"),tr("This file has empty DBC entries, probably, it is corrupted."));
            break;
        case DBCFILE_ERROR_STRING_DATA_IS_EMPTY:
        {
            switch(QMessageBox::warning(this,tr("Warning"),tr("This file has empty string data. This is not normal, because it's size must be at least 1 byte. This file may be corrupted.\n\nDo you really want to try open this file ignoring the empty strings block?"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No)) {
                case QMessageBox::Yes:
                    errorCorrection = (errorCorrection | DBCFILE_IGNORE_EMPTY_STRING_BLOCK);
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            QMessageBox::critical(this,tr("Fatal Error"),tr("Failed to open the file due to an unknown error, code # %1").arg(f->errorCode()));
    }
    emit closeFile();
    if(errorCorrection != 0)
        emit openFile(fn,errorCorrection);
}

void MainWindow::fileClosed()
{
    this->setWindowTitle(tr("QDBCView"));
    ui->tableView->setModel(NULL);
    ui->dockWidget->hide();
    this->disconnect(ui->tableView->selectionModel(),SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                     this,SLOT(tableSelectionChanged(QItemSelection,QItemSelection)));
}
