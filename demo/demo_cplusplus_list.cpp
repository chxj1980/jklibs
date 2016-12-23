//
// Created by v on 16-12-23.
//

#include <cstdio>
#include <list>

typedef struct {
    int       iParam;
} ListInfo;


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
    list_test();
}