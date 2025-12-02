/**
 * Exception Handling Examples
 * Demonstrates various C++ exception patterns and their conversion
 */

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

/**
 * Example 1: Basic try-catch with standard exceptions
 */
class FileReader {
private:
    std::string filename;
    bool is_open;

public:
    FileReader(const std::string& name) : filename(name), is_open(false) {}

    // Function that may throw std::runtime_error
    void open() {
        try {
            if (filename.empty()) {
                throw std::invalid_argument("Filename cannot be empty");
            }

            // Simulate file opening
            is_open = true;
        } catch (const std::invalid_argument& e) {
            // Handle invalid argument
            is_open = false;
            throw; // Re-throw
        }
    }

    // Function that may throw std::out_of_range
    std::string readLine(int line_number) {
        if (line_number < 0) {
            throw std::out_of_range("Line number must be positive");
        }

        return "Line content"; // Simplified
    }

    void close() noexcept {
        is_open = false;
    }
};

/**
 * Example 2: Multiple catch clauses
 */
class DataValidator {
public:
    int validateAndConvert(const std::string& data) {
        try {
            if (data.empty()) {
                throw std::invalid_argument("Data is empty");
            }

            int value = std::stoi(data);

            if (value < 0) {
                throw std::range_error("Value must be positive");
            }

            return value;
        } catch (const std::invalid_argument& e) {
            // Handle parse error
            return -1;
        } catch (const std::range_error& e) {
            // Handle range error
            return 0;
        } catch (const std::exception& e) {
            // Catch all other standard exceptions
            return -2;
        } catch (...) {
            // Catch all unknown exceptions
            return -99;
        }
    }
};

/**
 * Example 3: RAII with exception safety
 */
class Resource {
private:
    int* data;
    size_t size;

public:
    Resource(size_t n) : size(n) {
        try {
            data = new int[size];
        } catch (const std::bad_alloc& e) {
            // Handle memory allocation failure
            data = nullptr;
            size = 0;
            throw;
        }
    }

    ~Resource() {
        delete[] data;
    }

    // Copy operations deleted for simplicity
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;

    int& at(size_t index) {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }
};

/**
 * Example 4: Custom exception class
 */
class NetworkException : public std::runtime_error {
public:
    explicit NetworkException(const std::string& message)
        : std::runtime_error(message) {}
};

class NetworkClient {
public:
    void connect(const std::string& host) {
        if (host.empty()) {
            throw NetworkException("Host cannot be empty");
        }

        try {
            // Simulate connection
            bool success = true;

            if (!success) {
                throw NetworkException("Connection failed");
            }
        } catch (const NetworkException& e) {
            // Log error and re-throw
            throw;
        }
    }

    std::string request(const std::string& endpoint) {
        try {
            if (endpoint.empty()) {
                throw std::invalid_argument("Endpoint cannot be empty");
            }

            // Simulate network request
            return "Response data";
        } catch (const std::exception& e) {
            // Convert to custom exception
            throw NetworkException("Request failed: " + std::string(e.what()));
        }
    }
};

/**
 * Example 5: Exception specifications
 */
class Calculator {
public:
    // Function marked noexcept - will not throw
    int add(int a, int b) noexcept {
        return a + b;
    }

    // Function that may throw
    int divide(int a, int b) {
        if (b == 0) {
            throw std::invalid_argument("Division by zero");
        }
        return a / b;
    }

    // Function with exception specification (C++17 style)
    double safeDivide(double a, double b) noexcept {
        try {
            if (b == 0.0) {
                throw std::invalid_argument("Division by zero");
            }
            return a / b;
        } catch (...) {
            // Convert exception to safe default
            return 0.0;
        }
    }
};

/**
 * Example 6: Exception propagation chain
 */
class ServiceLayer {
private:
    std::unique_ptr<NetworkClient> client;

public:
    ServiceLayer() : client(std::make_unique<NetworkClient>()) {}

    std::string fetchData(const std::string& host, const std::string& endpoint) {
        try {
            client->connect(host);
            return client->request(endpoint);
        } catch (const NetworkException& e) {
            // Handle network exception
            throw std::runtime_error("Service error: " + std::string(e.what()));
        } catch (const std::exception& e) {
            // Handle all other exceptions
            throw;
        }
    }
};

/**
 * Example 7: Nested try-catch blocks
 */
class TransactionManager {
public:
    bool executeTransaction() {
        try {
            // Outer transaction
            beginTransaction();

            try {
                // Inner operation
                performOperation();
                commitTransaction();
                return true;
            } catch (const std::runtime_error& e) {
                // Handle operation error
                rollbackTransaction();
                throw; // Re-throw to outer handler
            }
        } catch (const std::exception& e) {
            // Handle transaction error
            return false;
        }
    }

private:
    void beginTransaction() { /* ... */ }
    void performOperation() { /* ... */ }
    void commitTransaction() { /* ... */ }
    void rollbackTransaction() { /* ... */ }
};

/**
 * Example 8: Exception-safe container operations
 */
template<typename T>
class SafeVector {
private:
    std::vector<T> data;

public:
    void push_back_safe(const T& value) {
        try {
            data.push_back(value);
        } catch (const std::bad_alloc& e) {
            // Handle memory allocation failure
            throw std::runtime_error("Cannot add element: out of memory");
        }
    }

    T at_safe(size_t index) {
        try {
            return data.at(index);
        } catch (const std::out_of_range& e) {
            // Return default value instead of throwing
            return T();
        }
    }

    size_t size() const noexcept {
        return data.size();
    }
};

/**
 * Main function demonstrating various patterns
 */
int main() {
    // Example 1: File operations
    try {
        FileReader reader("example.txt");
        reader.open();
        std::string line = reader.readLine(0);
        reader.close();
    } catch (const std::exception& e) {
        // Handle error
    }

    // Example 2: Data validation
    DataValidator validator;
    int result = validator.validateAndConvert("42");

    // Example 3: RAII
    try {
        Resource res(1024);
        int value = res.at(0);
    } catch (const std::exception& e) {
        // Resource automatically cleaned up
    }

    // Example 4: Custom exceptions
    NetworkClient client;
    try {
        client.connect("example.com");
        std::string response = client.request("/api/data");
    } catch (const NetworkException& e) {
        // Handle network error
    }

    // Example 5: Safe operations
    Calculator calc;
    int sum = calc.add(1, 2);  // noexcept
    try {
        int quotient = calc.divide(10, 0);
    } catch (const std::exception& e) {
        double safe_result = calc.safeDivide(10.0, 0.0);
    }

    // Example 6: Service layer
    ServiceLayer service;
    try {
        std::string data = service.fetchData("api.example.com", "/data");
    } catch (const std::runtime_error& e) {
        // Handle service error
    }

    // Example 7: Transactions
    TransactionManager tm;
    bool success = tm.executeTransaction();

    // Example 8: Safe containers
    SafeVector<int> vec;
    vec.push_back_safe(42);
    int val = vec.at_safe(0);

    return 0;
}
