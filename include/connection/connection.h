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

#pragma once

#include "connection/iconnection.h"

#include <cstdint>
#include <atomic>
#include <vector>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace skylog_server {
namespace connection {

class Connection : public IConnection,
                   public boost::enable_shared_from_this<Connection> {
 public:
  Connection();
  ~Connection();

  Connection(Connection&&) = default;
  Connection& operator=(Connection&&) = default;

  void Read() final;

  boost::asio::ip::tcp::socket& socket() final;

 private:
  void Handle(const boost::system::error_code& error,
              const std::size_t bytes_transferred);

  void Stop();

  boost::asio::io_service service_;
  std::unique_ptr<boost::thread> thread_;

  boost::asio::ip::tcp::socket socket_;
  std::atomic_bool is_running_;

  std::vector<std::uint8_t> buffer_;
};

}  // namespace connection
}  // namespace skylog_server
