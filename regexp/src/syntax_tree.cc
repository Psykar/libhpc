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

#include "libhpc/debug/debug.hh"
#include "libhpc/logging/logging.hh"
#include "syntax_tree.hh"
#include "dfa.hh"
#include "codes.hh"

namespace hpc {
   namespace re {

      bool
      syntax_tree::dfa_state::compare::operator()( const scoped_ptr<dfa_state>& op0,
                                                   const scoped_ptr<dfa_state>& op1 ) const
      {
         return op0->indices < op1->indices;
         // if( indices.size() < op->indices.size() )
         //    return true;
         // else if( indices.size() > op->indices.size() )
         //    return false;
         // else
         // {
         //    auto it0 = indices.cbegin(), it1 = op->indices.cbegin();
         //    for( ; it0 != indices.cend(); ++it0, ++it1 )
         //    {
         //       if( *it0 < *it1 )
         //          return true;
         //       else if( *it0 > *it1 )
         //          return false;
         //    }
         // }
      }

      syntax_tree::dfa_state::dfa_state( unsigned id )
         : id( id ),
           open( std::numeric_limits<uint16>::max() ),
           close( std::numeric_limits<uint16>::max() )
      {
      }

      syntax_tree::dfa_state::dfa_state( const set<unsigned>& idxs,
                                         unsigned id )
         : id( id ),
           open( std::numeric_limits<uint16>::max() ),
           close( std::numeric_limits<uint16>::max() )
      {
         indices.insert( idxs.begin(), idxs.end() );
      }

      syntax_tree::node::node( byte data,
                               node* left,
                               node* right )
         : data( data )
      {
         child[0] = left;
         child[1] = right;
      }

      unsigned
      syntax_tree::node::calc_indices( unsigned idx )
      {
         if( child[0] || child[1] )
         {
            if( child[0] )
               idx = child[0]->calc_indices( idx );
            if( child[1] )
               idx = child[1]->calc_indices( idx );
            index = -1;
            return idx;
         }
         else
         {
            index = idx;
            return idx + 1;
         }
      }

      void
      syntax_tree::node::calc_idx_to_node( vector<node*>& idx_to_node ) const
      {
         if( index != -1 )
            idx_to_node[index] = (node*)this;
         for( unsigned ii = 0; ii < 2; ++ii )
         {
            if( child[ii] )
               child[ii]->calc_idx_to_node( idx_to_node );
         }
      }

      bool
      syntax_tree::node::nullable() const
      {
         if( data == static_cast<byte>( code_split ) )
         {
            ASSERT( child[0] && child[1] );
            return child[0]->nullable() || child[1]->nullable();
         }
         else if( data == static_cast<byte>( code_concat ) )
         {
            ASSERT( child[0] && child[1] );
            return child[0]->nullable() && child[1]->nullable();
         }
         else if( data == static_cast<byte>( code_many ) )
         {
            ASSERT( (bool)child[0] );
            return true;
         }
         else if( data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            return child[0]->nullable();
         }
         else
         {
            return false;
         }
      }

      void
      syntax_tree::node::calc_firstpos()
      {
         LOG_ENTER();

         firstpos.clear();
         for( unsigned ii = 0; ii < 2; ++ii )
         {
            if( child[ii] )
               child[ii]->calc_firstpos();
         }

         if( data == static_cast<byte>( code_split ) )
         {
            for( unsigned ii = 0; ii < 2; ++ii )
            {
               ASSERT( (bool)child[ii] );
               firstpos.insert( child[ii]->firstpos.begin(), child[ii]->firstpos.end() );
            }
         }
         else if( data == static_cast<byte>( code_concat ) )
         {
            ASSERT( (bool)child[0] );
            firstpos.insert( child[0]->firstpos.begin(), child[0]->firstpos.end() );
            if( child[0]->nullable() )
            {
               ASSERT( (bool)child[1] );
               firstpos.insert( child[1]->firstpos.begin(), child[1]->firstpos.end() );
            }
         }
         else if( data == static_cast<byte>( code_many ) ||
                  data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            firstpos.insert( child[0]->firstpos.begin(), child[0]->firstpos.end() );
         }
         else
         {
            ASSERT( index != -1 );
            firstpos.insert( index );
         }

         LOGLVLN( logging::debug, "firstpos (", (int)data, "): ", firstpos );
         LOG_EXIT();
      }

