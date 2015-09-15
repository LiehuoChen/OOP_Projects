#ifndef CHENDEQUE_H
#define CHENDEQUE_H
#include <malloc.h>
#include <string.h>
#include <stdlib.h>



#define Deque_DEFINE(object) \
struct Deque_##object##_node{ \
	object data; \
};\
Deque_Iterator(object) \
bool Deque_##object##_Iterator_equal(Deque_##object##_Iterator it1, Deque_##object##_Iterator it2){ \
		return it1.node == it2.node; \
} \
Deque_container(object) \
typedef bool (*compare_##object)(const object&, const object&); \
void Deque_##object##_ctor(struct Deque_##object*, compare_##object func){ \
	struct Deque_##object *dequeue = (struct Deque_##object *)malloc(sizeof(struct Deque_##object)); \
	strcpy(dequeue->type_name, "Deque_"#object); \
    dequeue->capacity = 200; \
    dequeue->fronte = 0; \
    dequeue->backe = 0; \
    dequeue->element = (struct Deque_##object##_node *) \
    malloc(sizeof(struct \
                Deque_##object##_node)*dequeue->capacity); \
	dequeue->initialize(); \
	dequeue->compare_less = func; \
} \
bool Deque_##object##_equal(Deque_##object obj1, \
        Deque_##object obj2) \
{ \
    size_t size1 = obj1.size(&obj1); \
    size_t size2 = obj2.size(&obj2); \
    if (size1 != size2) \
        return false;\
    size_t index = 0; \
    while(index < size1) \
    { \
        object temp1 = obj1.at(&obj1, index); \
        object temp2 = obj2.at(&obj2, index); \
        if(!obj1.compare_less(temp1, temp2)) \
            return false; \
    } \
    return true; \
} \

#define Deque_Iterator(T) \
struct Deque_##T##_Iterator{ 				\
	typedef Deque_##T##_Iterator*	iterator; 	\
	typedef T*		pointer;		\
	typedef T		value_type;		\
	typedef T&		reference;		\
	typedef	Deque_##T##_node* 	link_type;		\
	link_type	node;				\
	void inc(struct Deque_##T##_Iterator * it){	\
		it->node = (link_type)(it->node)+1; \
	} \
	void dec(struct Deque_##T##_Iterator *it){	\
		it->node = (link_type)(it->node)-1; \
	} \
	struct Deque_##T##_Iterator next(struct Deque_##T##_Iterator *it){ \
		struct Deque_##T##_Iterator it2; \
		it2.node = (link_type)(it->node)+1; \
		return it2; \
	} \
	reference deref(struct Deque_##T##_Iterator *it){ \
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
    size_t capacity; \
    size_t fronte; \
    size_t backe; \
    link_type element; \
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
		it.node = (link_type)&element[fronte]; \
		return it; \
	} \
	iterator end(self *dequeue){ \
		iterator it; \
		it.node = (link_type)&element[backe]; \
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
	void initialize(){ \
        fronte = 0; \
        backe = 0; \
	} \
    void clear(self *dequeue) { \
        initialize(); \
    } \
    void dtor(self *dequeue) { \
        free(dequeue->element); \
        free(dequeue);\
    } \
    bool empty(self *dequeue){ \
        if(fronte == backe) \
            return true; \
        return false; \
    } \
    bool full(self *dequeue) \
    { \
        if(((backe + 1)%capacity) == fronte); \
            return true; \
        return false; \
    } \
    reference front(self *dequeue) { \
        return  dequeue->element[fronte].data; \
    } \
    reference back(self *dequeue) {\
        return \
        dequeue->element[(backe-1+capacity)%capacity].data; \
    } \
    reference at(self *dequeue, size_t index){ \
        if(index < capacity) \
        { \
            size_t newindex = (fronte + index)%capacity; \
            if(newindex < backe) \
                return dequeue->element[newindex].data; \
        } \
    } \
    void reallocate(self *dequeue) { \
    } \
	void push_front(self *dequeue,  T x) { \
        element[fronte].data = x; \
        fronte = (fronte-1+capacity) % capacity; \
        if(full(dequeue)) \
        { \
            capacity = capacity * 2; \
            reallocate(dequeue); \
        } \
	} \
	iterator push_back(self *dequeue, T x){ \
        element[backe].data = x; \
        backe = (backe + 1)%capacity; \
        if(full(dequeue)){ \
            capacity = capacity * 2; \
            reallocate(dequeue); \
        } \
	} \
	void free_node(link_type p){ \
		free(p); \
	} \
 \
	void destroy_node(link_type p){ \
		free_node(p); \
	} \
 \
    void pop_front(self *dequeue) { \
        iterator it = dequeue->begin(dequeue); \
        fronte = (fronte+1)%capacity; \
        destroy_node(it.node); \
    } \
    void pop_back(self *dequeue) { \
        iterator last = dequeue->end(dequeue); \
        backe = (backe  -1)%capacity; \
        destroy_node(last.node); \
    } \
 \
};

#endif
