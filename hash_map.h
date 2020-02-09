/**Copyright 2020 Daniil Vasilev (c)**/
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

#pragma once

using std::list;
using std::pair;
using std::vector;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 public:
    struct Node {
        pair<const KeyType, ValueType> p;
        typename list<Node*>::iterator ptr;

        explicit Node(const pair<const KeyType, ValueType>& _p) : p(_p) {}

        bool operator==(const Node& n) const {
            return p == n.p && ptr == n.ptr;
        }
    };

 private:
    vector<list<Node>> container;
    list<Node*> pointers;
    Hash hash;
    size_t Size = 0, Capacity = 0;

 public:
    explicit HashMap(const Hash& h = Hash()) : hash(h) {}
    explicit HashMap(const HashMap& hm) : hash(hm.hash_function()) {
        for (const auto& i : hm)
            insert(i);
    }

    template<typename Iter>
    HashMap(Iter begin, const Iter& end, const Hash& h = Hash()) : hash(h) {
        for (; begin != end; ++begin)
            insert(*begin);
    }

    HashMap(std::initializer_list<pair<const KeyType, ValueType>> l,
            const Hash& h = Hash()) : hash(h) {
        for (const auto& i : l)
            insert(i);
    }

    const HashMap& operator=(const HashMap& hm) {
        if (container == hm.container)
            return *this;
        clear();
        hash = hm.hash_function();
        for (const auto& i : hm)
            insert(i);
        return *this;
    }

    size_t size() const noexcept { return Size; }
    bool empty() const noexcept { return size() == 0; }
    Hash hash_function() const noexcept { return hash; }

    size_t h(const KeyType& key) const noexcept {
        return hash(key) % Capacity;
    }

    void insert(const pair<const KeyType, ValueType>& p) {
        if (find(p.first) == end()) {
            if (Size == Capacity) {
                vector<pair<const KeyType, ValueType>> allPairs;
                allPairs.reserve(size() + 1);
                for (auto i = begin(); i != end(); ++i)
                    allPairs.push_back(*i);
                allPairs.push_back(p);
                clear();
                Capacity = Capacity ? (Capacity << 1) : 1;
                container.resize(Capacity);
                for (const auto& i : allPairs)
                    insert(i);
            } else {
                size_t cell = h(p.first);
                container[cell].push_back(Node(p));
                pointers.push_back(&container[cell].back());
                auto d = pointers.end();
                container[cell].back().ptr = --d;
                ++Size;
            }
        }
    }

    struct const_iterator;
    struct iterator {
        typename list<Node*>::iterator it;
        iterator() = default;
        explicit iterator(
                const typename list<Node*>::iterator& i) : it(i) {}

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
            return (**it).p;
        }
        pair<const KeyType, ValueType>* operator->() const {
            return &(**it).p;
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
            return (**it).p;
        }
        const pair<const KeyType, ValueType>* operator->() const {
            return &(**it).p;
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

    iterator begin() noexcept {
        return iterator(pointers.begin());
    }
    iterator end() noexcept {
        return iterator(pointers.end());
    }

    const_iterator begin() const noexcept {
        return const_iterator(pointers.begin());
    }
    const_iterator end() const noexcept {
        return const_iterator(pointers.end());
    }

    const_iterator find(const KeyType& key) const {
        if (Capacity == 0)
            return end();
        size_t cell = h(key);
        for (auto i = container[cell].begin(); i != container[cell].end(); ++i)
            if ((*i).p.first == key)
                return const_iterator((*i).ptr);
        return end();
    }

    iterator find(const KeyType& key) {
        if (Capacity == 0)
            return end();
        size_t cell = h(key);
        for (auto i = container[cell].begin(); i != container[cell].end(); ++i)
            if ((*i).p.first == key)
                return iterator((*i).ptr);
        return end();
    }

    void erase(const KeyType& key) {
        if (Capacity == 0)
            return;
        size_t c = h(key);
        for (auto i = container[c].begin(); i != container[c].end(); ++i) {
            if ((*i).p.first == key) {
                pointers.erase((*i).ptr);
                container[c].erase(i);
                --Size;
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
        for (auto i = begin(); i != end(); ++i)
            allPairs.push_back(*i);
        for (const auto& i : allPairs)
            erase(i.first);
    }
};
