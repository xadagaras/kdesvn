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
#include "commandline_part.h"
#include "kdesvn_part.h"
#include "svnfrontend/commandexec.h"

#include <kstandarddirs.h>

commandline_part::commandline_part(QObject *parent, const QVariantList& args)
 : QObject(parent)
{
    KGlobal::locale()->insertCatalog("kdesvn");

    ///@fixme replace with kde4
    //KInstance * inst = kdesvnPartFactory::instance();

    /*
    KGlobal::locale()->insertCatalog(inst->instanceName());
    KGlobal::dirs()->addResourceType( inst->instanceName() + "data",
        KStandardDirs::kde_default("data")+ QString::fromLatin1( inst->instanceName() ) + '/' );
    */

    m_pCPart = new CommandExec(this,args);
}

commandline_part::~commandline_part()
{
}

int commandline_part::exec()
{
    return m_pCPart->exec();
}

#include "commandline_part.moc"