      void
      syntax_tree::node::calc_lastpos()
      {
         lastpos.clear();
         for( unsigned ii = 0; ii < 2; ++ii )
         {
            if( child[ii] )
               child[ii]->calc_lastpos();
         }

         if( data == static_cast<byte>( code_split ) )
         {
            for( unsigned ii = 0; ii < 2; ++ii )
            {
               ASSERT( (bool)child[ii] );
               lastpos.insert( child[ii]->lastpos.begin(), child[ii]->lastpos.end() );
            }
         }
         else if( data == static_cast<byte>( code_concat ) )
         {
            ASSERT( (bool)child[1] );
            lastpos.insert( child[1]->lastpos.begin(), child[1]->lastpos.end() );
            if( child[1]->nullable() )
            {
               ASSERT( (bool)child[0] );
               lastpos.insert( child[0]->lastpos.begin(), child[0]->lastpos.end() );
            }
         }
         else if( data == static_cast<byte>( code_many ) ||
                  data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            lastpos.insert( child[0]->lastpos.begin(), child[0]->lastpos.end() );
         }
         else
         {
            ASSERT( index != -1 );
            lastpos.insert( index );
         }
      }

      void
      syntax_tree::node::calc_followpos( multimap<unsigned,unsigned>& followpos )
      {
         LOG_ENTER();

         if( data == static_cast<byte>( code_concat ) )
         {
            LOGLVLN( logging::debug, "Concatenation." );

            // Depth first.
            ASSERT( child[0] && child[1] );
            for( unsigned ii = 0; ii < 2; ++ii )
               child[ii]->calc_followpos( followpos );

            for( const auto& lp : child[0]->lastpos )
            {
               for( const auto& fp : child[1]->firstpos )
               {
                  LOGLVLN( logging::debug, "Adding ", fp, " to ", lp );
                  followpos.insert( lp, fp );
               }
            }
         }
         else if( data == static_cast<byte>( code_many ) )
         {
            // Depth first.
            child[0]->calc_followpos( followpos );

            for( const auto& lp : lastpos )
            {
               for( const auto& fp : firstpos )
                  followpos.insert( lp, fp );
            }
         }
         else if( data == static_cast<byte>( code_capture ) )
         {
            // Depth first.
            child[0]->calc_followpos( followpos );
         }
         else
         {
            for( unsigned ii = 0; ii < 2; ++ii )
            {
               if( child[ii] )
                  child[ii]->calc_followpos( followpos );
            }
         }

         LOG_EXIT();
      }

      uint16
      syntax_tree::node::calc_capture_indices( uint16 idx,
                                               list<node*>& queue )
      {
         LOG_ENTER();

         if( data == static_cast<byte>( code_concat ) ||
             data == static_cast<byte>( code_split ) )
         {
            ASSERT( child[0] && child[1] );
            for( unsigned ii = 0; ii < 2; ++ii )
               idx = child[ii]->calc_capture_indices( idx, queue );
         }
         else if( data == static_cast<byte>( code_many ) )
         {
            ASSERT( (bool)child[0] );
            idx = child[0]->calc_capture_indices( idx, queue );
         }
         else if( data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            queue.push_back( child[0].get() );
            capture_index = idx++;
            LOGLVLN( logging::debug, "Indexed a capture at ", capture_index );
         }

         LOG_EXIT();
         return idx;
      }

