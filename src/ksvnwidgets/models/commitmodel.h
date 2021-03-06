/***************************************************************************
 *   Copyright (C) 2005-2009 by Rajko Albrecht                             *
 *   ral@alwins-world.de                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
 ***************************************************************************/

#ifndef COMMITMODEL_H
#define COMMITMODEL_H

#include "commitmodelfwd.h"
#include "commitmodelhelper.h"
#include "svnqt/svnqttypes.h"

#include <QAbstractListModel>
#include <QScopedPointer>
#include <QSortFilterProxyModel>

class CommitModel: public QAbstractItemModel
{
    Q_OBJECT
protected:
    explicit CommitModel(const CommitActionEntries &, const CommitActionEntries &, QObject *parent = 0);
    void setCommitData(const CommitActionEntries &, const CommitActionEntries &);
public:
    explicit CommitModel(const svn::CommitItemList &, QObject *parent = 0);
    void setCommitData(const svn::CommitItemList &);

    virtual ~CommitModel();

    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex())const;
    virtual QModelIndex parent(const QModelIndex &)const;
    QVariant data(const QModelIndex &index, int role) const;

    virtual int rowCount(const QModelIndex &) const;
    virtual int columnCount(const QModelIndex &) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    virtual int ActionColumn()const;
    virtual int ItemColumn()const;

    CommitModelNodePtr node(const QModelIndex &);
    CommitActionEntries checkedEntries()const;
    void markItems(bool mark, CommitActionEntry::ACTION_TYPE _type);
    void removeEntries(const QStringList &_items);

    const CommitModelNodePtr dataForRow(int row) const;
protected:
    CommitModelNodeList m_List;
};

class CommitModelCheckitem: public CommitModel
{
    Q_OBJECT
public:
    CommitModelCheckitem(const CommitActionEntries &, const CommitActionEntries &, QObject *parent = 0);
    virtual ~CommitModelCheckitem();

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual int ActionColumn()const;
    virtual int ItemColumn()const;

};

class CommitFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
 public:
    explicit CommitFilterModel(QObject *parent);
    ~CommitFilterModel();

    void setSourceModel(QAbstractItemModel *sourceModel);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

    void hideItems(bool bHide, CommitActionEntry::ACTION_TYPE aType);
private:
    CommitModel *m_sourceModel;
    CommitActionEntry::ActionTypes m_visibleTypes;
};

#endif
