#ifndef libhpc_interactive_command_hh
#define libhpc_interactive_command_hh

#include <boost/regex.hpp>

namespace hpc {
   namespace command {

      class command
      {
      public:

         typedef std::function<void(const boost::smatch&)> function_type;

      public:

         command();

         command( std::string const& expr,
                  function_type action );

         std::string const&
         expression() const;

         boost::regex const&
         re() const;

         void
         operator()( const boost::smatch& match ) const;

      protected:

         std::string _re_str;
         boost::regex _re;
         function_type _act;
      };

   }
}

#endif
