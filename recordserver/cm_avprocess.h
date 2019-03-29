//
// Created by v on 19-2-22.
//

#ifndef JKLIBS_CM_AVPROCESS_H
#define JKLIBS_CM_AVPROCESS_H

typedef struct tagCMAVProcess *CMAVProcessP;

int cm_avprocess_init(CMAVProcessP *av);

int cm_avprocess_deinit(CMAVProcessP *av);

int cm_avprocess_av_ready(CMAVProcessP av, int width, int height);

int cm_avprocess_write(CMAVProcessP av, unsigned char *data, size_t len);

#endif //JKLIBS_CM_AVPROCESS_H
