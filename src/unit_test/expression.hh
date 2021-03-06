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

#ifndef hpc_unit_test_expression_hh
#define hpc_unit_test_expression_hh

#include <boost/format.hpp>
#include "libhpc/system/cuda.hh"
#include "libhpc/debug/assert.hh"
#include "test_case.hh"

namespace hpc {
   namespace test {

      template< class, class >
      class expression;

      template< class T >
      class side
      {
      public:

         typedef T value_type;

      public:

         CUDA_DEV_HOST
         side( T const& val )
            : _val( val )
         {
         }

         template< class U >
         CUDA_DEV_HOST
         expression<T,U>
         operator==( U const& op ) const
         {
            bool res = _val == op;
            return expression<T,U>( *this, side<U>( op ), res );
         }

         template< class U >
         CUDA_DEV_HOST
         expression<T,U>
         operator!=( const U& op ) const
         {
            bool res = _val != op;
            return expression<T,U>( *this, side<U>( op ), res );
         }

         template< class U >
         CUDA_DEV_HOST
         expression<T,U>
         operator<( const U& op ) const
         {
            bool res = _val < op;
            return expression<T,U>( *this, side<U>( op ), res );
         }

         template< class U >
         CUDA_DEV_HOST
         expression<T,U>
         operator<=( const U& op ) const
         {
            bool res = _val <= op;
            return expression<T,U>( *this, side<U>( op ), res );
         }

         template< class U >
         CUDA_DEV_HOST
         expression<T,U>
         operator>( const U& op ) const
         {
            bool res = _val > op;
            return expression<T,U>( *this, side<U>( op ), res );
         }

         template< class U >
         CUDA_DEV_HOST
         expression<T,U>
         operator>=( const U& op ) const
         {
            bool res = _val >= op;
            return expression<T,U>( *this, side<U>( op ), res );
         }

         template< class U >
         CUDA_DEV_HOST
         expression<T,U>
         delta( U const& op,
                double epsilon ) const
         {
            bool res = (_val >= op - epsilon && _val <= op + epsilon);
            return expression<T,U>( *this, side<U>( op ), res );
         }

         CUDA_DEV_HOST
         const T&
         operator*() const
         {
            return _val;
         }

      protected:

         T _val;
      };

      template<>
      class side<char const*>
         : public side<std::string>
      {
      public:

         CUDA_DEV_HOST_INL
         side( char const* val )
            : side<std::string>( val )
         {
         }
      };

      template< size_t N >
      class side<char[N]>
         : public side<std::string>
      {
      public:

         CUDA_DEV_HOST_INL
         side( char const val[N] )
            : side<std::string>( val )
         {
         }
      };

      template< class T,
                class U >
      class expression
      {
      public:

         CUDA_DEV_HOST
         expression( side<T> const& left,
                     side<U> const& right,
                     bool result )
            : _left( left ),
              _right( right ),
              _res( result ),
              _lhs_str( 0 ),
              _rhs_str( 0 ),
              _eps( 0.0 ),
              _expr_str( 0 ),
              _file( 0 ),
              _line( 0 )
         {
         }

         CUDA_DEV_HOST
         expression&
         set_info( char const* expr_str,
                   char const* file,
                   int line )
         {
            _expr_str = expr_str;
            _file = file;
            _line = line;
            return *this;
         }

         CUDA_DEV_HOST
         expression&
         set_delta_info( char const* lhs_str,
                         char const* rhs_str,
                         double eps,
                         char const* file,
                         int line )
         {
            _lhs_str = lhs_str;
            _rhs_str = rhs_str;
            _eps = eps;
            _file = file;
            _line = line;
            return *this;
         }

         CUDA_DEV_HOST
         void
         test( result_buffer<>& rb,
               char const* info = 0 );

         CUDA_DEV_HOST
         bool
         result() const
         {
            return _res;
         }

         operator bool()
         {
            return _res;
         }

         CUDA_DEV_HOST
         typename side<T>::value_type const&
         lhs() const
         {
            return *_left;
         }

         char const*
         lhs_str() const
         {
            return _lhs_str;
         }

         CUDA_DEV_HOST
         typename side<U>::value_type const&
         rhs() const
         {
            return *_right;
         }

         char const*
         rhs_str() const
         {
            return _rhs_str;
         }

         char const*
         str() const
         {
            return _expr_str;
         }

         double
         epsilon() const
         {
            return _eps;
         }

         CUDA_DEV_HOST
         char const*
         file() const
         {
            return _file;
         }

         CUDA_DEV_HOST
         int
         line() const
         {
            return _line;
         }

      protected:

         side<T> _left;
         side<U> _right;
         bool _res;
         char const* _lhs_str;
         char const* _rhs_str;
         double _eps;
         char const* _expr_str;
         char const* _file;
         int _line;
      };

      class decompose
      {
      public:

         template< class T >
         CUDA_DEV_HOST
         side<T>
         operator->*( T const& op ) const
         {
            return side<T>( op );
         }
      };

   }
}

#include "failures.hh"
#include "impl/expression.hh"

#endif
