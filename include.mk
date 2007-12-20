############################################################
# These aren't being used yet, because CAFE doesn't install.
# It only builds in place.
CAFE_PATH = /usr
CAFE_LIB_PATH = $(CAFE_PATH)/lib
CAFE_INC_PATH = $(CAFE_PATH)/include
############################################################

# These Macros depend on the individual Makefiles to
# define the INCLUDEDIR macro for themselves.
UTIL_INCLUDEDIR = $(INCLUDEDIR)/Utils
CONF_INCLUDEDIR = $(INCLUDEDIR)/Config
SPA_INCLUDEDIR = $(INCLUDEDIR)/SPAnalysis


MYSQLPP_INC = -I /usr/include -I /usr/include/mysql
MYSQLPP_LIB = -L /usr/lib

BUTILS_INC = -I /usr/include/BUtils
BUTILS_LIB = -L /usr/lib

PROJECTION_INC = -I /usr/include/BUtils
PROJECTION_LIB = -L /usr/lib


