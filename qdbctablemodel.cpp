#include "qdbctablemodel.h"

QDBCTableModel::QDBCTableModel(QObject *parent): QAbstractTableModel(parent)
{
    m_dbcfile = NULL;
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(this);
}

QDBCTableModel::~QDBCTableModel()
{
    m_dbcfile = NULL;
}

QVariant QDBCTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole && index.isValid() && m_dbcfile != 0) {
        switch(m_dbcfile->fieldType(index.column())) {
        case FIELD_TYPE_FLOAT:
            return m_dbcfile->getFloat(index.column(),index.row());
        case FIELD_TYPE_INT:
            return m_dbcfile->getInt(index.column(),index.row());
        case FIELD_TYPE_TEXT:
        case FIELD_TYPE_NULLTEXT:
            return m_dbcfile->getString(index.column(),index.row());
        case FIELD_TYPE_BOOL:
            return m_dbcfile->getBool(index.column(),index.row());
        case FIELD_TYPE_BITMASK:
            return m_dbcfile->getBitmaskTextRepresentation(index.column(),index.row());
        case FIELD_TYPE_UNKNOWN:
            return QString("UNKNOWN");
        default:
            return m_dbcfile->getUInt(index.column(),index.row());
        }
    }
    return QVariant();
}

QVariant QDBCTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        return QString("%1").arg(section);
    return QVariant();
}

Qt::ItemFlags QDBCTableModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return Qt::ItemIsEnabled;
}

int QDBCTableModel::rowCount(const QModelIndex &parent) const
{
    if(m_dbcfile != 0)
        return m_dbcfile->recordCount();
    return 0;
}

int QDBCTableModel::columnCount(const QModelIndex &parent) const
{
    if(m_dbcfile != 0)
        return m_dbcfile->fieldCount();
    return 0;
}

void QDBCTableModel::setDBCFile(QDBCFile *f)
{
    m_dbcfile = f;
    m_proxyModel->setSourceModel(this);
}
