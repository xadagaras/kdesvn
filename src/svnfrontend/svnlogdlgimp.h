/***************************************************************************
 *   Copyright (C) 2005-2007 by Rajko Albrecht                             *
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
#ifndef SVNLOGDLGIMP_H
#define SVNLOGDLGIMP_H

#include "svnlogdlg.h"
#include "simple_logcb.h"
#include "src/svnqt/log_entry.hpp"
#include "src/svnqt/client.hpp"
#include "src/svnqt/shared_pointer.hpp"

#include <qsize.h>
#include <qregexp.h>

class LogListViewItem;
class SvnActions;

class SvnLogDlgImp: public SvnLogDialogData,public SimpleLogCb
{
Q_OBJECT
public:
    SvnLogDlgImp(SvnActions*,QWidget *parent = 0, const char *name = 0);
    virtual ~SvnLogDlgImp();
    void dispLog(const svn::SharedPointer<svn::LogEntriesMap>&,const QString&,const QString&,const svn::Revision&peg,const QString&pegUrl);
    void saveSize();
    QSize dialogSize();

    virtual bool getSingleLog(svn::LogEntry&t,const svn::Revision&r,const QString&what,const svn::Revision&peg,QString&root);

signals:
    void makeDiff(const QString&,const svn::Revision&,const QString&,const svn::Revision&,QWidget*);
    void makeCat(const svn::Revision&,const QString&,const QString&,const svn::Revision&,QWidget*);

protected slots:
    virtual void slotSelectionChanged(QListViewItem*);
protected slots:
    virtual void slotDispPrevious();
    virtual void slotDispSelected();
    virtual void slotItemClicked(int,QListViewItem*,const QPoint &,int);
protected:
    QString _name;
    QString _base;
    static const char* groupName;
    LogListViewItem *m_first,*m_second;
    SvnActions*m_Actions;
    bool m_ControlKeyDown;
    virtual void keyPressEvent (QKeyEvent * e);
    virtual void keyReleaseEvent (QKeyEvent * e);
    virtual void slotBlameItem();
    svn::SharedPointer<svn::LogEntriesMap> m_Entries;
    QString _bugurl;
    QRegExp _r1,_r2;

protected slots:
    virtual void slotListEntries();
    virtual void slotEntriesSelectionChanged();
    virtual void slotSingleContext(QListViewItem*, const QPoint &, int);
    virtual void slotSingleDoubleClicked(QListViewItem*);

protected:
    void replaceBugids(QString&msg);
    QString genReplace(const QString&);
    svn::Revision m_peg;
    svn::Path m_PegUrl;
};

#endif
