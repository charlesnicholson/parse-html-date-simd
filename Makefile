SRCS := parse_html_date.c \
		main.cc

BUILD_DIR := build
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
OS := $(shell uname)

CPPFLAGS += -MMD -MP -march=haswell -O3 -g -Wall -Werror -Wextra
CFLAGS = --std=c99
CXXFLAGS = --std=c++17

$(BUILD_DIR)/test: $(OBJS) Makefile
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c Makefile
	mkdir -p $(dir $@) && $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cc.o: %.cc Makefile
	mkdir -p $(dir $@) && $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/test.timestamp: $(BUILD_DIR)/test
	$(BUILD_DIR)/test && touch $(BUILD_DIR)/test.timestamp

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

.DEFAULT_GOAL := $(BUILD_DIR)/test.timestamp

-include $(DEPS)

