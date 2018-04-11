//
// Created by v on 16-12-23.
//

#include <list>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string>

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

int list_test_string() {
    std::list<std::string> string_list;
    char url[512] = "xxxxx";
    string_list.push_back(url);
    string_list.push_back("value");
 
    for (std::list<std::string>::iterator iter = string_list.begin(); iter != string_list.end(); iter++) {
         std::string s = *iter;
         printf("string: [%s]\n", s.c_str());
    }
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

class TBase {
public:
    TBase();
    ~TBase();

    void output();
    int test_args(int i, int j, bool value = false);

private:
    int         value;
};

TBase::TBase() {
     value = 8;
}

TBase::~TBase() {

}

void TBase::output() {
    printf("yes, tbas %d e\n", value);
}

int TBase::test_args(int i, int j, bool value) {
    printf("-------- ok : %d, %d\n", i,j );
}

TBase **gbase;

void set_tbase(TBase **base) {
    gbase = base;
}

void *base_test(void *arges)
{
    printf("before gbase %p\n", gbase);
//    gbase->output();
    sleep(4);
    printf("gbase %p\n", gbase);
    (*gbase)->output();
}

int main() {
//    list_test();
//    demo();
    list_test_string();

//    TBase *base = new TBase();
    //base->test_args(1, 2);

    sleep(10);

    return 0;
}
