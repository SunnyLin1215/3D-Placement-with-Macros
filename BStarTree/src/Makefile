CXX := g++ -g
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra
LIBS := -lm
EXEC := ../bin/b_star_tree
SRC_DIRS := Parser\
			Solver\
			ResultWriter\
			Structure
SRCS := main.cpp $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS)

ifeq (test,$(firstword $(MAKECMDGOALS)))
  TESTCASE_NAME := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(TESTCASE_NAME):;@:)
endif

test: $(EXEC)
	@echo test on $(TESTCASE_NAME) 0.15
	./$(EXEC) ../testcases/$(TESTCASE_NAME).hardblocks ../testcases/$(TESTCASE_NAME).nets ../testcases/$(TESTCASE_NAME).pl ../output/$(TESTCASE_NAME).floorplan 0.15 1
	../verifier/verifier ../testcases/$(TESTCASE_NAME).hardblocks ../testcases/$(TESTCASE_NAME).nets ../testcases/$(TESTCASE_NAME).pl ../output/$(TESTCASE_NAME).floorplan 0.15	
	@echo test on $(TESTCASE_NAME) 0.1
	./$(EXEC) ../testcases/$(TESTCASE_NAME).hardblocks ../testcases/$(TESTCASE_NAME).nets ../testcases/$(TESTCASE_NAME).pl ../output/$(TESTCASE_NAME).floorplan 0.1 1
	../verifier/verifier ../testcases/$(TESTCASE_NAME).hardblocks ../testcases/$(TESTCASE_NAME).nets ../testcases/$(TESTCASE_NAME).pl ../output/$(TESTCASE_NAME).floorplan 0.1
	

.PHONY: all clean test
-include $(DEPS)
