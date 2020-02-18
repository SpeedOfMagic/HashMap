/**Copyright 2020 Daniil Vasilev (c)**/
#pragma once

#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

using std::list;
using std::pair;
using std::vector;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 public:
    struct Node {
        pair<const KeyType, ValueType> key_val;
        typename list<Node*>::iterator iterator_to_pointer;

        explicit Node(const pair<const KeyType, ValueType>& p) : key_val(p) {}

        bool operator==(const Node& other) const {
            return key_val == other.key_val &&
                   iterator_to_pointer == other.iterator_to_pointer;
        }
    };

 private:
    vector<list<Node>> container;
    list<Node*> pointers;
    Hash hash;
    size_t _size = 0, capacity = 0;

 public:
    explicit HashMap(const Hash& h = Hash()) : hash(h) {}

    HashMap(const HashMap& other) : hash(other.hash_function()) {
        for (const auto& i : other)
            insert(i);
    }

    template<typename Iter>
    HashMap(Iter begin, const Iter& end, const Hash& h = Hash()) : hash(h) {
        for (; begin != end; ++begin)
            insert(*begin);
    }

    HashMap(std::initializer_list<pair<const KeyType, ValueType>> list,
            const Hash& h = Hash()) : hash(h) {
        for (const auto& pair : list)
            insert(pair);
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other)
            return *this;
        clear();
        hash = other.hash_function();
        for (const auto& pair : other)
            insert(pair);
        return *this;
    }

    size_t size() const noexcept { return _size; }

    bool empty() const noexcept { return size() == 0; }

    Hash hash_function() const { return hash; }

    size_t get_bucket_index(const KeyType& key) const {
        return hash(key) % capacity;
    }

    void insert(const pair<const KeyType, ValueType>& key_val) {
        if (find(key_val.first) != end())
            return;
        if (size() == capacity) {
            vector<pair<const KeyType, ValueType>> allPairs;
            allPairs.reserve(size() + 1);
            for (auto i = begin(); i != end(); ++i)
                allPairs.push_back(*i);
            allPairs.push_back(key_val);
            clear();
            capacity = capacity ? (capacity << 1) : 1;
            container.resize(capacity);
            for (const auto& pair : allPairs)
                insert(pair);
        } else {
            list<Node>& bucket = container[get_bucket_index(key_val.first)];
            bucket.push_back(Node(key_val));
            pointers.push_back(&bucket.back());
            auto end_pointer = pointers.end();
            bucket.back().iterator_to_pointer = --end_pointer;
            ++_size;
        }
    }

    struct const_iterator;

    struct iterator {
        typename list<Node*>::iterator it;
        iterator() = default;
        explicit iterator(const typename list<Node*>::iterator& i) : it(i) {}

        iterator operator++() {
            ++it;
            return *this;
        }

        iterator operator++(int) {
            iterator i = *this;
            operator++();
            return i;
        }

        pair<const KeyType, ValueType>& operator*() const {
            return (**it).key_val;
        }

        pair<const KeyType, ValueType>* operator->() const {
            return &(**it).key_val;
        }

        bool operator==(const const_iterator& i) const {
            return it == i.it;
        }

        bool operator==(const iterator& i) const {
            return it == i.it;
        }

        bool operator!=(const const_iterator& i) const {
            return !operator==(i);
        }

        bool operator!=(const iterator& i) const {
            return !operator==(i);
        }
    };

    struct const_iterator {
        typename list<Node*>::const_iterator it;
        const_iterator() = default;
        explicit const_iterator(
                const typename list<Node*>::const_iterator& i) : it(i) {}

        const_iterator operator++() {
            ++it;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator i = *this;
            operator++();
            return i;
        }

        const pair<const KeyType, ValueType>& operator*() const {
            return (**it).key_val;
        }

        const pair<const KeyType, ValueType>* operator->() const {
            return &(**it).key_val;
        }

        bool operator==(const const_iterator& i) const {
            return it == i.it;
        }

        bool operator==(const iterator& i) const {
            return it == i.it;
        }

        bool operator!=(const const_iterator& i) const {
            return !operator==(i);
        }

        bool operator!=(const iterator& i) const {
            return !operator==(i);
        }
    };

    iterator begin() noexcept { return iterator(pointers.begin()); }

    iterator end() noexcept { return iterator(pointers.end()); }

    const_iterator begin() const noexcept {
        return const_iterator(pointers.begin());
    }

    const_iterator end() const noexcept {
        return const_iterator(pointers.end());
    }

    iterator find(const KeyType& key) {
        if (capacity == 0)
            return end();
        for (const Node& node : container[get_bucket_index(key)])
            if (node.key_val.first == key)
                return iterator(node.iterator_to_pointer);
        return end();
    }

    const_iterator find(const KeyType& key) const {
        if (capacity == 0)
            return end();
        for (const Node& node : container[get_bucket_index(key)])
            if (node.key_val.first == key)
                return const_iterator(node.iterator_to_pointer);
        return end();
    }

    void erase(const KeyType& key) {
        if (capacity == 0)
            return;

        list<Node>& bucket = container[get_bucket_index(key)];
        for (auto i = bucket.begin(); i != bucket.end(); ++i) {
            if (i -> key_val.first == key) {
                pointers.erase(i -> iterator_to_pointer);
                bucket.erase(i);
                --_size;
                return;
            }
        }
    }

    ValueType& operator[](const KeyType& key) {
        auto iter = find(key);
        if (iter == end()) {
            insert({key, ValueType()});
            iter = find(key);
        }
        return (*iter).second;
    }

    const ValueType& at(const KeyType& key) const {
        auto iter = find(key);
        if (iter == end())
            throw std::out_of_range("404 Not Found");
        else
            return (*iter).second;
    }

    void clear() {
        vector<pair<const KeyType, ValueType>> allPairs;
        allPairs.reserve(size());
        for (const auto& pair : *this)
            allPairs.push_back(pair);
        for (const auto& pair : allPairs)
            erase(pair.first);
    }
};
