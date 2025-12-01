/**
 * STL Containers Example
 *
 * Demonstrates conversion of C++ STL containers to Rust and Go equivalents
 */

#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <optional>

class DataProcessor {
private:
    // Vector -> Vec (Rust) / []T (Go)
    std::vector<int> numbers;
    std::vector<std::string> names;

    // Map -> BTreeMap (Rust) / map[K]V (Go)
    std::map<std::string, int> scores;

    // UnorderedMap -> HashMap (Rust) / map[K]V (Go)
    std::unordered_map<int, std::string> id_to_name;

    // Set -> BTreeSet (Rust) / map[T]bool (Go)
    std::set<int> unique_ids;

    // String -> String (Rust) / string (Go)
    std::string description;

    // Optional -> Option (Rust) / *T (Go)
    std::optional<int> optional_value;

public:
    DataProcessor() {
        numbers = std::vector<int>();
        description = "Data processor";
    }

    // Vector operations
    void addNumber(int n) {
        numbers.push_back(n);
    }

    int getNumber(size_t index) const {
        if (index < numbers.size()) {
            return numbers[index];
        }
        return -1;
    }

    size_t getNumberCount() const {
        return numbers.size();
    }

    // Map operations
    void setScore(const std::string& name, int score) {
        scores[name] = score;
    }

    int getScore(const std::string& name) const {
        auto it = scores.find(name);
        if (it != scores.end()) {
            return it->second;
        }
        return 0;
    }

    bool hasScore(const std::string& name) const {
        return scores.find(name) != scores.end();
    }

    // UnorderedMap operations
    void registerUser(int id, const std::string& name) {
        id_to_name[id] = name;
    }

    std::string getUserName(int id) const {
        auto it = id_to_name.find(id);
        if (it != id_to_name.end()) {
            return it->second;
        }
        return "";
    }

    // Set operations
    void addUniqueId(int id) {
        unique_ids.insert(id);
    }

    bool hasUniqueId(int id) const {
        return unique_ids.find(id) != unique_ids.end();
    }

    size_t getUniqueIdCount() const {
        return unique_ids.size();
    }

    // Optional operations
    void setValue(int value) {
        optional_value = value;
    }

    void clearValue() {
        optional_value = std::nullopt;
    }

    bool hasValue() const {
        return optional_value.has_value();
    }

    int getValue() const {
        return optional_value.value_or(-1);
    }

    // String operations
    void setDescription(const std::string& desc) {
        description = desc;
    }

    std::string getDescription() const {
        return description;
    }
};

// Nested containers
class NestedDataStructure {
private:
    // Vector of vectors
    std::vector<std::vector<int>> matrix;

    // Map of vectors
    std::map<std::string, std::vector<double>> data_sets;

    // Vector of maps
    std::vector<std::map<std::string, int>> records;

public:
    NestedDataStructure() {}

    void addRow(const std::vector<int>& row) {
        matrix.push_back(row);
    }

    void addDataSet(const std::string& name, const std::vector<double>& data) {
        data_sets[name] = data;
    }

    void addRecord(const std::map<std::string, int>& record) {
        records.push_back(record);
    }

    size_t getRowCount() const {
        return matrix.size();
    }
};

// Template function with STL containers
template<typename T>
class Container {
private:
    std::vector<T> items;

public:
    void add(const T& item) {
        items.push_back(item);
    }

    T get(size_t index) const {
        return items[index];
    }

    size_t size() const {
        return items.size();
    }

    bool empty() const {
        return items.empty();
    }
};

int main() {
    DataProcessor processor;

    // Add numbers
    processor.addNumber(1);
    processor.addNumber(2);
    processor.addNumber(3);

    // Set scores
    processor.setScore("Alice", 95);
    processor.setScore("Bob", 87);

    // Register users
    processor.registerUser(1, "Alice");
    processor.registerUser(2, "Bob");

    // Add unique IDs
    processor.addUniqueId(100);
    processor.addUniqueId(200);
    processor.addUniqueId(100); // Duplicate, won't be added

    // Optional value
    processor.setValue(42);

    return 0;
}
