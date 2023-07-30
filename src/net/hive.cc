#include "net/hive.hh"

#include <thread>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>

#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/optional.hh"

using namespace std;
using namespace hive::client;
using namespace hive::utils;

void Hive::upload_files(const std::vector<storage::PutRequest> & upload_requests,
                        const std::function<void( const storage::PutRequest & )> & success_callback) {
    // const size_t thread_count = config_.max_threads;
    // const size_t batch_size = config_.max_batch_size;
    
    for(size_t file_id = 0; file_id < upload_requests.size(); file_id++) {
        const string & filename = upload_requests.at( file_id ).filename.string();
        const string & object_key = upload_requests.at( file_id ).object_key;
        // cout<<"object_key: "<<object_key<<endl;

        string contents;
        FileDescriptor file {CheckSystemCall("open " + filename, open(filename.c_str(), O_RDONLY))};
        while (not file.eof()) { contents.append(file.read()); }
        file.close();

        // cout<<"[hive client] start upload file: "<< filename << endl;
        // cout<<"[hive client] file size: "<< contents.size() << endl;
        try{
            client.put(object_key, contents);
            // client->put(object_key, contents);
        }
        catch (const std::exception& e) {
            std::cerr << "hive_client: put: " << e.what() << std::endl;
        }
        // cout<<"[hive client] success upload"<<endl;

        success_callback( upload_requests.at( file_id ) );
    }

    // vector<thread> threads;
    // for( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    //     if( thread_index < upload_requests.size() ) {
    //         threads.emplace_back(
    //             [&] ( const size_t index) {
    //                 for( size_t first_file_idx = index;
    //                     first_file_idx < upload_requests.size();
    //                     first_file_idx += thread_count * batch_size ) {

    //                     for( size_t file_id = first_file_idx;
    //                         file_id < min( upload_requests.size(), first_file_idx + thread_count * batch_size );
    //                         file_id += thread_count )  {
    //                             const string & filename = upload_requests.at( file_id ).filename.string();
    //                             const string & key = upload_requests.at( file_id ).object_key;

    //                             string contents;
    //                             FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
    //                             while ( not file.eof() ) { contents.append( file.read() ); }
    //                             file.close();

    //                             client.put( key, contents);
    //                             success_callback( upload_requests.at( file_id ) );
    //                     } 
    //                 }
    //             }, thread_index
    //         );
    //     }
    // }

    // for( auto & thread : threads ) { thread.join(); }

}

void Hive::download_files( const vector<storage::GetRequest> & download_requests,
                           const function<void( const storage::GetRequest & )> & success_callback) {
    // const size_t thread_count = config_.max_threads;
    // const size_t batch_size = config_.max_batch_size;
    
    for(size_t file_id = 0; file_id < download_requests.size(); file_id++) {
        const string & filename = download_requests.at( file_id ).filename.string();
        const string & object_key = download_requests.at( file_id ).object_key;

        // cout<<"[hive client] start download file: "<< filename << endl;
        try{
            string contents = client.get(object_key);
            roost::atomic_create(contents, filename,
                            download_requests.at(file_id).mode.initialized(),
                            download_requests.at(file_id).mode.get_or(0));
        }
        catch(const std::exception& e) {
            std::cerr << "hive_client: get: " << e.what() << std::endl;
        }
        // cout<<"[hive client] file size: "<< contents.size() << endl;
        // cout<<"[hive client] success download"<<endl;

        success_callback( download_requests.at( file_id ) );
    }

    // vector<thread> threads;
    // for( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    //     if( thread_index < download_requests.size() ) {
    //         threads.emplace_back(
    //             [&] (const size_t index)
    //             {
    //                 for( size_t first_file_idx = index;
    //                     first_file_idx < download_requests.size();
    //                     first_file_idx += thread_count * batch_size ) {

    //                     for( size_t file_id = first_file_idx;
    //                         file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
    //                         file_id += thread_count ) {
    //                             const string & filename = download_requests.at( file_id ).filename.string();
    //                             const string & key = download_requests.at( file_id ).object_key;

    //                             string contents;
    //                             contents = client.get( key );
                                
    //                             roost::atomic_create(contents, filename,
    //                                                 download_requests.at(file_id).mode.initialized(),
    //                                                 download_requests.at(file_id).mode.get_or(0));
                                
    //                             success_callback( download_requests.at( file_id ) );
    //                     }
    //                 }
    //             }, thread_index
    //         );
    //     }
    // }

    // for( auto & thread : threads ) { thread.join(); }
}

void Hive::upload_files_hash(const std::string &workflow, const std::string &thunk_hash, const std::vector<storage::PutRequest> & upload_requests,
                       const std::function<void( const storage::PutRequest & )> & success_callback) {
    std::string workflow_name = "mosh";
    if(workflow != "") {
        workflow_name = workflow;
    }
    for(size_t file_id = 0; file_id < upload_requests.size(); file_id++) {
            const string & filename = upload_requests.at( file_id ).filename.string();
            const string & object_key = upload_requests.at( file_id ).object_key;
            cout<<"object_key: "<<object_key<<endl;

            string contents;
            FileDescriptor file {CheckSystemCall("open " + filename, open(filename.c_str(), O_RDONLY))};
            while (not file.eof()) { contents.append(file.read()); }
            file.close();

            // cout<<"[hive client] start upload file: "<< filename << endl;
            // cout<<"[hive client] file size: "<< contents.size() << endl;
            try{
                // client.put(object_key, contents);
                client.hdag_put(workflow_name, thunk_hash, object_key, contents);
                // client->put(object_key, contents);
            }
            catch (const std::exception& e) {
                std::cerr << "hive_client: put: " << e.what() << std::endl;
            }
            // cout<<"[hive client] success upload"<<endl;

            success_callback( upload_requests.at( file_id ) );
        }
}

void Hive::download_files_hash(const std::string & workflow, const std::string & thunk_hash, const std::vector<storage::GetRequest> & download_requests,
                            const std::function<void( const storage::GetRequest & )> & success_callback) {

    std::string workflow_name = "mosh";
    if(workflow != "") {
        workflow_name = workflow;
    }
    for(size_t file_id = 0; file_id < download_requests.size(); file_id++) {
        const string & filename = download_requests.at( file_id ).filename.string();
        const string & object_key = download_requests.at( file_id ).object_key;

        // cout<<"[hive client] start download file: "<< filename << endl;
        try{
            // string contents = client.get(object_key);
            string contents = client.hdag_get(workflow_name, thunk_hash, object_key);
            roost::atomic_create(contents, filename,
                            download_requests.at(file_id).mode.initialized(),
                            download_requests.at(file_id).mode.get_or(0));
        }
        catch(const std::exception& e) {
            std::cerr << "hive_client: get: " << e.what() << std::endl;
        }
        // cout<<"[hive client] file size: "<< contents.size() << endl;
        // cout<<"[hive client] success download"<<endl;

        success_callback( download_requests.at( file_id ) );
    }
}

std::string Hive::get_host() const {
    return config_.ip;
}

uint16_t Hive::get_port() const {
    return config_.port;
}