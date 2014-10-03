#include "qdbcfile.h"

QDBCFile::QDBCFile(const QString &fn, QObject *parent) :
    QObject(parent)
{
    m_fieldSize = 0;
    m_recordSize = 0;
    m_recordCount = 0;
    m_fieldCount = 0;
    m_stringSize = 0;
    m_file = new QFile(fn,this);
    m_errorCode = DBCFILE_ERROR_OK;
}

QDBCFile::~QDBCFile()
{
}

quint32 QDBCFile::dataOffset(quint32 field, quint32 row) const
{
    return (row*m_recordSize)+(field*m_fieldSize);
}

float QDBCFile::getFloat(quint32 field, quint32 row) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0 && m_fieldSize == 4)
        return qFromLittleEndian<float>(*reinterpret_cast<const float*>(m_recordData.data()+dataOffset(field,row)));
    return 0.0f;
}

qint32 QDBCFile::getInt(quint32 field, quint32 row) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0) {
        switch(m_fieldSize) {
        case 1:
            return qFromLittleEndian<qint8>(*reinterpret_cast<const qint8*>(m_recordData.data()+dataOffset(field,row)));
        case 2:
            return qFromLittleEndian<qint16>(*reinterpret_cast<const qint16*>(m_recordData.data()+dataOffset(field,row)));
        case 4:
            return qFromLittleEndian<qint32>(*reinterpret_cast<const qint32*>(m_recordData.data()+dataOffset(field,row)));
        default:
            break;
        }
    }
    return 0;
}

quint32 QDBCFile::getUInt(quint32 field, quint32 row) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0) {
        switch(m_fieldSize) {
        case 1:
            return qFromLittleEndian<quint8>(*reinterpret_cast<const quint8*>(m_recordData.data()+dataOffset(field,row)));
        case 2:
            return qFromLittleEndian<quint16>(*reinterpret_cast<const quint16*>(m_recordData.data()+dataOffset(field,row)));
        case 4:
            return qFromLittleEndian<quint32>(*reinterpret_cast<const quint32*>(m_recordData.data()+dataOffset(field,row)));
        default:
            break;
        }
    }
    return 0;
}

bool QDBCFile::getBool(quint32 field, quint32 row) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0)
        return (getUInt(field,row) != 0);
    return false;
}

unsigned char QDBCFile::getUChar(quint32 offset) const
{
    if(offset >= 0 && offset < m_stringSize)
        return *reinterpret_cast<const uchar*>(m_stringData.data()+offset);
    return 0;
}

QString QDBCFile::getString(quint32 field, quint32 row) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0) {
        if(getUInt(field,row) < (quint32)m_stringSize)
            return QString::fromUtf8(reinterpret_cast<const char*>(m_stringData.data()+getUInt(field,row)));
    }
    return QString();
}
bool QDBCFile::hasFullString(quint32 field, quint32 row) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0 && m_stringSize > 1) {
        if(getUInt(field,row) < m_stringSize)
            if(getUChar(getUInt(field,row)-1)=='\0' && field > 0)
                return true;
    }
    return false;
}

bool QDBCFile::hasUsualFloat(quint32 field, quint32 row) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0 && m_fieldSize == 4) {
        if(getFloat(field,row) == getFloat(field,row) &&
           QVariant(getFloat(field,row)).toString().count(QString::fromUtf8("e"),Qt::CaseInsensitive) < 1) {
            return true;
        }
    }
    return false;
}

bool QDBCFile::hasInteger(quint32 field, quint32 row, bool isSigned) const
{
    if(field < m_fieldCount && row < m_recordCount && field >= 0 && row >= 0) {
        QVariant i;
        QVariant u;
        switch(m_fieldSize) {
        case 1:
        case 2:
        case 4:
            {
                i = getInt(field,row);
                u = getUInt(field,row);
                break;
            }
        default:
            return false;
        }
        if(isSigned) {
            if(i != u)
                return true;
        }
        else {
            if(i == u)
                return true;
        }
    }
    return false;
}

