#ifndef STORAGE_BACKEND_JIFFY_HH
#define STORAGE_BACKEND_JIFFY_HH

#include "storage/backend.hh"
#include "net/jiffy.hh"

class JiffyStorageBackend : public StorageBackend
{
private:
    Jiffy client_;
public:
    JiffyStorageBackend(JiffyClientConfig &config)
        : client_(config)
    {}

    void put(const std::vector<storage::PutRequest> &requests,
             const PutCallback &success_callback = [](const storage::PutRequest &) {}) override;

    void get(const std::vector<storage::GetRequest> &requests,
             const GetCallback &success_callback = [](const storage::GetRequest &) {}) override;
};

#endif /* STORAGE_BACKEND_JIFFY_HH */
