#ifndef _BITS_BST
#define _BITS_BST

#include "bits_bst_node.hpp"
#include "bits_bst_iterator.hpp"



#include <iostream>
#include <utility>
#include <memory>
#include <iterator>
#include <vector>


/**
 * Header with members and methods for an implementation of a 
 * Binary Search Tree.
 * It contains a unique pointer to the head of the tree and
 * an instance of OP type.
 * 
 * @tparam k_t template for the key type
 * @tparam v_t template for the value type
 * @tparam OP template for the total order relation that rules the bst; default is std::less<k_t>
 */





template <typename k_t, typename v_t, typename OP = std::less<k_t> >
class bst{


    using node = _node<k_t,v_t>;
    using iterator = _iterator<k_t,k_t,v_t>;
    using const_iterator = _iterator<const k_t,k_t,v_t>;

    /**
     * @brief Private variables
     */
    std::unique_ptr<node> head;
    OP cmp; 
    
    /**
     * This private function will be usefull to define
     * begin
     * @return iterator to the node with the smalles key (wrt OP)
     */
    iterator left_most() noexcept {                            
        auto tmp = head.get();                      
        while(tmp->left){                           
            tmp = tmp->left.get();
        }
        return iterator{tmp};
    }


    /**
     * This private function will be usefull to define
     * cbegin
     * @return const_iterator to the node with the smalles key (wrt OP)
     */
    const_iterator left_most() const noexcept {
        auto tmp = head.get();
        while(tmp->left){
            tmp = tmp->left.get();
        }
        return const_iterator{tmp};
    }

    /**
     * This private function implements the insertion of a node
     * and it works with both l and r value references.
     * The bool is true if a new node has been allocated,
     *  false otherwise (i.e., the key was already present in the tree)
     * 
     * @param x pair 
     * @return pair of an iterator (pointing to the node) and a bool
     */
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

    /**
     * @brief Auxiliary function to balance the tree.
     * 
     * 
     * @param v an std::vector of pairs
     * @param start first index
     * @param end last index
     * @param parent pointer to the parent node
     * @return pointer to the node with the median element 
     */

    template <typename P>
    node* balancing(std::vector<P> v, int start, int end, node* parent){
        // Base Case 
        if (start > end) 
        return nullptr; 
  
        // Get the median element and make a node out of it (head)
        int median = (start + end)/2; 
        node* tmp = new node{v[median]}; 
        tmp->parent = parent;


        // Recursively construct the left subtree 
        // and make it left child  
        tmp->left.reset(balancing(v, start, median - 1, tmp)); 
  
        // Recursively construct the right subtree 
        // and make it right child 
        tmp->right.reset(balancing(v, median + 1, end, tmp)); 
  
        return tmp; 
    }

    /**
     * @brief Auxiliary function 
     */
    bool _is_empty() const noexcept {return head == nullptr;}

public: 
    
    /**
     * @brief Default ctor
     * 
     */ 
    bst() noexcept = default;

    /**
     * @brief Default dtor
     * 
     */
    ~bst() noexcept = default;



    /**
     * @brief Move ctor 
     */
    bst(bst&& x) noexcept: head{std::move(x.head)}, cmp{std::move(x.cmp)} {}
    
    /**
     * @brief Move assignment
     */
    bst& operator=(bst&& x) noexcept{
        head = std::move(x.head);
        cmp = std::move(x.cmp);
        return *this;
    }
    

    
    /**
     * @brief Copy ctor
     *  
     */
    bst(const bst& x){
        if(x.head){    
            cmp = x.cmp;                 
            head.reset(new node{x.head, x.head->parent});   // as far as x is not an empty bst I copy it by
        }                                                   // calling recursively the node ctor
    }


    /**
     * @brief Copy assignment
     */
    bst& operator=(const bst& x){
        auto tmp{x};                 // construct a copy of x
        *(this) = std::move(tmp);    // move it into myself
        return *this;                // return myself
    }

    /**
     * @brief Retrieve the left most node.
     * Auxiliary function left_most is used
     * 
     * @return iterator to the left most node
     */
    iterator begin(){return left_most();}

    /**
     * @brief Retrieve the left most node.
     * Auxiliary function left_most is used
     * 
     * @return const_iterator to the left most node
     */
    const_iterator begin() const {return left_most();}

    /**
     * @brief Retrieve the left most node.
     * Auxiliary function left_most is used
     * 
     * @return const_iterator to the left most node
     */
    const_iterator cbegin() const {return left_most();}
    
    /**
     * @return iterator to one past the last node
     */
    iterator end(){return iterator{nullptr};}

