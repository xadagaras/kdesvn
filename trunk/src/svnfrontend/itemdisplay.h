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

#ifndef ITEMDISPLAY_H
#define ITEMDISPLAY_H

#include "src/svnqt/svnqttypes.h"
#include <QString>
#include <kurl.h>
#include <QObject>

class QWidget;

class SvnItem;

namespace svn
{
    class Status;
    class Revision;
}

class ItemDisplay
{
public:
    ItemDisplay();
    virtual ~ItemDisplay(){}
    virtual bool isWorkingCopy()const;
    virtual QWidget*realWidget() = 0;
    virtual SvnItem*Selected()const=0;
    virtual void SelectionList(QList<SvnItem*>&)const=0;
    virtual const QString&baseUri()const;
    virtual const svn::Revision&baseRevision()const=0;
    virtual bool openUrl( const KUrl &url,bool noReinit=false )=0;
    virtual SvnItem*SelectedOrMain()const=0;
    virtual bool isNetworked()const;
    virtual const QString&lastError()const;
    virtual bool filterOut(const SvnItem*);
    QString relativePath(const SvnItem*item);

protected:
    void setWorkingCopy(bool);
    void setNetworked(bool);
    void setBaseUri(const QString&);
    QString m_LastException;

private:
    bool m_isWorkingCopy;
    bool m_isNetworked;
    QString m_baseUri;
};

#endif
