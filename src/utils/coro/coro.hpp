#pragma once

#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <functional>

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
        struct promise_type;  ///< Forward declaration of the promise type
        using handle_type = std::coroutine_handle<promise_type>;  ///< Type alias for the coroutine handle

        handle_type coro;  ///< Handle to the coroutine

        /**
         * @brief Constructs a Task from a coroutine handle.
         *
         * @param h The coroutine handle to wrap.
         */
        Task(handle_type h) : coro(h) {}

        Task(const Task &) = delete;  ///< Deleted copy constructor
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

        /**
         * @brief Awaitable structure to allow awaiting on the task using co_await.
         */
        struct Awaitable
        {
            handle_type coro;  ///< Handle to the coroutine

            bool await_ready() { return coro.done(); }  ///< Checks if the coroutine is already done
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); }  ///< Resumes the coroutine when suspended
            T await_resume() { return coro.promise().result.value(); }  ///< Returns the result of the coroutine
        };

        Awaitable operator co_await() { return Awaitable{coro}; }  ///< Provides co_await support for the task

        /**
         * @brief The promise_type represents the state of the coroutine and provides methods
         * for resuming and completing the coroutine.
         */
        struct promise_type
        {
            std::optional<T> result;  ///< Optional result of the task
            std::function<void()> continuation;  ///< Optional continuation to be called on completion

            Task get_return_object() { return Task{handle_type::from_promise(*this)}; }  ///< Creates the Task from the promise
            std::suspend_never initial_suspend() { return {}; }  ///< Indicates no suspension at the start of the coroutine
            std::suspend_always final_suspend() noexcept {  ///< Indicates suspension at the end of the coroutine
                if (continuation) {
                    continuation();  ///< If a continuation is set, call it
                }
                return {};
            }
            void return_value(T v) { result = v; }  ///< Sets the result value of the coroutine
            void unhandled_exception() { std::terminate(); }  ///< Terminates if an exception is unhandled
        };
    };

    /**
     * @brief Specialization of Task for void return type.
     */
    template<>
    struct Task<void>
    {
        struct promise_type;  ///< Forward declaration of the promise type
        using handle_type = std::coroutine_handle<promise_type>;  ///< Type alias for the coroutine handle

        handle_type coro;  ///< Handle to the coroutine

        /**
         * @brief Constructs a Task from a coroutine handle.
         *
         * @param h The coroutine handle to wrap.
         */
        Task(handle_type h) : coro(h) {}

        Task(const Task &) = delete;  ///< Deleted copy constructor
        Task(Task &&t) noexcept : coro(t.coro) { t.coro = nullptr; }  ///< Move constructor
        ~Task() { if (coro) coro.destroy(); }  ///< Destructor that cleans up the coroutine

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

        /**
         * @brief Awaitable structure to allow awaiting on the task using co_await.
         */
        struct Awaitable
        {
            handle_type coro;  ///< Handle to the coroutine

            bool await_ready() { return coro.done(); }  ///< Checks if the coroutine is already done
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); }  ///< Resumes the coroutine when suspended
            void await_resume() { coro.promise().result; }  ///< Nothing to return for void tasks
        };

        Awaitable operator co_await() { return Awaitable{coro}; }  ///< Provides co_await support for the task

        /**
         * @brief The promise_type represents the state of the coroutine and provides methods
         * for resuming and completing the coroutine.
         */
        struct promise_type
        {
            std::optional<int> result;  ///< Optional integer to track completion
            std::function<void()> continuation;  ///< Optional continuation to be called on completion

            Task get_return_object() { return Task{handle_type::from_promise(*this)}; }  ///< Creates the Task from the promise
            std::suspend_never initial_suspend() { return {}; }  ///< Indicates no suspension at the start of the coroutine
            std::suspend_always final_suspend() noexcept {  ///< Indicates suspension at the end of the coroutine
                if (continuation) {
                    continuation();  ///< If a continuation is set, call it
                }
                return {};
            }
            void return_void() { result = 0; }  ///< Sets result to 0 to indicate completion for void tasks
            void unhandled_exception() { std::terminate(); }  ///< Terminates if an exception is unhandled
        };
    };

    /**
     * @brief Synchronously waits for a coroutine task to complete and returns the result.
     *
     * This function blocks the calling thread until the coroutine completes, making it useful
     * when you need to integrate coroutines into existing synchronous code.
     *
     * @tparam T The type of the result returned by the task.
     * @param task The coroutine task to wait on.
     * @return The result of the coroutine task.
     */
    template<typename T>
    inline T sync_wait(Task<T>&& task) {
        std::mutex mtx;
        std::condition_variable cv;
        std::optional<T> result;
        bool done = false;

        auto handle = task.coro;

        // If the coroutine is not already completed, set up a continuation
        if (!handle.done()) {
            handle.promise().continuation = [&]() {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    done = true;
                    result = std::move(handle.promise().result);  ///< Store the result
                }
                cv.notify_one();  ///< Notify that the task is done
            };

            handle.resume();  ///< Resume the coroutine

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return done; });  ///< Wait for the task to complete
        } else {
            result = std::move(handle.promise().result);  ///< Task already completed
        }

        return result.value();
    }

    /**
     * @brief Specialization of sync_wait for void return type.
     *
     * This function blocks the calling thread until the coroutine completes.
     *
     * @param task The coroutine task to wait on.
     */
    inline void sync_wait(Task<void>&& task) {
        std::mutex mtx;
        std::condition_variable cv;
        bool done = false;

        auto handle = task.coro;

        // If the coroutine is not already completed, set up a continuation
        if (!handle.done()) {
            handle.promise().continuation = [&]() {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    done = true;
                }
                cv.notify_one();  ///< Notify that the task is done
            };

            handle.resume();  ///< Resume the coroutine

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return done; });  ///< Wait for the task to complete
        }
    }

} // namespace gb