      void
      syntax_tree::node::calc_captures()
      {
         LOG_ENTER();

         if( data == static_cast<byte>( code_concat ) ||
             data == static_cast<byte>( code_split ) )
         {
            ASSERT( child[0] && child[1] );
            for( unsigned ii = 0; ii < 2; ++ii )
               child[ii]->calc_captures();
         }
         else if( data == static_cast<byte>( code_many ) )
         {
            ASSERT( (bool)child[0] );
            child[0]->calc_captures();
         }
         else if( data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            child[0]->calc_capture_open( capture_index );
            child[0]->calc_capture_close( capture_index );
            child[0]->calc_captures();
         }

         LOG_EXIT();
      }

      void
      syntax_tree::node::calc_capture_open( uint16 idx )
      {
         LOG_ENTER();

         if( (bool)child[0] )
            child[0]->calc_capture_open( idx );
         else
         {
            LOGLVLN( logging::debug, "Capture open at '", static_cast<char>( data ), "' with index ", idx );
            open.insert( idx );
         }

         LOG_EXIT();
      }

      void
      syntax_tree::node::calc_capture_close( uint16 idx )
      {
         LOG_ENTER();

         if( data == static_cast<byte>( code_many ) ||
             data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            child[0]->calc_capture_close( idx );
         }
         else if( (bool)child[1] )
            child[1]->calc_capture_close( idx );
         else
         {
            LOGLVLN( logging::debug, "Capture close at '", static_cast<char>( data ), "' with index ", idx );
            close.insert( idx );
         }

         LOG_EXIT();
      }

      int16
      syntax_tree::node::calc_split_indices( int16 idx )
      {
         LOG_ENTER();

         if( data == static_cast<byte>( code_concat ) )
         {
            ASSERT( child[0] && child[1] );
            for( unsigned ii = 0; ii < 2; ++ii )
               idx = child[ii]->calc_split_indices( idx );
         }
         else if( data == static_cast<byte>( code_many ) ||
                  data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            idx = child[0]->calc_split_indices( idx );
         }
         else if( data == static_cast<byte>( code_split ) )
         {
            ASSERT( child[0] && child[1] );
            child[0]->calc_split_terminal( idx );
            child[1]->calc_split_terminal( -idx );
            for( unsigned ii = 0; ii < 2; ++ii )
               idx = child[ii]->calc_split_indices( idx + 1 );
         }

         LOG_EXIT();
         return idx;
      }

      void
      syntax_tree::node::calc_split_terminal( int16 idx )
      {
         LOG_ENTER();

         if( data == static_cast<byte>( code_many ) ||
             data == static_cast<byte>( code_capture ) )
         {
            ASSERT( (bool)child[0] );
            child[0]->calc_split_terminal( idx );
         }
         else if( (bool)child[1] )
            child[1]->calc_split_terminal( idx );
         else
         {
            LOGLVLN( logging::debug, "Setting split index at '", static_cast<char>( data ), "' with index ", idx );
            split_index = idx;
         }

         LOG_EXIT();
      }

      void
      syntax_tree::clear()
      {
         LOG_ENTER();

         _root = NULL;
         _num_captures = 0;

         LOG_EXIT();
      }

      void
      syntax_tree::construct( const string& expr )
      {
         LOG_ENTER();

         // Clear out anything we may have.
         clear();

         // Only continue if we have something to work with.
         if( !expr.empty() )
         {
            const char* ptr = expr.c_str();
            _root = _construct_recurse( ptr );

            // Add final terminal state.
            _root = new node( static_cast<byte>( code_concat ), _root.release(), new node( static_cast<byte>( code_match ) ) );
         }

         LOG_EXIT();
      }

