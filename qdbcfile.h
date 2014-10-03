#ifndef QDBCFILE_H
#define QDBCFILE_H

#include "constants.h"
#include <math.h>
//Qt
#include <QtCore/QtEndian>
#include <QtCore/QDebug>
#include <QtCore/QVariant>
#include <QtCore/QFile>
#include <QtCore/QThread>

class QDBCFile : public QObject
{
    Q_OBJECT
public:
    explicit QDBCFile(const QString &fn, QObject *parent = 0);
    ~QDBCFile();

    DBCFileError errorCode() const { return m_errorCode; }
    bool hasErrorCorrectionFlag(qint32 i, qint32 flag = 0) const { return ((i & flag) != 0); }

    QFile*  file()  const { return m_file; }

    int recordCount() const { return m_recordCount; }
    int fieldCount() const { return m_fieldCount; }

    quint32       dataOffset(quint32 field, quint32 row) const;
    float         getFloat(quint32 field, quint32 row) const;
    qint32        getInt(quint32 field, quint32 row) const;
    quint32       getUInt(quint32 field, quint32 row) const;
    bool          getBool(quint32 field, quint32 row) const;
    QString       getString(quint32 field, quint32 row) const;
    unsigned char getUChar(quint32 offset) const;

    bool hasFullString(quint32 field, quint32 row) const;
    bool hasUsualFloat(quint32 field, quint32 row) const;
    bool hasInteger(quint32 field, quint32 row, bool isSigned) const;

    QString getIntegerTextRepresentation(quint32 field, quint32 row, int base) const;
    QString getBitmaskTextRepresentation(quint32 field, quint32 row, BitmaskRepresentation view=BITMASK_VIEW_DEFAULT) const;

    DBCFieldType fieldType      (quint32 field) const              { return m_fieldTypesList[field]; }
    void         setFieldType   (quint32 field, DBCFieldType type) { m_fieldTypesList[field] = type; }
    SQLFieldType fieldTypeSQL   (quint32 field) const              { return m_fieldSQLTypesList[field]; }
    void         setFieldTypeSQL(quint32 field, SQLFieldType type) { m_fieldSQLTypesList[field] = type; }

    bool detectFieldType(quint32 field);

    bool openFile(qint32 errorCorrection);
    void closeFile();

private:
    QFile                 *m_file;
    QByteArray             m_recordData;
    QByteArray             m_stringData;
    quint32                m_recordSize;
    quint32                m_recordCount;
    quint32                m_fieldSize;
    quint32                m_fieldCount;
    quint32                m_stringSize;
    DBCFileError           m_errorCode;
    QList<DBCFieldType>    m_fieldTypesList;
    QList<SQLFieldType>    m_fieldSQLTypesList;

    bool makeFileError(DBCFileError code, bool atEnd)
    {
        m_errorCode = code;
        m_file->close();
        qDebug() << "STOP! DBC FILE READING ERROR #" << code;
        qDebug() << "Reached the end of file:" << atEnd;
        qDebug() << "------------------------";
        return false;
    }
    bool makeDetectionError(DBCFileError code, qint32 field)
    {
        m_errorCode = code;
        qDebug() << "------------------------";
        qDebug() << "STOP! FIELD TYPE AUTO-DETECTION ERROR #" << code;
        qDebug() << "Field:" << field;
        qDebug() << "------------------------";
        return false;
    }
signals:

public slots:

};

#endif // QDBCFILE_H
