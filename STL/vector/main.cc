#include "vector.hpp"
#include <iostream>
#include <string>

using namespace acat;

// 测试基本功能
void test_basic() {
    std::cout << "=== 测试基本功能 ===" << std::endl;
    
    vector<int> v1;
    std::cout << "空vector大小: " << v1.size() << std::endl;
    
    vector<int> v2(5, 10);
    std::cout << "v2(5, 10): ";
    for (size_t i = 0; i < v2.size(); ++i) {
        std::cout << v2[i] << " ";
    }
    std::cout << std::endl;
    
    vector<int> v3{1, 2, 3, 4, 5};
    std::cout << "v3{1,2,3,4,5}: ";
    for (const auto& val : v3) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

// 测试push_back和扩容
void test_push_back() {
    std::cout << "\n=== 测试push_back和扩容 ===" << std::endl;
    
    vector<int> v;
    std::cout << "初始容量: " << v.capacity() << std::endl;
    
    for (int i = 0; i < 10; ++i) {
        v.push_back(i);
        std::cout << "size=" << v.size() 
                  << ", capacity=" << v.capacity() << std::endl;
    }
    
    std::cout << "元素: ";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

// 测试拷贝和移动
void test_copy_move() {
    std::cout << "\n=== 测试拷贝和移动 ===" << std::endl;
    
    vector<int> v1{1, 2, 3, 4, 5};
    vector<int> v2 = v1;  // 拷贝构造
    std::cout << "v1: ";
    for (const auto& val : v1) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    std::cout << "v2(拷贝): ";
    for (const auto& val : v2) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    vector<int> v3 = std::move(v1);  // 移动构造
    std::cout << "v3(移动): ";
    for (const auto& val : v3) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << "v1移动后大小: " << v1.size() << std::endl;
}

// 测试insert和erase
void test_insert_erase() {
    std::cout << "\n=== 测试insert和erase ===" << std::endl;
    
    vector<int> v{1, 2, 3, 4, 5};
    std::cout << "原始: ";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    v.insert(v.begin() + 2, 99);
    std::cout << "在位置2插入99: ";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    v.erase(v.begin() + 2);
    std::cout << "删除位置2: ";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    v.pop_back();
    std::cout << "pop_back后: ";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

// 测试resize和reserve
void test_resize_reserve() {
    std::cout << "\n=== 测试resize和reserve ===" << std::endl;
    
    vector<int> v{1, 2, 3};
    std::cout << "初始: size=" << v.size() 
              << ", capacity=" << v.capacity() << std::endl;
    
    v.reserve(10);
    std::cout << "reserve(10)后: size=" << v.size() 
              << ", capacity=" << v.capacity() << std::endl;
    
    v.resize(8, 0);
    std::cout << "resize(8, 0)后: size=" << v.size() 
              << ", capacity=" << v.capacity() << std::endl;
    std::cout << "元素: ";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    v.resize(2);
    std::cout << "resize(2)后: size=" << v.size() << std::endl;
    std::cout << "元素: ";
    for (const auto& val : v) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

// 测试异常安全
void test_exception() {
    std::cout << "\n=== 测试异常安全 ===" << std::endl;
    
    vector<int> v{1, 2, 3, 4, 5};
    try {
        std::cout << "v.at(10): " << v.at(10) << std::endl;
    } catch (const std::out_of_range& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
}

// 测试自定义类型
void test_custom_type() {
    std::cout << "\n=== 测试自定义类型 ===" << std::endl;
    
    vector<std::string> v;
    v.push_back("Hello");
    v.push_back("World");
    v.push_back("C++");
    
    std::cout << "字符串vector: ";
    for (const auto& str : v) {
        std::cout << str << " ";
    }
    std::cout << std::endl;
}

int main() {
    test_basic();
    test_push_back();
    test_copy_move();
    test_insert_erase();
    test_resize_reserve();
    test_exception();
    test_custom_type();
    
    std::cout << "\n=== 所有测试完成 ===" << std::endl;
    return 0;
}

