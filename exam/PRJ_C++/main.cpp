#include "bst.hpp"
#include <iostream>


int main(){



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

    std::cout << "test" << std::endl;
    std::cout << test << std::endl;


    bst<int,int> cp_ctor{test};
    std::cout << "\nbst build by cp ctor\n"
              << cp_ctor << std::endl;

    bst<int,int> cp_as = test;
    std::cout << "\nbst build by cp as\n"
              << cp_as << std::endl;

    bst<int,int> mv_ctor{std::move(cp_ctor)};
    std::cout << "\nbst build by mv ctor\n"
              << mv_ctor << std::endl;

    bst<int,int> mv_as = std::move(cp_as);
    std::cout << "\nbst build by mv as\n"
              << mv_as << std::endl;


    test.erase(8);

    std::cout << "\nafter erase 8 \n" << std::endl;
    std::cout << test << std::endl;

    test.erase(10);

    std::cout << "\nafter erase 10 \n" << std::endl;
    std::cout << test << std::endl;

    


    std::cout << "\ninstanciated a copy of test named cp and using emplace (2,2) was inserted" << std::endl;
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

    
    return 0;
}