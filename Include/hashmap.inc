#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>

#pragma once

#ifdef CLANGD_ONLY
#include "hashmap.h"
#endif

#define TKV template <typename TKey, typename TValue>
#define TMAP Hashmap<TKey, TValue>

TKV Node<TKey, TValue>::Node(const TKey &key, const TValue &data)
    : key(key), data(data), next(nullptr) {}

TKV TMAP::Hashmap()
    : _bucket_count(DEFAULT_HASHMAP_BUCKET_COUNT), _item_count(0) {
    _buckets = new Node_t *[_bucket_count];
    for (int i = 0; i < _bucket_count; ++i) {
        _buckets[i] = nullptr;
    }
}

TKV TMAP::Hashmap(const Hashmap &other)
    : _bucket_count(other._bucket_count), _item_count(other._item_count) {
    _buckets = new Node_t *[_bucket_count];
    copy_from(other._buckets, _buckets, _bucket_count);
}

TKV TMAP::Hashmap(Hashmap &&other)
    : _bucket_count(other._bucket_count), _item_count(other._item_count),
      _buckets(other._buckets) {
    other._buckets = nullptr;
}

TKV TMAP::~Hashmap() {
    if (_buckets != nullptr) {
        clear(); // deletes nodes
        delete[] _buckets;
    }
}

TKV bool TMAP::add(const TKey &key, const TValue &value) {
    hash_t hval = hash(key);
    Node_t *node = get_node(hval);
    if (node != nullptr) {
        return false;
    }
    add_node(hval, key, value);
    return true;
}

TKV void TMAP::put(const TKey &key, const TValue &value) {
    hash_t hval = hash(key);
    Node_t *node = get_node(hval);
    if (node == nullptr) {
        add_node(hval, key, value);
    } else {
        node->value = value;
    }
}

TKV TValue &TMAP::get(const TKey &key) {
    hash_t hval = hash(key);
    Node_t *node = get_node(hval);
    if (node == nullptr) {
        throw key_not_found("No node found for key");
    }
    return node->data;
}

TKV const TValue &TMAP::get(const TKey &key) const {
    hash_t hval = hash(key);
    Node_t *node = get_node(hval);
    if (node == nullptr) {
        throw key_not_found("No node found for key");
    }
    return node->data;
}

TKV TValue TMAP::remove(const TKey &key) {
    hash_t hval = hash(key);
    Node_t *bucket = _buckets[hval % _bucket_count];

    Node_t *current = bucket;
    Node_t *parent = bucket;

    if (current->key == key) {
        TValue val = current->data;
        current->data = nullptr;
        _buckets[hval % _bucket_count] = current->next;
        delete current;
        _item_count--;
        return val;
    }

    for (; current != nullptr; parent = parent->next) {
        current = current->next;
        if (current->key == key) {
            TValue val = current->data;
            current->data = nullptr;
            parent->next = current->next;
            delete current;
            _item_count--;
            return val;
        }
    }
    throw key_not_found("No mode found for key");
}

TKV bool TMAP::contains(const TKey &key) const {
    hash_t hval = hash(key);
    Node_t *bucket = _buckets[hval % _bucket_count];

    for (Node_t *current = bucket; current != nullptr;
         current = current->next) {
        if (current->key == key) {
            return true;
        }
    }
    return false;
}

TKV size_t TMAP::size() const { return _item_count; }

TKV Node<TKey, TValue> *TMAP::get_node(hash_t hval, const TKey &key) {
    Node_t *node = _buckets[hval % _bucket_count];
    for (Node_t *current = node; current != nullptr; current = current->next) {
        if (current->key == key) {
            return current;
        }
    }
    return nullptr;
}

TKV void TMAP::add_node(hash_t hval, const TKey &key, const TValue &value) {
    Node_t *node = new Node_t(key, value);
    Node_t *bucket = _buckets[hval % _bucket_count];
    if (bucket == nullptr) {
        _buckets[hval % _bucket_count] = node;
    } else {
        Node_t *current = bucket;
        for (; current->next != nullptr; current = current->next)
            ;
        current->next = node;
    }
    _item_count++;
}
