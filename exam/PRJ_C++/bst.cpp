#include <iostream>
#include <utility>
#include <memory>
#include <iterator>
#include <vector>




// NODE STRUCT
template <typename k_t, typename v_t >
struct _node{
    std::pair<k_t,v_t>    _pair;
    std::unique_ptr<_node> right;
    std::unique_ptr<_node> left;
    _node* parent;
    // parent can not be a unique_ptr
    // since it points to a node that is the child
    // of some other node and so we would have a conflict
    // for the purpose of the code a raw_ptr is enough              

    _node() = default;
    // the only ctors we need take an std::pair<k_t,v_t> as input
    explicit _node(const std::pair<k_t,v_t>& pair) noexcept: _pair(pair) {}           // the implicit conversion is not necessary    
    explicit _node(std::pair<k_t,v_t>&& pair) noexcept: _pair(std::move(pair)) {}     // the implicit conversion is not necessary

    // this is a custom ctor that takes as input a unique ptr to a node that must me copied
    // and a raw ptr to what it's supposed to be its parent. It is a recursive
    // ctor that will be later used in the copy semantics of the bst
    explicit _node(const std::unique_ptr<_node>& x, _node* parent)noexcept: _pair{x->_pair}, parent{parent}{
            // take care of left and right children:

            // on the right:
            if(x->right){
                right.reset(new _node{x->right,&*this});     // the children of the node have the node itself as parent
            }
            else{right.reset();}                            // for sake of completeness
            // on the left:
            if(x->left){
                left.reset(new _node{x->left,&*this});
            }
            else{left.reset();}                             // for sake of compliteness

            // the idea is to call recursively the ctor (of node) on every side untill we meet a nullptr
        }

    // default destructor is okay since no mem. acquisition occour
    ~_node() = default;
};



template <typename k_t, typename v_t, typename OP = std::less<k_t> >
class bst{

// ITERATOR CLASS
template <typename O>
class _iterator{
    
    using node = _node<k_t,v_t>;
    node* current;              // iterator is basically a (raw)ptr to node


public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type&;
    using pointer = value_type*;

    _iterator() = default;
    explicit _iterator(node* x) noexcept: current{x} {}  // the implicit conversion is not necessary
    ~_iterator() = default;

    // overloading of pre-increment operator:
    _iterator& operator++() noexcept{          

        // the iterator must increase such that when we traverse the bst
        // we do in a way in which the keys appear ordered (wrt to OP)

        // the base idea is that the next node in the bst
        // is always on the right of the current one.

        // first case: the current node has a right child:
        // we move into it and then visit all the left child nodes so that the final one
        // will be the node with the smallest key in the sub-tree
        // and that is also bigger than the key of the node we started from
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
            // if current is the root node in a bst with no right child
            if(!current->parent){current = nullptr; return *this;}
            
            auto tmp = current->parent;

            while( tmp && current != tmp->left.get()){
                current = tmp;
                tmp = tmp->parent;
            }

            current = tmp;
        } 

        return *this;
    }

    // overloading of post-increment operator:
    _iterator operator++(int) noexcept{
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

    node* where() const noexcept{return current;}

    // overload of == and != operators
    friend
    bool operator==(const _iterator& a, const _iterator& b) noexcept {
        return a.current == b.current;   // they must point to the same node
    }
    friend
    bool operator!=(const _iterator& a, const _iterator& b) noexcept {return !(a == b);}
};

    using node = _node<k_t,v_t>;
    // PRIVATE MEMBERS
    std::unique_ptr<node> head;
    // instance of the total relation order that rules the bst
    OP cmp; 
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
                new_node->parent = tmp;                  // at every step take note of the "momentaneous" parent (we don't know when the loop ends)
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
    bst(bst&& x) noexcept: head{std::move(x.head)}, cmp{std::move(x.cmp)} {}
    //  mv assignment
    bst& operator==(bst&& x) noexcept{
        head = std::move(x.head);
        cmp = std::move(x.cmp);
        return *this;
    }
    
