#ifndef _BITS_BST_NODE_
#define _BITS_BST_NODE_

#include <utility>
#include <memory>

/**
 * Header for struct node, it contains a pair,
 * a unique pointer to each of its child node and
 * a raw pointer to the parent node
 * 
 * @tparam k_t template for the key type
 * @tparam v_t template for the value type
 */




template <typename k_t, typename v_t >
struct _node{

    /**
     * @brief Pair of key and value
     * 
     */
    std::pair<k_t,v_t>    _pair;

    /**
     * @brief Pointer to right child
     * 
     */
    std::unique_ptr<_node> right;

    /**
     * @brief Pointer to left child
     * 
     */
    std::unique_ptr<_node> left;

    /**
     * @brief Raw pointer to the parent node
     * 
     */
    _node* parent;
             

    /**
     * @brief Default ctor
     */
    _node() noexcept = default;
    
    /**
     * @brief Curstom ctor, no implicit conversion
     * 
     * @param pair an l-value std::pair<k_t,v_t>
     * @return a node containing the input pair
     */
    explicit _node(const std::pair<k_t,v_t>& pair) noexcept: _pair(pair) {}    

    /**
     * @brief Curstom ctor, no implicit conversion
     * 
     * @param pair an r-value std::pair<k_t,v_t>
     * @return a node containing the input pair
     */      
    explicit _node(std::pair<k_t,v_t>&& pair) noexcept: _pair(std::move(pair)) {}     

    /**
     * Custom ctor that takes as input a unique ptr to a node that must me copied
     * and a raw ptr to what it's supposed to be its parent. It is a recursive
     * ctor that will be later used in the copy semantics of the bst
     * 
     * @param x Unique pointer to the node to copy from
     * @param parent Raw pointer to the parent node
     */
    explicit _node(const std::unique_ptr<_node>& x, _node* parent)noexcept: _pair{x->_pair}, parent{parent}{
            // take care of left and right children:

            // on the right:
            if(x->right){
                right.reset(new _node{x->right,&*this});    // the children of the node have the node itself as parent
            }
            else{right.reset();}                            // for sake of completeness
            // on the left:
            if(x->left){
                left.reset(new _node{x->left,&*this});
            }
            else{left.reset();}                             // for sake of completeness

            // the idea is to call recursively the ctor (of node) on every side untill we meet a nullptr
        }

    /**
     * @brief Default dtor
     * 
     */
    ~_node() noexcept = default;
};

#endif