    /**
     * @return const_iterator to one past the last node
     */
    const_iterator end() const {return const_iterator{nullptr};}

    /**
     * @return const_iterator to one past the last node
     */
    const_iterator cend() const {return const_iterator{nullptr};}


    /**
     * @brief Find a given key. If the key is present, returns an iterator to the proper node, end() otherwise.
     * 
     * @param x key to look for
     * @return iterator to the key or iterator to one past the last node
     */
    iterator find(const k_t& x) noexcept {
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

    /**
     * @brief Find a given key. If the key is present, returns an iterator to the proper node, end() otherwise.
     * 
     * @param x key to look for
     * @return const_iterator to the key or iterator to one past the last node
     */
    const_iterator find(const k_t& x) const noexcept {
        auto tmp{head.get()};
        // we traverse the bst until tmp is nullptr
        while (tmp)     
        {
            if(!cmp(tmp->_pair.first,x) && !cmp(x,tmp->_pair.first)){  // found it
                return const_iterator{tmp};
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

        // notice that since the function is marked with const
        // the version of end() that is called is the one that returns a const_iterator
        return end(); 
    }


    /**
     * @brief It is used to insert a new node.
     * The bool is true if a new node has been allocated,
     * false otherwise (i.e., the key was already present in the tree)
     * 
     * @param x pair to be inserted
     * @return a pair of an iterator (pointing to the node) and a bool
     */
    std::pair<iterator, bool> insert(const std::pair<k_t, v_t>& x)  {return _insert(x);}



    /**
     * @brief It is used to insert a new node.
     * The bool is true if a new node has been allocated,
     * false otherwise (i.e., the key was already present in the tree)
     * 
     * @param x pair to be inserted
     * @return a pair of an iterator (pointing to the node) and a bool
     */
    std::pair<iterator, bool> insert(std::pair<k_t, v_t>&& x) {return _insert(std::move(x));}



    /**
     * @brief Inserts a new element into the container constructed in-place
     *  with the given args if there is no element with the key in the container
     *  
     * @param args both the key and the value of the node to be inserted 
     * @return a pair of an iterator (pointing to the node) and a bool
     */
    template <typename... Types >
    std::pair<iterator,bool> emplace(Types&&... args){                      // emplace just needs to construct
        return _insert(std::pair<k_t,v_t>{std::forward<Types>(args)...});   // an std::pair and call insert
    }

    /**
     * @brief Clear the content of the tree
     * 
     */
    void clear() noexcept {head.reset();}

    
    
    /**
     * This method balances the tree.
     * At first it traverse the bst inorder and stores all the pairs in a vector v;
     * then clear the tree and insert the nodes
     * starting from the median of v and again recursively on the left and right 
     * subvector of v
     * 
     * 
     */
    void balance(){
        std::vector<std::pair<k_t,v_t>> ordered;

        // fill ordered with inorder elements of the bst
        for(const_iterator i = cbegin(); i != cend() ; ++i){
            ordered.emplace_back(*i, i.value());
        }
        
        // clear the bst (*this) 
        clear();
        // rebuild the bst
        head.reset(balancing(ordered, 0, ordered.size()-1, nullptr));
    }


    


    /**
     * @brief Removes the element (if one exists) with the key equivalent to key.
     * 
     * @param x the key of the node to be deleted
     */
    void erase(const k_t& x) noexcept {
    
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

    /**
     * @brief Overload of operator put to 
     */
    friend
    std::ostream& operator<<(std::ostream& os, const bst& x){
        if(x._is_empty()){os << "WARNING: empty tree"; return os;}

        for(auto& key : x){
            os << key << " ";
        }
        os << std::endl;
        return os;
    }


    /**
     * Overload of subscripting operator
     * Returns a reference to the value that is mapped
     * to a key equivalent to x, performing an insertion if such key does not already exist
     * 
     * @param x l-value reference to the key
     * @return reference to the value of the key
     */
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
    

    /**
     * Overload of subscripting operator
     * Returns a reference to the value that is mapped
     * to a key equivalent to x, performing an insertion if such key does not already exist
     * 
     * @param x r-value reference to the key
     * @return reference to the value of the key
     */
    v_t& operator[](k_t&& x) {
        auto tmp{std::move(x)};
        if(find(tmp) != iterator{nullptr}){   // if the key is already present
            auto address = find(tmp);         // we return the associated value
            return address.value();
        }
        // otherwise we insert a new node with the
        // requested key and the default value of v_t
        auto def_value = v_t{};
        emplace(tmp,def_value);
        auto address = find(tmp);
        return address.value();
    }

};

#endif