    // cp sem:
    //  cp ctor
    bst(const bst& x) noexcept{
        if(x.head){    
            cmp = x.cmp;                 
            head.reset(new node{x.head, x.head->parent});   // as far as x is not an empty bst I copy it by
        }                                                   // calling recursively the node ctor on line 24
    }
    //  cp assignment
    bst& operator=(const bst& x) noexcept{
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
            if(!cmp(tmp->_pair.first,x) && !cmp(x,tmp->_pair.first)){  // found it  
                return iterator{tmp};
            }
            else{
                // otherwise move to the right or left child
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
            if(!cmp(tmp->_pair.first,x) && !cmp(x,tmp->_pair.first)){  // found it
                return iterator{tmp};
            }
            else{
                // otherwise move to the right or left child
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
    void clear() noexcept {head.reset();}

    // BALANCE
    void balance(){
        std::vector<std::pair<k_t,v_t>> ordered;

        // fill ordered with inorder elements of the bst
        for(iterator i = begin(); i != end() ; ++i){
            ordered.emplace_back(*i, i.value());
        }
        
        // clear the bst (*this) 
        clear();
        // rebuild the bst
        head.reset(balancing(ordered, 0, ordered.size()-1, nullptr));
    }


    node* balancing(std::vector<std::pair<k_t,v_t>> v, int start, int end, node* parent){
        // Base Case 
        if (start > end) 
        return nullptr; 
  
        // Get the median element and make a node out of it (head)
        int median = (start + end)/2; 
        node* tmp = new node(v[median]); 
        tmp->parent = parent;


        // Recursively construct the left subtree 
        // and make it left child  
        tmp->left.reset(balancing(v, start, median - 1, tmp)); 
  
        // Recursively construct the right subtree 
        // and make it right child 
        tmp->right.reset(balancing(v, median + 1, end, tmp)); 
  
        return tmp; 
    }

    bool _is_empty() const noexcept {return head == nullptr;}

    // ERASE
    void erase(const k_t& x){
    
        if(find(x)!=iterator{nullptr}){         // check that the key is present in the bst
            node* starting_node = find(x).where();

            // starting_node is now pointing to
            // the node we want to delete; let's cover all the 
            // possible cases

            // CASE ONE: THE NODE IS A LEAF
            if(!starting_node->left && !starting_node->right){                         // check the node has no children
                auto key_of_parent = starting_node->parent->_pair.first;
                auto parent = starting_node->parent;
                // we release and reset the pointer of the parent node
                if(cmp(x,key_of_parent)){parent->left.release();parent->left.reset();} // the node to delete is a left child
                else{parent->right.release();parent->right.reset();}                   // the node to delete is a right child   
            }


            // CASE TWO: THE NODE HAS JUST ONE CHILD
            if((!starting_node->left && starting_node->right) || (starting_node->left && !starting_node->right))          // check the node has only one child
            {
                if(starting_node->left){                                // if the child is on the left
                    auto left_child = starting_node->left.release();    // ptr to the unique child
                    auto parent = starting_node->parent;                // ptr to parent node

                    auto key_of_parent = parent->_pair.first;
                    if(cmp(x,key_of_parent)){parent->left.release();parent->left.reset(left_child); left_child->parent = parent;}  // if the starting node is on the left of its parent
                    else{parent->right.release();parent->right.reset(left_child); left_child->parent = parent;}                    // otherwise on the right
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
                // find the right most node between nodes
                // that are on the right of the node we must delete
                // and exchange key and value; then delete the old node

                // we move to the right and then iterate over the left children
                auto swap_node = starting_node->right.get();
                while(swap_node->left){                           
                    swap_node = swap_node->left.get();
                }
                // exchange keys and values (swap_node is no needed anymore)
                starting_node->_pair = swap_node->_pair;

                // since swap_node will now be eliminated we will
                // reset the ptr that points to it but we have to consider
                // two cases:
                // swap_node is not the right child of the deleted node,
                // in this case 
                if(swap_node->parent != starting_node){
                    auto tmp = swap_node->right.release(); // swap_node may have a right subtree
                    swap_node->parent->left.reset(tmp);    // attach the subtree to the parent
                }
                // otherwise we must
                else{
                    swap_node->right->parent = starting_node;
                    auto tmp = swap_node->right.release();
                    starting_node->right.reset(tmp);

                }
            }
        }

        else{
            std::cerr << "ERROR: no element has key = " << x << std::endl;
        }
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

    try{

    bst<int,int> test;
    std::cout << "bst after creation" << std::endl;
    std::cout << test << std::endl;

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

    test.erase(8);

    std::cout << "\nafter erase 8 \n" << std::endl;
    std::cout << test << std::endl;

    test.erase(10);

    std::cout << "\nafter erase 10 \n" << std::endl;
    std::cout << test << std::endl;

    


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
    std::cout << "test[3] = " << test[3] << std::endl;
    std::cout << "test after test[3]" << std::endl;
    std::cout << test << std::endl;

    std::cout << "cp after all" << std::endl;
    std::cout << cp << std::endl;


    test.clear();
    std::cout << "after clear() on test" << std::endl;
    std::cout << "test\n" << test << "\ncp\n" << cp << std::endl;

    cp.balance();
    std::cout << "after balancing cp" << std::endl;
    std::cout << cp << std::endl;
    std::cout << "test\n" << test << std::endl;
    std::cout << "end" << std::endl;

    }
    catch(std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}