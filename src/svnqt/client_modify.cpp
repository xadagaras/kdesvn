/*
 * Port for usage with qt-framework and development for kdesvn
 * Copyright (C) 2005-2009 by Rajko Albrecht (ral@alwins-world.de)
 * http://kdesvn.alwins-world.de
 */
/*
 * ====================================================================
 * Copyright (c) 2002-2005 The RapidSvn Group.  All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library (in the file LGPL.txt); if not,
 * write to the Free Software Foundation, Inc., 51 Franklin St,
 * Fifth Floor, Boston, MA  02110-1301  USA
 *
 * This software consists of voluntary contributions made by many
 * individuals.  For exact contribution history, see the revision
 * history and logs, available at http://rapidsvn.tigris.org/.
 * ====================================================================
 */
#if defined( _MSC_VER) && _MSC_VER <= 1200
#pragma warning( disable: 4786 )// debug symbol truncated
#endif
// svncpp
#include "svnqt/client_impl.h"

// subversion api
#include "svn_client.h"

#include "svnqt/exception.h"
#include "svnqt/pool.h"
#include "svnqt/targets.h"
#include "svnqt/svnqt_defines.h"
#include "svnqt/stringarray.h"
#include "svnqt/client_parameter.h"
#include "svnqt/client_commit_parameter.h"
#include "svnqt/client_update_parameter.h"
#include "svnqt/url.h"

#include "svnqt/helper.h"
    
namespace svn
{
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    struct mBaton {
        mBaton():m_context(0),m_revision(Revision::UNDEFINED),m_date(),author(),commit_error(),repos_root(){}
        Context*m_context;
        svn::Revision m_revision;
        QString m_date,author,commit_error,repos_root;
    };
    
    static svn_error_t * commit_callback2 (const svn_commit_info_t * commit_info, void *baton, apr_pool_t *pool)
    {
        Q_UNUSED(pool);
        mBaton * m_baton = (mBaton*)baton;
        Context*m_context = m_baton->m_context;
        svn_client_ctx_t*ctx = m_context->ctx();
        if (ctx&&ctx->cancel_func) {
            SVN_ERR(ctx->cancel_func(ctx->cancel_baton));
        }
        m_baton->author = QString::FROMUTF8(commit_info->author);
        m_baton->commit_error = QString::FROMUTF8(commit_info->post_commit_err);
        m_baton->m_date = QString::FROMUTF8(commit_info->date);
        m_baton->repos_root = QString::FROMUTF8(commit_info->repos_root);
        m_baton->m_revision = commit_info->revision;
        return SVN_NO_ERROR;
    }
#endif

  Revision
  Client_impl::checkout (const CheckoutParameter&parameters) throw (ClientException)
  {
    Pool subPool;
    svn_revnum_t revnum = 0;
    svn_error_t * error = 0;
    error = svn_client_checkout3(&revnum,
                parameters.moduleName().cstr(),
                parameters.destination().cstr(),
                parameters.peg().revision(),
                parameters.revision().revision (),
                internal::DepthToSvn(parameters.depth()),
                parameters.ignoreExternals(),
                parameters.overWrite(),
                *m_context,
                subPool);
    if(error != NULL)
      throw ClientException (error);
    return Revision(revnum);
  }

  Revision
  Client_impl::remove (const Targets & targets,
                  bool force,
                  bool keep_local,
                  const PropertiesMap&revProps
                      ) throw (ClientException)
  {
    Pool pool;
    svn_error_t * error;

#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    mBaton _baton;
    _baton.m_context = m_context;
    error = svn_client_delete4(
        targets.array(pool),
        force,
        keep_local,
        map2hash(revProps,pool),
        commit_callback2,
        &_baton,
        *m_context,
        pool
        );
#else
    svn_commit_info_t * commit_info = 0;
    error =
            svn_client_delete3(
                               &commit_info,
                               targets.array(pool),
                               force,
                               keep_local,
                               map2hash(revProps,pool),
                               *m_context,
                               pool
                              );
#endif
    if(error != 0) {
        throw ClientException (error);
    }
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    return _baton.m_revision;
#else
    if (commit_info) {
        return commit_info->revision;
    }
    return Revision::UNDEFINED;
#endif
  }