QString QDBCFile::getIntegerTextRepresentation(quint32 field, quint32 row, int base) const
{
    switch(fieldType(field)) {
    case FIELD_TYPE_INT:
        return QString::number(getInt(field,row),base);
    default:
        return QString::number(getUInt(field,row),base);
    }
}

QString QDBCFile::getBitmaskTextRepresentation(quint32 field, quint32 row, BitmaskRepresentation view) const
{
    switch(view) {
    case BITMASK_VIEW_COMMA_SEPARATED_POSITIVE_BITS:
    case BITMASK_VIEW_COMMA_SEPARATED_POSITIVE_BITS_ALT:
        {
            quint32 index = 0;
            QString str = QString("Bitflags: ");
            if(fieldType(field) == FIELD_TYPE_INT) {
                qint32 i = getInt(field,row);
                if(i == 0)
                    return str.append("Empty.");
                if(i == 0x7FFFFFFF || i < -0x7FFFFFFF)
                    return str.append("All.");
                while(i != 0 || index <= BITMASK_MAX_BIT) {
                    qint32 value = qPow(2,index);
                    if((i & value) == value) {
                        i = (i & ~value);
                        (view == BITMASK_VIEW_COMMA_SEPARATED_POSITIVE_BITS) ? str.append(QString::number(index)) : str.append(QString::number(index+1));
                        if(i != 0)
                            str.append(",");
                        else
                            return str.append(".");
                    }
                    index++;
                }
            }
            else {
                quint32 u = getUInt(field,row);
                if(u == 0)
                    return str.append("Empty.");
                if(u == 0xffffffff)
                    return str.append("All.");
                while(u != 0 || index <= BITMASK_MAX_BIT) {
                    quint32 value = qPow(2,index);
                    if((u & value) == value) {
                        u = (u & ~value);
                        (view == BITMASK_VIEW_COMMA_SEPARATED_POSITIVE_BITS) ? str.append(QString::number(index)) : str.append(QString::number(index+1));
                        if(u != 0)
                            str.append(",");
                        else
                            return str.append(".");
                    }
                    index++;
                }
            }
        }
    }
    return QString();
}

