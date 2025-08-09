#include "hashmap_iter.h"
#include "hash.h"
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <iostream>

const size_t DEFAULT_HASHMAP_BUCKET_COUNT = 16;

template <typename TKey, typename TValue> class Hashmap;

struct key_not_found: public std::logic_error {
    key_not_found(const char* message):
        std::logic_error(message)
    {}

    key_not_found():
        std::logic_error("key not found") 
    {}
};

template <typename TKey, typename TValue>
std::ostream& operator<<(std::ostream& out, const Hashmap<TKey, TValue>& map);

template <typename TKey, typename TValue> struct Node {
    TKey key;
    TValue data;
    Node<TKey, TValue>* next;
    
    Node(const TKey& key, const TValue& data);
};

template <typename TKey, typename TValue> class Hashmap {
    using Node_t = Node<TKey, TValue>;
public:
    /// @brief default constructor
    Hashmap();

    /// @brief copy constructor
    /// @param other map to copy from
    Hashmap(const Hashmap &other);

    /// @brief move constructor
    /// @param other map to move from
    Hashmap(Hashmap &&other);
    
    ~Hashmap();

    /// @brief attempts to add a new item, fails if the item already exists
    /// @return bool if the operation succeeded
    /// @param key the key of the item to be added
    /// @param value the value of the item to be added
    bool add(const TKey& key, const TValue& value);

    /// @brief adds a new item to the list, overwrites any item of the same key
    /// @param key the key of the item to be added
    /// @param value the value of the item to be added
    void put(const TKey& key, const TValue& value);

    /// @brief removes the item at the key
    /// @throws key_not_found if the key was not found
    /// @return TValue the value of the item that was removed
    /// @param key the key of the item to be removed
    TValue remove(const TKey& key);

    /// @brief checks if there is an item with that key
    /// @returns bool if the key exists, return true; else false
    /// @param key the key of the item to check
    bool contains(const TKey& key) const;

    /// @brief gets the value attatched to the key
    /// @returns TValue& the value which was attatched to the key
    /// @param key the key of the item we want to get
    /// @throws key_not_found if the key was not found
    TValue& get(const TKey& key);

    /// @brief gets the value attatched to the key
    /// @returns const TValue& the value which was attatched to the key
    /// @param key the key of the item we want to get
    /// @throws key_not_found if the key was not found
    const TValue& get(const TKey& key) const;
    
    /// @brief returns the number of items in the map
    /// @returns size_t the number of items in the map
    size_t size() const;

    /// @brief removes all data in the map
    void clear();

    Hashmap<TKey, TValue>& operator=(const Hashmap<TKey, TValue>& map); // copy operator

    Hashmap<TKey, TValue>& operator=(Hashmap<TKey, TValue>&& map); // move operator
    
    /// @brief makes a new map, formed by combining two others
    /// @returns Hashmap<TKey, TValue>& the new map to be created
    /// @param other the map to add to the current map
    /// @remarks if the two maps contain items with the same keys, the conflicting items of the right map will be ignored.
    Hashmap<TKey, TValue>& operator+(const Hashmap<TKey, TValue>& other) const;

    /// @brief modifies this map by adding the items of another map
    /// @returns Hashmap<TKey, TValue>& a reference to this map
    /// @param other the map to add to the current map
    /// @remarks if the two maps contain items with the same keys, the conflicting items of the right map will be ignored.
    Hashmap<TKey, TValue>& operator+=(const Hashmap<TKey, TValue>& other);

    bool operator==(const Hashmap<TKey, TValue>& other) const;

    bool operator!=(const Hashmap<TKey, TValue>& other) const;

    friend std::ostream& operator<< <>(std::ostream& out, const Hashmap<TKey, TValue>& map);

private:
    Node_t** _buckets;
    size_t _bucket_count;
    size_t _item_count;

    void copy_from(Node_t** source, Node_t** destination, size_t size);
    Node_t* get_node(hash_t hval, const TKey& key);
    void add_node(hash_t hval, const TKey& key, const TValue& value);
};

#include "hashmap.inc"
