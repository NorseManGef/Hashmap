#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>

#pragma once

// #ifdef CLANGD_ONLY
#include "hashmap.h"
// #endif

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

TKV TMAP::Hashmap(int count) : _bucket_count(count), _item_count(0) {
    _buckets = new Node_t *[_bucket_count];
    for (int i = 0; i < _bucket_count; ++i) {
        _buckets[i] = nullptr;
    }
}

TKV TMAP::Hashmap(const Hashmap &other)
    : _bucket_count(other._bucket_count), _item_count(0), _buckets(nullptr) {
    copy_from(other._buckets, _bucket_count);
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
    Node_t *node = get_node(hval, key);
    if (node != nullptr) {
        return false;
    }
    add_node(hval, key, value);
    return true;
}

TKV void TMAP::put(const TKey &key, const TValue &value) {
    hash_t hval = hash(key);
    Node_t *node = get_node(hval, key);
    if (node == nullptr) {
        add_node(hval, key, value);
    } else {
        node->data = value;
    }
}

TKV TValue &TMAP::get(const TKey &key) {
    hash_t hval = hash(key);
    Node_t *node = get_node(hval, key);
    if (node == nullptr) {
        throw key_not_found("No node found for key");
    }
    return node->data;
}

TKV const TValue &TMAP::get(const TKey &key) const {
    hash_t hval = hash(key);
    Node_t *node = get_node(hval, key);
    if (node == nullptr) {
        throw key_not_found("No node found for key");
    }
    return node->data;
}

TKV TValue TMAP::remove(const TKey &key) {
    hash_t hval = hash(key);
    Node_t *bucket = _buckets[hval % _bucket_count];

    if (bucket != nullptr) {
        Node_t *current = bucket;
        Node_t *parent = bucket;

        if (current->key == key) {
            TValue val = current->data;
            _buckets[hval % _bucket_count] = current->next;
            delete current;
            _item_count--;
            return val;
        }

        while (current != nullptr) {
            current = current->next;
            if (current->key == key) {
                TValue val = current->data;
                parent->next = current->next;
                delete current;
                _item_count--;
                return val;
            }
            parent = parent->next;
        }
    }
    throw key_not_found("No node found for key");
}

TKV bool TMAP::contains(const TKey &key) const {
    return get_node(hash(key), key) != nullptr;
}

TKV size_t TMAP::size() const { return _item_count; }

TKV void TMAP::clear() {
    for (int i = 0; i < _bucket_count; ++i) {
        Node_t *current = _buckets[i];
        Node_t *parent = _buckets[i];
        while (current != nullptr) {
            current = current->next;
            delete parent;
            parent = current;
        }
        _buckets[i] = nullptr;
    }
    _item_count = 0;
}

TKV Hashmap<TKey, TValue> &TMAP::operator=(const Hashmap<TKey, TValue> &map) {
    if (this != &map) {
        copy_from(map._buckets, map._item_count);
    }
    return *this;
}

TKV Hashmap<TKey, TValue> &TMAP::operator=(Hashmap<TKey, TValue> &&map) {
    if (this != &map) {
        if (_buckets != nullptr) {
            clear();
            delete[] _buckets;
        }
        _buckets = map._buckets;
        map._buckets = nullptr;
        _item_count = map._item_count;
        _bucket_count = map._bucket_count;
    }
    return *this;
}

TKV Hashmap<TKey, TValue>
TMAP::operator+(const Hashmap<TKey, TValue> &other) const {
    Hashmap<TKey, TValue> newMap = (other._bucket_count > _bucket_count)
                                       ? Hashmap(other._bucket_count)
                                       : Hashmap(_bucket_count);
    for (int i = 0; i < _bucket_count; ++i) {
        for (Node_t *current = _buckets[i]; current != nullptr;
             current = current->next) {
            newMap.add(current->key, current->data);
        }
    }
    for (int i = 0; i < other._bucket_count; ++i) {
        for (Node_t *current = other._buckets[i]; current != nullptr;
             current = current->next) {
            newMap.add(current->key, current->data);
        }
    }
    return newMap;
}

TKV Hashmap<TKey, TValue> &
TMAP::operator+=(const Hashmap<TKey, TValue> &other) {
    for (int i = 0; i < other._bucket_count; ++i) {
        for (Node_t *current = other._buckets[i]; current != nullptr;
             current = current->next) {
            this->add(current->key, current->data);
        }
    }
    return *this;
}

