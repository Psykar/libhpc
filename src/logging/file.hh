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

#ifndef libhpc_logging_file_hh
#define libhpc_logging_file_hh

#ifndef NLOG

#include "logger.hh"

namespace hpc {
   namespace log {

      ///
      ///
      ///
      class file
         : public logger
      {
      public:

         file( const std::string& filename,
               unsigned min_level=0 );

         virtual
         ~file();

         virtual void
         open();

         virtual void
         close();

         virtual void
         write();

      protected:

         std::string _filename;
         std::ofstream _file;
      };
   }
}

#endif

#endif
