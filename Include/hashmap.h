#include "hashmap_iter.h"
#include "hash.h"
#include <cstddef>
#include <functional>

template <typename TKey, typename TValue> struct Node {
    TKey key;
    TValue data;
    Node<TKey, TValue>* next;
};

template <typename TKey, typename TValue> class Hashmap {
public:
    /// @brief default constructor
    Hashmap();

    /// @brief copy constructor
    /// @param other map to copy from
    Hashmap(const Hashmap &other);

    /// @brief move constructor
    /// @param other map to move from
    Hashmap(Hashmap &&other);
    
    /// @brief constructs a hashmap from begin to end
    /// @tparam ForwardIterator a forward iterator
    /// @param begin beginning of the iteration
    /// @param end end of the iteration
    template <class ForwardIterator>
    Hashmap(ForwardIterator begin, ForwardIterator end);

    ~Hashmap();

private:

};

#include "hashmap.inc"
