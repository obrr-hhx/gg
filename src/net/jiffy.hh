#ifndef NET_JIFFY_HH
#define NET_JIFFY_HH

#include <vector>
#include <string>
#include <functional>
#include <jiffy/client/jiffy_client.h>
#include "net/requests.hh"

struct JiffyClientConfig
{
    std::string ip { "0.0.0.0" };
    uint16_t dir_port { 9090 };
    uint16_t lease_port { 9091 };

    size_t max_threads { 32 };
    size_t max_batch_size { 32 };
};

class Jiffy
{
private:
    JiffyClientConfig config_;
    jiffy_client client;
public:
    Jiffy( const JiffyClientConfig & config )
        : config_( config ), client( config.ip, config.dir_port, config.lease_port )
    {}

    void upload_files( const std::vector<storage::PutRequest> & upload_requests,
                       const std::function<void( const storage::PutRequest & )> & success_callback
                         = []( const storage::PutRequest & ){} );

    void download_files( const std::vector<storage::GetRequest> & download_requests,
                         const std::function<void( const storage::GetRequest & )> & success_callback
                           = []( const storage::GetRequest & ){} );
};

#endif /* NET_JIFFY_HH */