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

#ifndef hpc_mpi_async_hh
#define hpc_mpi_async_hh

#include <boost/unordered_map.hpp>
#include "libhpc/debug/assert.hh"
#include "libhpc/logging.hh"
#include "libhpc/mpi/comm.hh"

namespace hpc {
   namespace mpi {

      class async_event_handler
      {
      public:

         async_event_handler( int tag = 0 );

         int
         tag() const;

         virtual
         bool
         event( MPI_Status const& stat ) = 0;

      protected:

         int _tag;
      };

      class async
      {
      public:

         typedef async_event_handler event_handler;

      public:

         async( mpi::comm const& comm = mpi::comm::world );

         void
         set_comm( mpi::comm const& comm );

	 mpi::comm const&
	 comm() const;

	 mpi::comm const&
	 worker_comm() const;

         void
         set_max_events( unsigned max_evts );

         void
         add_event_handler( event_handler* eh );

         boost::unordered_map<int,event_handler*> const&
         event_handlers() const;

         bool
         run();

	 void
	 done( int ec = 0 ) const;

      protected:

         boost::unordered_map<int,event_handler*> _ev_hndlrs;
         unsigned _max_evts;
         mpi::comm const* _comm;
	 mpi::comm _wkr_comm;
      };

   }
}

#endif
