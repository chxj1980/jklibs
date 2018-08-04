#include <stdio.h>
#include <errno.h>

#include <sys/statvfs.h>

int main(int argc, char **args) {
	struct statvfs st;
	int ret = statvfs(args[1], &st);
	printf(" type: size : %lu avaliable: %lu\n",  st.f_blocks * st.f_bsize, st.f_bsize * st.f_bavail);
	printf("ret = %d, errno = %d\n", ret, errno);

        return 0;
}