      void
      syntax_tree::to_dfa( dfa& dfa )
      {
         LOG_ENTER();

         // Only process if we have something to process.
         if( _root )
         {

            // First convert to local DFA representation.
            _calc_dfa();

            // Build the moves. Use '_num_states' to only store moves for
            // non-meta states.
            vector<uint16> moves( _num_states*256 );
            vector<uint16> meta_moves( _num_meta_states );
            vector<uint16> open( _num_meta_states );
            vector<uint16> close( _num_meta_states );
            for( auto it = _states.cbegin(); it != _states.cend(); ++it )
            {
               dfa_state* cur = it->get();
               _conv_moves( cur, moves, meta_moves, open, close );
            }
            for( auto it = _meta_states.cbegin(); it != _meta_states.cend(); ++it )
            {
               dfa_state* cur = it->get();
               _conv_moves( cur, moves, meta_moves, open, close );
            }

            // // Setup the captures.
            // csr<uint16> open, close;
            // _to_dfa_captures( open, close );

            // Setup dfa.
            dfa.set_states( moves, meta_moves, open, close );
         }
         else
            dfa.clear();

         LOG_EXIT();
      }

      syntax_tree::node*
      syntax_tree::_construct_recurse( const char*& ptr )
      {
         LOG_ENTER();

         // Keep this sub-branch isolated, begin from scratch.
         node* cur_node = NULL;

         // Walk over each character.
         char ch = *ptr;
         bool force = false, concat = false;
         node* new_node;
         while( ch != 0 )
         {
            if( force )
            {
               LOGLVLN( logging::debug, "Forced: ", ch );

               // Concatenate.
               new_node = new node( ch );
               concat = true;
               force = false;
            }
            else if( ch == '\\' )
            {
               // The next character is a terminal.
               force = true;
            }
            else if( ch == '|' )
            {
               LOGLVLN( logging::debug, "Splitting." );

               // 'Or'.
               ASSERT( cur_node, "Must be expression to left of |." );
               cur_node = new node( static_cast<byte>( code_split ), cur_node, _construct_recurse( ++ptr ) );
               ch = *ptr;
            }
            else if( ch == '*' )
            {
               LOGLVLN( logging::debug, "Star." );

               // Closure?
               ASSERT( cur_node, "Must be existing expression." );
               cur_node = new node( static_cast<byte>( code_many ), cur_node );
            }
            else if( ch == '(' )
            {
               LOGLVLN( logging::debug, "Opening capture." );

               // Create new sub-branch.
               new_node = new node( static_cast<byte>( code_capture ), _construct_recurse( ++ptr ) );

               // // Mark the capture.
               // LOGLN( "Marking capture at level ", level );
               // new_node->open.insert( level );
               // new_node->close.insert( level );

               // // Keep track of max levels and number of captures.
               // _num_levels = std::max<uint16>( level + 1, _num_levels );
               // ++_num_captures;

               ch = *ptr;
               concat = true;
            }
            else if( ch == ')' )
            {
               LOGLVLN( logging::debug, "Closing capture." );

               // // Insert closing capture.
               // new_node = new node( static_cast<byte>( code_concat ),
               //                      cur_node,
               //                      new node( static_cast<byte>( code_close_capture ) ) );

               // // Mark closing capture.
               // cur_node->close.insert( std::make_pair( level - 1, cap_idx++ ) );

               // Increment number of global captures.
               ++_num_captures;

               LOG_EXIT();
               return cur_node;
            }
            else
            {
               // Concatenate.
               LOGLVLN( logging::debug, "Concatenate: ", ch );
               new_node = new node( ch );
               concat = true;
            }

            // Handle concatenation.
            if( concat )
            {
               if( cur_node )
                  cur_node = new node( static_cast<byte>( code_concat ), cur_node, new_node );
               else
                  cur_node = new_node;
               concat = false;
            }

            // Advance.
            if( ch != 0 )
               ch = *++ptr;
         }

         LOG_EXIT();
         return cur_node;
      }

      void
      syntax_tree::_calc_followpos( multimap<unsigned,unsigned>& followpos )
      {
         followpos.clear();

         ASSERT( (bool)_root );
         _num_states = _root->calc_indices( 0 );
         _num_meta_states = 0;
         _calc_idx_to_node( _idx_to_node );
         _root->calc_firstpos();
         _root->calc_lastpos();
         _root->calc_followpos( followpos );
      }

