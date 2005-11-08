#include "modifiedthread.h"
#include "tcontextlistener.h"

#include <qobject.h>
#include <kdebug.h>
#include <kapplication.h>

CheckModifiedThread::CheckModifiedThread(QObject*_parent,const QString&what,bool _updates)
    : QThread(),mutex()
{
    m_Parent = _parent;
    m_CurrentContext = new svn::Context();
    m_SvnContext = new ThreadContextListener(0,0);
    m_CurrentContext->setListener(m_SvnContext);
    m_what = what;
    m_Svnclient.setContext(m_CurrentContext);
    m_updates = _updates;
}

CheckModifiedThread::~CheckModifiedThread()
{
    delete m_CurrentContext;
}

void CheckModifiedThread::cancelMe()
{
    // method is threadsafe!
    m_SvnContext->setCanceled(true);
}

const svn::StatusEntries&CheckModifiedThread::getList()const
{
    return m_Cache;
}

void CheckModifiedThread::run()
{
    // what must be cleaned!
    svn::Revision where = svn::Revision::HEAD;
    QString ex;
    try {
        //                                  rec  all   up        noign
        m_Cache = m_Svnclient.status(m_what,true,false,m_updates,false,where);
    } catch (svn::ClientException e) {
        kdDebug()<<"Exception in thread"<<endl;
    }
    kdDebug()<<"Thread finished"<<endl;
    KApplication*k = KApplication::kApplication();
    if (k) {
//        k->postEvent(m_Parent,new ThreadEndEvent(this));
    }
}

ThreadEndEvent::ThreadEndEvent(QThread*_thread)
    : QEvent(EVENT_THREAD_FINISHED)
{
    m_threadObject = _thread;
}

ThreadEndEvent::~ThreadEndEvent()
{
}

QThread* ThreadEndEvent::threadObject()
{
    return m_threadObject;
}