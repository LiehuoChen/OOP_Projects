#include <memory>
#include <functional>
#include <iostream>
#include <iterator>
#include <malloc.h>
#include <stdexcept>

using namespace std;
namespace cs540 {
	typedef	bool	__rb_tree_color_type;
	const __rb_tree_color_type	__rb_tree_red = false;
	const __rb_tree_color_type	__rb_tree_black = true;

	struct __rb_tree_node_base {
		typedef __rb_tree_color_type	color_type;
		typedef __rb_tree_node_base*    base_ptr;

		color_type	color;
		base_ptr	parent;
		base_ptr	left;
		base_ptr	right;

		static base_ptr minimum(base_ptr x) {
			while (x->left != 0)
				x = x->left;
			return x;
		}

		static base_ptr maximum(base_ptr x)
		{
			while (x->right != 0)
				x = x->right;
			return x;
		}
	};

	inline void 
	__rb_tree_rotate_left(__rb_tree_node_base *x,
			__rb_tree_node_base*& root)
	{
		__rb_tree_node_base *y = x->right;
		x->right = y->left;
		if (y->left !=0 )
			y->left->parent = x;
		y->parent = x->parent;

		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}

	inline void
	__rb_tree_rotate_right(__rb_tree_node_base *x,
			__rb_tree_node_base*& root)
	{
		__rb_tree_node_base *y = x->left;
		x->left = y->right;
		if (y->right != 0)
			y->right->parent = x;
		y->parent = x->parent;

		if (x == root)
			root = y;
		else if (x == x->parent->right)
			x->parent->right = y;
		else
			x->parent->left = y;
		y->right = x;
		x->parent = y;
	}


	template <class Value>
	struct __rb_tree_node : public __rb_tree_node_base
	{
		typedef __rb_tree_node<Value> *link_type;
		Value value_field;
	};


	struct __rb_tree_base_iterator
	{
		typedef __rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;
		typedef	ptrdiff_t	difference_type;
		base_ptr	node;

		void increment()
		{
			if (node->right != 0) {	// if right node exists
				node = node->right;
				while (node->left != 0)
					node = node->left;
			} else { // there is no right node
				base_ptr y = node->parent;
				while (node == y->right) {
					node = y;
					y = y->parent;
				}
				if (node->right != y)
					node = y;
			}
		}

		void decrement() 
		{
			if (node->color == __rb_tree_red && node->parent->parent == node) {
				node = node->right;
            }
			else if (node->left != 0) {
				base_ptr y = node->left;
				while (y->right != 0)
					y = y->right;
				node = y;
			} else {
				base_ptr y = node->parent;
				while (node == y->left) {
					node = y;
					y = y->parent;
				}
				node = y;
			}
		}
	};

	template <class Value, class Ref, class Ptr>
	struct __rb_tree_iterator : public __rb_tree_base_iterator
	{
		typedef Value 	value_type;
		typedef	Ref		reference;
		typedef	Ptr		pointer;
		typedef	__rb_tree_iterator<Value, Value&, Value*> Iterator;
		typedef __rb_tree_iterator<Value, const Value&, const Value*> ConstIterator;
		typedef __rb_tree_iterator<Value, Ref, Ptr> self;
		typedef __rb_tree_node<Value>* link_type;

		__rb_tree_iterator() {}
		__rb_tree_iterator(link_type x) { node = x; }
		__rb_tree_iterator(const Iterator &it) { node = it.node; }

		reference operator* () const { return link_type(node)->value_field; }
        pointer operator->() const { return &(operator*()); }

		self& operator++ () { 
            printf("iterator increment\n");
			increment(); 
			return *this; 
		}

		self operator++ (int) {
			self tmp = *this;
			increment();
			return tmp;
		}

		self &operator-- () {
			decrement();
			return *this;
		}

		self operator-- (int) {
			self tmp = *this;
			decrement();
			return tmp;
		}
	};

