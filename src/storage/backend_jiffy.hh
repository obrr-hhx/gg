#ifndef STORAGE_BACKEND_JIFFY_HH
#define STORAGE_BACKEND_JIFFY_HH

#include "backend.hh"
#include "net/aws.hh"
#include "jiffy/client/jiffy_client.h"

using namespace jiffy::client;
using namespace jiffy::storage;
using namespace jiffy::directory;
using namespace jiffy::utils;

class JiffyStorageBackend : public StorageBackend
{
private:
    jiffy_client client_;
    std::shared_ptr<hash_table_client> hash_cli = nullptr;
public:
    JiffyStorageBackend(const std::string &host, const int dir_port, const int lease_port)
        : client_(host, dir_port, lease_port)
    {}

    void put(const std::vector<storage::PutRequest> &requests,
             const PutCallback &success_callback = [](const storage::PutRequest &) {}) override;

    void get(const std::vector<storage::GetRequest> &requests,
             const GetCallback &success_callback = [](const storage::GetRequest &) {}) override;
};

#endif /* STORAGE_BACKEND_JIFFY_HH */
