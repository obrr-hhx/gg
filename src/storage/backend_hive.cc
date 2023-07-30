#include "backend_hive.hh"

#include <fcntl.h>

#include "util/exception.hh"
#include "util/file_descriptor.hh"

void HiveStorageBackend::put(const std::vector<storage::PutRequest> &requests,
                             const PutCallback &success_callback)
{
    client_.upload_files(requests, success_callback);
}

void HiveStorageBackend::get(const std::vector<storage::GetRequest> &requests,
                             const GetCallback &success_callback)
{
    client_.download_files(requests, success_callback);
}

void HiveStorageBackend::hive_put(const std::string &workflow, const std::string &thunk_hash, const std::vector<storage::PutRequest> &requests,
                             const PutCallback &success_callback)
{
    client_.upload_files_hash(workflow, thunk_hash, requests, success_callback);
}

void HiveStorageBackend::hive_get(const std::string &workflow, const std::string &thunk_hash, const std::vector<storage::GetRequest> &requests,
                            const GetCallback &success_callback)
{
    client_.download_files_hash(workflow, thunk_hash, requests, success_callback);
}