    inline bool operator==(const __rb_tree_base_iterator &x,
            const __rb_tree_base_iterator &y) 
    {
        printf("operator== base iterator called\n");
        return x.node == y.node;
    }

    inline bool operator!=(const __rb_tree_base_iterator &x,
            const __rb_tree_base_iterator &y) 
    {
        printf("operator!= base iterator called\n");
        printf("x.node %lx : y.node: %lx\n", x.node, y.node);
        //printf("x.node %d : y.node: %d\n", (*x.node).first, (*y.node).first);
        return x.node != y.node;
    }

    template <class T>
    inline void destroy(T *pointer) {
        pointer->~T();
    }

    template <class T1, class T2>
    inline void construct(T1 *p, const T2 &value) {
        new(p) T1(value);
    }

	template <class Key, class Value, class KeyOfValue, class Compare,
			 class Alloc = std::allocator<pair<const Key, Value> > >
	 class rb_tree {
		protected:
			typedef	void *void_pointer;
			typedef	__rb_tree_node_base*    base_ptr;
			typedef __rb_tree_node<Value>	rb_tree_node;
			typedef	__rb_tree_color_type	color_type;
		public:
			typedef	Key 		key_type;
			typedef	Value		value_type;
			typedef	value_type*	pointer;
			typedef	const value_type*	const_pointer;
			typedef	value_type&	reference;
			typedef	const value_type&	const_reference;
			typedef	rb_tree_node*   link_type;
			typedef	size_t		size_type;
			typedef	ptrdiff_t	difference_type;
		protected:
			link_type get_node() {
                return (link_type)malloc(sizeof(rb_tree_node));
			}

			void put_node(link_type p) {
                free((void*)p);
			}

			link_type create_node(const value_type& x) {
				link_type tmp = get_node();
                construct(&tmp->value_field, x);
				return tmp;
			}

			link_type clone_node(link_type x) {
				link_type tmp = create_node(x->value_field);
				tmp->color = x->color;
				tmp->left = 0;
				tmp->right = 0;
				return tmp;
			}

			link_type destroy_node(link_type p) {
				destroy(&p->value_field);
				put_node(p);
			}

		protected:
			size_type	node_count;
			link_type	header;
			Compare		key_compare;

			link_type& root() const { return (link_type&)header->parent; }
			link_type& leftmost() const { return (link_type&)header->left; }
			link_type& rightmost() const { return (link_type&)header->right; }

			static link_type& left(link_type x) {
				return (link_type&)(x->left);
			}
			static link_type& right(link_type x) {
				return (link_type&)(x->right);
			}
			static link_type& parent(link_type x) {
				return (link_type&)(x->parent);
			}
			static reference value(link_type x) {
				return x->value_field;
			}
			static const Key& key(link_type x) {
				return KeyOfValue()(value(x));
			}
			static color_type& color(link_type x) {
				return (color_type&)(x->color);
			}

            static link_type& left(base_ptr x) { return (link_type&)(x->left); }
            static link_type& right(base_ptr x) { return (link_type&)(x->right); }
            static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
            static reference value(base_ptr x) { return ((link_type)x)->value_field; }
            static const Key& key(base_ptr x) { return KeyOfValue()(value(link_type(x))); }
            static color_type& color(base_ptr x) { return (color_type&)(link_type(x)->color); }

			static link_type minimum(link_type x) {
				return (link_type)__rb_tree_node_base::minimum(x);
			}

			static link_type maximum(link_type x) {
				return (link_type)__rb_tree_node_base::maximum(x);
			}

		public:
			typedef __rb_tree_iterator<value_type, reference, pointer> Iterator;
			typedef __rb_tree_iterator<value_type, const_reference, const_pointer> ConstIterator;
			typedef reverse_iterator<Iterator> ReverseIterator;

