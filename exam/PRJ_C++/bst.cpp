#include <iostream>
#include <utility>
#include <memory>
#include <iterator>
#include <assert.h>

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in:\nFILE : " << __FILE__ \
                      << "\nLINE : " << __LINE__ << "\nERROR : " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif



template <typename k_t, typename v_t, typename OP = std::less<k_t> >
class bst{

// NODE STRUCT
struct node{
    std::pair<k_t,v_t>    _pair{};
    std::unique_ptr<node> right{};
    std::unique_ptr<node> left{};
    node* parent{nullptr};
    // parent can not be a unique_ptr
    // since it points to a node that is the child
    // of some other node and so we would have a conflict
    // for the purpose of the code a raw_ptr is enough              

    // the only ctors we need take an std::pair<k_t,v_t> as input
    // the ptrs are okay with default value
    explicit node(const std::pair<k_t,v_t>& pair): _pair(pair) {}           // the implicit conversion is not necessary    
    explicit node(std::pair<k_t,v_t>&& pair): _pair(std::move(pair)) {}     // the implicit conversion is not necessary
    explicit node(const std::unique_ptr<node>& x): _pair{x->_pair}{
            // if needed connect it with its parent node
            if(x->parent){
                parent = x->parent;
            }
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
    // default destructor is okay since no mem. acquisition appears 
    // in the ctors
    ~node() = default;
};

// ITERATOR CLASS
template <typename O>
class _iterator{
    using node = bst<k_t,v_t,OP>::node;
    node* current;
    // our iterator is basically a (raw)ptr to node

public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type&;
    using pointer = value_type*;

    explicit _iterator(node* x): current{x} {}  // the implicit conversion is not necessary
    ~_iterator() = default;

    // overloading of pre-increment operator:
    _iterator& operator++(){
        k_t starting_key = **this; // the key of the node the iterator points to before it is increased
        auto cmp = OP{};           // instance of std::less<k_t>(std::greater<k_t>)

        // the iterator must increase such that when we traverse the bst
        // we do in a way such that the keys appear ordered (wrt to OP)

        // the base idea is that the next node in the bst
        // is always on the right of the current one.

        // first case: the current node has a right child:
        // we move into it and then visit all the left child nodes so that thw final
        // will be the node with the smallest key that is also bigger than the key
        // of the node we started from
        if(current->right){
            current = current->right.get();                      // mv into right child
            while(current->left){
                current = current->left.get();                   // traverse left child nodes
            } 
        }
        // second case: the current node has no right child:
        // we visit back the parent node until either we meet
        // a node whose key is bigger or; if the starting node
        // is the right most one we return nullptr
        // (it has no next node so the output iterator is the same as end() output)
        else{
            current = current->parent;                           // mv into parent node
                while(cmp(current->_pair.first,starting_key)){   // repeat untill a bigger key is met
                    if(!(current->parent)){                      // this covers the case in which the starting node is the right most one
                        current = nullptr;
                        break;
                        }
                    current = current->parent;                   // recursively visit back the parent node
                }
        } 
        return *this;
    }

    // overloading of post-increment operator:
    _iterator operator++(int){
        auto tmp{*this};
        ++(*this);
        return tmp;
    }

    pointer operator->() noexcept {return &**this;}
    // once dereferenced the iterator must return the key of the pointed node
    reference operator*() const noexcept {return current->_pair.first;}

    // value returns the value of the key of the pointed node
    v_t& value() {return current->_pair.second;}
    const v_t& value() const {return current->_pair.second;}

    // overload of == and != operators
    friend
    bool operator==(const _iterator& a, const _iterator& b) noexcept {
        return a.current == b.current;   // they must point to the same node
    }
    friend
    bool operator!=(const _iterator& a, const _iterator& b) noexcept {return !(a == b);}
};

    // PRIVATE MEMBERS
    std::unique_ptr<node> head;
    // the bst class just needs a (unique)ptr to the head of the tree

