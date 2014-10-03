#ifndef QDBCINSTANCE_H
#define QDBCINSTANCE_H

#include "qdbcfile.h"
#include "qdbctablemodel.h"
//Qt
#include <QtCore/QObject>
#include <QtGui/QItemSelection>

class QDBCInstance : public QObject
{
    Q_OBJECT
public:
    explicit QDBCInstance(QObject *gui, QObject *parent = 0);
    ~QDBCInstance();

    bool haveOpenFile() const { return (m_dbcfile != 0); }

signals:
    void fileClosed();
    void fileOpened(QDBCFile* f);
    void fileOpenFailed(QDBCFile* f, qint32 errorCorrection = 0);
    void fileDataDetectionBegin(QDBCFile* f);
    void fileDataDetectionFailed(QDBCFile* f);
    void fileDataDetectionProgress(quint32 step);
    void fileDataDetectionEnd(QDBCFile* f);
    void fileDataTableModelReady(QDBCTableModel* m);
    void widgetDataQueryResponse(quint32 row, quint32 column, DBCFieldType type, QString dec, QString hex, QString bin, QString mask, QString f, QString b, QString s);

public slots:
    void beginFileDataDetection(QDBCFile* f);
    void queryDataForWidget(QItemSelection selected);
    void openFile(const QString &f, qint32 errorCorrection = 0);
    void closeFile();

private:
    QDBCTableModel       *m_tableModel;
    QDBCFile             *m_dbcfile;
    QThread               m_thread;
};

#endif // QDBCINSTANCE_H
