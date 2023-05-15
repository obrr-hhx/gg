#include <jiffy/client/jiffy_client.h>
#include <jiffy/utils/time_utils.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <stdlib.h>
#include <thread>


using namespace std;
using namespace jiffy::client;
using namespace jiffy::directory;
using namespace jiffy::storage;
using namespace jiffy::utils;

string to_padded_string(size_t num, size_t padding){
	std::ostringstream ss;
	ss << std::setw(static_cast<int>(padding)) << std::setfill('0') << num;
	std::string result = ss.str();
	if (result.length() > padding) {
		result.erase(0, result.length() - padding);
	}
	return result;
}

void printProgressBar(int progress, int total, int barWidth = 100) {
    float percentage = static_cast<float>(progress) / total;
    int filledWidth = barWidth * percentage;

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth)
            std::cerr << "=";
        else if (i == filledWidth)
            std::cerr << ">";
        else
            std::cerr << " ";
    }
    std::cerr << "] " << static_cast<int>(percentage * 100) << "%\r";
    std::cerr.flush();
}

void test_hash_table(jiffy_client & client){
	shared_ptr<hash_table_client> hash_cli;
	string tmp_path = "/test";
	string back_path = "local://tmp";
	int keys_num = 240;
	int values_num = 960;
	
	auto l0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
	    string root = "/" + to_string(i);
	    printProgressBar(i, keys_num);
	    if (client.fs()->exists(root+tmp_path)) {
                data_status dstat = client.fs()->dstatus(root+tmp_path);
                vector<storage_mode> modes = dstat.mode();
                if (modes[0] == storage_mode::on_disk) {
                    client.load(root+tmp_path, back_path + root+tmp_path);
                }
                hash_cli = client.open_hash_table(root+tmp_path);
	    } else {
	        hash_cli = client.create_hash_table(root+tmp_path,back_path+root+tmp_path, 1, 1, 0x00);
	    }
	}
	auto l1 = time_utils::now_ms();
	cout<<"\nCreate hash table: "<< l1-l0 << "ms" <<endl;

	auto k0 = time_utils::now_ms();
	int k = 0;
	for(int i=0; i<keys_num; i++){
	    string root = "/" + to_string(i);
	    hash_cli = client.open_hash_table(root+tmp_path);
	    for (int j = 0; j < values_num; j++){
		printProgressBar(k++, keys_num*values_num);
		string key = to_padded_string(i*j, 22);
		string value = string(1000, 'x');
		if(hash_cli->exists(key)) hash_cli->update(key, value);
		else hash_cli->put(key, value);
	    }
	    client.close(root+tmp_path);
	}
	auto k1 = time_utils::now_ms();
	cout<<"\nWrite to DRAM: "<< k1-k0 <<"ms"<<endl;
	
	k = 0;

	auto t0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
	    string root = "/"+to_string(i);
	    hash_cli = client.open_hash_table(root+tmp_path);
	    for(int j=0; j<values_num; j++) {
		printProgressBar(k++, keys_num*values_num);
	        hash_cli->get(to_padded_string(i*j, 22));
	    }
	    client.close(root+tmp_path);
	}
	auto t1 = time_utils::now_ms();
	cout<<"\nRead from DRAM: "<< t1-t0 <<"ms"<<endl;

	k0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
	    printProgressBar(i, keys_num);
	    string root = "/" + to_string(i);
	    client.dump(root+tmp_path, back_path+root+tmp_path);
	}
	k1 = time_utils::now_ms();
	cout<<"\nDump the file to persistent storage and clear the block: "<< k1-k0 <<"ms"<<endl;

	k=0;
	auto a0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
	    string root = "/" + to_string(i);
	    client.load(root+tmp_path, back_path + root+tmp_path);
	    hash_cli = client.open_hash_table(root+tmp_path);
	    for(int j=0; j<values_num; j++){
		printProgressBar(k++, keys_num*values_num);
	        hash_cli->get(to_padded_string(i*j, 22));
	    }
	    client.close(root+tmp_path);
	}
	auto a1 = time_utils::now_ms();
	cout << "\nRead from Persistent storage: "<<a1-a0<<"ms"<<endl;
}

