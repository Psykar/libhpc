// Copyright 2012 Luke Hodkinson

// This file is part of libhpc.
// 
// libhpc is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// libhpc is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with libhpc.  If not, see <http://www.gnu.org/licenses/>.

#ifndef hpc_logging_globals_hh
#define hpc_logging_globals_hh

#include "libhpc/system/cuda.hh"

#if !defined(NLOG) && defined(CUDA_HOST)

#include "libhpc/system/stream.hh"
#include "libhpc/system/narg.hh"
#include "logger.hh"
#include "stack.hh"
#include "stdout.hh"
#include "levels.hh"

#define _LOG_( count, ... )                             \
   _OSTREAM##count( ::hpc::log::_stack, __VA_ARGS__ )

#define _LOG( count, ... )                      \
   _LOG_( count, __VA_ARGS__ )

#define LOG( ... )                                      \
   _LOG( PP_NARG( 0, ##__VA_ARGS__ ), ##__VA_ARGS__ )

#define LOGLN( ... )                            \
   LOG( __VA_ARGS__, ::hpc::log::endl )

#define LOGLV( level, ... )                             \
   LOG( ::hpc::log::pushlevel( level ), __VA_ARGS__,    \
        ::hpc::log::poplevel )

#define LOGLVLN( level, ... )                           \
   LOG( ::hpc::log::pushlevel( level ), __VA_ARGS__,    \
        ::hpc::log::endl, ::hpc::log::poplevel )

#ifndef NLOGTRIVIAL
#define LOGT( ... )                             \
   LOGLV( ::hpc::log::trivial, __VA_ARGS__ )
#else
#define LOGT( ... )
#endif

#ifndef NLOGDEBUG
#define LOGD( ... )                             \
   LOGLV( ::hpc::log::debug, __VA_ARGS__ )
#else
#define LOGD( ... )
#endif

#define LOGI( ... )                             \
   LOGLV( ::hpc::log::info, __VA_ARGS__ )

#define LOGW( ... )                             \
   LOGLV( ::hpc::log::warning, __VA_ARGS__ )

#define LOGE( ... )                             \
   LOGLV( ::hpc::log::error, __VA_ARGS__ )

#ifndef NLOGTRIVIAL
#define LOGTLN( ... )                           \
   LOGLVLN( ::hpc::log::trivial, __VA_ARGS__ )
#else
#define LOGTLN( ... )
#endif

#ifndef NLOGDEBUG
#define LOGDLN( ... )                           \
   LOGLVLN( ::hpc::log::debug, __VA_ARGS__ )
#define LOGDLN_TAG( tag, ... )                                          \
   (LOG_PUSH_TAG( tag ), LOGDLN( __VA_ARGS__ ), LOG_POP_TAG( tag ))
#else
#define LOGDLN( ... )
#define LOGDLN_TAG( tag, ... )
#endif

#define LOGILN( ... )                           \
   LOGLVLN( ::hpc::log::info, __VA_ARGS__ )

#define LOGWLN( ... )                           \
   LOGLVLN( ::hpc::log::warning, __VA_ARGS__ )

#define LOGELN( ... )                           \
   LOGLVLN( ::hpc::log::info, __VA_ARGS__ )

#define LOG_ENTER()							\
   LOGTLN( "Entering: ", __PRETTY_FUNCTION__, ::hpc::setindent( 2 ) )

#define LOG_EXIT()							\
   LOGTLN( "Exiting: ", __PRETTY_FUNCTION__, ::hpc::setindent( -2 ) );

#define LOG_PUSH( logger )                      \
      ::hpc::log::push( logger )

#define LOG_FILE( filename )                    \
   ::hpc::log::push(                            \
      new ::hpc::log::file( filename ) )

#define LOG_CONSOLE()                           \
   ::hpc::log::push(                            \
                    new ::hpc::log::stdout() )

#define LOG_POP()                               \
   ::hpc::log::pop()

#define LOG_PUSH_TAG( tag )                     \
   ::hpc::log::push_tag( tag )

#define LOG_POP_TAG( tag )                      \
   ::hpc::log::pop_tag( tag )

#define WARN( expr, ... )                               \
   ((expr) ? LOGWLN( "WARNING: ", __VA_ARGS__ ) : 0)

namespace hpc {
   namespace log {

      extern stack _stack;

      void
      push( logger* log );

      void
      pop();

      void
      clear();

      void
      push_tag( const std::string& tag );

      void
      pop_tag( const std::string& tag );

   }
}

#else

#define LOG( ... )
#define LOGLN( ... )
#define LOGLV( level, ... )
#define LOGLVLN( level, ... )
#define LOGT( ... )
#define LOGD( ... )
#define LOGI( ... )
#define LOGW( ... )
#define LOGE( ... )
#define LOGTLN( ... )
#define LOGDLN( ... )
#define LOGILN( ... )
#define LOGWLN( ... )
#define LOGELN( ... )
#define LOG_ENTER()
#define LOG_EXIT()
#define LOG_PUSH( logger )
#define LOG_FILE( filename )
#define LOG_CONSOLE()
#define LOG_POP()
#define LOG_PUSH_TAG( tag )
#define LOG_POP_TAG( tag )
#define WARN( expr, ... )

#endif

#endif
