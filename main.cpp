#include <iostream>
#include "BinaryTree.hpp"

// Add a type trait for a custom type 
namespace palkin::details {

template <>
struct value_traits<char> {
    using type = char;
    static constexpr type default_initializer = '1';
};

} // namespace palkin::details

struct UserDefinedPolicy {
    template <typename T>
    static void assign(T& lhs, T&& rhs) {
        lhs = std::forward<T>(rhs);
    }
};

int main() {
    palkin::BinaryTree<char, UserDefinedPolicy>::TreeNode node;
    std::cout << "node.value: " << node.value << '\n';

    palkin::BinaryTree<int> tree; 
}