#include<iostream>
#include <utility>
#include <memory>
#include <assert.h>
//#include "iterator_bst.cpp"
//#include "node.cpp"




template <typename t_k, typename t_v, typename OP = std::less<t_k> >
class bst{

    // NODE STRUCT
    struct node{
        std::pair<const t_k, t_v> _pair;
        std::unique_ptr<node> left;
        std::unique_ptr<node> right;
        std::unique_ptr<node> father;

        node() noexcept = default;
        node(const t_k& key, const t_v& value) noexcept : _pair{key, value}, left{nullptr}, right{nullptr}, father{nullptr} {}
        node(t_k&& key, t_v&& value) noexcept : _pair{std::move(key),std::move(value)}, left{nullptr}, right{nullptr}, father{nullptr}  {}
        node(const std::pair<t_k,t_v>& pair): _pair{pair}, left{nullptr}, right{nullptr}, father{nullptr}  {}
        node(std::pair<t_k,t_v>&& pair): _pair{std::move(pair)}, left{nullptr}, right{nullptr}, father{nullptr}  {}
        ~node() noexcept = default;
        // define a ctor to be used in the cp ctor of bst
        explicit node(const std::unique_ptr<node>& x): _pair{x->_pair}{
            // take care of left and right children:
            // on the right:
            if(x->right){
                right.reset(new node{x->right});
            }
            else{right.reset();}
            // on the left:
            if(x->left){
                left.reset(new node{x->left});
            }
            else{left.reset();}
            // the idea is to call recursively the ctor (of node) on every side untill we meet a nullptr
        }

        friend
        std::ostream& operator<<(std::ostream& os, const node& x){
            os << "(" << x._pair.first << ":" << x._pair.second << ")" << std::endl;
            return os;
        }
    };
    // ####################### END NODE STRUCT DEF

    // CLASS ITERATOR
    template <typename O>
    class _iterator{
        using node = typename bst<t_k,t_v,OP>::node;
        node* current;      // a raw ptr to node is enough as class member

    public:
        using value_type = O;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using reference = value_type&;
        using pointer = value_type*;
    
        // the default ctor should be initializated
        // such that it points to the left most node in the bst
        _iterator() noexcept: current{begin()} {} 
        // I don't want to define an implicit conversion
        // from node* to Iterator
        explicit _iterator(node* x) noexcept: current{x} {}      // evaluate to put a const infront of node* x
        explicit _iterator(node&& x) noexcept: current{std::move(x)} {}
        // default destructor is ok
        ~_iterator() noexcept = default;

        _iterator& operator++() {
        // here I must think how to move
        // from the current node to the next.
        // what does next means in this case?

        // in general the lesson example for L.L. was:
        // current = curent->next.get()
        // return *this
        return *this;
        }

        // overloading of + operator
        // to be later used in the subscripting iperator of bst
        _iterator& operator+(const std::size_t i) {
            for(std::size_t j{0}; j < i; j++){
                *(this)++;
            }
            return +(this);
        }

        pointer operator->() noexcept{return &**this;}
        reference operator*() const noexcept{return current->_pair.first;} // we are interested in the key 
                                                                       // dereferencing iterator must 
                                                                       // give me the key
                                                                    
        friend
        bool operator==(const _iterator& a, const _iterator& b) noexcept {
            return a.current == b.current;   // two nodes are equal when they point to the same object
        }
        friend
        bool operator!=(const _iterator& a, const _iterator& b) noexcept {return !(a == b);}
    };
    // ####################### END ITERATOR CLASS DEF

    // PRIVATE MEMBERS OF BST
    // the bst needs a node and a ptr to the head
    std::size_t num_of_nodes{0};
    node _node;
    std::unique_ptr<node> head;
    _iterator<const t_k> it{cbegin()};


    using iterator = _iterator<t_k>;
    using const_iterator = _iterator<const t_k>;
    iterator begin() noexcept {return iterator{left_most()};}
    const_iterator begin() const noexcept {return const_iterator{left_most()};}
    const_iterator cbegin() const noexcept {return const_iterator{left_most()};}

    iterator end() noexcept {return iterator{nullptr};}
    const_iterator end() const noexcept{return const_iterator{nullptr};}
    const_iterator cend() const noexcept{return const_iterator{nullptr};}

    // auxiliary function to retrieve the node with minimum (wrt OP) key
    node* left_most() {
        auto tmp = head.get();
        while(tmp){
            tmp = tmp->left;
        }
        return tmp;
    }

