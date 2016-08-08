//
// Created by v on 16-8-4.
//

#ifndef JKLIBS_V4_H
#define JKLIBS_V4_H

typedef struct tagJKProV4Header {
    int       iVersion; // 1-15
    int       iCrypt;  // 1,2,3,4
    int       iACK;
    int       iLength;
} JKProV4Header;

typedef struct tagJKProV4Body {
    char    *szData;  // need malloc of (iLength
} JKProV4Body;

typedef struct tagJKProV4 {
    JKProV4Header   szHeader;
    JKProV4Body     szBody;

    int             (*deinit)(struct tagJKProV4 *v4);
    // clear expect version and crypt.
    int             (*clear)(struct tagJKProV4 *v4);
    // Generate
    int             (*set_header)(struct tagJKProV4 *v4, int ack, int length);
    int             (*set_body)(struct tagJKProV4 *v4, const char *data);
    // result: pointer to bytes start position
    // Don't forget free result.
    int             (*to_bytes)(struct tagJKProV4 *v4, char **result);

    // Parse
    int             (*parse)(struct tagJKProV4 *v4, const char *bytes);

    // Free szData of Body.
    // Call when you want reuse this, but never call deinit and clear.
    int             (*free_body)(struct tagJKProV4 *v4);
} JKProV4;

int jk_pro_v4_init(JKProV4 *v4, int version, int crypt);

// Parse

#endif //JKLIBS_V4_H
