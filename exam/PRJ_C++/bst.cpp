#include <iostream>
#include <memory>
#include <iterator>


template <typename k_t, typename v_t, typename OP = std::less<k_t> >
class bst{

// NODE STRUCT
struct node{
    std::pair<k_t,v_t> _pair{};
    std::unique_ptr<node> right{};
    std::unique_ptr<node> left{};
    node* parent{nullptr};

    node(const std::pair<k_t,v_t>& pair): _pair(pair) {}
    node(std::pair<k_t,v_t>&& pair): _pair(std::move(pair)) {}
    ~node() = default;
};

// ITERATOR CLASS
template <typename O>
class _iterator{
    using node = bst<k_t,v_t,OP>::node;
    node* current;

public:
    using value_type = O;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;
    using reference = value_type&;
    using pointer = value_type*;

    explicit _iterator(node* x): current{x} {}  // the implicit conversion is not necessary
    ~_iterator() = default;

    _iterator& operator++(){
        k_t starting_key = **this;
        auto cmp = OP{};

        if(current->right){
            current = current->right.get();
            while(current->left){
                current = current->left.get();
            }
            //std::cout << "\nright != nullptr\n" << std::endl;
            //return *this; 
        }
        else{
            current = current->parent;
                while(cmp(current->_pair.first,starting_key)){
                    if(!(current->parent)){
                        current = nullptr;
                        break;
                        }
                    current = current->parent;  // before only this line wa present
                }
        } 
        return *this;
    }

    _iterator operator++(int){
        auto tmp{*this};
        ++(*this);
        return tmp;
    }

    pointer operator->() noexcept {return &**this;}
    reference operator*() const noexcept {return current->_pair.first;}

    v_t& value() {return current->_pair.second;}
    const v_t& value() const {return current->_pair.second;}


    friend
    bool operator==(const _iterator& a, const _iterator& b) noexcept {
        return a.current == b.current;   // they must point to the same node
    }
    friend
    bool operator!=(const _iterator& a, const _iterator& b) noexcept {return !(a == b);}
};

    // PRIVATE MEMBERS
    std::unique_ptr<node> head;

    using iterator = _iterator<k_t>;
    using const_iterator = _iterator<const k_t>;
    iterator left_most(){
        auto tmp = head.get();
        while(tmp->left){
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

    iterator right_most(){
        auto tmp = head.get();
        while(tmp->right){
            tmp = tmp->right.get();
        }
        return iterator{tmp};
    }

    const_iterator right_most() const {
        auto tmp = head.get();
        while(tmp->right){
            tmp = tmp->right.get();
        }
        return const_iterator{tmp};
    }

    template <typename O>
    std::pair<iterator, bool> _insert(O&& x){
        auto pair = std::forward<O>(x);
        auto tmp = head.get();
        auto cmp = OP{};
        auto new_node{new node{pair}};

        // base case
        if(!head){
            head.reset(new_node);
            return std::pair<iterator, bool>{iterator{new_node},true};
        }
        
        if (find(pair.first)!=iterator{nullptr}){                      // check is a node with the same key is already present
            return std::pair<iterator, bool>{find(pair.first), false}; // if so return an iterator to that node
        }
        // if we didn't return yet
        // we are sure that we will insert a new node
        else{
            while(tmp){
                new_node->parent = tmp;
                if( cmp(tmp->_pair.first, pair.first) ){
                    if(!tmp->right.get()){
                        tmp->right.reset(new_node);
                        tmp = nullptr;
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

public:
    bst() = default;
    ~bst() = default;

    
    iterator begin(){return left_most();}
    const_iterator begin() const {return left_most();}
    const_iterator cbegin() const {return left_most();}

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

    // both functions calls private member function: -- pair_type _insert(O&& x) --
    std::pair<iterator, bool> insert(const std::pair<k_t, v_t>& x)  {return _insert(x);}
    std::pair<iterator, bool> insert(std::pair<const k_t, v_t>&& x) {return _insert(std::move(x));}

    template <typename... Types >
    std::pair<iterator,bool> emplace(Types&&... args){
        return insert(std::pair<k_t,v_t>{std::forward<Types>(args)...});
    }


    // CLEAR
    void clear(){head.reset();}

    // PUT TO OPERATOR
    friend
    std::ostream& operator<<(std::ostream& os, const bst& x){
        for(auto& key : x){
            os << key << " ";
        }
        os << std::endl;
        return os;
    }

    v_t& operator[](const k_t& x) {
        //assert(find(x) != iterator{nullptr} && "This key is not present");
        auto address = find(x);
        return address.value();
    }
    
    
    v_t& operator[](k_t&& x) {
        auto tmp{std::move(x)};
        //assert(find(tmp)!= iterator{nullptr} && "This key is not present");
        auto address = find(tmp);
        return address.value();
    }





};






int main(){

    bst<int,int> test;
    test.insert(std::pair<int,int>{8,2});
    test.insert(std::pair<int,int>{3,2});
    test.insert(std::pair<int,int>{1,2});
    test.insert(std::pair<int,int>{6,2});
    test.insert(std::pair<int,int>{4,2});
    test.insert(std::pair<int,int>{7,2});
    test.insert(std::pair<int,int>{10,2});
    test.insert(std::pair<int,int>{13,2});
    test.insert(std::pair<int,int>{14,2});
    

    

    std::cout << test << std::endl;
    

    
    return 0;
}