  void
  Client_impl::revert (const Targets & targets,
                  Depth depth,
                  const StringArray&changelist
                      ) throw (ClientException)
  {
    Pool pool;

    svn_error_t * error =
            svn_client_revert2 ((targets.array (pool)),
                                internal::DepthToSvn(depth),
                                changelist.array(pool),
                                *m_context,
                                pool);
    if(error != NULL) {
        throw ClientException (error);
    }
  }

  void
  Client_impl::add (const Path & path,
                    svn::Depth depth,bool force, bool no_ignore, bool add_parents) throw (ClientException)
  {
    Pool pool;
    svn_error_t * error =
            svn_client_add4(path.cstr (),
                             internal::DepthToSvn(depth),
                             force,
                             no_ignore,
                             add_parents,
                             *m_context,
                             pool);
    if(error != NULL)
      throw ClientException (error);
  }

  Revisions
  Client_impl::update (const UpdateParameter&params) throw (ClientException)
  {
    Pool pool;
    Revisions resulting;
    svn_error_t * error;

    apr_pool_t *apr_pool = pool.pool();
    apr_array_header_t *apr_revisions = apr_array_make (apr_pool,
                      params.targets().size(),
                      sizeof (svn_revnum_t));
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    error = svn_client_update4(&apr_revisions,params.targets().array(pool),params.revision(),
                               internal::DepthToSvn(params.depth()),params.sticky_depth(),
                               params.ignore_externals(),params.allow_unversioned(),
                               params.add_as_modification(),params.make_parents(),
                               *m_context,pool
                              );
#else
    error = svn_client_update3(&apr_revisions,params.targets().array(pool),params.revision(),internal::DepthToSvn(params.depth()),params.sticky_depth(),params.ignore_externals(),params.allow_unversioned(),*m_context,pool);
#endif
    if (error!=NULL) {
        throw ClientException(error);
    }
    for (int i = 0; i < apr_revisions->nelts; ++i)
    {
      svn_revnum_t * _rev =
        &APR_ARRAY_IDX (apr_revisions, i, svn_revnum_t);

      resulting.push_back((*_rev));
    }
    return resulting;
  }

  svn::Revision
  Client_impl::commit (const CommitParameter&parameters) throw (ClientException)
  {
    Pool pool;
   
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)    
    mBaton _baton;
    _baton.m_context = m_context;
#else
    svn_commit_info_t *commit_info = NULL;
#endif
    m_context->setLogMessage (parameters.message());
    svn_error_t * error =
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)    
        svn_client_commit5(
            parameters.targets().array(pool),
                           internal::DepthToSvn(parameters.depth()),
                           parameters.keepLocks(),
                           parameters.keepChangeList(),
                           parameters.commitAsOperations(),
                           parameters.changeList().array(pool),
                           map2hash(parameters.revisionProperties(),pool),
                           commit_callback2,
                           &_baton,
                           *m_context,
                           pool
        );
#else
        svn_client_commit4 (
                &commit_info,
                parameters.targets().array (pool),
                internal::DepthToSvn(parameters.depth()),
                parameters.keepLocks(),
                parameters.keepChangeList(),
                parameters.changeList().array(pool),
                map2hash(parameters.revisionProperties(),pool),
                *m_context,
                pool);
#endif
    if (error != 0) {
        throw ClientException (error);
    }
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)    
    return _baton.m_revision;
#else    
    if (commit_info && SVN_IS_VALID_REVNUM (commit_info->revision))
      return (commit_info->revision);
