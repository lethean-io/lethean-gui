#include "ThreadVerifyHaproxy.h"

using boost::asio::ip::tcp;

// helper function to store head and body response from boost
std::string ThreadVerifyHaproxy::buffer_to_string(const boost::asio::streambuf &buffer)
{
  using boost::asio::buffers_begin;
  auto bufs = buffer.data();
  std::string result(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());
  return result;
}

// make a request to the proxy and return a response object with code headers and body
// based on original sync_client documentation sample from Boost
// https://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/http/client/sync_client.cpp
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
HttpResponse ThreadVerifyHaproxy::proxyRequest(std::string proxyHost, std::string proxyPort, std::string requestURL, std::string provider) {
    HttpResponse output = HttpResponse(0);

    // regular proxies use \r\n as boundaries but not this one, we need \n validation only
    std::string regularBoundary = "\r\n";
    std::string haproxyBoundary = "\n";

    try {

        boost::asio::io_service io_service;

        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_service);

        // set the host and port of the proxy to query
        tcp::resolver::query query(proxyHost, proxyPort, boost::asio::ip::resolver_query_base::numeric_service);

        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Try each endpoint until we successfully establish a connection.
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // create the request wihh all headers including mgmtid
        boost::asio::streambuf request;
        std::ostream request_stream(&request);

        std::string httpHost = proxyHost;
        if (requestURL.length() >= 9 && requestURL.substr(0, 7) == std::string("http://")) {
            httpHost = requestURL.substr(7);
            if (httpHost.find('/') != std::string::npos)
                httpHost = httpHost.substr(0, httpHost.find('/'));
        }
        request_stream << "GET " << requestURL << " HTTP/1.0" << regularBoundary;
        request_stream << "Host: " << httpHost << regularBoundary;
        request_stream << "Accept: */*" << regularBoundary;
        request_stream << "Connection: close" << regularBoundary;
        request_stream << "X-ITNS-MgmtId: " << provider << regularBoundary << regularBoundary;

        // Send the request.
        boost::asio::write(socket, request);

        // Read the response status line
        boost::asio::streambuf response;

        // regular boundaries do not work here
        boost::asio::read_until(socket, response, haproxyBoundary);

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        unsigned int status_code;
        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);


        // Setting status code
        output.setStatusCode(status_code);

        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            //Invalid response
            return output;
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;

        // append header to the response headers variable
        while (std::getline(response_stream, header) && header != haproxyBoundary) {
            output.addHeader(header);
        }

        // variable to keep contents of response body
        std::string body;

        // start storing response body
        if (response.size() > 0) {
            body += buffer_to_string(response);
        }


        // Read until EOF, save data for sending to caller
        boost::system::error_code error;
        while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
            body += buffer_to_string(response);
        }

        if (error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }
        output.setBody(body);

    }
    catch (std::exception& e)
    {
        // "Exception: " << e.what() << "\n";
    }
    return output;
}

QString ThreadVerifyHaproxy::startVerifyHaproxy() {
    // TODO - this needs to be updated when new dispatcher is available
    std::string endpoint = std::string("http://_remote_/status");

    HttpResponse response = proxyRequest( m_haproxyHost, m_haproxyPort, endpoint, m_haproxyProvider );

    // return response based on the header
    if ( response.getStatusCode() == 200 ) {
        // return OK with the proxy works
        return "OK";
    }
    else if( response.getStatusCode() == 503 ||
        response.getHeaders()["X-ITNS-Status"] == "CONNECTION_ERROR" ) {
        // return Connection error to stop proxy
        return "CONNECTION_ERROR";
    } else if (response.getStatusCode() == 403) {
        return "NO_PAYMENT";
    } else if ( !response.getBody().empty() ) {
        return QString::fromStdString(response.getBody());
    }

    return "READY";
}

// set the returnable variables
void ThreadVerifyHaproxy::setup(std::string host, std::string port, std::string provider) {
    m_haproxyHost = host;
    m_haproxyPort = port;
    m_haproxyProvider = provider;
}
