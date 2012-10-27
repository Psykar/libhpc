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

#include <cxxtest/TestSuite.h>
#include "libhpc/containers/po2_ring_buffer.hh"

using namespace hpc;

class po2_ring_buffer_suite : public CxxTest::TestSuite {
public:

   void test_default_ctor()
   {
      po2_ring_buffer<int> rb;
   }

   void test_resize()
   {
      po2_ring_buffer<int> rb;
      rb.resize( 8 );
      TS_ASSERT_EQUALS( rb._buf.size(), 8 );
      TS_ASSERT_EQUALS( rb._start, 0 );
      TS_ASSERT_EQUALS( rb._size, 0 );
   }

   void test_insert_nothing()
   {
      po2_ring_buffer<int> rb;
      rb.resize( 8 );
      po2_ring_buffer<int>::size_type res;
      res = rb.insert( &res, &res );
      TS_ASSERT_EQUALS( res, 0 );
      TS_ASSERT_EQUALS( rb._start, 0 );
      TS_ASSERT_EQUALS( rb._size, 0 );
   }

   void test_insert_when_full()
   {
      po2_ring_buffer<int> rb;
      rb.resize( 8 );
      po2_ring_buffer<int>::size_type res;
      for( unsigned ii = 0; ii < 8; ++ii )
         rb.insert( &ii, &ii + 1 );
      res = rb.insert( &res, &res + 1 );
      TS_ASSERT_EQUALS( res, 0 );
      TS_ASSERT_EQUALS( rb._start, 0 );
      TS_ASSERT_EQUALS( rb._size, 8 );
   }

   void test_insert_one_element()
   {
      po2_ring_buffer<int> rb;
      rb.resize( 8 );
      po2_ring_buffer<int>::size_type res;
      for( unsigned ii = 0; ii < 8; ++ii )
      {
         res = rb.insert( &ii, &ii + 1 );
         TS_ASSERT_EQUALS( res, 1 );
         TS_ASSERT_EQUALS( rb._start, 0 );
         TS_ASSERT_EQUALS( rb._size, ii + 1 );
      }
   }

   void test_insert_many()
   {
      po2_ring_buffer<int> rb;
      rb.resize( 8 );
      vector<int> buf( 8 );
      hpc::iota( buf.begin(), buf.end(), 0 );
      po2_ring_buffer<int>::size_type res;
      res = rb.insert( buf.begin(), buf.end() );
      TS_ASSERT_EQUALS( res, 8 );
      TS_ASSERT_EQUALS( rb._start, 0 );
      TS_ASSERT_EQUALS( rb._size, 8 );
   }

   void test_insert_many_full()
   {
      po2_ring_buffer<int> rb;
      rb.resize( 8 );
      vector<int> buf( 6 );
      hpc::iota( buf.begin(), buf.end(), 0 );
      po2_ring_buffer<int>::size_type res;
      res = rb.insert( buf.begin(), buf.end() );
      TS_ASSERT_EQUALS( res, 6 );
      TS_ASSERT_EQUALS( rb._start, 0 );
      TS_ASSERT_EQUALS( rb._size, 6 );
      res = rb.insert( buf.begin(), buf.end() );
      TS_ASSERT_EQUALS( res, 2 );
      TS_ASSERT_EQUALS( rb._start, 0 );
      TS_ASSERT_EQUALS( rb._size, 8 );
   }
};