#endif
    return svn::Revision::UNDEFINED;
  }

  Revision
    Client_impl::copy(const CopyParameter&parameter)throw (ClientException)
    {
        if (parameter.srcPath().size()<1)
        {
            throw ClientException("Wrong size of sources.");
        }

        Pool pool;
        apr_array_header_t * sources = apr_array_make(pool,parameter.srcPath().size(),sizeof(svn_client_copy_source_t *));
        // not using .array() 'cause some extra information is needed for copy
        for (size_t j=0;j<parameter.srcPath().size();++j)
        {
            svn_client_copy_source_t* source = (svn_client_copy_source_t*)apr_palloc(pool, sizeof(svn_client_copy_source_t));
            source->path = apr_pstrdup(pool,parameter.srcPath()[j].path().TOUTF8());
            source->revision=parameter.srcRevision().revision();
            source->peg_revision=parameter.pegRevision().revision();
            APR_ARRAY_PUSH(sources, svn_client_copy_source_t *) = source;
        }
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
        mBaton _baton;
        _baton.m_context = m_context;

        svn_error_t * error =
            svn_client_copy6(
                            sources,
                            parameter.destination().cstr(),
                            parameter.asChild(),parameter.makeParent(),parameter.ignoreExternal(),
                            map2hash(parameter.properties(),pool),
                            commit_callback2,&_baton,
                            *m_context,pool);

#elif ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 6)) || (SVN_VER_MAJOR > 1)
        svn_commit_info_t *commit_info = 0L;
        svn_error_t * error =
            svn_client_copy5(&commit_info,
                sources,
                parameter.destination().cstr(),
                parameter.asChild(),parameter.makeParent(),parameter.ignoreExternal(),
                map2hash(parameter.properties(),pool),*m_context,pool);
#else
        svn_commit_info_t *commit_info = 0L;
        svn_error_t * error =
            svn_client_copy4(&commit_info,
                sources,
                parameter.destination().cstr(),
                parameter.asChild(),parameter.makeParent(),map2hash(parameter.properties(),pool),*m_context,pool);
#endif
        if (error!=0){
            throw ClientException (error);
        }
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
        return _baton.m_revision;
#else
        if (commit_info) {
            return commit_info->revision;
        }
#endif
        return Revision::UNDEFINED;
    }

  Revision
  Client_impl::copy (const Path & srcPath,
                const Revision & srcRevision,
                const Path & destPath) throw (ClientException)
  {
    return copy(CopyParameter(srcPath,destPath).srcRevision(srcRevision).asChild(true).makeParent(false));
  }

  svn::Revision Client_impl::move (const CopyParameter&parameter) throw (ClientException)
  {
      Pool pool;
      
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
      mBaton _baton;
      _baton.m_context = m_context;
      svn_error_t * error = svn_client_move6(
            parameter.srcPath().array(pool),
            parameter.destination().cstr(),
            parameter.asChild(),
            parameter.makeParent(),
            map2hash(parameter.properties(),pool),
            commit_callback2,
            &_baton,
            *m_context,
            pool
      );
      
#else
      svn_commit_info_t *commit_info = 0;
      svn_error_t * error = svn_client_move5(
                                             &commit_info,
                                             parameter.srcPath().array(pool),
                                             parameter.destination().cstr(),
                                             false, // is ignored since subversion 1.7 so we ignore it, too
                                             parameter.asChild(),
                                             parameter.makeParent(),
                                             map2hash(parameter.properties(),pool),
                                             *m_context,
                                             pool
                                            );
#endif
      if (error!=0) {
          throw ClientException (error);
      }
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
        return _baton.m_revision;
#else
      if (commit_info) {
          return commit_info->revision;
      }
      return Revision::UNDEFINED;
#endif
      
  }

  svn::Revision
  Client_impl::mkdir (const Targets & targets,
                 const QString&msg,
                 bool makeParent,
                 const PropertiesMap&revProps
                     ) throw (ClientException)
  {
    Pool pool;
    m_context->setLogMessage(msg);

    

    svn_error_t * error = 0;
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    mBaton _baton;
    _baton.m_context = m_context;
    error = svn_client_mkdir4
            (
                const_cast<apr_array_header_t*>(targets.array (pool)),
                makeParent,
                map2hash(revProps,pool),
                commit_callback2,&_baton,
                *m_context,pool
            );
#else
    svn_commit_info_t *commit_info = NULL;
    error = svn_client_mkdir3
            (&commit_info,
              const_cast<apr_array_header_t*>(targets.array (pool)),
              makeParent,
              map2hash(revProps,pool),
              *m_context, pool);
#endif
    /* important! otherwise next op on repository uses that logmessage again! */
    m_context->setLogMessage(QString());

    if(error != NULL)
      throw ClientException (error);

#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    return _baton.m_revision;
#else
    if (commit_info) {
        return commit_info->revision;
    }
    return Revision::UNDEFINED;
#endif
  }

  void
  Client_impl::cleanup (const Path & path) throw (ClientException)
  {
    Pool subPool;
    apr_pool_t * apr_pool = subPool.pool ();

    svn_error_t * error =
      svn_client_cleanup (path.cstr (), *m_context, apr_pool);

    if(error != NULL)
      throw ClientException (error);
  }

  void Client_impl::resolve(const Path & path,Depth depth,const ConflictResult&resolution) throw (ClientException)
  {
    Pool pool;
    const svn_wc_conflict_result_t*aResult=resolution.result(pool);
    svn_error_t*error=svn_client_resolve(path.cstr(),internal::DepthToSvn(depth),aResult->choice,*m_context,pool);

    if(error != NULL) {
        throw ClientException (error);
    }
  }

  Revision
  Client_impl::doExport (const CheckoutParameter&params) throw (ClientException)
  {
    Pool pool;
    svn_revnum_t revnum = 0;
    const char*_neol;
    if (params.nativeEol().isNull()) {
        _neol = (const char*)0;
    } else {
        _neol = params.nativeEol().TOUTF8();
    }
    // TODO ignoreKeywords into UI
    svn_error_t * error =
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
            svn_client_export5(&revnum,
                            params.moduleName().cstr(),
                            params.destination().cstr(),
                            params.peg().revision(),
                            params.revision().revision(),
                            params.overWrite(),
                            params.ignoreExternals(),
                            params.ignoreKeywords(),
                            internal::DepthToSvn(params.depth()),
                            _neol,
                            *m_context,
                            pool);
#else
            svn_client_export4(&revnum,
                                params.moduleName().cstr(),
                                params.destination().cstr(),
                                params.peg().revision(),
                                params.revision().revision(),
                                params.overWrite(),
                                params.ignoreExternals(),
                                internal::DepthToSvn(params.depth()),
                                _neol,
                                *m_context,
                                pool);
#endif
    if(error != NULL)
      throw ClientException (error);
    return Revision(revnum);
  }

  Revision
  Client_impl::doSwitch (
                         const Path & path, const Url& url,
                         const Revision & revision,
                         Depth depth,
                         const Revision & peg,
                         bool sticky_depth,
                         bool ignore_externals,
                         bool allow_unversioned
                        ) throw (ClientException)
  {
    Pool pool;
    svn_revnum_t revnum = 0;
    svn_error_t * error = 0;
    
    error = svn_client_switch2(
                               &revnum,
                               path.cstr(),
                               url,
                               peg.revision(),
                               revision.revision(),
                               internal::DepthToSvn(depth),
                               sticky_depth,
                               ignore_externals,
                               allow_unversioned,
                               *m_context,
                               pool
                              );
    if(error != NULL) {
        throw ClientException (error);
    }
    return Revision(revnum);
  }

  Revision
  Client_impl::import (const Path & path,
                  const Url& url,
                  const QString& message,
                  svn::Depth depth,
                  bool no_ignore,bool no_unknown_nodetype,
                  const PropertiesMap&revProps
                      ) throw (ClientException)

  {
    Pool pool;

    m_context->setLogMessage (message);
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    mBaton _baton;
    _baton.m_context = m_context;
    svn_error_t * error =    
        svn_client_import4(path.cstr (),url,
                       internal::DepthToSvn(depth),no_ignore,no_unknown_nodetype,
                       map2hash(revProps,pool),
                       commit_callback2,&_baton,
                       *m_context,pool);
#else    
    svn_commit_info_t *commit_info = NULL;
    svn_error_t * error =    
        svn_client_import3(&commit_info,path.cstr (),url,
            internal::DepthToSvn(depth),no_ignore,no_unknown_nodetype,
                                 map2hash(revProps,pool),
                                 *m_context,pool);
#endif   

    /* important! otherwise next op on repository uses that logmessage again! */
    m_context->setLogMessage(QString());

    if(error != 0) {
        throw ClientException (error);
    }
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    return _baton.m_revision;
#else
    if (commit_info) {
        return commit_info->revision;
    }
    return Revision::UNDEFINED;
#endif
  }

  void
  Client_impl::relocate (const Path & path,
                    const Url& from_url,
                    const Url& to_url,
                    bool recurse) throw (ClientException)
  {
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
      Q_UNUSED(recurse);
#endif
    Pool pool;
    svn_error_t * error =
#if ((SVN_VER_MAJOR == 1) && (SVN_VER_MINOR >= 7)) || (SVN_VER_MAJOR > 1)
    svn_client_relocate2 (path.cstr (),
                        from_url,
                        to_url,
                        false, // TODO ignore_externals as parameter for svn 1.7 
                        *m_context,
                        pool);

#else
      svn_client_relocate (path.cstr (),
                         from_url,
                         to_url,
                         recurse,
                         *m_context,
                         pool);
#endif
    if(error != NULL)
      throw ClientException (error);
  }

}

/* -----------------------------------------------------------------
 * local variables:
 * eval: (load-file "../../rapidsvn-dev.el")
 * end:
 */
