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

#ifndef NLOG

#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include "libhpc/debug/assert.hh"
#include "libhpc/system/stream.hh"
#include "logger.hh"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace hpc {
   namespace log {

      double log_base_time;

      logger::logger( unsigned min_level,
		      const std::string& tag )
         : _min_level( min_level )
      {
	 if( !tag.empty() )
	    _tags.insert( tag );
      }

      logger::~logger()
      {
         for( std::map<boost::thread::id,std::stringstream*>::iterator it = _buf.begin();
              it != _buf.end();
              ++it )
         {
            if( it->second )
               delete it->second;
         }
      }

      void
      logger::open()
      {
      }

      void
      logger::close()
      {
      }

      void
      logger::new_line()
      {
         if( visible() )
         {
            write_buffer( "\n" );
            _get_new_line() = true;
         }
      }

      void
      logger::prefix()
      {
         write_buffer( indent );
         _get_new_line() = false;
      }

      void
      logger::add_tag( const std::string& tag )
      {
	 _tags.insert( tag );
      }

      void
      logger::push_level( unsigned level )
      {
         levels().push_front( level );
      }

      void
      logger::pop_level()
      {
         levels().pop_front();
      }

      void
      logger::push_tag( const std::string& tag )
      {
	 if( current_tags().find( tag ) == current_tags().end() )
	    current_tags().insert( std::make_pair( tag, 0 ) );
	 ++current_tags()[tag];
      }

      void
      logger::pop_tag( const std::string& tag )
      {
	 ASSERT( current_tags().find( tag ) != current_tags().end(), "Unable to find tag: ", tag );
	 int& val = current_tags()[tag];
	 ASSERT( val > 0, "Tag has invalid count: ", tag );
	 if( --val == 0 )
	    current_tags().erase( tag );
      }

      bool
      logger::visible()
      {
         std::map<std::string,int> const& cur_tags = current_tags();
	 bool level_vis = levels().empty() || levels().front() >= _min_level;
	 bool tag_vis = _tags.empty() && cur_tags.empty();
	 if( !tag_vis )
	 {
            for( std::set<std::string>::const_iterator it = _tags.begin();
                 it != _tags.end();
                 ++it )
	    {
	       if( cur_tags.find( *it ) != cur_tags.end() )
	       {
		  tag_vis = true;
		  break;
	       }
	    }
	 }
         return level_vis && tag_vis;
      }

      std::stringstream&
      logger::buffer()
      {
         boost::thread::id tid = boost::this_thread::get_id();
         _buf_mutex.lock();
         if( _buf.find( tid ) == _buf.end() )
            _buf.insert( std::make_pair( tid, new std::stringstream ) );
         _buf_mutex.unlock();
         return *_buf[tid];
      }

      std::list<unsigned>&
      logger::levels()
      {
         boost::thread::id tid = boost::this_thread::get_id();
         _levels_mutex.lock();
         if( _levels.find( tid ) == _levels.end() )
            _levels.insert( std::make_pair( tid, std::list<unsigned>() ) );
         _levels_mutex.unlock();
         return _levels[tid];
      }

      std::map<std::string,int>&
      logger::current_tags()
      {
         boost::thread::id tid = boost::this_thread::get_id();
         _tags_mutex.lock();
         if( _cur_tags.find( tid ) == _cur_tags.end() )
            _cur_tags.insert( std::make_pair( tid, std::map<std::string,int>() ) );
         _tags_mutex.unlock();
         return _cur_tags[tid];
      }

      bool&
      logger::_get_new_line()
      {
         boost::thread::id tid = boost::this_thread::get_id();
         _new_line_mutex.lock();
         if( _new_line.find( tid ) == _new_line.end() )
            _new_line.insert( std::make_pair( tid, true ) );
         _new_line_mutex.unlock();
         return _new_line[tid];
      }

      void
      logger::_traits<logger& ( logger& )>::impl::operator()( logger& log,
                                                              logger& (*fp)( logger& ) )
      {
         fp( log );
      }

      void
      logger::_traits<setindent_t>::impl::operator()( logger& log,
                                                      setindent_t si )
      {
         log( si, false );
      }

      ///
      ///
      ///
      logger&
      endl( logger& log )
      {
         log.new_line();
         return log;
      }

      ///
      ///
      ///
      level_t
      pushlevel( unsigned level )
      {
         level_t lv;
         lv.level = level;
         return lv;
      }

      ///
      ///
      ///
      logger&
      operator<<( logger& log,
                  level_t level )
      {
         log.push_level( level.level );
         return log;
      }

      ///
      ///
      ///
      logger&
      poplevel( logger& log )
      {
         log.pop_level();
         return log;
      }
   }
}

#endif
