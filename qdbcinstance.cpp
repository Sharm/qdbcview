#include "qdbcinstance.h"

QDBCInstance::QDBCInstance(QObject *gui, QObject *parent) :
    QObject(parent)
{
    m_dbcfile = NULL;
    m_tableModel = new QDBCTableModel(this);
    m_tableModel->setDBCFile(m_dbcfile);
    this->moveToThread(&m_thread);
    m_thread.start();
    qRegisterMetaType<QItemSelection>("QItemSelection");
    qRegisterMetaType<DBCFieldType>("DBCFieldType");
    this->connect(gui,SIGNAL(beginFileDataDetection(QDBCFile*)),this,SLOT(beginFileDataDetection(QDBCFile*)));
    this->connect(gui,SIGNAL(queryDataForWidget(QItemSelection)),this,SLOT(queryDataForWidget(QItemSelection)));
    this->connect(gui,SIGNAL(openFile(QString,qint32)),this,SLOT(openFile(QString,qint32)));
    this->connect(gui,SIGNAL(closeFile()),this,SLOT(closeFile()));
    this->connect(this,SIGNAL(fileClosed()),gui,SLOT(fileClosed()));
    this->connect(this,SIGNAL(fileOpened(QDBCFile*)),gui,SLOT(fileOpened(QDBCFile*)));
    this->connect(this,SIGNAL(fileOpenFailed(QDBCFile*,qint32)),gui,SLOT(fileOpenFailed(QDBCFile*,qint32)));
    this->connect(this,SIGNAL(fileDataDetectionBegin(QDBCFile*)),gui,SLOT(fileDataDetectionBegin(QDBCFile*)));
    this->connect(this,SIGNAL(fileDataDetectionFailed(QDBCFile*)),gui,SLOT(fileDataDetectionFailed(QDBCFile*)));
    this->connect(this,SIGNAL(fileDataDetectionProgress(quint32)),gui,SLOT(fileDataDetectionProgress(quint32)));
    this->connect(this,SIGNAL(fileDataDetectionEnd(QDBCFile*)),gui,SLOT(fileDataDetectionEnd(QDBCFile*)));
    this->connect(this,SIGNAL(fileDataTableModelReady(QDBCTableModel*)),gui,SLOT(fileDataTableModelReady(QDBCTableModel*)));
    this->connect(this,SIGNAL(widgetDataQueryResponse(quint32,quint32,DBCFieldType,QString,QString,QString,QString,QString,QString,QString)),gui,SLOT(widgetDataQueryResponse(quint32,quint32,DBCFieldType,QString,QString,QString,QString,QString,QString,QString)));
}

QDBCInstance::~QDBCInstance()
{
    m_thread.exit();
    m_thread.wait();
}

void QDBCInstance::closeFile()
{
    m_dbcfile->closeFile();
    m_dbcfile = NULL;
    m_tableModel->setDBCFile(m_dbcfile);
    emit fileClosed();
}

void QDBCInstance::openFile(const QString &f, qint32 errorCorrection)
{
    if(haveOpenFile())
        closeFile();
    m_dbcfile = new QDBCFile(f,this);
    if(m_dbcfile->openFile(errorCorrection))
        emit fileOpened(m_dbcfile);
    else
        emit fileOpenFailed(m_dbcfile,errorCorrection);
}

void QDBCInstance::beginFileDataDetection(QDBCFile* f)
{
    emit fileDataDetectionBegin(f);
    for(qint32 field=0; field < (f->fieldCount()); field++) {
        if(!f->detectFieldType(field)) {
            emit fileDataDetectionFailed(f);
            return;
        }
        emit fileDataDetectionProgress(field);
    }
    emit fileDataDetectionEnd(f);
    m_tableModel->setDBCFile(f);
    emit fileDataTableModelReady(m_tableModel);
}

void QDBCInstance::queryDataForWidget(QItemSelection selected)
{
    if(haveOpenFile()) {
        QModelIndex index = m_tableModel->getProxy()->mapSelectionToSource(selected).indexes().last();
        quint32 row = index.row();
        quint32 column = index.column();
        DBCFieldType type = m_dbcfile->fieldType(column);
        QString dec = m_dbcfile->getIntegerTextRepresentation(column,row,10);
        QString hex = m_dbcfile->getIntegerTextRepresentation(column,row,16).prepend("0x");
        QString bin = m_dbcfile->getIntegerTextRepresentation(column,row,2);
        QString mask = m_dbcfile->getBitmaskTextRepresentation(column,row);
        QString f = QVariant(m_dbcfile->getFloat(column,row)).toString();
        QString b = QVariant(m_dbcfile->getBool(column,row)).toString();
        QString s = m_dbcfile->getString(column,row);
        emit widgetDataQueryResponse(row,column,type,dec,hex,bin,mask,f,b,s);
    }
}
