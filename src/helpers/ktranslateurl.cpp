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
#include "ktranslateurl.h"

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kfileitem.h>
#include <kdesktopfile.h>

#include <qstringlist.h>
#include <qdir.h>

namespace helpers {

KTranslateUrl::KTranslateUrl()
{
}


KTranslateUrl::~KTranslateUrl()
{
}

KURL KTranslateUrl::translateSystemUrl(const KURL&_url)
{
    QString proto = _url.protocol();
    KURL res;
    QString name,path;

    if (proto!="system") {
        return _url;
    }
    KGlobal::dirs()->addResourceType("system_entries",
    KStandardDirs::kde_default("data") + "systemview");
    QStringList dirList = KGlobal::dirs()->resourceDirs("system_entries");
    if (!parseURL(_url,name,path)) {
        return _url;
    }
    res = findSystemBase(name);
    if (!res.isValid()) {
        return _url;
    }
    res.addPath(path);
    res.setQuery(_url.query());
    return res;
}

bool KTranslateUrl::parseURL(const KURL&url,QString&name,QString&path)
{
    QString url_path = url.path();
    int i = url_path.find('/', 1);
    if (i > 0)
    {
        name = url_path.mid(1, i-1);
        path = url_path.mid(i+1);
    }
    else
    {
        name = url_path.mid(1);
        path = QString::null;
    }

    return name != QString::null;
}

KURL KTranslateUrl::findSystemBase(const QString&filename)
{
    QStringList dirList = KGlobal::dirs()->resourceDirs("system_entries");

    QStringList::ConstIterator dirpath = dirList.begin();
    QStringList::ConstIterator end = dirList.end();
    for(; dirpath!=end; ++dirpath)
    {
        QDir dir = *dirpath;
        if (!dir.exists()) continue;

        QStringList filenames
                = dir.entryList( QDir::Files | QDir::Readable );


        KIO::UDSEntry entry;

        QStringList::ConstIterator name = filenames.begin();
        QStringList::ConstIterator endf = filenames.end();

        for(; name!=endf; ++name)
        {
            if (*name==filename+".desktop")
            {
                KDesktopFile desktop(*dirpath+filename+".desktop", true);
                if ( desktop.readURL().isEmpty() )
                {
                    KURL url;
                    url.setPath( desktop.readPath() );
                    return url;
                }

                return desktop.readURL();
            }
        }
    }

    return KURL();
}

}