void test_file(jiffy_client & client, int file_num, int file_size){
	shared_ptr<file_client> file_cli;
	string tmp_path = "/test";
	string back_path = "local://tmp";
	int keys_num = 240;
	if(file_num > 0) keys_num = file_num;
	
	// int MEGABIBYTE = 1024 * 1024;
	int STRING_SIZE = 1 * 1024 * 1024;

	if(file_size > 0) STRING_SIZE = file_size;

	// each file just has one value
	auto l0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
	    string root = "/" + to_padded_string(i, 22);
	    printProgressBar(i, keys_num);
	    if (client.fs()->exists(root+tmp_path)) {
				data_status dstat = client.fs()->dstatus(root+tmp_path);
				vector<storage_mode> modes = dstat.mode();
				if (modes[0] == storage_mode::on_disk) {
					client.load(root+tmp_path, back_path + root+tmp_path);
				}
				file_cli = client.open_file(root+tmp_path);
	    } else {
	        file_cli = client.create_file(root+tmp_path,back_path+root+tmp_path, 1, 1, 0x00);
	    }
	}
	auto l1 = time_utils::now_ms();
	cout<<"\nCreate file: "<< l1-l0 << "ms" <<endl;

	auto k0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
		string root = "/" + to_padded_string(i, 22);
	    file_cli = client.open_file(root+tmp_path);
	    printProgressBar(i, keys_num);
	    string value = string(STRING_SIZE, 'x');
	    file_cli->write(value);
	    client.close(root+tmp_path);
	}
	auto k1 = time_utils::now_ms();
	cout<<"\nWrite to DRAM: "<< k1-k0 <<"ms"<<endl;

	auto t0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
		string root = "/" + to_padded_string(i, 22);
	    file_cli = client.open_file(root+tmp_path);
	    printProgressBar(i, keys_num);
		string buf;
		while((file_cli->read(buf, 128)) != -1);
	    client.close(root+tmp_path);
	}
	auto t1 = time_utils::now_ms();
	cout<<"\nRead from DRAM: "<< t1-t0 <<"ms"<<endl;

	k0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
	    printProgressBar(i, keys_num);
	    string root = "/" + to_padded_string(i, 22);
	    client.dump(root+tmp_path, back_path+root+tmp_path);
	}
	k1 = time_utils::now_ms();
	cout<<"\nDump the file to persistent storage and clear the block: "<< k1-k0 <<"ms"<<endl;

	auto a0 = time_utils::now_ms();
	for(int i=0; i<keys_num; i++){
	    string root = "/" + to_padded_string(i, 22);
	    client.load(root+tmp_path, back_path + root+tmp_path);
	    file_cli = client.open_file(root+tmp_path);
	    printProgressBar(i, keys_num);
		string buf;
		while((file_cli->read(buf, 128)) != -1);
	    client.close(root+tmp_path);
	}
	auto a1 = time_utils::now_ms();
	cout << "\nRead from Persistent storage: "<<a1-a0<<"ms"<<endl;
}

int main(int args, char* argv[]) {
	jiffy_client client("172.31.4.73", 9090, 9091);

	if(args < 2){
	    cout<<"Usage: ./test_loadAndsync [hash_table|file] [num] [size]"<<endl;
	    return 0;
	}
	if(!strcmp(argv[1],"hash_table")){
		cout<<"Test hash table"<<endl;
		test_hash_table(client);
	}else if(!strcmp(argv[1],"file")){
		cout<<"Test file"<<endl;
		int file_num = atoi(argv[2]);
		cout<<"file_num: "<<file_num<<endl;
		cout<<"file_size: "<<argv[3]<<endl;
		string tmp_size(argv[3]);
		//get the last char to find the unit
		char unit = tmp_size[tmp_size.size()-1];
		//get the number
		int size = atoi(tmp_size.substr(0, tmp_size.size()-1).c_str());
		if(unit == 'K' || unit == 'k') size *= 1024;
		else if(unit == 'M' || unit == 'm') size *= 1024*1024;
		else if(unit == 'G' || unit == 'g') size *= 1024*1024*1024;
		test_file(client, file_num, size);
	}

	
}
