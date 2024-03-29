#pragma once

#include <cstddef>
#include <stdexcept>
#include <cstring>

#include <chrono>
#include <type_traits>

/// Type name output
#ifndef _MSC_VER
#  if __cplusplus < 201103
#    define CONSTEXPR11_TN
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN
#  elif __cplusplus < 201402
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN noexcept
#  else
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN constexpr
#    define NOEXCEPT_TN noexcept
#  endif
#else  // _MSC_VER
#  if _MSC_VER < 1900
#    define CONSTEXPR11_TN
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN
#  elif _MSC_VER < 2000
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN noexcept
#  else
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN constexpr
#    define NOEXCEPT_TN noexcept
#  endif
#endif  // _MSC_VER

class static_string
{
   const char* const p_;
   const std::size_t sz_;

public:
   typedef const char* const_iterator;

   template <std::size_t N>
   CONSTEXPR11_TN static_string( const char( &a )[ N ] ) NOEXCEPT_TN
      : p_( a )
      , sz_( N - 1 )
   {}

   CONSTEXPR11_TN static_string( const char* p, std::size_t N ) NOEXCEPT_TN
      : p_( p )
      , sz_( N )
   {}

   CONSTEXPR11_TN const char* data() const NOEXCEPT_TN { return p_; }
   CONSTEXPR11_TN std::size_t size() const NOEXCEPT_TN { return sz_; }

   CONSTEXPR11_TN const_iterator begin() const NOEXCEPT_TN { return p_; }
   CONSTEXPR11_TN const_iterator end()   const NOEXCEPT_TN { return p_ + sz_; }

   CONSTEXPR11_TN char operator[]( std::size_t n ) const
   {
      return n < sz_ ? p_[ n ] : throw std::out_of_range( "static_string" );
   }
};

inline
std::ostream&
operator<<( std::ostream& os, static_string const& s )
{
   return os.write( s.data(), s.size() );
}

template <class T>
CONSTEXPR14_TN
static_string
type_name()
{
#ifdef __clang__
   static_string p = __PRETTY_FUNCTION__;
   return static_string( p.data() + 31, p.size() - 31 - 1 );
#elif defined(__GNUC__)
   static_string p = __PRETTY_FUNCTION__;
#  if __cplusplus < 201402
   return static_string( p.data() + 36, p.size() - 36 - 1 );
#  else
   return static_string( p.data() + 46, p.size() - 46 - 1 );
#  endif
#elif defined(_MSC_VER)
   static_string p = __FUNCSIG__;
   return static_string( p.data() + 38, p.size() - 38 - 7 );
#endif
}


/// Execution time check
// non void job result
template< typename JobType, class Duration = std::chrono::nanoseconds, std::enable_if_t< !std::is_same_v< std::invoke_result_t< JobType >, void > >* = nullptr >
auto ExecutionDurationCheck( JobType&& job )
{
   auto t1 = std::chrono::high_resolution_clock::now();
   auto jobResult = job();
   auto t2 = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast< std::chrono::nanoseconds >( t2 - t1 ).count();

   return std::make_pair( jobResult, duration );
}

// void job result
template< typename JobType, class Duration = std::chrono::nanoseconds, std::enable_if_t< std::is_same_v< std::invoke_result_t< JobType >, void >, int >* = nullptr >
auto ExecutionDurationCheck( JobType&& job )
{
   auto t1 = std::chrono::high_resolution_clock::now();
   job();
   auto t2 = std::chrono::high_resolution_clock::now();

   return std::chrono::duration_cast< std::chrono::nanoseconds >( t2 - t1 ).count();
}

template< class JobType, class ... Args, class Duration = std::chrono::nanoseconds >
auto ExecutionDurationCheck( JobType job, Args&& ... args )
{
   return ExecutionDurationCheck( [&]() { return job( std::forward< Args... >( args... ) ); } );
}



/// Common debug output
#define COUT_FUNCSIG_ENDL std::cout << __FUNCSIG__ << std::endl

#define COUT_FUNCSIG_2SPOT_SPACE std::cout << __FUNCSIG__ << ": "
