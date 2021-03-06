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

#ifndef hpc_algorithm_binary_partitioner_hh
#define hpc_algorithm_binary_partitioner_hh

#include "libhpc/mpi/comm.hh"
#include "libhpc/system/math.hh"
#include "libhpc/mpi/helpers.hh"
#include "libhpc/mpi/partition.hh"
#include "median.hh"

namespace hpc {

   template< class IterT >
   class median_iterator
   {
   public:

      typedef IterT iterator_type;
      typedef typename iterator_type::value_type median_type;

   public:

      median_iterator( iterator_type it,
		       median_type med = 0 )
	 : _it( it ),
	   _med( med )
      {
      }

      bool
      operator!=( iterator_type const& other ) const
      {
	 return _it = other._it;
      }

      int
      operator*() const
      {
	 return *_it <= _med;
      }

      void
      operator++()
      {
	 ++_it;
      }

   protected:

      iterator_type _it;
      median_type _med;
   };

   template< class DimIterT,
	     class PermuteT >
   class binary_partitioner
   {
   public:

      typedef DimIterT dim_iter_type;
      typedef PermuteT permute_type;
      typedef typename dim_iter_type::const_iterator coord_iter_type;
      typedef typename coord_iter_type::value_type coord_type;

   public:

      binary_partitioner( dim_iter_type dims_begin,
			  dim_iter_type dims_end,
			  permute_type perm,
			  unsigned ppc = 1000,
			  mpi::comm const& comm = mpi::comm::world )
	 : _dims_begin( dims_begin ),
	   _dims_end( dims_end ),
	   _perm( perm ),
	   _max_ppc( ppc )
      {
	 _lsize = dims_begin->end() - dims_begin->begin();
	 _gsize = comm.all_gather( _lsize );
	 _n_leaf_cells = _lsize/_max_ppc;
	 if( _n_leaf_cells*_max_ppc < _lsize )
	    ++_n_leaf_cells;
	 _depth = log2i( _n_leaf_cells );
	 if( (1 << _depth) < _n_leaf_cells )
	    ++_depth;
	 _n_inc_leaf_cells = (1 << _depth) - _n_leaf_cells;

	 // Calculate number of cells, including branch cells.
	 _n_cells = 0;
	 for( unsigned ii = 0; ii < _depth; ++ii )
	    _n_cells += 1 << ii;
	 _n_cells -= (1 << _depth) - _n_inc_leaf_cells;
      }

      unsigned
      n_cells()
      {
	 return _n_cells;
      }

      unsigned
      n_leaf_cells()
      {
	 return _n_leaf_cells;
      }

      void
      partition( unsigned cell,
		 std::vector<std::array<coord_type,2> > const& bnds,
		 mpi::comm const& comm )
      {
	 // Which dimension is best to split?
	 unsigned dim = _choose_dim( bnds );
	 coord_iter_type crd_begin = (_dims_begin + dim)->begin();
	 coord_iter_type crd_end = (_dims_begin + dim)->end();

	 // Different version depending on serial or parallel.
	 if( comm.size() > 1 )
	 {
	    // Calculate an initial median.
	    unsigned left_size = mpi::balanced_left_size( _lsize, comm );
	    coord_type med = select( crd_begin, crd_end, left_size, comm );

	    // Calculate which ranks will collect on the left.
	    mpi::balanced_partition part( comm );
	    part.split( median_iterator_type( crd_begin, med ), median_iterator_type( crd_end ) );

	    // Reevaluate the kth position to split on such that all
	    // ranks on the left keep their array sizes.
	    unsigned sub_size = part.sub_comm().all_reduce( _lsize );
	    bool root_on_left = comm.bcast2( part.collecting_left() );
	    unsigned root_sub_size = comm.bcast2( sub_size );
	    left_size = root_on_left ? root_sub_size : (_gsize - root_sub_size);
	    med = select( crd_begin, crd_end, left_size, comm );

	    // Calculate mpi::partition.
	    part.connect( median_iterator_type( crd_begin, med ), median_iterator_type( crd_end ) );

	    // Stash results.
	    _sub_comm = part.sub_comm();
	    _split = std::tuple<coord_type,unsigned>( med, dim );
	    _sub_size = sub_size;

	    // Call permutation callback.
	    _perm( part );
	 }
	 else
	 {
	    // Use the provided "cell" index to identify which particles to
	    // use as the cell to be split.
	    crd_begin += _offs[cell];
	    crd_end += _offs[cell] + _cnts[cell];

	    // Calculate median.
	    coord_type med = median( crd_begin, crd_end, mpi::comm::self );

	    // Create partition.
	    mpi::balanced_partition part( mpi::comm::self );
	    part.split( median_iterator_type( crd_begin, med ), median_iterator_type( crd_end ) );

	    // Stash results.
	    unsigned lc = _left_child( cell );
	    unsigned rc = _right_child( cell );
	    _offs[lc] = _offs[cell];
	    _cnts[lc] = part.left_size();
	    _offs[rc] = _offs[cell] + _cnts[lc];
	    _cnts[rc] = part.right_size();
	    _sub_comm = mpi::comm::self;
	    _split = std::tuple<coord_type,unsigned>( med, dim );

	    // Call permutation.
	    _perm( part );
	 }
      }

      mpi::comm const&
      sub_comm() const
      {
	 return _sub_comm;
      }

      std::tuple<coord_type,unsigned> const&
      split() const
      {
	 return _split;
      }

   protected:

      void
      _left_child( unsigned cell ) const
      {
	 return (cell + 1)*2;
      }

      void
      _right_child( unsigned cell ) const
      {
	 return _left_child( cell ) + 1;
      }

      void
      _choose_dim( std::vector<std::array<coord_type,2> > bnds )
      {
	 // Just select the longest dimension.
	 auto res = std::max_element( bnds.begin(), bnds.end(),
				      []( auto const& x, auto const& y )
				      {
					 return (x[1] - x[0]) < (y[1] - y[0]);
				      } );
	 return res - bnds.begin();
      }

   protected:

      dim_iter_type _dims_begin, _dims_end;
      permute_type _perm;
      unsigned _max_ppc;
      unsigned _depth;
      unsigned _lsize, _gsize;
      unsigned _n_leaf_cells, _n_cells, _n_inc_leaf_cells;
      std::vector<coord_type> _min, _max;
      std::vector<unsigned> _offs, _cnts;
      std::tuple<coord_type,unsigned> _split;
      unsigned _sub_size;
      mpi::comm _sub_comm;
   };

}

#endif
