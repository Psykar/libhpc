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

#include "libhpc/debug/assert.hh"
#include "timer.hh"

namespace hpc {
   namespace profile {

      timer::timer()
	 : _total( 0.0 ),
	   _cnt( 0 )
      {
      }

      void
      timer::start()
      {
	 _start = unix::timer();
      }

      void
      timer::stop()
      {
	 _total += unix::seconds( unix::timer() - _start );
      }

      void
      timer::stop_tally()
      {
	 stop();
	 ++_cnt;
      }

      double
      timer::total() const
      {
	 return _total;
      }

      double
      timer::mean() const
      {
	 ASSERT( _cnt );
	 return _total/(double)_cnt;
      }
   }
}
