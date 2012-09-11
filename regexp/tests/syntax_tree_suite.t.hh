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
#include "libhpc/logging/logging.hh"
#include "libhpc/regexp/syntax_tree.hh"

using namespace hpc;

class syntax_tree_suite : public CxxTest::TestSuite {
public:

   void test_default_ctor()
   {
      re::syntax_tree st;
   }

   void test_empty()
   {
      re::syntax_tree st;
      st.construct( "" );
   }

   void test_concatenation()
   {
      re::syntax_tree st;
      st.construct( "ab" );
   }

   void test_force()
   {
      re::syntax_tree st;
      st.construct( "\\|a" );
   }

   void test_split()
   {
      re::syntax_tree st;
      st.construct( "b|c" );
   }

   void test_invalid_split()
   {
      re::syntax_tree st;
      TS_ASSERT_THROWS_ANYTHING( st.construct( "|c" ) );
      TS_ASSERT_THROWS_ANYTHING( st.construct( "a(|c)" ) );
   }

   void test_capture()
   {
      re::syntax_tree st;
      st.construct( "(a)" );
   }

   void test_long()
   {
      re::syntax_tree st;
      st.construct( "(a|b)cd(efg|hij)" );
   }

   void test_followpos()
   {
      re::syntax_tree st;
      st.construct( "(a|b)*a" );
      multimap<unsigned,unsigned> followpos;
      st._calc_followpos( followpos );
   }

   void test_calc_dfa_0()
   {
      re::syntax_tree st;
      st.construct( "(a|b)*a" );
      st._calc_dfa();
   }

   void test_calc_dfa_1()
   {
      re::syntax_tree st;
      st.construct( "(one)|(two)|(three)" );
      st._calc_dfa();
   }
};