bool QDBCFile::detectFieldType(quint32 field)
{
    if(field < m_fieldCount && field >= 0) {
        if(field == 0) {
            //the first field seems to be always uint32, so set it asap to save some time
            setFieldType(field,FIELD_TYPE_ID);
            setFieldTypeSQL(field,FIELD_TYPE_SQL_ID);
            return true;
        }
        quint32 strings = 0;
        quint32 floats = 0;
        quint32 uints = 0;
        quint32 ints = 0;
        quint32 nulls = 0;
        quint32 ones = 0;
        quint32 biguints = 0;
        quint32 maxuints = 0;
        quint32 timeuints = 0;
        quint32 bitvaluints = 0;
        quint32 bigints = 0;
        quint32 maxints = 0;
        quint32 timeints = 0;
        quint32 bitvalints = 0;
        quint32 unk = 0;
        //
        DBCFieldType fieldtype = FIELD_TYPE_DEFAULT;
        SQLFieldType fieldtypesql = FIELD_TYPE_SQL_DEFAULT;
        quint8 uintsizelevel = 0;
        quint8 intsizelevel = 0;
        quint8 textsizelevel = 0;
        /*
        sql field size level:
        0 - tiny (tinyint, tinytext)
        1 - small (smallint, text)
        2 - medium (mediumint, mediumtext)
        3 - large (int, longtext)
        */

        for(quint32 record=0; record < m_recordCount; record++) {
            switch(m_fieldSize) {
            case 1:
            case 2:
            case 4:
                {
                    if(hasUsualFloat(field,record)) {
                        float f = getFloat(field,record);
                        if(f != 0.0f)
                            floats++;
                        else
                            nulls++;
                    }
                    else if(hasFullString(field,record)) {
                        strings++;
                        if(getInt(field,record) == 1)
                            ones++;
                        //
                        if(getString(field,record).size() > SQL_TINYTEXT && textsizelevel < 1)
                            textsizelevel = 1;
                        if(getString(field,record).size() > SQL_TEXT && textsizelevel < 2)
                            textsizelevel = 2;
                        if(getString(field,record).size() > SQL_MEDIUMTEXT && textsizelevel < 3)
                            textsizelevel = 3;
                    }
                    else if(hasInteger(field,record,false)) {
                        quint32 u = getUInt(field,record);
                        quint32 uMinusOne = u-1;
                        bool isDegreeOfTwo = !(uMinusOne&u);
                        if(u == 0)
                            nulls++;
                        else
                            uints++;
                        if(u == 1)
                            ones++;
                        /*
                    BITMASK DETECTION TEMPORARY HACKS: wild bitmask guessing. Test in progress.
                    The insteresting thing about it is the fact that blizzards used same shit
                    themselves at least once. Current precision is about 60-70%.
                    */
                        if(u > qPow(2,19)) {
                            biguints++;
                            if(u%10 == 0)
                                timeuints++;
                        }
                        if(u == 0xffffffff)
                            maxuints++;
                        if(isDegreeOfTwo && u > 8)
                            bitvaluints++;
                        //
                        if(u > SQL_TINYINT_UNSIGNED_HI && uintsizelevel < 1)
                            uintsizelevel = 1;
                        if(u > SQL_SMALLINT_UNSIGNED_HI && uintsizelevel < 2)
                            uintsizelevel = 2;
                        if(u > SQL_MEDIUMINT_UNSIGNED_HI && uintsizelevel < 3)
                            uintsizelevel = 3;
                    }
                    else if(hasInteger(field,record,true)) {
                        qint32 i = getInt(field,record);
                        quint32 absI = abs(i);
                        quint32 absIMinusOne = absI-1;
                        bool isDegreeOfTwo = !(absIMinusOne&absI);
                        if(i == 0)
                            nulls++;
                        else
                            ints++;
                        if(i == 1)
                            ones++;
                        /*
                    BITMASK DETECTION TEMPORARY HACKS: wild bitmask guessing. Test in progress.
                    The insteresting thing about it is the fact that blizzards used same shit
                    themselves at least once. Current precision is about 60-70%.
                    */
                        if(i == -1 || i == -2)
                            maxuints++;
                        if(absI > qPow(2,19)) {
                            bigints++;
                            if(absI%10 == 0)
                                timeints++;
                        }
                        if(absI >= 0x7FFFFFFF)
                            maxints++;
                        if(isDegreeOfTwo && absI > 8)
                            bitvalints++;
                        //
                        if((i < SQL_TINYINT_LOW || i > SQL_TINYINT_HI) && intsizelevel < 1)
                            intsizelevel = 1;
                        if((i < SQL_SMALLINT_LOW || i > SQL_SMALLINT_HI) && intsizelevel < 2)
                            intsizelevel = 2;
                        if((i < SQL_MEDIUMINT_LOW || i > SQL_MEDIUMINT_HI) && intsizelevel < 3)
                            intsizelevel = 3;
                    }
                    else {
                        //OMG WTF?!
                        unk++;
                    }
                    break;
                }
            default:
                //Unsupported field size, make error and get out of here
                return makeDetectionError(DBCFILE_ERROR_AUTO_DETECT_FAILED_UNK_FIELD_SIZE,field);
            }
        }
        //Auto-Detecting Generic Field Type...
       if( unk > 0 )
           //OMG WTF?! Normally this thing should NEVER EVER happen.
           fieldtype = FIELD_TYPE_UNKNOWN;
       else if( ones + nulls == m_recordCount && ones > 0 && nulls > 0 )
           fieldtype = FIELD_TYPE_BOOL;
       else if( strings + nulls == m_recordCount && strings > 0 )
           fieldtype = FIELD_TYPE_TEXT;
       else if( nulls == m_recordCount
                && (fieldType(field-1) == FIELD_TYPE_TEXT || fieldType(field-1) == FIELD_TYPE_NULLTEXT))
           //TODO: add some field count correction? Got one more excess column with nulls as nulltext somewhere.
           fieldtype = FIELD_TYPE_NULLTEXT;
       else if( nulls == m_recordCount )
           fieldtype = FIELD_TYPE_NULLS;
       else if( floats + nulls == m_recordCount
                || (floats > m_recordCount - nulls - floats && floats > 0))
           fieldtype = FIELD_TYPE_FLOAT;
       else if( (biguints > uints*0.10f && biguints != timeuints && biguints > 1)
                || (bitvaluints == uints && bitvaluints != ones  && (ones <= uints*0.09f || biguints > 0) && (ints == 0 || (uints > ints && ints == maxuints)) && m_recordCount > 1 && uints > 0)
                || (bitvaluints >= uints*0.30f && strings < uints*0.09f && bitvaluints != ones  && (ones <= uints*0.09f || biguints > 0) && (ints == 0 || (uints > ints && ints == maxuints)) && m_recordCount > 1 && uints > 0)
                || (bigints > ints*0.10f && bigints != timeints && bigints > 1)
                || (bitvalints == ints && bitvalints != ones  && (ones <= ints*0.09f || bigints > 0) && (uints == 0 || (ints > uints && uints == maxuints)) && m_recordCount > 1 && ints > 0)
                || (bitvalints >= ints*0.30f && strings < ints*0.09f && bitvalints != ones  && (ones <= ints*0.09f || bigints > 0) && (uints == 0 || (ints > uints && uints == maxuints)) && m_recordCount > 1 && ints > 0))
           fieldtype = FIELD_TYPE_BITMASK;//Yeah, i know. xD
       else if( ints == 0 || (uints > ints && ints == maxuints && maxuints > uints*0.30f) )
           fieldtype = FIELD_TYPE_UINT;
       else
           fieldtype = FIELD_TYPE_INT;

       switch(fieldtype) {
        case FIELD_TYPE_TEXT:
            switch(textsizelevel) {
            case 0:
                fieldtypesql = FIELD_TYPE_SQL_TINYTEXT;
                break;
            case 1:
                fieldtypesql = FIELD_TYPE_SQL_TEXT;
                break;
            case 2:
                fieldtypesql = FIELD_TYPE_SQL_MEDIUMTEXT;
                break;
            default:
                fieldtypesql = FIELD_TYPE_SQL_LONGTEXT;
            }
            break;
        case FIELD_TYPE_UINT:
            switch(uintsizelevel) {
            case 0:
                fieldtypesql = FIELD_TYPE_SQL_TINYINT_UNSIGNED;
                break;
            case 1:
                fieldtypesql = FIELD_TYPE_SQL_SMALLINT_UNSIGNED;
                break;
            case 2:
                fieldtypesql = FIELD_TYPE_SQL_MEDIUMINT_UNSIGNED;
                break;
            default:
                fieldtypesql = FIELD_TYPE_SQL_INT_UNSIGNED;
            }
            break;
        case FIELD_TYPE_INT:
            switch(intsizelevel) {
            case 0:
                fieldtypesql = FIELD_TYPE_SQL_TINYINT;
                break;
            case 1:
                fieldtypesql = FIELD_TYPE_SQL_SMALLINT;
                break;
            case 2:
                fieldtypesql = FIELD_TYPE_SQL_MEDIUMINT;
                break;
            default:
                fieldtypesql = FIELD_TYPE_SQL_INT;
            }
            break;
        case FIELD_TYPE_FLOAT:
            fieldtypesql = FIELD_TYPE_SQL_FLOAT;
            break;
        case FIELD_TYPE_BOOL:
            fieldtypesql = FIELD_TYPE_SQL_BOOL;
            break;
        case FIELD_TYPE_NULLS:
            fieldtypesql = FIELD_TYPE_SQL_TINYINT_UNSIGNED;
            break;
        case FIELD_TYPE_NULLTEXT:
            fieldtypesql = FIELD_TYPE_SQL_TINYTEXT;
            break;
        case FIELD_TYPE_BITMASK:
            fieldtypesql = FIELD_TYPE_SQL_BITMASK;
            break;
        case FIELD_TYPE_UNKNOWN:
            fieldtypesql = FIELD_TYPE_SQL_UNKNOWN;
            break;
        }
        setFieldType(field,fieldtype);
        setFieldTypeSQL(field,fieldtypesql);
        return true;
    }
    //OK, we are totally screwed if we are right here
    return makeDetectionError(DBCFILE_ERROR_AUTO_DETECT_FAILED_INVALID_FIELD,field);
}