    using iterator = _iterator<k_t>;
    using const_iterator = _iterator<const k_t>;
    iterator left_most(){                           // these private functions will be usefull to define 
        auto tmp = head.get();                      // begin and cbegin. They return an iterator
        while(tmp->left){                           // to the node with the smalles key (wrt OP)
            tmp = tmp->left.get();
        }
        return iterator{tmp};
    }

    const_iterator left_most() const {
        auto tmp = head.get();
        while(tmp->left){
            tmp = tmp->left.get();
        }
        return const_iterator{tmp};
    }

    // this implementation of insert support both r/l value refernces
    // as input
    template <typename O>
    std::pair<iterator, bool> _insert(O&& x){
        auto pair = std::forward<O>(x);
        auto tmp = head.get();
        auto cmp = OP{};
        auto new_node{new node{pair}};

        // base case: empty bst - no check must be performed
        if(!head){
            head.reset(new_node);
            return std::pair<iterator, bool>{iterator{new_node},true};
        }
        
        if (find(pair.first)!=iterator{nullptr}){                      // check if a node with the same key is already present
            return std::pair<iterator, bool>{find(pair.first), false}; // if so return an iterator to that node and flag the insertion as aborted
        }

        // if we didn't return yet
        // we are sure that we will insert a new node
        else{
            while(tmp){                                  // traverse the bst top to bottom until we get to the leaves
                new_node->parent = tmp;                  // at every step take node of the "momentaneous" parent (we don't know when the loop ends)
                if( cmp(tmp->_pair.first, pair.first) ){ // explore the two possible cases for the new key
                    if(!tmp->right.get()){               // if we reach a leaf
                        tmp->right.reset(new_node);      // attach the new node
                        tmp = nullptr;                   // to end the loop
                    }
                    else{tmp = tmp->right.get();}
                }
                else{
                    if(!tmp->left.get()){
                        tmp->left.reset(new_node);
                        tmp = nullptr;
                    }
                    else{tmp = tmp->left.get();}
                }
            }
        }
        return std::pair<iterator, bool>{iterator{new_node}, true};
    }

public: // BST INTERFACE
    
    // default ctor and destructor are okay 
    bst() = default;
    ~bst() = default;

    // mv sem:
    //  mv ctor:
    bst(bst&& x) noexcept: head{std::move(x.head)} {}
    //  mv assignment
    bst& operator==(bst&& x) noexcept{
        head = std::move(x.head);
        return *this;
    }
    
    // cp sem:
    //  cp ctor
    bst(const bst& x){
        if(x.head){                     
            head.reset(new node{x.head});   // as fare as x is not an empty bst I copy it by
        }                                   // calling recursively the node ctor on line 24
    }
    //  cp assignment
    bst& operator=(const bst& x){
        auto tmp{x};                 // construct a copy of x
        *(this) = std::move(tmp);    // move it into myself
        return *this;                // return myself
    }

    // retrieve the iterator to the first element of the bst. left_most is defined on line 133
    iterator begin(){return left_most();}
    const_iterator begin() const {return left_most();}
    const_iterator cbegin() const {return left_most();}
    // the end iterator is just a one pointing to nullptr
    iterator end(){return iterator{nullptr};}
    const_iterator end() const {return const_iterator{nullptr};}
    const_iterator cend() const {return const_iterator{nullptr};}


    iterator find(const k_t& x){
        auto tmp{head.get()};
        // we traverse the bst until tmp is nullptr
        while (tmp)     
        {
            if(tmp->_pair.first == x){  // found it
                return iterator{tmp};
            }
            else{
                // otherwise move to the right or left child
                auto cmp = OP{};
                if( cmp(tmp->_pair.first,x) ){
                    tmp = tmp->right.get();
                }
                else{ tmp = tmp->left.get();}
            }
        }
        // if the while loop did not return means that x is not in the bst
        return end(); 
    }
    const_iterator find(const k_t& x) const {
        auto tmp{head.get()};
        // we traverse the bst until tmp is nullptr
        while (tmp)     
        {
            if(tmp->_pair.first == x){  // found it
                return iterator{tmp};
            }
            else{
                // otherwise move to the right or left child
                auto cmp = OP{};
                if( cmp(tmp->_pair.first,x) ){
                    tmp = tmp->right.get();
                }
                else{ tmp = tmp->left.get();}
            }
        }
        // if the while loop did not return means that x is not in the bst
        return end(); 
    }

