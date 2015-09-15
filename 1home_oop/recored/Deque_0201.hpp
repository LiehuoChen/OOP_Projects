#ifndef CHENDEQUE_H
#define CHENDEQUE_H
#include <malloc.h>
#include <string.h>




#define Deque_DEFINE(object) \
struct Deque_##object##_node{ \
	void* prev; \
	void* next; \
	object data; \
};\
Deque_Iterator(object) \
bool Deque_##object##_Iterator_equal(Deque_##object##_Iterator it1, Deque_##object##_Iterator it2){ \
		return it1.node == it2.node; \
} \
Deque_container(object) \
typedef bool (*compare_##object)(const object&, const object&); \
struct Deque_##object* Deque_##object##_new(compare_##object func){ \
	struct Deque_##object *dequeue = (struct Deque_##object *)malloc(sizeof(struct Deque_##object)); \
	strcpy(dequeue->type_name, "Deque_"#object); \
	dequeue->empty_initialize(); \
	dequeue->compare_less = func; \
}






#define Deque_Iterator(T) \
struct Deque_##T##_Iterator{ 				\
	typedef Deque_##T##_Iterator*	iterator; 	\
	typedef T*		pointer;		\
	typedef T		value_type;		\
	typedef T&		reference;		\
	typedef	Deque_##T##_node* 	link_type;		\
	link_type	node;				\
	void inc(struct Deque_##T##_Iterator * it){	\
		it->node = (link_type)((*(it->node)).next); \
	} \
	void dec(struct Deque_##T##_Iterator *it){	\
		it->node = (link_type)((*(it->node)).prev); \
	} \
	struct Deque_##T##_Iterator next(struct Deque_##T##_Iterator *it){ \
		struct Deque_##T##_Iterator it2; \
		it2.node = (link_type)((it->node)->next); \
		return it2; \
	} \
	value_type deref(struct Deque_##T##_Iterator *it){ \
		return (*(it->node)).data; \
	} \
};

#define Deque_container(T) \
struct Deque_##T{	\
	typedef Deque_##T##_Iterator	iterator; \
	typedef T*		pointer;	\
	typedef T&		reference;	\
	typedef Deque_##T##_node*	link_type;	\
	typedef Deque_##T 	self;	\
 \
	link_type	end_node;	\
	char type_name[sizeof("Deque_"#T)]; \
 \
	bool (*compare_less)(const T&, const T&); \
 \
	bool equal(iterator it1, iterator it2){ \
		return (it1.node == it2.node); \
	} \
 \
	iterator begin(self *dequeue){ \
		iterator it; \
		it.node = (link_type)((*end_node).next); \
		return it; \
	} \
	iterator end(self *dequeue){ \
		iterator it; \
		it.node = end_node; \
		return it; \
	} \
	link_type get_node(){ \
		link_type tmp = (link_type)malloc(sizeof(struct Deque_##T##_node)); \
		return tmp; \
	} \
	link_type create_node (T x){ \
		link_type p = get_node(); \
		p->data = x; \
		return p; \
	} \
    int size(self *dequeue) { \
        int size = 0; \
        for(iterator it = dequeue->begin(dequeue); \
                !Deque_##T##_Iterator_equal(it, dequeue->end(dequeue)); \
                it.inc(&it)) \
        { \
            size++; \
        } \
        return size; \
    } \
	iterator insert(iterator position, T x){ \
		iterator it; \
		link_type tmp = create_node(x); \
		tmp->next = position.node; \
		tmp->prev = position.node->prev; \
		(link_type(position.node->prev))->next = tmp; \
		position.node->prev = tmp; \
		it.node = tmp; \
		return it; \
	} \
	void empty_initialize(){ \
		end_node = get_node(); \
		end_node->next = end_node; \
		end_node->prev = end_node; \
	} \
    bool empty(self *dequeue){ \
        if(end_node->next == end_node->prev) \
            return true; \
        return false; \
    } \
    reference front(self *dequeue) { \
        return  dequeue->begin(dequeue).node->data; \
    } \
    reference back(self *dequeue) {\
        iterator it = dequeue->end(dequeue); \
        it.dec(&it); \
        return it.node->data; \
    } \
	iterator push_front(self *dequeue,  T x) { \
		return insert(begin(dequeue), x); \
	} \
	iterator push_back(self *dequeue, T x){ \
		return insert(end(dequeue), x); \
	} \
	void transfer(iterator position, iterator first, iterator last){ \
		if (position.node != last.node){ \
			(*(link_type((*last.node).prev))).next = position.node; \
			(*(link_type((*first.node).prev))).next = last.node; \
			(*(link_type((*position.node).prev))).next = first.node; \
			link_type tmp = link_type((*position.node).prev); \
			(*position.node).prev = (*last.node).prev; \
			(*last.node).prev = (*first.node).prev; \
			(*first.node).prev = tmp; \
		} \
	} \
	void splice(iterator position, self *, iterator i){ \
		iterator j = i; \
		j.inc(&j); \
		if (j.node == position.node || i.node == position.node) \
			return; \
		transfer(position, i, j); \
	} \
 \
	void put_node(link_type p){ \
		free(p); \
	} \
 \
	void destroy_node(link_type p){ \
		put_node(p); \
	} \
 \
	iterator erase(self * dequeue, iterator it){  \
		iterator it2; \
		link_type next_node = link_type(it.node->next);	\
		link_type prev_node = link_type(it.node->prev); \
		prev_node->next = next_node; \
		next_node->prev = prev_node; \
		destroy_node(it.node); \
		it2.node = next_node; \
		return it2; \
	} \
 \
};

#endif