    template <typename O>
    std::pair<iterator, bool> _insert(O&& x){
        bool inserted{false};
        // if the node is already present we return <nullptr, false>
        if (find(std::forward<O>(x.first))) {return std::pair<iterator, bool>{nullptr,inserted};} 
        
        // we traverse the bst starting from the head
        auto tmp{head.get()};
        while(inserted == false) // at this point we are sure that a new node wll be inserted so we stop when we succeed
        {
            // at every iteration of the while cicle we 
            // visit a level of the bst until we reach the leaves

            if ( OP(std::forward<O>(x.first),tmp->_pair.first) ){          // key_to_be_inserted < key_in_bst
                if(tmp->left == nullptr){                                  // if we are at a leaf (wrt insertion)
                    tmp->left = new node{std::forward<O>(x.first)};        // append a new node
                    inserted = true;                                       // notify we are done
                    num_of_nodes += 1;                                     // update
                    return std::pair<iterator, bool>{tmp->left,inserted};  // return
                }
                tmp = tmp->left;                                       // otherwise we just move to the left
            }
            else{                                                           // key_to_be_inserted > key_in_bst
                if(tmp->right == nullptr){                                  // if we are at a leaf (wrt insertion)
                    tmp->right = new node{std::forward<O>(x.first)};        // append a new node
                    inserted = true;                                        // notify we are done
                    num_of_nodes += 1;                                      // update
                    return std::pair<iterator, bool>{tmp->right,inserted};  // return
                }
                tmp = tmp->right;
            }
        }   
    }

public:

    // custom ctors and destructor
    bst() noexcept = default;
    bst(const node& node): _node{node} {}
    bst(node&& node): _node{std::move(node)} {}
    ~bst() noexcept = default;

    // mv sem:
    // mv ctor
    bst(bst&& x) noexcept: _node{std::move(x._node)}, head{std::move(x.head)}, num_of_nodes{std::move(x.num_of_nodes)} {}
    // mv assignment
    bst& operator==(bst&& x) noexcept{
        num_of_nodes = std::move(x.num_of_nodes);
        head = std::move(x.head);
        _node = std::move(x._node);
        return *this;
    }
    
    // xp sem:
    // cp ctor
    bst(const bst& x){
        num_of_nodes = x.num_of_nodes;
        if(x.head){                     
            head.reset(new node{x.head});   // as fare as x is not an empty bst I copy it by
        }                                   // calling recursively the node ctor
    }
    // cp assignment
    bst& operator=(const bst& x){
        auto tmp{x};                 // construct a copy of x
        *(this) = std::move(tmp);    // move it into myself
        return *this;                // return myself
    }

    void clear() {head.reset();}

    iterator find(const t_k& x){
        auto tmp{head.get()};
        // we traverse the bst untill tmp is nullptr
        while (tmp)     
        {
            if(tmp->_pair.first == x){  // found it
                return iterator{tmp};
            }
            // otherwise move to the right or left child
            if(tmp->_pair.first < x){
                tmp = tmp->right;
            }
            else{ tmp = tmp->left;}
        }
        // if the while loop did not return means that x is not in the bst
        return iterator{nullptr};
        
    }
    // same strategy as above but the returned iterator is constructed as a const one
    const_iterator find(const t_k& x) const {
        auto tmp{head.get()};
        // we traverse the bst untill tmp is nullptr
        while (tmp)     
        {
            if(tmp->_pair.first == x){  // found it
                return const_iterator{tmp};
            }
            // otherwise move to the right or left child
            if(tmp->_pair.first < x){
                tmp = tmp->right;
            }
            else{ tmp = tmp->left;}
        }
        // if the while loop did not return means that x is not in the bst
        return const_iterator{nullptr};
    }

    
    // both the versions of insert
    // call the private membe function _insert 
    // that exploits forwarding refernces
    // such that we wrote one function for two purposes
    std::pair<iterator, bool> insert(const std::pair<const t_k,t_v>& x) {return _insert(x);}
    std::pair<iterator, bool> insert(std::pair<const t_k,t_v>&& x)      {return _insert(x);}

    // subscripting operator
    t_k& operator[](const std::size_t i) noexcept {assert(i < num_of_nodes); return *(it+i);}
    const t_k& operator[](const std::size_t i) const noexcept {assert(i < num_of_nodes); return *(it+i);} 
    // since this is only for reading I can prepend the const keyword

    // put to operator
    friend
    std::ostream& operator<<(std::ostream& os, const bst& x){
        for(const auto& key : x){
            os << key;
        }
        os << std::endl;
        return os;
    }
};




int main(){

    return 0;
}