    // both functions calls private member function _insert(O&& x) defined on line 152
    std::pair<iterator, bool> insert(const std::pair<k_t, v_t>& x)  {return _insert(x);}
    std::pair<iterator, bool> insert(std::pair<const k_t, v_t>&& x) {return _insert(std::move(x));}

    template <typename... Types >
    std::pair<iterator,bool> emplace(Types&&... args){                      // emplace just needs to construct
        return insert(std::pair<k_t,v_t>{std::forward<Types>(args)...});    // an std::pair and call insert
    }

    // CLEAR
    void clear(){head.reset();}

    bool _is_empty()const noexcept {return head == nullptr;}

    // ERASE
    void erase(const k_t& x){
    
        if(find(x)!=iterator{nullptr}){         // check that the key is present in the bst
            auto cmp = OP{};
            auto tmp = head.get();
            node* starting_node = head.get();                // ptr to the node we must delete

            // look for it
            bool still_looking = true;
            while (still_looking)     
            {
                if(tmp->_pair.first == x){  // found it
                    starting_node = tmp;
                    still_looking = false;
                }
                else{
                    // otherwise move to the right or left child
                    if( cmp(tmp->_pair.first,x) ){
                        tmp = tmp->right.get();
                    }
                    else{ tmp = tmp->left.get();}
                }
            }

            // starting_node is now pointing to
            // the node we want to delete; let's cover all the 
            // possible cases

            // CASE ONE: THE NODE IS A LEAF
            if(!starting_node->left && !starting_node->right){  // check the node has no children
                auto key_of_parent = starting_node->parent->_pair.first;
                auto parent = starting_node->parent;
                // we release and reset the pointer of the parent node
                if(cmp(x,key_of_parent)){parent->left.release();parent->left.reset();} // the node to delete is a left child
                else{parent->right.release();parent->right.reset();}                   // the node to delete is a right child   
            }


            // CASE TWO: THE NODE HAS JUST ONE CHILD
            if((starting_node->left.get()==nullptr && starting_node->right.get()!=nullptr) || (starting_node->left.get()!=nullptr && starting_node->right.get()==nullptr))          // check the node has only one child
            {
                if(starting_node->left){                                // if the child is on the left
                    auto left_child = starting_node->left.release();    // ptr to unique child
                    starting_node->left.reset();
                    auto parent = starting_node->parent;                // ptr to parent node

                    auto key_of_parent = parent->_pair.first;
                    if(cmp(x,key_of_parent)){parent->left.release();parent->left.reset(left_child); left_child->parent = parent;}  // if the starting node is on the left of its parent
                    else{parent->right.release();parent->right.reset(left_child); left_child->parent = parent;}                     // otherwise on the right
                }
                else{                                                   // if the child is on the right
                    auto right_child = starting_node->right.release();  // ptr to unique child
                    auto parent = starting_node->parent;                // ptr to parent node

                    auto key_of_parent = parent->_pair.first;
                    if(cmp(x,key_of_parent)){parent->left.release();parent->left.reset(right_child); right_child->parent = parent;} // if the starting node is on the left of its parent
                    else{parent->right.release();parent->right.reset(right_child); right_child->parent = parent;}                    // otherwise on the right
                }
            }

            // LAST CASE: THE NODE HAS TWO CHILDREN
            if(starting_node->left && starting_node->right){
                auto swap_node = starting_node->right.get();
                while(swap_node->left){                           
                    swap_node = swap_node->left.get();
                }
                auto parent = starting_node->parent;

                if(swap_node->parent->_pair.first == x){starting_node->right.release();}
                else{swap_node->parent->left.release();}
                

                if(swap_node->right){
                    swap_node->right->parent = swap_node->parent;
                }
                
                swap_node->parent->left.reset(swap_node->right.release());
                
                if(starting_node->left){
                    starting_node->left->parent = swap_node;
                }
                
            
                auto key_of_parent = parent->_pair.first;
                if(cmp(x,key_of_parent)){// if the starting node is on the left of its parent
                    parent->left.release();
                    parent->left.reset(swap_node);
                    swap_node->parent = parent; 
                    swap_node->right.reset(starting_node->right.release());
                    swap_node->left.reset(starting_node->left.release());
                } 
                else{// otherwise on the right
                    parent->right.release();
                    parent->right.reset(swap_node); 
                    swap_node->parent = parent;
                    swap_node->right.reset(starting_node->right.release());
                    swap_node->left.reset(starting_node->left.release());
                }                    
            }
        }
        // in the end if the key is not present there is no
        // node that must be eliminated
        else{std::cerr << "ERROR: NO NODE WITH KEY = " << x << std::endl;}
    }

