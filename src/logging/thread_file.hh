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

#ifndef libhpc_logging_thread_file_hh
#define libhpc_logging_thread_file_hh

#if !defined(NLOG) && !defined(NTHREAD)

#include <thread>
#include <mutex>
#include <typeinfo>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#include "libhpc/logging/logging.hh"

namespace hpc {
   namespace logging {
      namespace thread {

	 ///
	 ///
	 ///
	 class file
	    : public logging::file
	 {
	 public:

	    file( std::string const& filename,
		  unsigned min_level = 0 );

	    virtual
	    ~file();

	    virtual void
	    open();

	    virtual void
	    write();

	 protected:

	    void
	    _open_file();

	    void
	    _close_file();

	 protected:

            std::mutex _write;
	    std::set<std::thread::id> _tids;
	    std::string _base;
	 };

      }
   }
}

#endif

#endif
