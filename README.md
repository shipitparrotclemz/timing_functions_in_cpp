# Timing Functions in CPP

## CPP Version

C++14
- For chrono

## Building and Running

```
// build the build system
cmake .

// Using the build system above, compile the project into an Executable; TimingPractice
cmake --build .

// Run the executable
./TimingPractice
```

## How do we time this?

**Method 1: Use the lifecycle of a Struct to time a function.**

**Create a Timing struct, and define three attributes:**

- Start time of the function - `std::chrono::time_point<std::chrono::high_resolution_clock> start`
- End time of the function - `std::chrono::time_point<std::chrono::high_resolution_clock> end`
- Overall time taken by the function, in seconds - `std::chrono::duration<float> duration`

```c++
struct Timing {
    /**
     * Uses the Struct construction / destruction / life cycle to time a function.
     * This is used to time a function by placing it at the start of the function.
     * When the function is called, this timing struct is created in the function call (on the call stack)
     * Once the function ends, this struct will be destructed with the function call (and removed from the call stack)
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
```

Now the smart parrots might ask:

**Q: How does this Timing Struct work to tim a function?**

We define this Struct at the start of the function you wish to time

```c++
void mockDatabaseCall() {
    Timing timing;
    using namespace std::chrono_literals;
    std::cout << "Starting the mock database call" << std::endl;
    std::this_thread::sleep_for(1s);
    std::cout << "Ending the mock database call" << std::endl;
    return;
}
```

When the function is called, it creates a function stack frame, a data structure used to store information about the function call, on the call-stack.

This Struct is then created at the start of the function, timing the start time of the function.

When the function ends, the struct, along with the function stack frame is destructed, calling the destructor.

In the destructor, we get the end time, and print the time taken for the whole function.

**Method 2: Create a function that receives a function pointer, and times the function**

```c++
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
```

## Testing out both timing strategies

```
int main() {
    // Time function with method 1; place a timing struct in the function implementation
    mockDatabaseCall();
    // Time function with method 2: use a timeit function
    timeit(anotherMockDatabaseCall);
    timeit(yetAnotherMockDatabaseCall);
}
```

```linux
➜  timing_in_c++ git:(master) ✗ ./TimingPractice
Starting the mock database call
Ending the mock database call
function took 1005 ms
Starting the mock database call
Ending the mock database call
function took 1005 ms
Starting the mock database call
Ending the mock database call
function took 1000 ms
```

## Sources and Credits:

1. The Cherno, for his amazing tutorial on timing functions - Timing in C++
- https://www.youtube.com/watch?v=oEx5vGNFrLk&ab_channel=TheCherno

