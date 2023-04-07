#include "backend_jiffy.hh"

#include <fcntl.h>

#include "util/exception.hh"
#include "util/file_descriptor.hh"

using namespace std;
using namespace jiffy::client;
using namespace jiffy::storage;
using namespace jiffy::directory;
using namespace jiffy::utils;

#define STORAGE_MODE 0x00 // 0x00 for DRAM_ONLY, 0x04 for MAPPED

void JiffyStorageBackend::put(const std::vector<storage::PutRequest> &requests,
                              const PutCallback &success_callback)
{
    for(size_t i = 0; i < requests.size(); i++) {
        const string & filename = requests.at(i).filename.string();
        const string & object_key = requests.at(i).object_key;

        string contents;
        FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
        while ( not file.eof() ) { contents.append( file.read() ); }
        file.close();

        if (client_.fs()->exists("/a/file")) {
            data_status dstat = client_.fs()->dstatus("/a/file");
            std::vector<storage_mode> modes = dstat.mode();
            // check if file is on disk and load it
            if (modes.at(0) == storage_mode::on_disk) {
                client_.load("/a/file", "local://tmp/a/file");
            }
            hash_cli = client_.open_hash_table("/a/file");
        } else {
            hash_cli = client_.create_hash_table("/a/file", "local://tmp/a/file", 1, 1, STORAGE_MODE);
        }
        hash_cli->put(object_key, contents);
        success_callback(requests.at(i));
    }
}

void JiffyStorageBackend::get(const std::vector<storage::GetRequest> &requests,
                              const GetCallback &success_callback)
{
    for(size_t i = 0; i < requests.size(); i++) {
        const string & filename = requests.at(i).filename.string();
        const string & object_key = requests.at(i).object_key;

        if (client_.fs()->exists("/a/file")) {
            data_status dstat = client_.fs()->dstatus("/a/file");
            std::vector<storage_mode> modes = dstat.mode();
            if (modes.at(0) == storage_mode::on_disk) {
                client_.load("/a/file", "local://tmp/a/file");
            }
        } else {
            throw runtime_error("File not found");
        } 
        hash_cli = client_.open_hash_table("/a/file");
        string contents = hash_cli->get(object_key);
        roost::atomic_create(contents, filename,
                             requests.at(i).mode.initialized(),
                             requests.at(i).mode.get_or(0) );
        success_callback(requests.at(i));
    }
}