    // PUT TO OPERATOR
    friend
    std::ostream& operator<<(std::ostream& os, const bst& x){
        if(x._is_empty()){std::cerr << "ERROR: --EMPTY BST--\n"; return os;}

        for(auto& key : x){
            os << key << " ";
        }
        os << std::endl;
        return os;
    }

    v_t& operator[](const k_t& x) {
        if(find(x) != iterator{nullptr}){   // if the key is already present
            auto address = find(x);         // we return the associated value
            return address.value();
        }
        // otherwise we insert a new node with the
        // requested key and the default value of v_t
        auto def_value = v_t{};
        emplace(x,def_value);
        auto address = find(x);
        return address.value();
    }
    
    
    v_t& operator[](k_t&& x) {
        auto tmp{std::move(x)};
        if(find(tmp) != iterator{nullptr}){   // if the key is already present
            auto address = find(tmp);         // we return the associated value
            return address.value();
        }
        // otherwise we insert a new node with the
        // requested key and the default value of v_t
        auto def_value = v_t{};
        emplace(x,def_value);
        auto address = find(x);
        return address.value();
    }





};






int main(){

    bst<int,int> test;
    //std::cout << test << std::endl;
    test.insert(std::pair<int,int>{8,2});
    test.insert(std::pair<int,int>{3,2});
    test.insert(std::pair<int,int>{1,2});
    test.insert(std::pair<int,int>{6,2});
    test.insert(std::pair<int,int>{4,2});
    test.insert(std::pair<int,int>{7,2});
    test.insert(std::pair<int,int>{10,2});
    test.insert(std::pair<int,int>{13,2});
    test.insert(std::pair<int,int>{14,2});

    std::cout << "test before copy" << std::endl;
    std::cout << test << std::endl;

    test.erase(13);

    std::cout << "\nafter erase\n" << std::endl;
    std::cout << test << std::endl;

    /*
    std::cout << "\ninstanciated a copy of test named cp and using emplace: (2,2) was inserted" << std::endl;
    bst<int,int> cp{test};
    cp.emplace(2,2);

    std::cout << "cp" << std::endl;
    std::cout << cp << std::endl;
    std::cout << "test after insertion in cp" << std::endl;
    std::cout << test << std::endl;
    std::cout << "test[8] = " << test[8] << std::endl;
    std::cout << "test after test[8]" << std::endl;
    std::cout << test << std::endl;
    std::cout << "test[50] = " << test[50] << std::endl;
    std::cout << "test after test[50]" << std::endl;
    std::cout << test << std::endl;

    std::cout << "cp after all" << std::endl;
    std::cout << cp << std::endl;
    */
    


    
    return 0;
}