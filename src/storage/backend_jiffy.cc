#include "backend_jiffy.hh"

#include <fcntl.h>

#include "util/exception.hh"
#include "util/file_descriptor.hh"

void JiffyStorageBackend::put(const std::vector<storage::PutRequest> &requests,
                              const PutCallback &success_callback)
{
    client_.upload_files(requests, success_callback);
}

void JiffyStorageBackend::get(const std::vector<storage::GetRequest> &requests,
                              const GetCallback &success_callback)
{
    client_.download_files(requests, success_callback);
}