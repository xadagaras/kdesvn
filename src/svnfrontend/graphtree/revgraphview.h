/***************************************************************************
 *   Copyright (C) 2006 by Rajko Albrecht                                  *
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
#ifndef REVGRAPHVIEW_H
#define REVGRAPHVIEW_H

#include <svnqt/revision.hpp>

#include <qcanvas.h>

namespace svn {
    class LogEntry;
    class Client;
}

class KTempFile;
class KProcess;
class RevisionTree;
class GraphTreeLabel;
class GraphViewTip;
class GraphMark;
class PannerView;
class CContextListener;

/**
	@author Rajko Albrecht <ral@alwins-world.de>
*/
class RevGraphView : public QCanvasView
{
    Q_OBJECT
public:
    enum ZoomPosition { TopLeft, TopRight, BottomLeft, BottomRight, Auto };
    /* avoid large copy operations */
    friend class RevisionTree;

    RevGraphView(QObject*,svn::Client*,QWidget * parent = 0, const char * name = 0, WFlags f = 0);
    virtual ~RevGraphView();

    void showText(const QString&s);
    void clear();

    void beginInsert();
    void endInsert();

    struct targetData {
        char Action;
        QString key;
        targetData(const QString&n,char _a)
        {
            key = n;
            Action = _a;
        }
        targetData(){Action=0;key="";}
    };
    typedef QValueList<targetData> tlist;

    struct keyData {
        QString name,Author,Date,Message;
        long rev;
        char Action;
        tlist targets;
    };

    typedef QMap<QString,keyData> trevTree;

    QString toolTip(const QString&nodename,bool full=false)const;

    void setBasePath(const QString&);
    void dumpRevtree();

signals:
    void dispDiff(const QString&);
    void dispDetails(const QString&);
    void makeCat(const svn::Revision&,const QString&,const QString&,const svn::Revision&,QWidget*);

public slots:
    virtual void contentsMovingSlot(int,int);
    virtual void zoomRectMoved(int,int);
    virtual void zoomRectMoveFinished();
    virtual void slotClientException(const QString&what);

protected slots:
    virtual void readDotOutput(KProcess *   proc,char *   buffer,int   buflen);
    virtual void dotExit(KProcess*);

protected:
    QCanvas*m_Canvas;
    GraphMark*m_Marker;
    svn::Client*m_Client;
    GraphTreeLabel*m_Selected;
    QObject*m_Listener;
    KTempFile*dotTmpFile;
    QString dotOutput;
    KProcess*renderProcess;
    trevTree m_Tree;
    QColor getBgColor(const QString&nodeName)const;
    bool isStart(const QString&nodeName)const;
    char getAction(const QString&)const;
    const QString&getLabelstring(const QString&nodeName);

    QMap<QString,GraphTreeLabel*> m_NodeList;
    QMap<QString,QString> m_LabelMap;

    int _xMargin,_yMargin;
    GraphViewTip*m_Tip;
    PannerView*m_CompleteView;
    double _cvZoom;
    ZoomPosition m_LastAutoPosition;

    virtual void resizeEvent(QResizeEvent*);
    virtual void contentsMousePressEvent ( QMouseEvent * e );
    virtual void contentsMouseReleaseEvent ( QMouseEvent * e );
    virtual void contentsMouseMoveEvent ( QMouseEvent*e);
    virtual void contentsContextMenuEvent(QContextMenuEvent*e);
    virtual void contentsMouseDoubleClickEvent ( QMouseEvent * e );

    bool _isMoving;
    QPoint _lastPos;

    bool _noUpdateZoomerPos;

    QString _basePath;

private:
    void updateSizes(QSize s = QSize(0,0));
    void updateZoomerPos();
    void setNewDirection(int dir);
    void makeDiffPrev(GraphTreeLabel*);
    void makeDiff(const QString&,const QString&);
    void makeSelected(GraphTreeLabel*);
    void makeCat(GraphTreeLabel*_l);
};

#endif
