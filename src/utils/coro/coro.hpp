//
// Created by ilesik on 7/15/24.
//

#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <optional>

namespace gb {

    /**
     * @brief Represents an asynchronous task that can return a result of type T.
     *
     * This structure utilizes C++20 coroutines to enable asynchronous programming
     * patterns, allowing functions to be suspended and resumed without blocking.
     *
     * @tparam T The type of result returned by the task.
     */
    template<typename T>
    struct Task
    {
        struct promise_type;              ///< Forward declaration of the promise type
        using handle_type = std::coroutine_handle<promise_type>;  ///< Type alias for the coroutine handle

        handle_type coro;                 ///< Handle to the coroutine

        /**
         * @brief Constructs a Task from a coroutine handle.
         *
         * @param h The coroutine handle to wrap.
         */
        Task(handle_type h) : coro(h) {}

        Task(const Task &) = delete;       ///< Deleted copy constructor
        Task(Task &&t) noexcept : coro(t.coro) { t.coro = nullptr; }  ///< Move constructor
        ~Task() { if (coro) coro.destroy(); } ///< Destructor that cleans up the coroutine

        Task& operator=(const Task &) = delete;  ///< Deleted copy assignment operator
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
            handle_type coro;               ///< Handle to the coroutine

            bool await_ready() { return coro.done(); } ///< Checks if the coroutine is done
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); }  ///< Resumes the coroutine
            T await_resume() { return coro.promise().result.value(); } ///< Returns the result of the coroutine
        };

        Awaitable operator co_await() { return Awaitable{coro}; } ///< Provides co_await support

        struct promise_type
        {
            std::optional<T> result;         ///< Optional result of the task

            Task get_return_object() { return Task{handle_type::from_promise(*this)}; }  ///< Creates the Task from promise
            std::suspend_never initial_suspend() { return {}; } ///< Indicates no suspension at the start
            std::suspend_always final_suspend() noexcept { return {}; } ///< Indicates suspension at the end
            void return_value(T v) { result = v; } ///< Sets the result value
            void unhandled_exception() { std::terminate(); } ///< Terminates on exception
        };
    };

    // Specialization for void return type
    template<>
    struct Task<void>
    {
        struct promise_type;
        using handle_type = std::coroutine_handle<promise_type>; ///< Type alias for the coroutine handle

        handle_type coro;                  ///< Handle to the coroutine

        /**
         * @brief Constructs a Task from a coroutine handle.
         *
         * @param h The coroutine handle to wrap.
         */
        Task(handle_type h) : coro(h) {}

        Task(const Task &) = delete;        ///< Deleted copy constructor
        Task(Task &&t) noexcept : coro(t.coro) { t.coro = nullptr; } ///< Move constructor
        ~Task() { if (coro) coro.destroy(); } ///< Destructor that cleans up the coroutine

        Task& operator=(const Task &) = delete; ///< Deleted copy assignment operator
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
            handle_type coro;               ///< Handle to the coroutine

            bool await_ready() { return coro.done(); } ///< Checks if the coroutine is done
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); } ///< Resumes the coroutine
            void await_resume() { coro.promise().result; } ///< Returns nothing for void tasks
        };

        Awaitable operator co_await() { return Awaitable{coro}; } ///< Provides co_await support

        struct promise_type
        {
            std::optional<int> result;       ///< Optional integer to track completion

            Task get_return_object() { return Task{handle_type::from_promise(*this)}; } ///< Creates the Task from promise
            std::suspend_never initial_suspend() { return {}; } ///< Indicates no suspension at the start
            std::suspend_always final_suspend() noexcept { return {}; } ///< Indicates suspension at the end
            void return_void() { result = 0; } ///< Sets result to 0 to indicate completion
            void unhandled_exception() { std::terminate(); } ///< Terminates on exception
        };
    };

} // namespace gb
