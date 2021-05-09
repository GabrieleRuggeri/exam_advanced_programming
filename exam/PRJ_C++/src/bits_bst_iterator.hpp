#ifndef _BITS_BST_ITERATOR_
#define _BITS_BST_ITERATOR_

#include <iterator>
#include <utility>
#include <memory>

#include "bits_bst_node.hpp"

/**
 * Header for class iterator. 
 * It is a subclass of the bst.
 * It contains a raw pointer to a node
 * 
 * @tparam O template fot the iterator
 * @tparam k_t template for the key type of node
 * @tparam v_t template for the value type of node
 */

// ITERATOR CLASS
template <typename O, typename k_t, typename v_t>
class _iterator{
    
    using node = _node<k_t,v_t>;

    /**
     * @brief pointer to the node
     * 
     */
    node* current;              // iterator is basically a (raw)ptr to node


public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type&;
    using pointer = value_type*;


    /**
     * @brief Default ctor
     * 
     */
    _iterator() noexcept = default;

    /**
     * @brief Custom ctor, no implicit conversion
     * 
     * @param x pointer to a node
     * @return an iterator pointing to x
     */
    explicit _iterator(node* x) noexcept: current{x} {}

    /**
     * @brief Defaul dtor
     * 
     */
    ~_iterator() noexcept = default;

    /**
     * @brief Overloading of the preincrement operator
     * 
     * @return an iterator pointing to the next node in the bst, wrt the inorder relation  
     */
    _iterator& operator++() noexcept{          

        /**
         * the iterator must increase such that when we traverse the bst
         * we do in a way in which the keys appear ordered (wrt to OP)

         * the base idea is that the next node in the bst
         * is always on the right of the current one.

         * first case: the current node has a right child:
         * we move into it and then visit all the left child nodes so that the final one
         * will be the node with the smallest key in the sub-tree
         * and that is also bigger than the key of the node we started from
         */
        if(current->right){
            current = current->right.get();                      // mv into right child
            while(current->left){
                current = current->left.get();                   // traverse left child nodes
            } 
        }
        
        /** second case: the current node has no right child:
             * we visit back the parent node until either we meet
             * a node whose key is bigger or; if the starting node
             * is the right most one we return nullptr
             * (it has no next node so the output iterator is the same as end())
             */
        else{
            
            // otherwise
            auto tmp = current->parent;

            while( tmp && current != tmp->left.get()){
                current = tmp;
                tmp = tmp->parent;
            }

            current = tmp;
        } 

        return *this;
    }

    /**
     * @brief Overloading of post increment operator 
     */
    _iterator operator++(int) noexcept{
        auto tmp{*this};
        ++(*this);
        return tmp;
    }


    /**
     * @brief Overloading of operator ->
     * 
     * @return a pointer to the key of the node the iterator points to 
     */
    pointer operator->() noexcept {return &**this;}
    

    /**
     * @brief Overloading of the dereferencing operator
     * 
     * @return the key of the node the iterator points to 
     */
    reference operator*() const noexcept {return current->_pair.first;}

    /**
     * @return a reference to the value of the key of the pointed node 
     */
    v_t& value() {return current->_pair.second;}

    /**
     * @return a const reference to the value of the key of the pointed node
     */
    const v_t& value() const {return current->_pair.second;}

    /**
     * @return a pointer to the current node 
     */
    node* where() const noexcept{return current;}

    /**
     * @brief Overload of == operator
     * 
     * @param a left hand side of the equality
     * @param b right hand side of the equality 
     */
    friend
    bool operator==(const _iterator& a, const _iterator& b) noexcept {
        return a.current == b.current;   // they must point to the same node
    }

    /**
     * @brief Overloading of the operator !=
     * 
     * @param a left hand side
     * @param b right hand side 
     */
    friend
    bool operator!=(const _iterator& a, const _iterator& b) noexcept {return !(a == b);}
};

#endif