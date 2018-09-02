#ifndef LLRBT_HPP_INCLUDED
#define LLRBT_HPP_INCLUDED

#include <cstdio>

template<typename K, typename V>
struct RBNode {
    RBNode* parent;
    RBNode* left;
    RBNode* right;
    bool is_red;
    K key;
    V val;
    RBNode(const K& _key, const V& _val, RBNode* _parent):
        parent(_parent), left(nullptr), right(nullptr),
        is_red(true), key(_key), val(_val) {}
    SetKeyValue(RBNode* node) {
        this->key = node->key;
        this->val = node->val;
    }
};
template<typename K, typename V>
static inline RBNode<K, V>*& next_node(RBNode<K, V>* node, const K& key) {
    return node->key > key? node->left : node->right;
}

template<typename K, typename V>
static inline bool is_red(RBNode<K, V>* node) {
    return nullptr != node && node->is_red;
}

template<typename K, typename V>
static inline RBNode<K, V>* left_rotate(RBNode<K, V>* node) {
    auto x = node;
    auto y = node->right;
    auto z = node->right->left;
    auto p = x->parent;

    x->right = z;
    if (z) z->parent = x;

    y->left = x;
    y->is_red = x->is_red;
    x->is_red = true;
    x->parent = y;
    y->parent = p;

    if (p) (p->left == x? p->left : p->right) = y;
    return y;
}

template<typename K, typename V>
inline static RBNode<K, V>* right_rotate(RBNode<K, V>* node) {
    auto x = node;
    auto y = node->left;
    auto z = node->left->right;
    auto p = x->parent;

    x->left = z;
    if (z) z->parent = x;

    y->right = x;
    y->is_red = x->is_red;
    x->is_red = true;
    x->parent = y;
    y->parent = p;

    if (p) (p->left == x? p->left : p->right) = y;
    return y;
}

template<typename K, typename V>
inline static RBNode<K, V>* flip_color(RBNode<K, V>* node) {
    node->left->is_red = !node->left->is_red;
    node->right->is_red = !node->right->is_red;
    node->is_red = !node->is_red;
    return node;
}

template<typename K, typename V>
inline static RBNode<K, V>* fix_up(RBNode<K, V>* node) {
    if (is_red(node->right)) node = left_rotate(node);
    if (is_red(node->left) && is_red(node->left->left)) node = right_rotate(node);
    if (is_red(node->left) && is_red(node->right)) flip_color(node);
    return node;
}

template<typename K, typename V>
inline static RBNode<K, V>* move_red_left(RBNode<K, V>* node) {
    flip_color(node);
    if (node->right && is_red(node->right->left)) {
        node->right = right_rotate(node->right);
        node = left_rotate(node);
        flip_color(node);
    }
    return node;
}

template<typename K, typename V>
inline static RBNode<K, V>* move_red_right(RBNode<K, V>* node) {
    flip_color(node);
    if (node->left && is_red(node->left->left)) {
        node = right_rotate(node);
        flip_color(node);
    }
    return node;
}

template<typename K, typename V>
inline static RBNode<K, V>* find_min(RBNode<K, V>* node) {
    if (nullptr == node) return nullptr;
    while (node->left) node = node->left;
    return node;
}

template<typename K, typename V>
inline static RBNode<K, V>* find_max(RBNode<K, V>* node) {
    if (nullptr == node) return nullptr;
    while (node->right) node = node->right;
    return node;
}

template<typename K, typename V>
inline static RBNode<K, V>* delete_min(RBNode<K, V>* node) {
    if (nullptr == node) return nullptr;
    if (nullptr == node->left && nullptr == node->right) {
        if (node->parent) next_node(node->parent, node->key) = nullptr;
        delete node;
        return nullptr;
    }
    auto tmp_root = node->parent;
    while (node->left) {
        if (!is_red(node->left) && !is_red(node->left->left))
            node = move_red_left(node);
        node = node->left;
    }
    auto p = node->parent;
    K key = node->key;

    delete node;
    if (p) {
        next_node(p, key) = nullptr;
        while (p->parent != tmp_root) p = fix_up(p)->parent;
        return fix_up(p);
    } else
        return nullptr;
}
template<typename K, typename V>
class LLRBTree {
public:
    typedef RBNode<K, V> NodeType;

    void Insert(const K& key, const V& val) {
        if (nullptr == root_) {
            root_ = fix_up(new NodeType(key, val, nullptr));
        } else {
            NodeType *node = root_;
            while (next_node(node, key)) node = next_node(node, key); // up-down
            next_node(node, key) = new NodeType(key, val, node);
            node = next_node(node, key);
            while (node->parent) node = fix_up(node)->parent; // bottom-up
            root_ = fix_up(node);
        }
        root_->is_red = false;
    }

    NodeType* Find(const K& key) const {
        NodeType* node = root_;
        while(node && node->key != key) node = next_node(node, key);
        return node && node->key == key? node : nullptr;;
    }

    NodeType* FindMin() const {
        return find_min(root_);
    }

    NodeType* FindMax() const {
        return find_max(root_);
    }

    void DeleteMin() {
        root_ = delete_min(root_);
        if (root_) root_->is_red = false;
    }

    V& operator[] (const K& key) {
        auto iter = Find(key);
        if (iter) return iter->second;
        else return Insert(key, V{})->second;
    }

    void Remove(const K& key) {
        if (nullptr == Find(key)) return;
        if (nullptr == root_->left && nullptr == root_->right) {
            delete root_;
            root_ = nullptr;
            return;
        }
        NodeType* node = root_;
        NodeType* p = node->parent;
        while (true) {
            if (node->key > key) {
                if (node->left &&
                    (!is_red(node->left) && !is_red(node->left->left)))
                    node = move_red_left(node);
            } else {
                if (is_red(node->left))
                    node = right_rotate(node);
                if (node->key == key && !node->right) {
                    if (node->left) p = right_rotate(node);
                    next_node(p, key) = nullptr;
                    delete node;
                    node = p;
                    break;
                }
                if (node->right &&
                    !is_red(node->right) &&!is_red(node->right->left))
                    node = move_red_right(node);
                if (node->key == key) {
                    node->SetKeyValue(find_min(node->right));
                    node->right = delete_min(node->right);
                    break;
                }
            }
            p = node;
            node = next_node(node, key);
        }
        while (node->parent) node = fix_up(node)->parent; // bottom-up
        if (node) {
            root_ = fix_up(node);
            root_->is_red = false;
        } else
            root_ = nullptr;
    }

    ~LLRBTree() {
        if (nullptr == root_) return;
        auto head = root_;
        auto tail = head;
        head->parent = nullptr;
        while (head) {
            if (head->left) {
                tail->parent = head->left;
                tail = head->left;
                tail->parent = nullptr;
            }
            if (head->right) {
                tail->parent = head->right;
                tail = head->right;
                tail->parent = nullptr;
            }
            auto tmp_node = head;
            head = head->parent;
            delete tmp_node;
        }
    }
private:
    NodeType* root_ = nullptr;
};

#endif // LLRBT_HPP_INCLUDED
