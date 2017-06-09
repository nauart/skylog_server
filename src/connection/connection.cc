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

#include "connection/connection.h"

#include <cstdint>
#include <atomic>
#include <vector>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace {
const std::size_t kChunkSize = 1024u;
}

skylog_server::connection::Connection::Connection()
    : service_()
    , thread_(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &service_)))
    , socket_(service_)
    , is_running_(true)
    , buffer_() {}

skylog_server::connection::Connection::~Connection() {
  Stop();

  boost::system::error_code error;
  socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_receive, error);
  socket_.close(error);

  if (error) {
    std::cerr << "Connection closing error."
              << " Error message: " << error.message() << "\n";
  }
}

void skylog_server::connection::Connection::Read() {
  if (is_running_) {
    const std::size_t old_size = buffer_.size();
    const std::size_t new_size = old_size + kChunkSize;

    buffer_.resize(new_size);
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(&buffer_[old_size], kChunkSize),
        boost::bind(&Connection::Handle,
                    shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }
}

boost::asio::ip::tcp::socket& skylog_server::connection::Connection::socket() {
  return socket_;
}

void skylog_server::connection::Connection::Handle(
    const boost::system::error_code& error,
    const std::size_t bytes_transferred) {
  (void)bytes_transferred;
  // write received data to database

  if (!error) {
    Read();
  }
}

void skylog_server::connection::Connection::Stop() {
  bool expected_value = true;
  if (is_running_.compare_exchange_strong(expected_value, false)) {
    if (thread_) {
      thread_->join();
    }
  }
}