      void
      syntax_tree::_calc_idx_to_node( vector<node*>& idx_to_node ) const
      {
         idx_to_node.reallocate( _num_states );
         _root->calc_idx_to_node( idx_to_node );
      }

      void
      syntax_tree::_calc_capture_indices()
      {
         LOG_ENTER();

         list<node*> queue;
         queue.push_back( _root.get() );
         uint16 idx = 0;
         while( !queue.empty() )
         {
            node* cur = queue.front();
            queue.pop_front();
            idx = cur->calc_capture_indices( idx, queue );
         }

         // Store total number of captures.
         _num_captures = idx;

         LOG_EXIT();
      }

      void
      syntax_tree::_calc_captures()
      {
         ASSERT( (bool)_root );
         _root->calc_captures();
      }

      void
      syntax_tree::_calc_split_indices()
      {
         ASSERT( (bool)_root );
         _root->calc_split_indices( 0 );
      }

      void
      syntax_tree::_calc_dfa()
      {
         LOG_ENTER();

         _states.clear();
         _to_proc.clear();

         _calc_followpos( _followpos );
         _calc_capture_indices();
         _calc_captures();
         _calc_split_indices();

         unsigned cur_id = 0;
         dfa_state* s0 = new dfa_state( _root->firstpos, cur_id++ );
         _states.insert( s0 );
         _to_proc.push_back( s0 );
         while( !_to_proc.empty() )
         {
            _proc_dfa_state( *_to_proc.front(), cur_id );
            _to_proc.pop_front();
         }

         LOG_EXIT();
      }

      void
      syntax_tree::_proc_dfa_state( dfa_state& state,
                                    unsigned& cur_id )
      {
         LOG_ENTER();
         LOGLVLN( logging::debug, "Looking at state: ", state.indices );

         map<char,dfa_state*> new_states;
         bool is_accepting = false;
         for( auto idx : state.indices )
         {
            node* node = _idx_to_node[idx];
            char data = node->data;
            if( data == static_cast<byte>( code_match ) )
            {
               is_accepting = true;
               continue;
            }
            ASSERT( data < static_cast<byte>( code_terminal ) );
            LOGLVLN( logging::debug, "Index: ", idx, ", data: ", data );

            auto res = new_states.insert( data, NULL );
            if( res.second )
            {
               LOGLVLN( logging::debug, "Creating new temporary state." );
               res.first->second = new dfa_state;
            }
            dfa_state& new_state = *res.first->second;

            auto rng = _followpos.equal_range( idx );
            for( ; rng.first != rng.second; ++rng.first )
               new_state.indices.insert( rng.first->second );
         }
#ifndef NLOG
         LOGLVLN( logging::debug, "Potential new states:", setindent( 2 ) );
         for( const auto& state_pair : new_states )
            LOGLVLN( logging::debug, state_pair.second->indices );
         LOGLV( logging::debug, setindent( -2 ) );
#endif

         for( const auto& state_pair : new_states )
         {
            char data = state_pair.first;

            auto res = _states.insert( state_pair.second );
            dfa_state* new_state = res.first->get();

            // Insert the move operation.
            state.moves.insert( data, new_state );

            if( res.second )
            {
               LOGLVLN( logging::debug, "Created new state, ", (*res.first)->indices, ", with id ", cur_id );
               new_state->id = cur_id++;
               _to_proc.push_back( new_state );
            }
         }
#ifndef NLOG
         LOGLVLN( logging::debug, "State moves:", setindent( 2 ) );
         for( const auto& move : state.moves )
            LOGLVLN( logging::debug, move.first, " -> ", move.second->indices );
         LOGLV( logging::debug, setindent( -2 ) );
#endif

         // Insert any capture openings on this state.
         for( auto idx : state.indices )
         {
            node& cur_node = *_idx_to_node[idx];
            if( cur_node.data != static_cast<byte>( code_match ) )
            {
               dfa_state* new_state = state.moves.get( cur_node.data );

               dfa_state *cur_meta = NULL;
               for( auto open : cur_node.open )
               {
                  dfa_state* new_meta = new dfa_state( _num_states + _num_meta_states++ );
                  new_meta->open = open;
                  if( cur_meta )
                     cur_meta->moves.insert( 0, new_meta );
                  else
                     state.moves[cur_node.data] = new_meta;
                  cur_meta = new_meta;
                  _meta_states.push_back( cur_meta );
                  LOGLVLN( logging::debug, "Creating new meta state to open capture ", open, " along ", (char)cur_node.data );
               }
               for( auto close : cur_node.close )
               {
                  dfa_state* new_meta = new dfa_state( _num_states + _num_meta_states++ );
                  new_meta->close = close;
                  if( cur_meta )
                     cur_meta->moves.insert( 0, new_meta );
                  else
                     state.moves[cur_node.data] = new_meta;
                  cur_meta = new_meta;
                  _meta_states.push_back( cur_meta );
                  LOGLVLN( logging::debug, "Creating new meta state to close capture ", close, " along ", (char)cur_node.data );
               }
               if( cur_meta )
                  cur_meta->moves.insert( 0, new_state );

               // new_state.open.insert( cur_node.open.begin(), cur_node.open.end() );
               // LOGLN( "Adding open ", cur_node.open, " to state ", new_state.indices );

               // new_state.close.insert( cur_node.close.begin(), cur_node.close.end() );
               // LOGLN( "Adding close ", cur_node.close, " to state ", new_state.indices );
            }
         }

         // If this state is accepting (i.e. has a permissible movement
         // from the match code) add a movement to itself.
         if( is_accepting )
            state.moves.insert( static_cast<byte>( code_match ), &state );

         LOG_EXIT();
      }