bool QDBCFile::openFile(qint32 errorCorrection)
{
    qDebug() << "------------------------";
    qDebug() << "New file open request!";
    qDebug() << "Error correction code:" << errorCorrection;
    qDebug() << "File path:" << m_file->fileName();

    if(!m_file->exists())
        return makeFileError(DBCFILE_ERROR_FILE_DOES_NOT_EXIST,false);
    if(!m_file->open(QIODevice::ReadOnly))
        return makeFileError(DBCFILE_ERROR_FAILED_TO_OPEN_A_FILE,false);

    qDebug() << "File size:" << m_file->size() << "bytes";

    if(m_file->size() < 1)
        return makeFileError(DBCFILE_ERROR_FILE_IS_EMPTY,false);
    if(m_file->size() < 20)
        return makeFileError(DBCFILE_ERROR_FILE_IS_TOO_SMALL,false);

    quint32 header;
    QDataStream fstream(m_file);
    fstream.setByteOrder(QDataStream::LittleEndian);
    fstream >> header >> m_recordCount >> m_fieldCount >> m_recordSize >> m_stringSize;
    m_fieldSize  = m_recordSize/m_fieldCount;

    qDebug() << "File header:" << header << " expected:" << DBC_HEADER;

    if(header != DBC_HEADER)
        return makeFileError(DBCFILE_ERROR_FILE_IS_NOT_A_DBC_FILE,false);

    qDebug() << "Records:" << m_recordCount;
    qDebug() << "Fields:" << m_fieldCount;
    qDebug() << "Size of a field:" << m_fieldSize;
    qDebug() << "Size of a record:" << m_recordSize;
    qDebug() << "Size of strings:" << m_stringSize;

    if(m_recordCount < 1 || m_fieldCount < 1 || m_recordSize < 1 || m_stringSize < 1)
        return makeFileError(DBCFILE_ERROR_INVALID_DBC_TABLE_PARAMETERS,fstream.atEnd());
    // The fieldSize is not always == 4, but it rather unusual
    if(m_fieldSize != 4 && !hasErrorCorrectionFlag(errorCorrection,DBCFILE_IGNORE_FIELD_SIZE))
        return makeFileError(DBCFILE_ERROR_UNUSUAL_RECORD_SIZE,fstream.atEnd());

    m_recordData.resize(m_recordSize*m_recordCount);
    m_stringData.resize(m_stringSize);
    qint64 readRecords = fstream.readRawData(m_recordData.data(),m_recordSize*m_recordCount);
    qint64 readStrings = fstream.readRawData(m_stringData.data(),m_stringSize);

    qDebug() << "Records data read:" << readRecords << "bytes";
    qDebug() << "Strings data read:" << readStrings << "bytes";

    if(readRecords < 1)
        return makeFileError(DBCFILE_ERROR_RECORD_DATA_IS_EMPTY,fstream.atEnd());
    if(readStrings < 1 && !hasErrorCorrectionFlag(errorCorrection,DBCFILE_IGNORE_EMPTY_STRING_BLOCK))
        return makeFileError(DBCFILE_ERROR_STRING_DATA_IS_EMPTY,fstream.atEnd());

    qDebug() << "Reached the end of file:" << fstream.atEnd();
    qDebug() << "------------------------";

    m_file->close();
    for (quint32 column = 0; column < m_fieldCount; column++) {
            m_fieldTypesList.append(FIELD_TYPE_DEFAULT);
            m_fieldSQLTypesList.append(FIELD_TYPE_SQL_DEFAULT);
     }
    return true;
}

void QDBCFile::closeFile()
{
    qDebug() << "------------------------";
    qDebug() << "Closing file:" << m_file->fileName();

    if(m_errorCode != DBCFILE_ERROR_OK)
        qDebug() << "File had the error #" << m_errorCode << "occured before closing.";

    qDebug() << "------------------------";

    delete this;
}
