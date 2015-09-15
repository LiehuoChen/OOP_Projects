#ifndef YHLIST_H
#define YHLIST_H
#include <malloc.h>
#include <string.h>
#define List_Iterator(T) \
struct List_##T##_Iterator{ 				\
	typedef List_##T##_Iterator*	iterator; 	\
	typedef T*		pointer;		\
	typedef T		value_type;		\
	typedef T&		reference;		\
	typedef	List_##T##_node* 	link_type;		\
	link_type	node;				\
	void inc(struct List_##T##_Iterator * it){	\
		it->node = (link_type)((*(it->node)).next); \
	} \
	void dec(struct List_##T##_Iterator *it){	\
		it->node = (link_type)((*(it->node)).prev); \
	} \
	struct List_##T##_Iterator next(struct List_##T##_Iterator *it){ \
		struct List_##T##_Iterator it2; \
		it2.node = (link_type)((it->node)->next); \
		return it2; \
	} \
	value_type deref(struct List_##T##_Iterator *it){ \
		return (*(it->node)).data; \
	} \
};

#define List_container(T) \
struct List_##T{	\
	typedef List_##T##_Iterator	iterator; \
	typedef T*		pointer;	\
	typedef T&		reference;	\
	typedef List_##T##_node*	link_type;	\
	typedef List_##T 	self;	\
 \
	link_type	node;	\
	char type[sizeof("List_"#T)]; \
 \
	bool (*compare_less)(const T&, const T&); \
 \
	bool equal(iterator it1, iterator it2){ \
		return (it1.node == it2.node); \
	} \
 \
	iterator begin(self *list){ \
		iterator it; \
		it.node = (link_type)((*node).next); \
		return it; \
	} \
	iterator end(self *list){ \
		iterator it; \
		it.node = node; \
		return it; \
	} \
	link_type get_node(){ \
		link_type tmp = (link_type)malloc(sizeof(struct List_##T##_node)); \
		return tmp; \
	} \
	link_type create_node (T x){ \
		link_type p = get_node(); \
		p->data = x; \
		return p; \
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
		node = get_node(); \
		node->next = node; \
		node->prev = node; \
	} \
	iterator push_front(self *list,  T x) { \
		return insert(begin(list), x); \
	} \
	iterator push_back(self *list, T x){ \
		return insert(end(list), x); \
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
	void merge(self *list1, self *list2){ \
		iterator first1 = list1->begin(list1); \
		iterator last1 = list1->end(list1); \
		iterator first2 = list2->begin(list2); \
		iterator last2 = list2->end(list2); \
		while (!equal(first1, last1) && !equal(first2, last2)) \
			if (compare_less(first2.deref(&first2), first1.deref(&first1))){ \
				iterator next = first2.next(&first2); \
				transfer(first1, first2, next); \
				first2 = next; \
			} \
			else \
				first1.inc(&first1); \
		if (first2.node != last2.node) \
			transfer(last1, first2, last2); \
	} \
	void mergesort(self *list){ \
		self *list2 =(self*)malloc(sizeof(self)); \
		list2->empty_initialize(); \
		iterator orgfirst = list->begin(list); \
		iterator orglast = list->end(list); \
		if(equal(orgfirst.next(&orgfirst), orglast) || equal(orgfirst, orglast)) {\
			list2->delet(list2); \
			return; \
		} \
		iterator first1 = orgfirst; \
		iterator last1 = orgfirst.next(&orgfirst); \
		while (!equal(last1, orglast) && !equal(last1.next(&last1), orglast)){ \
			orgfirst.inc(&orgfirst); \
			last1 = last1.next(&last1); \
			last1 = last1.next(&last1); \
		} \
		last1 = orgfirst.next(&orgfirst); \
		transfer(list2->begin(list2), last1, orglast); \
		mergesort(list); \
		mergesort(list2); \
		merge(list, list2); \
		list2->delet(list2); \
	} \
 \
	void sort(self * list){ \
		mergesort(list); \
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
	iterator erase(self * list, iterator it){  \
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
	void delet(self *list){ \
		iterator first = list->begin(list); \
		iterator last = list->end(list); \
		iterator next = first.next(&first); \
		while (!equal(first, last)){ \
			put_node(first.node); \
			first = next; \
			next = next.next(&next); \
		} \
		put_node(list->node); \
		free(list); \
	} \
 \
};

#define List_DEFINE(object) \
struct List_##object##_node{ \
	typedef void* void_pointer; \
	void_pointer prev; \
	void_pointer next; \
	object data; \
};\
List_Iterator(object) \
List_container(object) \
typedef bool (*compare_##object)(const object&, const object&); \
bool List_##object##_Iterator_equal(List_##object##_Iterator it1, List_##object##_Iterator it2){ \
		return it1.node == it2.node; \
} \
struct List_##object* List_##object##_new(compare_##object func){ \
	struct List_##object *list = (struct List_##object *)malloc(sizeof(struct List_##object)); \
	strcpy(list->type, "List_"#object); \
	list->empty_initialize(); \
	list->compare_less = func; \
}
#endif
