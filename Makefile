COMPILER      := g++
INCLUDE_FLAGS := -I src/
DEFINE_FLAGS  := -D _DEBUG -D DISABLE_NEWLINES 

ARTIFACT_PATH := build
BINARY_PATH   := bin

PROGRAM_NAME  := $(BINARY_PATH)/diff

RAW_OBJECTS := queue.o nodetype.o io.o parse.o \
				 		   util.o derivative.o tree.o \
					 		 dump.o main.o node.o error.o context.o
OBJECTS     := $(addprefix $(ARTIFACT_PATH)/, $(RAW_OBJECTS))

C_FLAGS := -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++                   \
				   -Waggressive-loop-optimizations -Wc++14-compat                 \
				   -Wmissing-declarations -Wcast-align -Wcast-qual                \
				   -Wchar-subscripts -Wconditionally-supported                    \
				   -Wconversion -Wctor-dtor-privacy -Wempty-body                  \
				   -Wfloat-equal -Wformat-nonliteral -Wformat-security            \
				   -Wformat-signedness -Wformat=2 -Winline -Wlogical-op           \
				   -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual          \
				   -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls         \
				   -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
				   -Wstrict-overflow=2 -Wsuggest-attribute=noreturn               \
				   -Wsuggest-final-methods -Wsuggest-final-types                  \
				   -Wsuggest-override -Wswitch-default -Wsync-nand                \
				   -Wundef -Wunreachable-code -Wunused -Wuseless-cast             \
				   -Wvariadic-macros -Wno-literal-suffix                          \
				   -Wno-missing-field-initializers -Wno-narrowing                 \
				   -Wno-old-style-cast -Wno-varargs -Wstack-protector             \
				   -fcheck-new -fsized-deallocation -fstack-protector             \
				   -fstrict-overflow -flto-odr-type-merging                       \
				   -fno-omit-frame-pointer -Wlarger-than=64000                    \
				   -Wstack-usage=8192 -pie -fPIE -Werror=vla                      \
				   -fsanitize=address,alignment,bool,bounds,enum,$\
				   float-cast-overflow,float-divide-by-zero,$\
				   integer-divide-by-zero,leak,nonnull-attribute,$\
				   null,object-size,return,returns-nonnull-attribute,$\
				   shift,signed-integer-overflow,undefined,$\
				   unreachable,vla-bound,vptr

$(PROGRAM_NAME): $(OBJECTS)
	$(COMPILER) $(INCLUDE_FLAGS) $(C_FLAGS) $(OBJECTS) -o $@

$(ARTIFACT_PATH)/queue.o:      src/ds/queue/queue.cpp
$(ARTIFACT_PATH)/nodetype.o:   src/ds/tree/nodetype.cpp
$(ARTIFACT_PATH)/io.o:         src/diff/io/io.cpp
$(ARTIFACT_PATH)/parse.o:      src/diff/io/parse.cpp
$(ARTIFACT_PATH)/util.o:       src/misc/util.cpp
$(ARTIFACT_PATH)/derivative.o: src/diff/derivative.cpp
$(ARTIFACT_PATH)/tree.o:       src/ds/tree/tree.cpp
$(ARTIFACT_PATH)/dump.o:       src/ds/tree/dump/dump.cpp
$(ARTIFACT_PATH)/main.o:       src/main.cpp
$(ARTIFACT_PATH)/node.o:       src/ds/tree/node.cpp
$(ARTIFACT_PATH)/error.o:      src/error/error.cpp
$(ARTIFACT_PATH)/context.o:    src/diff/context.cpp

%.o:
	$(COMPILER) -c $(DEFINE_FLAGS) $(INCLUDE_FLAGS) $(C_FLAGS) $(OPTIONALS) $< -o $@

.PHONY: ensure_directories_exist clean

ensure_directories_exist:
	mkdir -p $(BINARY_PATH) $(ARTIFACT_PATH)

clean:
	rm -f $(OBJECTS) $(PROGRAM_NAME) 
