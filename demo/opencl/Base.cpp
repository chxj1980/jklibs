//
// Created by jmdvirus on 2017/7/27.
//

#include "Base.h"
#include <stdio.h>

Base::Base() {

}

Base::~Base() {

}

int Base::GetBaseInfo() {
    cl_int err = clGetPlatformIDs(0, 0, &m_platforms);
    if (err != 0) {
        LOG("get platform id error [%d]\n", err);
    }
    return err;
}