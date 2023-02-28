/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "util/timelog.hh"

#include <sstream>

using namespace std;
using namespace std::chrono;

TimeLog::TimeLog()
  : start_( duration_cast<milliseconds>( system_clock::now().time_since_epoch() ) ),
    prev_( start_ )
{}

void TimeLog::add_point_hash( const std::string & title, std::string hash_value, off_t size){
  if(title == "get_hash")  get_hash.emplace_back(hash_value,size);
  else upload_hash.emplace_back(hash_value,size);
}

void TimeLog::add_point_size(const std::string & title, off_t size)
{
  points_size.emplace_back(title, size);
}

void TimeLog::add_point_rw( const std::string & title, std::size_t cnt)
{
  points_rw.emplace_back(title, cnt);
}

void TimeLog::add_point( const std::string & title )
{
  auto now = duration_cast<milliseconds>( system_clock::now().time_since_epoch() );

  points_.emplace_back( title, now - prev_ );
  prev_ = now;
}

void TimeLog::add_execute(std::vector<std::string> &args)
{
  for(auto arg:args) execute_info += arg;
}

string TimeLog::str() const
{
  ostringstream oss;
  oss << start_.count() << endl;

  for ( const auto & point : points_ ) {
    oss << point.first << " " << point.second.count() << endl;
  }

  oss << "Execute Command: " << execute_info << endl;

  oss << "Read/Write Numbers" << endl;
  for( const auto & point : points_rw ) {
    oss << point.first << " " << point.second << endl;
  }

  oss << "Read/Write Sizes" << endl;
  for( const auto & point : points_size ) {
    oss << point.first << " " << point.second << endl;
  }

  oss << "Get Hash" << endl;
  for(const auto &hash : get_hash) {
    oss << "get_hash " << hash.first << " " << hash.second << endl;
  }

  oss << "Upload Hash" << endl;
  for(const auto &hash : upload_hash) {
    oss << "upload_hash " << hash.first << " "  << hash.second << endl;
  }

  return oss.str();
}