      void
      syntax_tree::_conv_moves( const dfa_state* state,
                                vector<uint16>& moves,
                                vector<uint16>& meta_moves,
                                vector<uint16>& open,
                                vector<uint16>& close ) const
      {
         // Separate out meta states.
         if( state->id < _num_states )
         {
            vector<uint16>::view state_moves( moves, 256, state->id*256 );
            std::fill( state_moves.begin(), state_moves.end(), std::numeric_limits<uint16>::max() );
            for( const auto& elem : state->moves )
               state_moves[static_cast<index>( elem.first )] = elem.second->id;
         }
         else
         {
            unsigned meta_id = state->id - _num_states;
            meta_moves[meta_id] = state->moves.get( 0 )->id;
            open[meta_id] = state->open;
            close[meta_id] = state->close;
         }
      }

      // void
      // syntax_tree::_to_dfa_captures( csr<uint16>& open,
      //                                csr<uint16>& close )
      // {
      //    // open.num_rows( _states.size() );
      //    // close.num_rows( _states.size() );
      //    // {
      //    //    vector<index>& displs = open.mod_displs();
      //    //    for( const auto& state : _states )
      //    //       displs[state->id] = state->open.size();
      //    // }
      //    // {
      //    //    vector<index>& displs = close.mod_displs();
      //    //    for( const auto& state : _states )
      //    //       displs[state->id] = state->close.size();
      //    // }
      //    // open.setup_array( true );
      //    // close.setup_array( true );
      //    // for( const auto& state : _states )
      //    // {
      //    //    std::copy( state->open.cbegin(), state->open.cend(), open[state->id].begin() );
      //    //    std::copy( state->close.cbegin(), state->close.cend(), close[state->id].begin() );
      //    // }
      // }

      std::ostream&
      operator<<( std::ostream& strm,
                  const syntax_tree& obj )
      {
         if( obj._root )
            strm << *obj._root;
         return strm;
      }

      std::ostream&
      operator<<( std::ostream& strm,
                  const syntax_tree::node& obj )
      {
         strm << (int)obj.data;
         for( unsigned ii = 0; ii < 2; ++ii )
         {
            strm << "\n" << indent << "|-";
            if( obj.child[ii] )
               strm << setindent( 1 ) << *obj.child[ii] << setindent( -1 );
         }
         return strm;
      }
   }
}