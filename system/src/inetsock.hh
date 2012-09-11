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

#ifndef libhpc_system_inetsock_hh
#define libhpc_system_inetsock_hh

#include <netinet/in.h>
#include "socket.hh"

namespace hpc {
   namespace unix {

      class inetsock
         : public socket
      {
      public:

         ERRORS( address_in_use );

      public:

         inetsock();

         inetsock( const string& ip_addr,
                   uint16 port=0 );

         void
         open();

         void
         bind( const string& ip_addr=string(),
               uint16 port=0 );

         void
         accept( inetsock& client );
      };
   }
}

#endif