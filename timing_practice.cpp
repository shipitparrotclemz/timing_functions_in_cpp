//
// Created by Goh Chang Ming Clement on 3/12/23.
//

#include "timing_practice.hpp"
#include <chrono>
#include <thread>
#include <iostream>
#include <type_traits>


struct Timing {
    /**
     * Uses the Struct construction / destruction / life cycle to time a function.
     * This is used to time a function by placing it at the start of the function.
     * When the function is called, this timing struct is created in the function stack frame (placed on the call stack)
     * Once the function ends, this struct will be destructed with the function stack frame (and removed from the call stack)
     *
     * At construction of this Struct, keep track of the start
     * At the destruction of this Struct, keep track of the end, and print out end - start as the time taken for a function
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    std::chrono::duration<float> duration;

    Timing() {
        start = std::chrono::high_resolution_clock::now();
    }

    ~Timing() {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        std::cout << "function took " << milliseconds.count() << " ms" << std::endl;
    }
};

/**
 * This version of timeit uses two separate function templates to handle the cases where func returns void and where it returns something else.
 * The function templates use the std::enable_if type trait to conditionally enable or disable the functions based on the return type of func.
 */

template<typename Func, typename... Args>
typename std::enable_if<std::is_void<decltype(std::declval<Func>()(std::declval<Args>()...))>::value>::type
timeit(Func&& func, Args&&... args) {
    /**
    * this specific function supports functions with void
    *
    * Uses a template and a variadic argument list
    *
    * This timeit is a function template that takes in a callable object `func` as its first argument
    * and any number of arguments as the rest of its arguments
    *
    * the `auto` return type of the function is deduced based on the return type of the function call
    * using `decltype`
    *
    * the std::forward function is used to forward the arguments in func in the correct way, depending on their value category
    *
    * If func returns void, the function does not return anything and instead directly calls func with the provided arguments.
    *
    * Otherwise, it calls func with the provided arguments and returns the result.
    *
    * this specific function won't support functions with void, and we get the error: variable has incomplete type 'void' when timing functions that returns void
    *
    */
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << "function took " << milliseconds.count() << " ms" << std::endl;
}

template<typename Func, typename... Args>
typename std::enable_if<!std::is_void<decltype(std::declval<Func>()(std::declval<Args>()...))>::value,
        decltype(std::declval<Func>()(std::declval<Args>()...))>::type
timeit(Func&& func, Args&&... args) {
    /**
    * this specific function won't support functions with void, and we get the error: variable has incomplete type 'void' when timing functions that returns void
    *
    * Uses a template and a variadic argument list
    *
    * This timeit is a function template that takes in a callable object `func` as its first argument
    * and any number of arguments as the rest of its arguments
    *
    * the `auto` return type of the function is deduced based on the return type of the function call
    * using `decltype`
    *
    * the std::forward function is used to forward the arguments in func in the correct way, depending on their value category
    *
    * If func returns void, the function does not return anything and instead directly calls func with the provided arguments.
    *
    * Otherwise, it calls func with the provided arguments and returns the result.
    *
    * we use std::conditional to conditionally choose the return type of the function based on whether the callable object returns void or not.
    *
    * The function conditionally chooses the return type based on whether func returns void or not using std::conditional_t and std::is_same_v.
    *
    * Otherwise, this function won't support functions with void, and we get the error: variable has incomplete type 'void' when timing functions that returns void
     *
    */
    using namespace std::chrono_literals;
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::chrono::milliseconds milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    std::cout << "function took " << milliseconds.count() << " ms" << std::endl;
    return result;
}

void mockDatabaseCall() {
    Timing timing;
    using namespace std::chrono_literals;
    std::cout << "Starting the mock database call" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Ending the mock database call" << std::endl;
    return;
}

void anotherMockDatabaseCall() {
    using namespace std::chrono_literals;
    std::cout << "Starting the mock database call" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Ending the mock database call" << std::endl;
    return;
}

int yetAnotherMockDatabaseCall() {
    using namespace std::chrono_literals;
    std::cout << "Starting the mock database call" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Ending the mock database call" << std::endl;
    return 10;
}

int main() {
    // Time function with method 1; place a timing struct in the function implementation
    mockDatabaseCall();
    // Time function with method 2: use a timeit function
    timeit(anotherMockDatabaseCall);
    timeit(yetAnotherMockDatabaseCall);
}

