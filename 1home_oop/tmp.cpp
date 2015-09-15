struct Deque_MyClass_node{ MyClass data;
 };
 struct Deque_MyClass_Iterator{ typedef Deque_MyClass_Iterator* iterator;
 typedef MyClass* pointer;
 typedef MyClass value_type;
 typedef MyClass& reference;
 typedef Deque_MyClass_node* link_type;
 link_type node;
 link_type eend;
 link_type estart;
 void inc(struct Deque_MyClass_Iterator * it){ if(eend == it->node) it->node = estart;
 else it->node = (link_type)(it->node)+1;
 } void dec(struct Deque_MyClass_Iterator *it){ if(estart == it->node) it->node = eend;
 else it->node = (link_type)(it->node)-1;
 } struct Deque_MyClass_Iterator next(struct Deque_MyClass_Iterator *it){ struct Deque_MyClass_Iterator it2;
 it2.node = (link_type)(it->node)+1;
 return it2;
 } reference deref(struct Deque_MyClass_Iterator *it){ return (*(it->node)).data;
 } };
 bool Deque_MyClass_Iterator_equal(Deque_MyClass_Iterator it1, Deque_MyClass_Iterator it2){ return it1.node == it2.node;
 } struct Deque_MyClass{ typedef Deque_MyClass_Iterator iterator;
 typedef MyClass* pointer;
 typedef MyClass& reference;
 typedef Deque_MyClass_node* link_type;
 typedef Deque_MyClass self;
 size_t capacity;
 size_t fronte;
 size_t backe ;
 link_type element;
 link_type estart;
 link_type eend;
 char type_name[sizeof("Deque_""MyClass")];
 bool (*compare_less)(const MyClass&, const MyClass&);
 bool equal(iterator it1, iterator it2){ return (it1.node == it2.node);
 } iterator begin(self *dequeue){ iterator it;
 it.eend = dequeue->eend;
 it.estart = dequeue->estart;
 it.node = (link_type)&element[fronte];
 return it;
 } iterator end(self *dequeue){ iterator it;
 it.eend = dequeue->eend;
 it.estart = dequeue->estart;
 it.node = (link_type)&element[backe];
 return it;
 } link_type get_node(){ link_type tmp = (link_type)malloc(sizeof(struct Deque_MyClass_node));
 return tmp;
 } link_type create_node (MyClass x){ link_type p = get_node();
 p->data = x;
 return p;
 } size_t size(self *dequeue) { size_t size = 0;
 for(iterator it = dequeue->begin(dequeue);
 !Deque_MyClass_Iterator_equal(it, dequeue->end(dequeue));
 it.inc(&it)) { size++;
 } return size;
 } void initialize(){ fronte = 0ul;
 backe = 0ul;
 } void clear(self *dequeue) { dequeue->initialize();
 } void dtor(self *dequeue) { free(dequeue->element);
 } bool empty(self *dequeue){ if(dequeue->fronte == dequeue->backe) return true;
 return false;
 } bool full(self *dequeue) { if(((dequeue->backe + 1)%capacity) == dequeue->fronte) return true;
 return false;
 } reference front(self *dequeue) { return dequeue->element[dequeue->fronte].data;
 } reference back(self *dequeue) { return dequeue->element[(dequeue->backe-1+capacity)%capacity].data;
 } reference at(self *dequeue, size_t index){ if(index < dequeue->size(dequeue)) { size_t newindex = (dequeue->fronte + index)%dequeue->capacity;
 return dequeue->element[newindex].data;
 } else return dequeue->element[dequeue->fronte].data;
 } void reallocate(self *dequeue) { dequeue->capacity = dequeue->capacity;
 } void push_front(self *dequeue, MyClass x) { dequeue->fronte = (dequeue->fronte-1+capacity) % capacity;
 element[dequeue->fronte].data = x;
 if(full(dequeue)) { capacity = capacity;
 reallocate(dequeue);
 } } void push_back(self *dequeue, MyClass x){ element[dequeue->backe].data = x;
 dequeue->backe = (dequeue->backe + 1)%capacity;
 if(full(dequeue)){ capacity = capacity;
 reallocate(dequeue);
 } } void free_node(link_type p){ free(p);
 } void destroy_node(link_type p){ free_node(p);
 } void pop_front(self *dequeue) { dequeue->fronte = (dequeue->fronte+1)%dequeue->capacity;
 } void pop_back(self *dequeue) { dequeue->backe = (dequeue->backe -1)%dequeue->capacity;
 } };
 typedef bool (*compare_MyClass)(const MyClass&, const MyClass&);
 void Deque_MyClass_ctor(struct Deque_MyClass* dequeue, compare_MyClass func){ strcpy((*dequeue).type_name, "Deque_""MyClass");
 dequeue->capacity = 1000ul;
 dequeue->fronte = 0ul;
 dequeue->backe = 0ul;
 dequeue->element = (struct Deque_MyClass_node *) malloc(sizeof(struct Deque_MyClass_node)*dequeue->capacity);
 dequeue->estart = &dequeue->element[0];
 dequeue->eend = &dequeue->element[(dequeue->capacity)-1];
 dequeue->initialize();
 dequeue->compare_less = func;
 } bool Deque_MyClass_equal(Deque_MyClass obj1, Deque_MyClass obj2) { size_t size1 = obj1.size(&obj1);
 size_t size2 = obj2.size(&obj2);
 if (size1 != size2) return false;
 size_t index = 0;
 while(index < size1) { MyClass temp1 = obj1.at(&obj1, index);
 MyClass temp2 = obj2.at(&obj2, index);
 if(!(!obj1.compare_less(temp1, temp2)&& !obj1.compare_less(temp2, temp1))) return false;
 index++;
 } return true;
 }
