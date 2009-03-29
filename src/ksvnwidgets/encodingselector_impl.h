/***************************************************************************
 *   Copyright (C) 2007 by Rajko Albrecht  ral@alwins-world.de             *
 *   http://kdesvn.alwins-world.de/                                        *
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
#ifndef ENCODINGSELECTOR_IMPL_H
#define ENCODINGSELECTOR_IMPL_H

#include "ui_encodingselector.h"

class QTextCodec;

class EncodingSelector_impl: public QWidget, public Ui::EncodingSelector {
Q_OBJECT
public:
    EncodingSelector_impl(QWidget *parent = 0);
    EncodingSelector_impl(const QString&cur, QWidget *parent = 0, const char *name = 0);
    virtual ~EncodingSelector_impl(){}

    void setCurrentEncoding(const QString&cur);

protected slots:
    virtual void itemActivated(int);

signals:
    void TextCodecChanged(const QString&);
};

#endif
