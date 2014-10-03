#ifndef QDBCTABLEMODEL_H
#define QDBCTABLEMODEL_H

#include "qdbcfile.h"
//QT
#include <QtCore/QAbstractTableModel>
#include <QtGui/QSortFilterProxyModel>

class QDBCTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    QDBCTableModel(QObject *parent = 0);
    ~QDBCTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    QSortFilterProxyModel* getProxy() const   { return m_proxyModel; }
    QDBCFile*              getDBCFile() const { return m_dbcfile; }
    void                   setDBCFile(QDBCFile* f);

private:
    QSortFilterProxyModel *m_proxyModel;
    QDBCFile              *m_dbcfile;
};

#endif // QDBCTABLEMODEL_H
