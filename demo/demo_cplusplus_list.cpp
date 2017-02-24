//
// Created by v on 16-12-23.
//

#include <cstdio>
#include <list>

typedef struct {
    int       iParam;
} ListInfo;

class DemoA {
public:
    DemoA();
    virtual ~DemoA();

    int hello();
    virtual int dosome() = 0;
};

DemoA::DemoA() {

}

DemoA::~DemoA() {

}

int DemoA::hello() {
    printf("--- demoA \n");
    dosome();
    return 0;
}

class DemoB : public DemoA {
public:
    DemoB();
    ~DemoB();

    int hello();

    virtual int dosome();
};

DemoB::DemoB() {

}
DemoB::~DemoB() {

}

int DemoB::hello() {
    printf("------hello\n");
    return 0;
}

int DemoB::dosome() {
    printf("hello b\n");
    return 0;
}

int demo() {

    DemoB * b = new DemoB();
    b->hello();
    return 0;
}

int list_test() {

    std::list<ListInfo*> list_info_;
    ListInfo *l1 = new ListInfo;
    l1->iParam = 1;
    ListInfo *l2 = new ListInfo;
    l2->iParam = 2;

    list_info_.push_back(l1);
    list_info_.push_back(l2);

    for (std::list<ListInfo*>::iterator iter = list_info_.begin(); iter != list_info_.end();) {
        ListInfo *t1 = *iter;
        delete t1;
        list_info_.erase(iter++);
//        break;
    }
    return 0;
}

int main() {
//    list_test();
    demo();
}