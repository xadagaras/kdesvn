/***************************************************************************
 *   Copyright (C) 2005 by Rajko Albrecht   *
 *   ral@alwins-world.de   *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <qregexp.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kpassdlg.h>

#include <iostream>
static KCmdLineOptions options[] =
{
    { "+[prompt]", I18N_NOOP("prompt"), 0 },
    KCmdLineLastOption
};

int main(int argc, char** argv)
{
    KAboutData about("kdesvnaskpass",I18N_NOOP("kdesvnaskpass"),"0.1",
                    I18N_NOOP("ssh-askpass for kdesvn"),
                    KAboutData::License_LGPL,
                    I18N_NOOP("Copyright (c) 2005 Rajko Albrecht"));
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    // no need to register with the dcop server
    KApplication::disableAutoDcopRegistration();

    KApplication app;
    // no need for session management
    app.disableSessionManagement();
    QString prompt;

    if( !KCmdLineArgs::parsedArgs()->count() ) {
        prompt = i18n("Please type in your password below.");
    } else {
        prompt = KCmdLineArgs::parsedArgs()->arg(0);
    }
    KPasswordDialog dlg(KPasswordDialog::Password, false, 0);
    dlg.setPrompt(prompt);
    if (dlg.exec()==KPasswordDialog::Accepted) {
        std::cout<<dlg.password() << std::endl;
        return 0;
    }
    return 1;
}
