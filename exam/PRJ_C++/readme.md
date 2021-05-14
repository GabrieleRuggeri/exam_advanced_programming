# Binary Search Tree
### Gabriele Ruggeri

The code provided implements a templated **binary search tree** and two auxiliary classes: **node** and **iterator**. The source files are in the `src` while the main file is `main.cpp`, in which some tests are performed to ensure the correctness of the code. To compile it's enough to use the `make` command.
For a more detailed documentation check the Doxygen generated files (pdf and html). 

## Implementation Details
The binary search tree stores pairs of `key` and `value`, respectively templated on two different types, and the pairs are ordered with respect to a total order relation templated on `OP`.

## Auxiliary classes:

### Node
To store the pairs a new struct was defined: `node`
### members
- a `pair` with key and value
- a pointer to the `right child`
- a pointer to the `left child`
- a pointer to the `parent node`

Moreover I defined both the default constructor and destructor and two custom constructors; the first one for creating a node given a pair while the second one is an auxiliary constructor that is invoked in the copy semamtics of the bst. 

### Iterator
A simple implementation of a `forward iterator`, defined as class, to traverse the tree inorder.
### private members
- a pointer to a node
### public interface
- default constructor and destructor
- custom constructor that takes a pointer to node and creates an iterator pointing to that node
- pre-increment operator 
- post-increment operator 
- referencing operator 
- dereferencing operator 
- value: returns the value of the key of the pointed node 
- where: returns the current position in the tree
- comparison operators 

## BST class
The main class implementing the bst has the following structure:
### private members
- a pointer to the root of the tree
- an instance of the comparison operator (`OP` type)

- `left_most`: auxiliary funtion to retrieve the left most node in the tree
- `_insert`: auxiliary function to implement, through forwarding references, the insertion of a node.
- `balancing`: auxiliary function invoked in `balance`
- `_is_empty`: auxiliary function to check whether the tree is empty
### public members
- default constructor and desctructor
- copy and move semantics
- `(c)begin`: return an (const)interator to the left most node
- `(c)end`: return an (const)interator to one past the last node 
- `find`: given a key it returns, if present, an iterator to the node with the key; `end()` otherwise. Starting from the root we traverse top-bottom the tree comparing the keys; if they are equal we return an iterator to the current node otherwise, if the key we are looking for is smaller than the current one we move to the left; if greater we move to the right. The procedure goes on until either we find the key or we get to a leaf node, meaning that the key of interest is not in the tree.

- `insert`: given a pair it inserts a new node and returns an iterator to the newly inserted node and a bool to check whether the insertion has been performed (`False` if the key of the node was already present). After checking if the tree is empty and if the key is already present we can then procede by finding the place where the node must be inserted and placing it there.
- `emplace`: given a key and a value it creates a pair out of them and inserts a new node, following the same idea of `insert`
- `clear`: clears the content of the tree
- `balance`: it balances the tree in place. After storing the pairs (sorted by key) in a vector, we clear the bst and recursively insert the median of the (sub)vector until all the nodes have been inserted again
- `erase`: given a key, if present, it erases the corresponding node. We distinguished three cases:
  - the node is a leaf: we simply delete it
  - the node has just one (left)right child: we delete it after connecting its parent to the (left)right child
  - the node has two children: we swap the node with the left most node in the right subtree and then delete such node, that is now a leaf
- `operator put to` print the keys by reading the tree inorder
- `subscripting operator` given a key, if it is present in the tree it returns the corresponding value, otherwise a new node with the key and the default value is inserted
























 

