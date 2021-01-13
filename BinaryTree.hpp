#include <stack>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace palkin {

namespace details {

// clang-format off
template <typename T, typename = std::void_t<>>
struct value_traits;

template <> struct value_traits<void>;
template <> struct value_traits<const void>;
template <> struct value_traits<volatile void>;
template <> struct value_traits<const volatile void>;

template <> struct value_traits<std::nullptr_t>;
template <> struct value_traits<const std::nullptr_t>;
template <> struct value_traits<volatile std::nullptr_t>;
template <> struct value_traits<const volatile std::nullptr_t>;

template <typename T>
struct value_traits<T, std::void_t<decltype(std::is_fundamental_v<T>)>> {
    using type = std::decay_t<T>;
    static constexpr type default_initializer = 0; 
};

struct assign_policy {
    template <typename T>
    static void assign(T& lhs, T&& rhs) {
        lhs = std::forward<T>(rhs);
    }
};

#define DEFINE_HAS_OPERATOR(NAME, OPERATION) \
        template <typename T, typename U, typename = std::void_t<>> \
        struct has_operator_##NAME : std::false_type {}; \
        \
        template <typename T, typename U> \
        struct has_operator_##NAME<T, U, \
            std::void_t<decltype(std::declval<T>() OPERATION std::declval<U>())>> : std::true_type {}; \
        \
        template <typename T, typename U = T> \
        static constexpr auto has_operator_##NAME ##_v = has_operator_##NAME<T, U>::value;

DEFINE_HAS_OPERATOR(less_than, <);
DEFINE_HAS_OPERATOR(equal_to, ==);

// clang-format on

} // namespace details

template <typename T,
          typename Policy = details::assign_policy, 
          typename Traits = details::value_traits<T>>
class BinaryTree {
public:
    static_assert(details::has_operator_less_than_v<T> &&
                  details::has_operator_equal_to_v<T> &&
                  std::is_move_constructible_v<T>);

    struct TreeNode {
        explicit constexpr TreeNode(T val = Traits::default_initializer) 
            noexcept(std::is_nothrow_move_constructible_v<T>) : value(std::move(val)) {}
        typename Traits::type value;
        TreeNode* left{nullptr};
        TreeNode* right{nullptr};
    };

    void Insert(const T& val) {}

    void Erase(const T& val) {}

    std::optional<TreeNode> Find(const T& value) const {}

private:
    TreeNode* Traverse(const T& value) const {
        if (root_->value == value) return root_;
        
        std::stack<TreeNode*> nodes;
        const auto extract_childen_nodes = [&nodes](TreeNode* node, const T& value) {
            if (node) {
                if (value < node->value && node->left) nodes.push(node->left);
                else if (node->right) nodes.push(node->right);
            }
        };

        auto curr = root_;
        extract_childen_nodes(curr, value);

        while (!nodes.empty()) {
            if (curr->value == value) return curr;
            
            curr = curr->top();
            curr->pop();

            extract_childen_nodes(curr, value);
        }

        return nullptr;
    }

private:
    TreeNode* root_{nullptr};
};

} // namespace palkin