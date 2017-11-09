CROSS_COMPILE=arm-hisi-linux-
## high gcc version will support more warning variables.
GCC_HIGH=no

#base function
BVBASE=yes

# process  in src
BVPROCESS=yes

EXTERN_NAME=arm-hisi-linux-gnu

BVPU_HISI3515=yes
BVPU_HISI=yes

## the path when you use out lib
LIPATH= -L./lib/$(OS)/
