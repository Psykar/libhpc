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

#include <boost/algorithm/string/trim.hpp>
#include "option.hh"

namespace hpc {
   namespace options {

      option_base::option_base( const hpc::string& name )
         : _name( name ),
           _has_val( false ),
           _is_list( false )
      {
	 std::transform( _name.begin(), _name.end(), _name.begin(), ::tolower );
      }

      option_base::~option_base()
      {
      }

      void
      option_base::store_visit( format& fmt )
      {
         ASSERT( 0, "Not implemented." );
      }

      void
      option_base::set_name( const hpc::string& name )
      {
         _name = name;
	 std::transform( _name.begin(), _name.end(), _name.begin(), ::tolower );
      }

      const hpc::string&
      option_base::name() const
      {
         return _name;
      }

      bool
      option_base::has_value() const
      {
         return _has_val;
      }

      bool
      option_base::is_list() const
      {
         return _is_list;
      }

      option_base&
      option_base::operator=( const hpc::string& value )
      {
         parse( value );
         return *this;
      }

      string::string( const hpc::string& name,
                      optional<hpc::string> default_value,
		      bool strip )
         : option<hpc::string>( name, default_value ),
	   _strip( strip )
      {
      }

      string::string( const hpc::string& name,
                      optional<const char*> default_value,
		      bool strip )
         : option<hpc::string>( name ),
	   _strip( strip )
      {
         if( default_value )
            _def = hpc::string( *default_value );
      }

      void
      string::parse( const hpc::string& value )
      {
         _val = boost::algorithm::trim_copy( value );
         _has_val = true;
      }

      hpc::string
      string::store() const
      {
         return *_val;
      }

      boolean::boolean( const hpc::string& name,
                        optional<bool> default_value )
         : option<bool>( name, default_value )
      {
      }

      void
      boolean::parse( const hpc::string& value )
      {
         _val = boost::lexical_cast<bool>( value );
         _has_val = true;
      }

      hpc::string
      boolean::store() const
      {
      }

      integer::integer( const hpc::string& name,
                        optional<unsigned long> default_value )
         : option<unsigned long>( name, default_value )
      {
      }

      void
      integer::parse( const hpc::string& value )
      {
         _val = boost::lexical_cast<unsigned long>( value );
         _has_val = true;
      }

      hpc::string
      integer::store() const
      {
         return to_string( *_val );
      }

      real::real( const hpc::string& name,
                  optional<double> default_value )
         : option<double>( name, default_value )
      {
      }

      void
      real::parse( const hpc::string& value )
      {
         _val = boost::lexical_cast<double>( value );
         _has_val = true;
      }

      hpc::string
      real::store() const
      {
         return to_string( *_val );
      }
   }
}
