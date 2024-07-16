//
// Created by ilesik on 7/15/24.
//

#pragma once
#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <optional>

namespace gb{
    template<typename T>
    struct Task
    {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        handle_type coro;

        Task(handle_type h) : coro(h) {}
        Task(const Task &) = delete;
        Task(Task &&t) noexcept : coro(t.coro) { t.coro = nullptr; }
        ~Task() { if (coro) coro.destroy(); }

        Task& operator=(const Task &) = delete;
        Task& operator=(Task &&t) noexcept
        {
            if (this != &t)
            {
                if (coro) coro.destroy();
                coro = t.coro;
                t.coro = nullptr;
            }
            return *this;
        }

        struct Awaitable
        {
            handle_type coro;
            bool await_ready() { return coro.done(); }
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); }
            T await_resume() { return coro.promise().result.value(); }
        };

        Awaitable operator co_await() { return Awaitable{coro}; }

        struct promise_type
        {
            std::optional<T> result;

            Task get_return_object() { return Task{handle_type::from_promise(*this)}; }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_value(T v) { result = v; }
            void unhandled_exception() { std::terminate(); }
        };
    };

// Specialization for void return type
    template<>
    struct Task<void>
    {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>;

        handle_type coro;

        Task(handle_type h) : coro(h) {}
        Task(const Task &) = delete;
        Task(Task &&t) noexcept : coro(t.coro) { t.coro = nullptr; }
        ~Task() { if (coro) coro.destroy(); }

        Task& operator=(const Task &) = delete;
        Task& operator=(Task &&t) noexcept
        {
            if (this != &t)
            {
                if (coro) coro.destroy();
                coro = t.coro;
                t.coro = nullptr;
            }
            return *this;
        }

        struct Awaitable
        {
            handle_type coro;
            bool await_ready() { return coro.done(); }
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); }
            void await_resume() { coro.promise().result; }
        };

        Awaitable operator co_await() { return Awaitable{coro}; }

        struct promise_type
        {
            std::optional<int> result; // Use int to track completion

            Task get_return_object() { return Task{handle_type::from_promise(*this)}; }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() { result = 0; } // Use 0 to indicate completion
            void unhandled_exception() { std::terminate(); }
        };
    };


    /*struct Awaitable
    {
        bool await_ready() { return false; }
        void await_suspend(std::coroutine_handle<> h){};
        void await_resume() {}
    };*/  //example reference
}
