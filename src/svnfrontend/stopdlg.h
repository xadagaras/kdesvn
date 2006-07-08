/***************************************************************************
 *   Copyright (C) 2005 by Rajko Albrecht                                  *
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
#ifndef STOPDLG_H
#define STOPDLG_H

#include <kdialogbase.h>

#include <qdatetime.h>
#include <qobject.h>

class QTimer;

class CContextListener;
class QLabel;
class KProgress;
class KTextBrowser;
class QVBoxLayout;

/**
@author Rajko Albrecht
*/
class StopDlg : public KDialogBase
{
Q_OBJECT
public:
    StopDlg(QObject*,QWidget *parent = 0, const char *name = 0,const QString&caption=QString::null,const QString&text=QString::null);
    ~StopDlg();

    bool cancelld();

protected:
    QObject*m_Context;
    int m_MinDuration;
    bool mCancelled;
    QTimer * mShowTimer;
    QString mCancelText;
    bool mShown,mWait;
    QLabel*mLabel;
    KProgress*m_ProgressBar;
    bool m_BarShown;
    QTime m_StopTick;
    KTextBrowser*m_LogWindow;
    QVBoxLayout*layout;

public slots:
    virtual void slotTick();
    virtual void slotWait(bool);
    virtual void slotExtraMessage(const QString&);

protected slots:
    virtual void slotAutoShow();
    virtual void slotCancel();
signals:
    void sigCancel(bool how);
};

#endif
