/*
 * Copyright (c) 2017 Artem Nosach
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "server/server.h"

#include <memory>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "connection/connection.h"

skylog_server::server::Server::Server(
    const boost::asio::ip::tcp::endpoint& endpoint)
    : service_(), acceptor_(service_, endpoint) {}

skylog_server::server::Server::~Server() {
  boost::system::error_code error;
  acceptor_.cancel(error);
  acceptor_.close(error);

  if (error) {
    std::cerr << "Server closing error."
              << " Error message: " << error.message() << "\n";
  }
}

void skylog_server::server::Server::Listen() {
  std::shared_ptr<connection::IConnection> connection =
      std::make_shared<connection::Connection>();
  acceptor_.async_accept(
      connection->socket(),
      boost::bind(
          &Server::Accept, this, connection, boost::asio::placeholders::error));
}

boost::asio::io_service& skylog_server::server::Server::service() {
  return service_;
}

void skylog_server::server::Server::Accept(
    const std::shared_ptr<connection::IConnection>& connection,
    const boost::system::error_code& error) {
  if (error) {
    std::cerr << "Server cannot accept connection."
              << " Error message: " << error.message() << "\n";
  } else {
    connection->Read();
    Listen();
  }
}
