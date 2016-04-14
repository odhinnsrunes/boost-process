// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PROCESS_WINDOWS_ON_EXIT_HPP_
#define BOOST_PROCESS_WINDOWS_ON_EXIT_HPP_

#include <boost/process/detail/config.hpp>
#include <boost/process/detail/handler_base.hpp>
#include <boost/process/detail/windows/on_exit.hpp>
#include <boost/process/detail/windows/async_handler.hpp>
#include <boost/detail/winapi/process.hpp>
#include <system_error>
#include <functional>

namespace boost { namespace process { namespace detail { namespace windows {

struct on_exit_ : boost::process::detail::windows::async_handler
{
    std::function<void(int, const std::error_code&)> handler;
    on_exit_(const std::function<void(int, const std::error_code&)> & handler) : handler(handler)
    {

    }

    struct on_exit_handler_t
    {
        std::function<void(int, const std::error_code&)> handler;
        void* process_handle;

        void operator()(const std::error_code & ec_in) const
        {
            ::boost::detail::winapi::DWORD_ exit_code = 0;
            auto ec = ec_in;
            if (!ec)
            {
                auto err_code = ::boost::detail::winapi::GetExitCodeProcess(process_handle, &exit_code);
                if (err_code)
                    ec = boost::process::detail::get_last_error();
            }
            handler(static_cast<int>(exit_code), ec);
        }

    };


    template<typename Executor>
    on_exit_handler_t on_exit_handler(Executor & exec) const
    {
        on_exit_handler_t oe;
        oe.handler = std::move(this->handler);

        ::boost::detail::winapi::PROCESS_INFORMATION_ & proc = exec.proc_info;
        oe.process_handle = proc.hProcess;

        return std::move(oe);


    };
};


}}}}
#endif /* INCLUDE_BOOST_PROCESS_WINDOWS_ON_EXIT_HPP_ */
