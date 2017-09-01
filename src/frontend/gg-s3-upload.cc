/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <cstdlib>
#include <string>
#include <iostream>
#include <cassert>
#include <ctime>
#include <map>
#include <sys/types.h>
#include <sys/fcntl.h>

#include "socket.hh"
#include "secure_socket.hh"
#include "http_request.hh"
#include "awsv4_sig.hh"
#include "http_response_parser.hh"
#include "exception.hh"

using namespace std;

const string region = "us-west-1";
const string endpoint = "s3-" + region + ".amazonaws.com";
const string message = "Hello, world.\n";

TCPSocket tcp_connection( const string & hostname, const string & service )
{
    TCPSocket sock;
    cerr << "Connecting to " << hostname << "... ";
    sock.connect( { hostname, service } );
    cerr << "done.\n";
    return sock;
}

class S3PutRequest
{
private:
    static std::string x_amz_date_( const std::time_t & t )
    {
        char sbuf[ 17 ];
        strftime( sbuf, 17, "%Y%m%dT%H%M%SZ", gmtime( &t ) );
        return string( sbuf, 16 );
    }

    std::string request_date_;
    std::string akid_, secret_, bucket_, object_, contents_;
    std::string first_line_;
    std::map<std::string, std::string> headers_ {};
    void add_authorization();

public:
    HTTPRequest to_http_request() const
    {
        HTTPRequest req;
        req.set_first_line( first_line_ );
        for ( const auto & header : headers_ ) {
            req.add_header( HTTPHeader { header.first, header.second } );
        }
        req.done_with_headers();

        req.read_in_body( contents_ );
        assert( req.state() == COMPLETE );

        return req;
    }

    S3PutRequest( const std::string & akid,
                  const std::string & secret,
                  const std::string & bucket,
                  const std::string & object,
                  const std::string & contents )
        : request_date_( x_amz_date_( time( 0 ) ) ),
          akid_( akid ), secret_( secret ),
          bucket_( bucket ), object_( object ),
          contents_( contents ),
          first_line_( "PUT /" + object + " HTTP/1.1" )
    {
        headers_[ "x-amz-date" ] = request_date_;
        headers_[ "host" ] = bucket + ".s3.amazonaws.com";
        headers_[ "content-length" ] = to_string( contents.length() );
        AWSv4Sig::sign_request( "PUT\n/" + object,
                                secret_, akid_, region, "s3", request_date_, contents,
                                headers_ );
    }
};

int main()
{
    const char * akid_cstr = getenv( "AWS_ACCESS_KEY_ID" );
    const char * secret_cstr = getenv( "AWS_SECRET_ACCESS_KEY" );

    if ( akid_cstr == nullptr or secret_cstr == nullptr ) {
        cerr << "Missing required environment variable: AWS_ACCESS_KEY_ID and AWS_SECRET_ACCESS_KEY\n";
        return EXIT_FAILURE;
    }

    SSLContext ssl_context;
    SecureSocket s3 = ssl_context.new_secure_socket( tcp_connection( endpoint, "https" ) );

    cerr << "Starting TLS session... ";
    s3.connect();
    cerr << "done.\n";

    HTTPResponseParser responses;

    FileDescriptor devurandom { CheckSystemCall( "open", open( "/dev/urandom", O_RDONLY ) ) };
    const string bigrandom = devurandom.read_exactly( 1048576 );    
    
    for ( unsigned int i = 0; i < 100; i++ ) {
        cerr << "Sending request " << i << "... ";
        S3PutRequest request( akid_cstr, secret_cstr, "ggfunbucket", "payload" + to_string( i ), bigrandom );
        HTTPRequest outgoing_request = request.to_http_request();
        responses.new_request_arrived( outgoing_request );
        s3.write( outgoing_request.str() );
        cerr << "done.\n";
    }

    while ( not s3.eof() ) {
        responses.parse( s3.read() );
        while ( not responses.empty() ) {
            cerr << "Response received: " << responses.front().first_line() << " (pending=" << responses.pending_requests() << ")\n";
            responses.pop();
        }

        if ( responses.pending_requests() == 0 ) {
            break;
        }
    }

    return EXIT_SUCCESS;
}