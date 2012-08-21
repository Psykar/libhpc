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

#ifndef libhpc_regexp_codes_hh
#define libhpc_regexp_codes_hh

namespace hpc {
   namespace re {

      enum codes
      {
         match = 0,
         concat = 127,
         split = 128,
         many = 129,
         capture = 130,
         open_capture = 131,
         close_capture = 132,
         terminal = 133
      };
   }
}

#endif
