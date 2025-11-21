#pragma once
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <initializer_list>
 
namespace my_std
{
    template<typename T>
    class vector
    {
    public:
        // Vector的迭代器是一个原生指针
        typedef T* iterator;
        typedef const T* const_iterator;
        iterator begin() {
            return _start;
        }
        iterator end() {
            return _finish;
        }
        const_iterator cbegin() const {
            return _start;
        }
        const_iterator cend() const {
            return _finish;
        }
 
        // 创建和销毁
        vector()
            :_start(nullptr)
            ,_finish(nullptr)
            ,_endOfStorage(nullptr)
        {}
 
        vector(size_t n, const T& value = T()) {
            _start = new T[n];
            for (size_t i = 0; i < n; i++) {
                _start[i] = value;
            }
            _finish = _endOfStorage = _start + n;
        }
 
        template<class InputIterator>
        vector(InputIterator first, InputIterator last) {
            // 先计算大小（InputIterator可能不支持-操作）
            size_t count = 0;
            for (auto it = first; it != last; ++it) {
                ++count;
            }
            _start = new T[count];
            size_t i = 0;
            for (auto it = first; it != last; ++it, ++i) {
                _start[i] = *it;
            }
            _finish = _endOfStorage = _start + count;
        }
 
        vector(const vector<T>& v) {
            _start = new T[v.size()];
            for (size_t i = 0; i < v.size(); i++) {
                _start[i] = v[i];
            }
            _finish = _endOfStorage = _start + v.size();
        }

        vector(std::initializer_list<T> init) {
            size_t count = init.size();
            _start = new T[count];
            size_t i = 0;
            for (const auto& item : init) {
                _start[i] = item;
                ++i;
            }
            _finish = _endOfStorage = _start + count;
        }
 
        vector<T>& operator=(const vector<T>& v) {
            if (this != &v) {
                vector<T> tmp(v);
                swap(tmp);
            }
            return *this;
        }
 
        ~vector()
        {
            if (_start)
            {
                delete[] _start;
                _start = _finish = _endOfStorage = nullptr;
            }
        }
 
        // 内存管理
        size_t size() const {
            return _finish - _start;
        }
        size_t capacity() const {
            return _endOfStorage - _start;
        }
 
        void reserve(size_t n) {
            if (n > capacity()) {
                size_t old_size = size();
                T* tmp = new T[n];
                if (_start != nullptr) {
                    for (size_t i = 0; i < old_size; i++) {
                        tmp[i] = _start[i];
                    }
                    delete[] _start;
                }
                _start = tmp;
                _finish = _start + old_size;
                _endOfStorage = _start + n;
            }
        }
 
        void resize(size_t n, const T& value = T()) {
            if (n > capacity()) {
                reserve(n);
            }
            if (n < size()) {
                // 缩小：销毁多余元素
                for (size_t i = n; i < size(); i++) {
                    _start[i].~T();
                }
            } else if (n > size()) {
                // 扩大：用value填充
                for (size_t i = size(); i < n; i++) {
                    _start[i] = value;
                }
            }
            _finish = _start + n;
        }
 
        // 元素访问
        T& operator[](size_t pos)
        {
            assert(pos < size());
            return _start[pos];
        }
 
        const T& operator[](size_t pos)const
        {
            assert(pos < size());
            return _start[pos];
        }
 
        T& front()
        {
            assert(size() > 0);
            return *_start;
        }

        const T& front() const
        {
            assert(size() > 0);
            return *_start;
        }
 
        T& back()
        {
            assert(size() > 0);
            return *(_finish - 1);
        }

        const T& back() const
        {
            assert(size() > 0);
            return *(_finish - 1);
        }
 
        // 增删
        void push_back(const T& x) {
            if (_finish == _endOfStorage) {
                size_t newcapacity = capacity() == 0 ? 1 : 2 * capacity();
                reserve(newcapacity);
            }
            *_finish = x;
            ++_finish;
        }
 
        void pop_back() {
            assert(size() > 0);
            --_finish;
        }
 
        void swap(vector<T>& v) {
            if (this != &v) {
                std::swap(_start, v._start);
                std::swap(_finish, v._finish);
                std::swap(_endOfStorage, v._endOfStorage);
            }
        }
 
        iterator insert(iterator pos, const T& x) {
            assert(pos < _finish&& pos >= _start);
 
            if (_finish == _endOfStorage)
            {
                size_t site = pos - _start;
                size_t newcapacity = capacity() == 0 ? 1 : 2 * capacity();
                reserve(newcapacity);
 
                pos = _start + site;//pos到新空间的位置上
            }
            iterator end = _finish - 1;
            while (end >= pos)//开始整体向后退
            {
                *(end + 1) = *end;
                end--;
            }
            *pos = x;
            ++_finish;
 
            return pos;
        }
 
        iterator erase(iterator pos){
            assert(pos < _finish&& pos >= _start);
            assert(size() > 0);
            //开始向前移动
            iterator start = pos + 1;
            while (start < _finish)
            {
                *(start - 1) = *start;
                start++;
            }
            _finish--;
            return pos;//返回删除的位置
        }
    private:
        iterator _start; // 指向数据块的开始
        iterator _finish; // 指向有效数据的尾
        iterator _endOfStorage; // 指向存储容量的尾
    };
}