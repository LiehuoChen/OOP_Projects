#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "Deque.hpp"

// May assume memcpy()-able.
// May assume = operator.

struct MyClass {
    int id;
    char name[10];
};

bool
MyClass_less_by_id(const MyClass &o1, const MyClass &o2) {
    return o1.id < o2.id;
}

void
MyClass_print(const MyClass *o) {
    printf("%d\n", o->id);
    printf("%s\n", o->name);
}

Deque_DEFINE(MyClass)

int
main() {

    Deque_MyClass deq;
    Deque_MyClass_ctor(&deq, MyClass_less_by_id);

    assert(deq.size(&deq) == 0);
    assert(deq.empty(&deq));

    // Should print "---- Deque_MyClass, 14".
//    strcpy(deq.type_name, "Deque_MyClass");
    printf("---- %s, %zu\n", deq.type_name, sizeof(deq.type_name));

    deq.push_back(&deq, MyClass{1, "Joe"});
    deq.push_back(&deq, MyClass{2, "Mary"});
    deq.push_back(&deq, MyClass{3, "Tom"});
    deq.push_front(&deq, MyClass{0, "Mike"});
    deq.push_front(&deq, MyClass{-1, "Mary"});

    MyClass_print(&deq.front(&deq));
    MyClass_print(&deq.back(&deq));
    assert(deq.front(&deq).id == -1);
    assert(deq.back(&deq).id == 3);

    deq.pop_front(&deq);
    deq.pop_back(&deq);
    assert(deq.front(&deq).id == 0);
    assert(deq.back(&deq).id == 2);

    assert(deq.size(&deq) == 3);

    for (Deque_MyClass_Iterator it = deq.begin(&deq);
     !Deque_MyClass_Iterator_equal(it, deq.end(&deq)); it.inc(&it)) {
        MyClass_print(&it.deref(&it));
    }

    // Test decrement.
    {
        auto it = deq.end(&deq);
        it.dec(&it);
        assert(it.deref(&it).id == 2);
    }

    printf("Using at.\n");

    for (size_t i = 0; i < 3; i++) {
        
        MyClass_print(&deq.at(&deq, i));
    }

    deq.clear(&deq);

    deq.dtor(&deq);

    // Test equality.  It is undefined behavior if the two deques were constructed with different
    // comparison functions.
    {
        Deque_MyClass deq1, deq2;
        Deque_MyClass_ctor(&deq1, MyClass_less_by_id);
        Deque_MyClass_ctor(&deq2, MyClass_less_by_id);

        deq1.push_back(&deq1, MyClass{1, "Joe"});
        deq1.push_back(&deq1, MyClass{2, "Jane"});
        deq1.push_back(&deq1, MyClass{3, "Mary"});
        deq2.push_back(&deq2, MyClass{1, "Joe"});
        deq2.push_back(&deq2, MyClass{2, "Jane"});
        deq2.push_back(&deq2, MyClass{3, "Mary"});

        assert(Deque_MyClass_equal(deq1, deq2));

        deq1.pop_back(&deq1);
        deq1.push_back(&deq1, MyClass{4, "Mary"});
        assert(!Deque_MyClass_equal(deq1, deq2));

        deq1.dtor(&deq1);
        deq2.dtor(&deq2);
    }
}
