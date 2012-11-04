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

#ifndef libhpc_profile_timer_hh
#define libhpc_profile_timer_hh

#include "libhpc/system/timer.hh"

namespace hpc {
   namespace profile {

      ///
      ///
      ///
      class timer
      {
      public:

	 timer();

	 void
	 start();

	 void
	 stop();

	 void
	 stop_tally();

	 double
	 total() const;

	 double
	 mean() const;

      protected:

	 unix::time_type _start;
	 double _total;
	 unsigned long _cnt;
      };
   }
}

#endif