#ifndef NET_HIVE_HH
#define NET_HIVE_HH

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <stdexcept>

#include "client/hive_client.h"
#include "net/requests.hh"

using namespace hive::client;
using namespace hive::utils;

struct HiveClientConfig
{
    std::string ip { "0.0.0.0" };
    uint16_t port { 9991 };

    size_t max_threads { 32 };
    size_t max_batch_size { 32 };
};

class Hive
{
private:
    HiveClientConfig config_;
    hive_client client;
public:
    Hive( const HiveClientConfig & config )
        : config_( config ), client(config_.ip, config_.port) {}

    void upload_files( const std::vector<storage::PutRequest> & upload_requests,
                       const std::function<void( const storage::PutRequest & )> & success_callback
                         = []( const storage::PutRequest & ){} );
    void upload_files_hash( const std::string & workflow, const std::string & thunk_hash, const std::vector<storage::PutRequest> & upload_requests,
                       const std::function<void( const storage::PutRequest & )> & success_callback
                         = []( const storage::PutRequest & ){} );

    void download_files( const std::vector<storage::GetRequest> & download_requests,
                         const std::function<void( const storage::GetRequest & )> & success_callback
                           = []( const storage::GetRequest & ){} );
    void download_files_hash( const std::string & workflow, const std::string & thunk_hash, const std::vector<storage::GetRequest> & download_requests,
                            const std::function<void( const storage::GetRequest & )> & success_callback
                            = []( const storage::GetRequest & ){} );
    std::string get_host() const;
    uint16_t get_port() const;
};

#endif /* NET_HIVE_HH */