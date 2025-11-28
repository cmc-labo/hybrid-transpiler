// Example C++ class for transpilation testing

#include <memory>
#include <string>

class Point {
private:
    int x;
    int y;

public:
    Point(int x, int y) : x(x), y(y) {}

    int getX() const { return x; }
    int getY() const { return y; }

    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }

    int distanceSquared() const {
        return x * x + y * y;
    }

    void translate(int dx, int dy) {
        x += dx;
        y += dy;
    }
};

class Rectangle {
private:
    Point topLeft;
    Point bottomRight;

public:
    Rectangle(const Point& tl, const Point& br)
        : topLeft(tl), bottomRight(br) {}

    int width() const {
        return bottomRight.getX() - topLeft.getX();
    }

    int height() const {
        return bottomRight.getY() - topLeft.getY();
    }

    int area() const {
        return width() * height();
    }
};

// Smart pointer usage example
class Resource {
private:
    std::unique_ptr<int[]> data;
    size_t size;

public:
    Resource(size_t n) : data(std::make_unique<int[]>(n)), size(n) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = 0;
        }
    }

    int& operator[](size_t index) {
        return data[index];
    }

    const int& operator[](size_t index) const {
        return data[index];
    }

    size_t getSize() const {
        return size;
    }
};

// Shared ownership example
class SharedData {
private:
    std::shared_ptr<std::string> message;

public:
    SharedData(const std::string& msg)
        : message(std::make_shared<std::string>(msg)) {}

    const std::string& getMessage() const {
        return *message;
    }

    void setMessage(const std::string& msg) {
        *message = msg;
    }
};

int main() {
    Point p1(10, 20);
    Point p2(30, 40);

    Rectangle rect(p1, p2);
    int a = rect.area();

    Resource res(100);
    res[0] = 42;

    SharedData data("Hello, World!");

    return 0;
}
