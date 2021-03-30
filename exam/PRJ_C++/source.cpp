#include<iostream>
#include <utility>
#include <memory>
#include "iterator_bst.cpp"




template <typename t_k, typename t_v, typename OP = std::less<t_k> >
class bst{

    //define struct node
    struct node{
        std::pair<const t_k, t_v> _pair;
        std::unique_ptr<node> left;
        std::unique_ptr<node> right;

        node(): _pair{}, left{nullptr}, right{nullptr} {}
        node(const t_k& key, const t_v& value) noexcept : _pair{key, value}, left{nullptr}, right{nullptr} {}
        node(t_k&& key, t_v&& value) noexcept : _pair{std::move(key),std::move(value)}, left{nullptr}, right{nullptr}  {}
        node(const std::pair<t_k,t_v>& pair): _pair{pair}, left{nullptr}, right{nullptr}  {}
        node(std::pair<t_k,t_v>&& pair): _pair{std::move(pair)}, left{nullptr}, right{nullptr}  {}
        ~node() noexcept = default;

        // should I define cp sem.?



        friend
        std::ostream& operator<<(std::ostream& os, const node& x){
            os << "(" << x._pair.first << ":" << x._pair.second << ")" << std::endl;
            return os;
        }
    };

    //the bst must have a node and a ptr to the root
    node _node;
    std::unique_ptr<node> head;

    template <typename O>
    class iterator; // defined in iterator_bst.cpp

    // custom ctors and destructor
    bst() noexcept = default;
    bst(const node& node): _node{node} {}
    bst(node&& node): _node{std::move(node)} {}
    ~bst() noexcept = default;

    // mv sem:
    // mv ctor
    bst(bst&& x) noexcept: _node{std::move(x._node)}, head{std::move(x.head)}
    // mv assignment
    bst& operator==(bst&& x) noexcept{
        head = std::move(x.head);
        _node = std::move(x._node);
        return *this;
    }

    // here I need cp sem.

    iterator<t_k> begin() noexcept {
        // to traverse the bst I must start from the most left
        // node. I can find it by starting from the head
        // and going one on the left side until I meet a nullptr
        // that will be the left most node and the point from which
        // I have to start
        auto tmp = iterator{head.get()};   
        while(tmp.current != nullptr){
            tmp.current = (tmp.current)->left;
        }
        return tmp;    
    }

    iterator<const t_k> begin() const noexcept {
        return static_cast<iterator<const t_k>(begin());   
    }



    



};




int main(){

    return 0;
}