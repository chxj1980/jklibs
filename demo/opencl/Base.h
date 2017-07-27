//
// Created by jmdvirus on 2017/7/27.
//

#ifndef JKLIBS_BASE_H
#define JKLIBS_BASE_H

#include "Base.h"
#ifdef __MACOS__
#include <OpenCL/CL.h>
#else
#endif

#define LOG printf

class Base {
public:
    Base();
    ~Base();

    int GetBaseInfo();

private:
    cl_uint m_platforms;
};


#endif //JKLIBS_BASE_H
