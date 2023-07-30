#ifndef STORAGE_BACKEND_HIVE_HH
#define STORAGE_BACKEND_HIVE_HH

#include "storage/backend.hh"
#include "net/hive.hh"

class HiveStorageBackend : public StorageBackend
{
private:
    Hive client_;
public:
    HiveStorageBackend(HiveClientConfig &config)
        : client_(config)
    {}

    void put(const std::vector<storage::PutRequest> &requests,
             const PutCallback &success_callback = [](const storage::PutRequest &) {}) override;

    void get(const std::vector<storage::GetRequest> &requests,
             const GetCallback &success_callback = [](const storage::GetRequest &) {}) override;
    
    void hive_put(const std::string &workflow, const std::string &thunk_hash, const std::vector<storage::PutRequest> &requests,
             const PutCallback &success_callback = [](const storage::PutRequest &) {});
    
    void hive_get(const std::string &workflow, const std::string &thunk_hash, const std::vector<storage::GetRequest> &requests,
                const GetCallback &success_callback = [](const storage::GetRequest &) {});
    // get client host and port
    std::string get_host() const {
        return client_.get_host();
    }
    uint16_t get_port() const {
        return client_.get_port();
    }
};

#endif /* STORAGE_BACKEND_HIVE_HH */