#include "net/jiffy.hh"

#include <thread>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>
#include <jiffy/client/jiffy_client.h>

#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/optional.hh"

using namespace std;
using namespace jiffy::client;
using namespace jiffy::storage;
using namespace jiffy::directory;
using namespace jiffy::utils;

#define STORAGE_MODE 0x00 // 0x00 for DRAM_ONLY, 0x04 for MAPPED

void Jiffy::upload_files( const std::vector<storage::PutRequest> & upload_requests,
                       const std::function<void( const storage::PutRequest & )> & success_callback)
{
    const size_t thread_count = config_.max_threads;
    const size_t batch_size = config_.max_batch_size;

    vector<thread> threads;
    for( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
        if( thread_index < upload_requests.size() ) {
            threads.emplace_back(
                [&] ( const size_t index )
                {
                    jiffy_client client(config_.ip, config_.dir_port, config_.lease_port);
                    std::shared_ptr<hash_table_client> hash_cli;
                    for( size_t first_file_idx = index;
                         first_file_idx < upload_requests.size();
                         first_file_idx += thread_count * batch_size ) {

                        for( size_t file_id = first_file_idx;
                            file_id < min( upload_requests.size(), first_file_idx + thread_count * batch_size );
                            file_id += thread_count ) {
                                const string & filename = upload_requests.at( file_id ).filename.string();
                                const string & object_key = upload_requests.at( file_id ).object_key;

                                string contents;
                                FileDescriptor file {CheckSystemCall("open " + filename, open(filename.c_str(), O_RDONLY))};
                                while (not file.eof()) { contents.append(file.read()); }
                                file.close();

                                if (client.fs()->exists("/a/file")) {
                                    data_status dstat = client.fs()->dstatus("/a/file");
                                    std::vector<storage_mode> modes = dstat.mode();
                                    // check if file is on disk and load it
                                    if (modes.at(0) == storage_mode::on_disk) {
                                        client.load("/a/file", "local://tmp/a/file");
                                    }
                                    hash_cli = client.open_hash_table("/a/file");
                                } else {
                                    hash_cli = client.create_hash_table("/a/file", "local://tmp/a/file", 1, 1, STORAGE_MODE);
                                }
                                hash_cli->put(object_key, contents);
                                success_callback(upload_requests.at(file_id));
                            }
                        }
                }, thread_index
            );
        }
    }
    for ( auto & thread : threads ) { thread.join(); }
    
}

void Jiffy::download_files( const vector<storage::GetRequest> & download_requests,
                            const function<void( const storage::GetRequest & )> & success_callback)
{
    const size_t thread_count = config_.max_threads;
    const size_t batch_size = config_.max_batch_size;

    vector<thread> threads;
    for( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
        if( thread_index < download_requests.size() ) {
            threads.emplace_back(
                [&] (const size_t index)
                {
                    jiffy_client client(config_.ip, config_.dir_port, config_.lease_port);
                    std::shared_ptr<hash_table_client> hash_cli;
                    for( size_t first_file_idx = index;
                         first_file_idx < download_requests.size();
                         first_file_idx += thread_count * batch_size ) {

                        for( size_t file_id = first_file_idx;
                            file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
                            file_id += thread_count ) {
                                const string & filename = download_requests.at( file_id ).filename.string();
                                const string & object_key = download_requests.at( file_id ).object_key;

                                if (client.fs()->exists("/a/file")) {
                                    data_status dstat = client.fs()->dstatus("/a/file");
                                    std::vector<storage_mode> modes = dstat.mode();
                                    // check if file is on disk and load it
                                    if (modes.at(0) == storage_mode::on_disk) {
                                        client.load("/a/file", "local://tmp/a/file");
                                    }
                                    hash_cli = client.open_hash_table("/a/file");
                                } else {
                                    hash_cli = client.create_hash_table("/a/file", "local://tmp/a/file", 1, 1, STORAGE_MODE);
                                }
                                string contents = hash_cli->get(object_key);
                                roost::atomic_create(contents,filename,
                                                    download_requests.at(file_id).mode.initialized(),
                                                    download_requests.at(file_id).mode.get_or(0));
                                success_callback(download_requests.at(file_id));
                            }
                        }
                }, thread_index
            );
        }
    }
    for( auto & thread : threads ) { thread.join(); }
}