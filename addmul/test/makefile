ifeq ($(BLISGEMM_USE_INTEL),true)
include $(BLISGEMM_DIR)/make.intel.inc
else
include $(BLISGEMM_DIR)/make.gnu.inc
endif


BLISGEMM_TEST_CC_SRC= \
									 test_dblisgemm.c \

BLISGEMM_TEST_CPP_SRC=\
#									 test_dblisgemm_stl.cpp \


BLISGEMM_TEST_EXE= $(BLISGEMM_TEST_CC_SRC:.c=.x) $(BLISGEMM_TEST_CPP_SRC:.cpp=.x)

all: $(BLISGEMM_TEST_EXE)

clean:
	rm -f $(BLISGEMM_TEST_EXE)

# ---------------------------------------------------------------------------
# Executable files compiling rules
# ---------------------------------------------------------------------------
%.x: %.c $(LIBBLISGEMM)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

%.x: %.cpp  $(LIBBLISGEMM)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)
# ---------------------------------------------------------------------------
