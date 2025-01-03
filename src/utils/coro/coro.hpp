#pragma once

#include <condition_variable>
#include <coroutine>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <stdexcept>

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
    struct Task {
        struct promise_type; ///< Forward declaration of the promise type
        using handle_type = std::coroutine_handle<promise_type>; ///< Type alias for the coroutine
                                                                 ///< handle

        handle_type coro; ///< Handle to the coroutine

        /**
         * @brief Constructs a Task from a coroutine handle.
         *
         * @param h The coroutine handle to wrap.
         */
        Task(handle_type h) : coro(h) {}

        Task(const Task &) = delete; ///< Deleted copy constructor
        Task(Task &&t) noexcept : coro(t.coro) { t.coro = nullptr; } ///< Move constructor
        ~Task() {
            if (coro)
                coro.destroy();
        } ///< Destructor that cleans up the coroutine

        Task &operator=(const Task &) = delete; ///< Deleted copy assignment operator
        Task &operator=(Task &&t) noexcept {
            if (this != &t) {
                if (coro)
                    coro.destroy();
                coro = t.coro;
                t.coro = nullptr;
            }
            return *this;
        }

        /**
         * @brief Awaitable structure to allow awaiting on the task using co_await.
         */
        struct Awaitable {
            handle_type coro; ///< Handle to the coroutine

            bool await_ready() { return coro.done(); } ///< Checks if the coroutine is already done
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); } ///< Resumes the coroutine when suspended
            T await_resume() { return coro.promise().result.value(); } ///< Returns the result of the coroutine
        };

        Awaitable operator co_await() { return Awaitable{coro}; } ///< Provides co_await support for the task

        /**
         * @brief The promise_type represents the state of the coroutine and provides
         * methods for resuming and completing the coroutine.
         */
        struct promise_type {
            std::optional<T> result; ///< Optional result of the task
            std::function<void()> continuation; ///< Optional continuation to be called on completion

            Task get_return_object() {
                return Task{handle_type::from_promise(*this)};
            } ///< Creates the Task from the promise
            std::suspend_never initial_suspend() {
                return {};
            } ///< Indicates no suspension at the start of the coroutine
            std::suspend_always final_suspend() noexcept { ///< Indicates suspension at
                                                           ///< the end of the coroutine
                if (continuation) {
                    continuation(); ///< If a continuation is set, call it
                }
                return {};
            }
            void return_value(T v) { result = v; } ///< Sets the result value of the coroutine
            void unhandled_exception() { std::terminate(); } ///< Terminates if an exception is unhandled
        };
    };

    /**
     * @brief Specialization of Task for void return type.
     */
    template<>
    struct Task<void> {
        struct promise_type; ///< Forward declaration of the promise type
        using handle_type = std::coroutine_handle<promise_type>; ///< Type alias for the coroutine
                                                                 ///< handle

        handle_type coro; ///< Handle to the coroutine

        /**
         * @brief Constructs a Task from a coroutine handle.
         *
         * @param h The coroutine handle to wrap.
         */
        Task(handle_type h) : coro(h) {}

        Task(const Task &) = delete; ///< Deleted copy constructor
        Task(Task &&t) noexcept : coro(t.coro) { t.coro = nullptr; } ///< Move constructor
        ~Task() {
            if (coro)
                coro.destroy();
        } ///< Destructor that cleans up the coroutine

        Task &operator=(const Task &) = delete; ///< Deleted copy assignment operator
        Task &operator=(Task &&t) noexcept {
            if (this != &t) {
                if (coro)
                    coro.destroy();
                coro = t.coro;
                t.coro = nullptr;
            }
            return *this;
        }

        /**
         * @brief Awaitable structure to allow awaiting on the task using co_await.
         */
        struct Awaitable {
            handle_type coro; ///< Handle to the coroutine

            bool await_ready() { return coro.done(); } ///< Checks if the coroutine is already done
            void await_suspend(std::coroutine_handle<> h) { coro.resume(); } ///< Resumes the coroutine when suspended
            void await_resume() { coro.promise().result; } ///< Nothing to return for void tasks
        };

        Awaitable operator co_await() { return Awaitable{coro}; } ///< Provides co_await support for the task

        /**
         * @brief The promise_type represents the state of the coroutine and provides
         * methods for resuming and completing the coroutine.
         */
        struct promise_type {
            std::optional<int> result; ///< Optional integer to track completion
            std::function<void()> continuation; ///< Optional continuation to be called on completion

            Task get_return_object() {
                return Task{handle_type::from_promise(*this)};
            } ///< Creates the Task from the promise
            std::suspend_never initial_suspend() {
                return {};
            } ///< Indicates no suspension at the start of the coroutine
            std::suspend_always final_suspend() noexcept { ///< Indicates suspension at
                                                           ///< the end of the coroutine
                if (continuation) {
                    continuation(); ///< If a continuation is set, call it
                }
                return {};
            }
            void return_void() { result = 0; } ///< Sets result to 0 to indicate completion for void tasks
            void unhandled_exception() { std::terminate(); } ///< Terminates if an exception is unhandled
        };
    };

    /**
     * @brief Synchronously waits for a coroutine task to complete and returns the
     * result.
     *
     * This function blocks the calling thread until the coroutine completes, making
     * it useful when you need to integrate coroutines into existing synchronous
     * code.
     *
     * @tparam T The type of the result returned by the task.
     * @param task The coroutine task to wait on.
     * @return The result of the coroutine task.
     */
    template<typename T>
    inline T sync_wait(Task<T> &task) { // Overload for lvalue reference
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
                    result = std::move(handle.promise().result); ///< Store the result
                }
                cv.notify_one(); ///< Notify that the task is done
            };

            handle.resume(); ///< Resume the coroutine

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return done; }); ///< Wait for the task to complete
        } else {
            result = std::move(handle.promise().result); ///< Task already completed
        }

        return result.value();
    }

    template<typename T>
    inline T sync_wait(Task<T> &&task) { // Overload for rvalue reference
        return sync_wait(task); // Delegate to the lvalue overload
    }

    /**
     * @brief Specialization of sync_wait for void return type.
     *
     * This function blocks the calling thread until the coroutine completes.
     *
     * @param task The coroutine task to wait on.
     */
    inline void sync_wait(Task<void> &task) { // Overload for lvalue reference
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
                cv.notify_one(); ///< Notify that the task is done
            };

            handle.resume(); ///< Resume the coroutine

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return done; }); ///< Wait for the task to complete
        }
    }

    inline void sync_wait(Task<void> &&task) { // Overload for rvalue reference
        sync_wait(task); // Delegate to the lvalue overload
    }


    /**
     * @class Coroutine_guard
     * @brief Guard class for managing void-returning coroutines.
     *
     * `Coroutine_guard` ensures that a coroutine completes its execution,
     * either by explicitly awaiting it or by enforcing completion upon
     * object destruction if it hasn't been awaited. This class enforces single-use,
     * meaning the coroutine can only be awaited once; further await attempts will
     * result in an exception.
     *
     * @tparam Task Type of the coroutine function, which must return `void`.
     */
    template<typename Task>
    class Coroutine_guard {
        /**
         * @brief Compile-time constraint to ensure Task returns `void`.
         *
         * Ensures that the Task type passed to `Coroutine_guard` returns `void`.
         * If Task does not return `void`, compilation will fail with a static assertion.
         */
        static_assert(std::is_void_v<decltype(std::declval<Task>().operator co_await().await_resume())>,
                      "Coroutine_guard can only be used with void-returning coroutines.");

    private:
        Task _task; ///< The coroutine task to be managed
        bool _awaited{false}; ///< Tracks whether the coroutine has been awaited

    public:
        /**
         * @brief Constructs a Coroutine_guard with a coroutine task.
         *
         * Initializes the guard with a coroutine task. The task is
         * expected to be void-returning and will be invoked once when
         * awaited or upon destruction.
         *
         * @param task The coroutine task to be managed.
         */
        explicit Coroutine_guard(Task &&task) : _task(std::forward<Task>(task)) {}

        /**
         * @brief Await function for co_awaiting the guard.
         *
         * Allows the Coroutine_guard to be awaited like a coroutine.
         * If the coroutine has already been awaited, it throws an exception
         * to prevent multiple awaits.
         *
         * @throws std::runtime_error if the coroutine has already been awaited.
         */
        auto await() {
            if (_awaited) {
                throw std::runtime_error("Coroutine has already been awaited.");
            }
            _awaited = true;
            return _task(); // Start the coroutine task
        }

        /**
         * @brief Destructor to enforce coroutine completion if not already awaited.
         *
         * If the coroutine has not been awaited when the guard is destroyed,
         * it will complete the coroutine using `sync_wait`. This ensures that
         * the coroutine completes exactly once.
         */
        ~Coroutine_guard() {
            if (!_awaited) {
                sync_wait(_task);
            }
        }

        // Deleted copy and move constructors to enforce single ownership
        Coroutine_guard(const Coroutine_guard &) = delete;
        Coroutine_guard &operator=(const Coroutine_guard &) = delete;
        Coroutine_guard(Coroutine_guard &&) = delete;
        Coroutine_guard &operator=(Coroutine_guard &&) = delete;
    };
} // namespace gb
