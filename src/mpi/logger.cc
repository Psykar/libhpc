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
#include "logger.hh"

namespace hpc {
   namespace mpi {

      double log_base_time;

      logger::logger( const std::string& filename,
		      unsigned level )
         : log::file( filename, level ),
           _base( filename )
      {
      }

      logger::~logger()
      {
      }

      void
      logger::open()
      {
         MPI_Comm_rank( MPI_COMM_WORLD, &_my_rank );
         std::stringstream ss;
         ss << _base << std::setfill( '0' ) << std::setw( 5 ) << _my_rank;
         _filename = ss.str();
         remove( _filename.c_str() );
         _file.open( _filename.c_str(), std::fstream::out | std::fstream::app );
         _get_new_line() = true;
      }

      void
      logger::prefix()
      {
	 double timestamp = MPI_Wtime() - log_base_time;
	 std::streamsize old_prec = _file.precision();
	 buffer() << std::setprecision( 9 ) << std::left << std::setw( 14 ) << timestamp << std::setprecision( old_prec );
	 buffer() << " " << _my_rank << " : " << indent;
	 write_buffer( "" );
	 _get_new_line() = false;
      }
   }
}

#endif