TKV bool TMAP::operator==(const Hashmap<TKey, TValue> &other) const {
    if (_item_count != other._item_count) {
        return false;
    }
    for (int i = 0; i < _bucket_count; ++i) {
        for (Node_t *current = _buckets[i]; current != nullptr;
             current = current->next) {
            const Node_t *other_node =
                other.get_node(hash(current->key), current->key);
            if (other_node == nullptr || other_node->data != current->data) {
                return false;
            }
        }
    }
    return true;
}

TKV bool TMAP::operator!=(const Hashmap<TKey, TValue> &other) const {
    if (_item_count != other._item_count) {
        return true;
    }
    for (int i = 0; i < _bucket_count; ++i) {
        for (Node_t *current = _buckets[i]; current != nullptr;
             current = current->next) {
            const Node_t *other_node =
                other.get_node(hash(current->key), current->key);
            if (other_node == nullptr || other_node->data != current->data) {
                return true;
            }
        }
    }
    return false;
}

TKV std::ostream &operator<<(std::ostream &out,
                             const Hashmap<TKey, TValue> &map) {
    out << "{ ";
    for (int i = 0; i < map._bucket_count; ++i) {
        for (Node<TKey, TValue> *current = map._buckets[i]; current != nullptr;
             current = current->next) {
            out << "(" << current->key << ", " << current->data << ") ";
        }
    }
    out << "}";
    return out;
}

TKV Node<TKey, TValue> *TMAP::get_node(hash_t hval, const TKey &key) {
    Node_t *node = _buckets[hval % _bucket_count];
    for (Node_t *current = node; current != nullptr; current = current->next) {
        if (current->key == key) {
            return current;
        }
    }
    return nullptr;
}

TKV const Node<TKey, TValue> *TMAP::get_node(hash_t hval,
                                             const TKey &key) const {
    Node_t *node = _buckets[hval % _bucket_count];
    for (Node_t *current = node; current != nullptr; current = current->next) {
        if (current->key == key) {
            return current;
        }
    }
    return nullptr;
}

TKV void TMAP::add_node(hash_t hval, const TKey &key, const TValue &value) {
    add_node(hval, key, value, _buckets, _bucket_count);
}

TKV void TMAP::add_node(hash_t hval, const TKey &key, const TValue &value,
                        Node_t **target_buckets, size_t target_count) {
    Node_t *node = new Node_t(key, value);
    Node_t *bucket = target_buckets[hval % target_count];
    if (bucket == nullptr) {
        target_buckets[hval % target_count] = node;
    } else {
        Node_t *current = bucket;
        for (; current->next != nullptr; current = current->next) {
        }
        current->next = node;
    }
    _item_count++;
}

TKV void TMAP::copy_from(Node_t **source, size_t size) {
    if (_buckets != nullptr) {
        clear();
        delete[] _buckets;
        _item_count = 0;
    }

    _buckets = new Node_t *[size];
    for (int i = 0; i < size; ++i) {
        Node_t *source_current = source[i];

        if (source_current != nullptr) {
            _buckets[i] = new Node_t(source_current->key, source_current->data);
            _item_count++;
            Node_t *dest_current = _buckets[i];
            source_current = source_current->next;

            while (source_current != nullptr) {
                dest_current->next =
                    new Node_t(source_current->key, source_current->data);
                _item_count++;
                dest_current = dest_current->next;
                source_current = source_current->next;
            }
            dest_current->next = nullptr;
        } else {
            _buckets[i] = nullptr;
        }
    }
    _bucket_count = size;
}

TKV void TMAP::resize() {
    Node_t **new_buckets = new Node_t *[_bucket_count * 2];
    for (int i = 0; i < _bucket_count * 2; ++i) {
        new_buckets[i] = nullptr;
    }
    _item_count = 0;
    for (Node_t **bucket = _buckets; bucket < _buckets + _bucket_count;
         ++bucket) {
        for (Node_t *current = *bucket; current != nullptr;
             current = current->next) {
            hash_t hval = hash(current->key);
            add_node(hval, current->key, current->data, new_buckets,
                     _bucket_count * 2);
        }
    }
    size_t temp_count = _item_count;
    clear();
    _item_count = temp_count;
    delete[] _buckets;
    _buckets = new_buckets;
    _bucket_count = _bucket_count * 2;
}
