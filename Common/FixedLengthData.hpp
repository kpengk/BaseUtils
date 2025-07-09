#include <iostream>
#include <span>
#include <vector>

class FixedLengthData {
private:
    std::vector<int> data;
    size_t capacity;
    size_t size;
    size_t front;

public:
    FixedLengthData(size_t length) : capacity(length), size(0), front(0) {
        data.resize(capacity);
    }

    void receiveData(int newData) {
        data[(front + size) % capacity] = newData;
        if (size < capacity) {
            size++;
        } else {
            front = (front + 1) % capacity; // 当队列已满时，移动front指针
        }
    }

    std::span<int> getFixedLengthData() {
        return {data.data() + front, size};
    }
};