		private:
			Iterator __insert(base_ptr x, base_ptr y, const value_type &v);
			link_type __copy(link_type x, link_type p);
			void __erase(link_type x);
			void clear() {
                printf("node_count is :%d\n", node_count);
				if (node_count != 0) {
					__erase(root());
					leftmost() = header;
					root() = 0;
					rightmost() = header;
					node_count = 0;
				}
			}

			void init() {
				header = get_node();
				color(header) = __rb_tree_red;
				root() = 0;
				leftmost() = header;
				rightmost() = header;
			}

		public:
            //constructor
			rb_tree(const Compare& comp = Compare())
			 : node_count(0), key_compare(comp) { init(); }

            //destructor
			~rb_tree() {
                printf("rb_tree destructor enter\n");
				clear();
				put_node(header);
                printf("rb_tree destructor out\n");
			}

            //copy constructor
            rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x)
              : node_count(0), key_compare(x.key_compare) {
                  printf("rb_tree copy constructor called\n");
                header = get_node();
                color(header) = __rb_tree_red;
                if (x.root() == 0) {
                  root() = 0;
                  leftmost() = header;
                  rightmost() = header;
                } else {
                  root() = __copy(x.root(), header);
                  leftmost() = minimum(root());
                  rightmost() = maximum(root());
                }
                node_count = x.node_count;
            }

            //move constructor
            rb_tree(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&& x)
                :key_compare(std::move(x.key_compare)) {
                printf("rb_tree move constructor called\n");
                header = x.header;
                node_count = x.node_count;
                x.header = 0;
                x.node_count = 0;
            }

            //copy assignment
			rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
			operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

            //move assignment
            rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
			operator=(rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&& x){
                printf("rb_tree move assignment called\n");
                clear();
                put_node(header);
                printf("after clear called\n");
                header = x.header;
                node_count = x.node_count;
                key_compare = std::move(x.key_compare);
                x.header = 0;
                x.node_count = 0;
            }

