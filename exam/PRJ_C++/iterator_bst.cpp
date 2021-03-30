#include <iterator>


template <typename t_k, typename t_v, typename OP = std::less<t_k> >
template <typename O>
class bst<t_k,t_v,OP>::iterator{
    using node = typename bst<t_k,t_v,OP>::node;
    node* current;      // a raw ptr to node is enough as class member

public:
    using value_type = t_k;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type&;
    using pointer = value_type*;
    
    //ctor: I don't want to define an implicit conversion
    // from node* to Iterator
    explicit iterator(node* x) noexcept: current{x} {}
    // default destructor is ok
    ~iterator() noexcept = default;

    iterator& operator++() {
        // here I must think how to move
        // from the current node to the next.
        // what does next means in this case?

        // in general the lesson example for L.L. was:
        // current = curent->next.get()
        // return *this
        return *this;
    }

    pointer operator->() noexcept{return &**this;}
    reference operator*() const noexcept{return current->_pair.first;} // we are interested in the key 

    friend
    bool operator==(const Iterator& a, const Iterator& b){
        return a.current == b.current;   // two nodes are equal when they point to the same object
    }
    bool operator!=(const Iterator& a, const Iterator& b){return !(a==b);}
};