		public:
			Compare key_comp() const { return key_compare; }
			Iterator begin() { return leftmost(); }
			ConstIterator begin() const { leftmost(); }
			Iterator end() { return header; }
			ConstIterator end() const { return header; }
			ReverseIterator rbegin() { return ReverseIterator(end()); }
            ReverseIterator rend() { return ReverseIterator(begin()); }
			bool empty() const { return node_count == 0; }
			size_type size() const { return node_count; }
			size_type max_size() const { return size_type(-1); }
		public:
			pair<Iterator, bool> insert_unique(const value_type &x);
            Iterator insert_unique(Iterator position, const value_type &x);
            void insert_unique(const value_type *first, const value_type *last);
			Iterator insert_equal(const value_type &x);
            void erase(Iterator position);
            void remove(const key_type &x);
			Iterator find(const key_type &x);
			ConstIterator find(const key_type &x) const;
	};

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, 
                           const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
        printf("x.size: %d:%d\n", x.size(), y.size());
        printf("y: %d\n", (*(y.begin())).second);
        printf("x: %d\n", (*(x.begin())).first);
        /*
        auto iter2 = y.begin();
        for(auto iter = x.begin(); iter != x.end(); iter++, iter2++) {
            printf("iter->first: %d:%d\n", (*iter), (*iter));
            printf("iter->first: %d:%d\n", iter2->first, iter2->second);
            if ((iter->first == iter2->first) && (iter->second == iter2->second))
                printf("equal\n");
            else
                printf("not equal\n");
            //printf("%d:%d\n", iter->first, iter->second);
        }
        return true;
        */
        return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::
    operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
      if (this != &x) {
          printf("rb_tree assignment called\n");
          if (header) {              // Note that Key may be a constant type.
              clear();
          } else {
              init();
          }
        key_compare = x.key_compare;
        if (x.root() == 0) {
          root() = 0;
          leftmost() = header;
          rightmost() = header;
        }
        else {
          root() = __copy(x.root(), header);
          leftmost() = minimum(root());
          rightmost() = maximum(root());
          node_count = x.node_count;
        }
      }
      return *this;
    }

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::Iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const Value &v)
	{
		link_type y = header;
		link_type x = root();
		while (x != 0) {
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}

		return __insert(x, y, v);
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::Iterator, bool>
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const Value &v)
	{
		link_type y = header;
		link_type x = root();
		bool comp = true;
		while (x != 0) {
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}

		Iterator j = Iterator(y);
		if (comp)
			if (j == begin())
				return pair<Iterator, bool>(__insert(x, y, v), true);
			else
				--j;

		if (key_compare(key(j.node), KeyOfValue()(v)))
			return pair<Iterator, bool>(__insert(x,y,v), true);

		return pair<Iterator, bool>(j, false);
	}


    template <class Key, class Val, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::Iterator 
    rb_tree<Key, Val, KeyOfValue, Compare, Alloc>::insert_unique(Iterator position,
                                                                 const Val& v) 
    { 
            if (position.node == header->left) // begin()
            if (size() > 0 && key_compare(KeyOfValue()(v), key(position.node)))
                return __insert(position.node, position.node, v);
                                    // first argument just needs to be non-null 
            else 
                return insert_unique(v).first;
        else if (position.node == header) // end()
            if (key_compare(key(rightmost()), KeyOfValue()(v)))
                return __insert(0, rightmost(), v);
            else 
                return insert_unique(v).first;
        else {
            Iterator before = position;
            --before;
            if (key_compare(key(before.node), KeyOfValue()(v))
                && key_compare(KeyOfValue()(v), key(position.node)))
                if (right(before.node) == 0)
                    return __insert(0, before.node, v);  
                else 
                    return __insert(position.node, position.node, v);
                                    // first argument just needs to be non-null 
            else 
                return insert_unique(v).first;
        }    
    }

    template <class K, class V, class KeyOfValue, class Compare, class Alloc>
    typename rb_tree<K, V, KeyOfValue, Compare, Alloc>::link_type
    rb_tree<K, V, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) {
                                    // structural copy.  x and p must be non-null.
      link_type top = clone_node(x);
      top->parent = p;
        if (x->right)
          top->right = __copy(right(x), top);
        p = top;
        x = left(x);

        while (x != 0) {
          link_type y = clone_node(x);
          p->left = y;
          y->parent = p;
          if (x->right)
            y->right = __copy(right(x), y);
          p = y;
          x = left(x);
        }
      return top;
    }


	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__erase(link_type x) {
		while (x != 0) {
			__erase(right(x));
			link_type y = left(x);
			destroy_node(x);
			x = y;
		}
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::Iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert
	(base_ptr x_, base_ptr y_, const Value& v) {
		link_type x = (link_type) x_;
		link_type y = (link_type) y_;
		link_type z;

		if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);
			left(y) = z;
			if (y == header) {
				root() = z;
				rightmost() = z;
			} else if (y == leftmost())
				leftmost() = z;
		} else {
			z = create_node(v);
			right(y) = z;
			if (y == rightmost())
				rightmost() = z;
		}
		parent(z) = y;
		left(z) = 0;
		right(z) = 0;

		__rb_tree_rebalance(z, header->parent);
		++node_count;
		return Iterator(z);
	}

	inline void
	__rb_tree_rebalance(__rb_tree_node_base *x, __rb_tree_node_base*& root) {
		x->color = __rb_tree_red;
		while (x != root && x->parent->color == __rb_tree_red) {
			if (x->parent == x->parent->parent->left) {
				__rb_tree_node_base *y = x->parent->parent->right;
				if (y && y->color == __rb_tree_red) {
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				} else {
					if (x == x->parent->right) {
						x = x->parent;
						__rb_tree_rotate_left(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);
				}
			} else {
				__rb_tree_node_base *y = x->parent->parent->left;
				if (y && y->color == __rb_tree_red) {
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				} else {
					if (x == x->parent->left) {
						x = x->parent;
						__rb_tree_rotate_right(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}
		root->color = __rb_tree_black;
	}

    inline __rb_tree_node_base*
    __rb_tree_rebalance_for_erase(__rb_tree_node_base* z,
                                  __rb_tree_node_base*& root,
                                  __rb_tree_node_base*& leftmost,
                                  __rb_tree_node_base*& rightmost)
    {
      __rb_tree_node_base* y = z;
      __rb_tree_node_base* x = 0;
      __rb_tree_node_base* x_parent = 0;
      if (y->left == 0)             // z has at most one non-null child. y == z.
        x = y->right;               // x might be null.
      else
        if (y->right == 0)          // z has exactly one non-null child.  y == z.
          x = y->left;              // x is not null.
        else {                      // z has two non-null children.  Set y to
          y = y->right;             //   z's successor.  x might be null.
          while (y->left != 0)
            y = y->left;
          x = y->right;
        }
      if (y != z) {                 // relink y in place of z.  y is z's successor
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) {
          x_parent = y->parent;
          if (x) x->parent = y->parent;
          y->parent->left = x;      // y must be a left child
          y->right = z->right;
          z->right->parent = y;
        }
        else
          x_parent = y;
        if (root == z)
          root = y;
        else if (z->parent->left == z)
          z->parent->left = y;
        else
          z->parent->right = y;
        y->parent = z->parent;
        ::swap(y->color, z->color);
        y = z;
        // y now points to node to be actually deleted
      }
      else {                        // y == z
        x_parent = y->parent;
        if (x) x->parent = y->parent;
        if (root == z)
          root = x;
        else
          if (z->parent->left == z)
            z->parent->left = x;
          else
            z->parent->right = x;
        if (leftmost == z)
          if (z->right == 0)        // z->left must be null also
            leftmost = z->parent;

        // makes leftmost == header if z == root
          else
            leftmost = __rb_tree_node_base::minimum(x);
        if (rightmost == z)
          if (z->left == 0)         // z->right must be null also
            rightmost = z->parent;
        // makes rightmost == header if z == root
          else                      // x == z->left
            rightmost = __rb_tree_node_base::maximum(x);
      }
      if (y->color != __rb_tree_red) {
        while (x != root && (x == 0 || x->color == __rb_tree_black))
          if (x == x_parent->left) {
            __rb_tree_node_base* w = x_parent->right;
            if (w->color == __rb_tree_red) {
              w->color = __rb_tree_black;
              x_parent->color = __rb_tree_red;
              __rb_tree_rotate_left(x_parent, root);
              w = x_parent->right;
            }
            if ((w->left == 0 || w->left->color == __rb_tree_black) &&
                (w->right == 0 || w->right->color == __rb_tree_black)) {
              w->color = __rb_tree_red;
              x = x_parent;
              x_parent = x_parent->parent;
            } else {
              if (w->right == 0 || w->right->color == __rb_tree_black) {
                if (w->left) w->left->color = __rb_tree_black;
                w->color = __rb_tree_red;
                __rb_tree_rotate_right(w, root);
                w = x_parent->right;
              }
              w->color = x_parent->color;
              x_parent->color = __rb_tree_black;
              if (w->right) w->right->color = __rb_tree_black;
              __rb_tree_rotate_left(x_parent, root);
              break;
            }
          } else {                  // same as above, with right <-> left.
            __rb_tree_node_base* w = x_parent->left;
            if (w->color == __rb_tree_red) {
              w->color = __rb_tree_black;
              x_parent->color = __rb_tree_red;
              __rb_tree_rotate_right(x_parent, root);
              w = x_parent->left;
            }
            if ((w->right == 0 || w->right->color == __rb_tree_black) &&
                (w->left == 0 || w->left->color == __rb_tree_black)) {
              w->color = __rb_tree_red;
              x = x_parent;
              x_parent = x_parent->parent;
            } else {
              if (w->left == 0 || w->left->color == __rb_tree_black) {
                if (w->right) w->right->color = __rb_tree_black;
                w->color = __rb_tree_red;
                __rb_tree_rotate_left(w, root);
                w = x_parent->left;
              }
              w->color = x_parent->color;
              x_parent->color = __rb_tree_black;
              if (w->left) w->left->color = __rb_tree_black;
              __rb_tree_rotate_right(x_parent, root);
              break;
            }
          }
        if (x) x->color = __rb_tree_black;
      }
      return y;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline void 
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(Iterator position) {
      link_type y = (link_type) __rb_tree_rebalance_for_erase(position.node,
                                                              header->parent,
                                                              header->left,
                                                              header->right);
      destroy_node(y);
      --node_count;
    }

    template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
    inline void 
    rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::remove(const Key& k) {
        Iterator it;
        it = find(k);
        if (it != end())
            erase(it);
        else
            throw std::out_of_range("map at key out of range");
    }
 
	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::Iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
		link_type y = header;
		link_type x = root();

		while (x != 0)
			if (!key_compare(key(x), k)) {
				y = x; 
				x = left(x);
			} else {
				x = right(x);
			}
		Iterator j = Iterator(y);
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::ConstIterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) const {
		link_type y = header;
		link_type x = root();

		while (x != 0)
			if (!key_compare(key(x), k)) {
				y = x; 
				x = left(x);
			} else {
				x = right(x);
			}
		ConstIterator j = ConstIterator(y);
		return (j == end() || key_compare(k, key(j.node))) ? end() : j;
	}

	template <class T>
	struct select1st : public unary_function<T, typename T::first_type> {
		const typename T::first_type& operator() (const T &x) const {
			return x.first;
		}
	};

	template <class Key, class T,
			 class Compare = std::less<Key>,
			 class Alloc = std::allocator<pair<const Key, T> > >
 	class Map {
		public:
			typedef Key key_type;
			typedef T	data_type;
			typedef pair<const Key, T> value_type;
			typedef	Compare	key_compare;

			class value_compare
				: public binary_function<value_type, value_type, bool> {
					friend class Map<Key, T, Compare, Alloc>;
					protected:
					Compare comp;
					value_compare(Compare c) : comp(c) {}
					public:
					bool operator()(const value_type& x, const value_type &y) const {
						return comp(x.first, y.first);
					}
				};
		private:
			typedef rb_tree<key_type, value_type, select1st<value_type>, key_compare,
					Alloc> rep_type;
			rep_type	t;
		public:
			typedef typename	rep_type::pointer		pointer;
			typedef typename	rep_type::const_pointer	const_pointer;
			typedef typename	rep_type::reference		reference;
			typedef	typename	rep_type::const_reference const_reference;
			typedef typename	rep_type::Iterator		Iterator;
			typedef	typename	rep_type::ConstIterator	ConstIterator;
			typedef	typename	rep_type::ReverseIterator	ReverseIterator;
			typedef typename	rep_type::size_type		size_type;
			typedef typename	rep_type::difference_type	difference_type;

            //constructor 
			Map() : t(Compare()) {}
			explicit Map(const Compare &comp) : t(comp) {}

            //constructor with initializer list
            Map(std::initializer_list<value_type> s):t(Compare()) {
                for (auto elem = s.begin(); elem != s.end(); ++elem)
                    this->insert(*elem);
                    // or operator[](elem->first) = elem->second;
            }

            //copy constructor 
			Map(const Map<Key, T, Compare, Alloc>& x) : t(x.t) { printf("Map copy constructor called\n");}

            //copy assignment
			Map<Key, T, Compare, Alloc>& operator= (const Map<Key, T, Compare, Alloc> &x) {
                printf("Map copy assignment called\n");
                if (this != &x)
                    t = x.t;
				return *this;
			}

            //move constructor
			Map(Map<Key, T, Compare, Alloc>&& x):t(std::move(x.t)) {
                printf("Map move constructor called\n");
            }

            //copy assignment
			Map<Key, T, Compare, Alloc>& operator= (Map<Key, T, Compare, Alloc> &&x) {
                printf("Map move assignment called\n");
                if (this != &x) {
                    t = std::move(x.t);
                }
				return *this;
			}

			key_compare key_comp() const { return t.key_comp(); }
			value_compare value_compare() const { return value_compare(t.key_comp()); }
			Iterator begin() { return t.begin(); }
			ConstIterator begin() const { return t.begin(); }
			Iterator end() { return t.end(); }
			ConstIterator end() const { return t.end(); }
			ReverseIterator rbegin() { return t.rbegin(); }
			ReverseIterator rend() { return t.rend(); }
			bool empty() const { return t.empty(); }
			size_type size() const { return t.size(); }
			size_type max_size() const { return t.max_size(); }

			T& operator[] (const key_type &k) {
				return (*((insert(value_type(k, T()))).first)).second;
			}

			void swap(Map<Key, T, Compare, Alloc>& x) { t.swap(x.t); }

			pair<Iterator, bool> insert(const value_type& x) {
				return t.insert_unique(x);
			}

            Iterator insert(Iterator position, const value_type &x) {
                return t.insert_unique(position, x);
            }

			void erase(Iterator position) { t.erase(position); }
			void remove(const key_type& x) { t.remove(x); }
			void clear() { t.clear(); }

			Iterator find(const key_type &x) { return t.find(x); }
			ConstIterator find(const key_type &x) const { return t.find(x); }

			T& at(const key_type &x) {
				Iterator it;
				it = find(x);
				if (it == end())
                    throw std::out_of_range("map at key out of range");
                return it->second;
			}

			const T& at(const key_type &x) const {
                printf("const at called\n");
				ConstIterator it;
				it = find(x);
				if (it == end())
                    throw std::out_of_range("map at key out of range");
                return it->second;
			}

            /*
            template <class K, class V, class C, class A>
            bool operator== (const Map<K, V, C, A> &x) {
                auto iter = begin();
                auto iter2 = x.begin();
                for (;iter != end(); iter++, iter2++) {
                    printf ("first: %d:%d\n", iter->first, iter->second);
                    printf ("first2: %d:%d\n", iter2->first, iter2->second);
                }

                return size() == x.size() && equal(begin(), end(), x.begin());
            }
            */

            /*
            template <class K, class V, class C, class A>
            friend bool operator== (const Map<K, V, C, A> &x, 
                                    const Map<K, V, C, A> &y);
                                    */

	        //template <class K, class V, class C, class A>
	
			//friend bool operator!= (const Map&, const Map&);
 	};

    template <class Key, class Value, class Compare, class Alloc>
    inline bool operator==(Map<Key, Value, Compare, Alloc>& x, 
                           Map<Key, Value, Compare, Alloc>& y) {
                auto iter = x.begin();
                auto iter2 = y.begin();
                //printf("first: %d:%d\n", iter->first, iter->second);
                for (iter = x.begin(); iter != x.end(); iter++,iter2++) {
                    if((iter->first == iter2->first) && (iter->second == iter2->second))
                        printf("iter->first: %d:%d\n", iter->first, iter2->second);
                        //printf("equal\n");
                }
                if (iter == x.end() && x.size() == y.size())
                    return true;
                return false;

        //return x.t == y.t;
    }

    /*
       template <class K, class V, class C, class A>
		inline bool operator==(const Map<K,V,C,A>& x, const Map<K,V,C,A>& y) {
                printf("operator== called\n");
                auto iter = x.begin();
                auto iter2 = y.begin();
                printf("first: %d:%d\n", iter->first, iter->second);
                for (iter = x.begin(); iter != x.end(); iter++,iter2++) {
                    if((iter->first == iter2->first) && (iter->second == iter2->second))
                        printf("iter->first: %d:%d\n", iter->first, iter2->first);
                        //printf("equal\n");
                }
                if (iter == x.end())
                    return true;
                else 
                    return false;
                return true;
                //return x.t == y.t;
            }
       */


}
