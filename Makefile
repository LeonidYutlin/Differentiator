include_flags := -I src/
define_flags  := -D _DEBUG -D DISABLE_NEWLINES 

artifact_path := build
binary_path   := bin

program_name  := $(binary_path)/diff

raw_objects := queue.o nodetype.o io.o parse.o \
				 		   util.o derivative.o tree.o \
					 		 dump.o main.o node.o error.o context.o
objects     := $(addprefix $(artifact_path)/, $(raw_objects))

c_flags := -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++                   \
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
				   -fno-omit-frame-pointer -Wlarger-than=65536                    \
				   -Wstack-usage=8192 -pie -fPIE -Werror=vla                      \
				   -fsanitize=address,alignment,bool,bounds,enum,$\
				   float-cast-overflow,float-divide-by-zero,$\
				   integer-divide-by-zero,leak,nonnull-attribute,$\
				   null,object-size,return,returns-nonnull-attribute,$\
				   shift,signed-integer-overflow,undefined,$\
				   unreachable,vla-bound,vptr

$(program_name): $(objects)
	g++ $(include_flags) $(c_flags) $(objects) -o $@

$(artifact_path)/queue.o:      src/ds/queue/queue.cpp
$(artifact_path)/nodetype.o:   src/ds/tree/nodetype.cpp
$(artifact_path)/io.o:         src/diff/io/io.cpp
$(artifact_path)/parse.o:      src/diff/io/parse.cpp
$(artifact_path)/util.o:       src/misc/util.cpp
$(artifact_path)/derivative.o: src/diff/derivative.cpp
$(artifact_path)/tree.o:       src/ds/tree/tree.cpp
$(artifact_path)/dump.o:       src/ds/tree/dump/dump.cpp
$(artifact_path)/main.o:       src/main.cpp
$(artifact_path)/node.o:       src/ds/tree/node.cpp
$(artifact_path)/error.o:      src/error/error.cpp
$(artifact_path)/context.o:    src/diff/context.cpp

%.o:
	g++ -c $(define_flags) $(include_flags) $(c_flags) $< -o $@

.PHONY: ensure_directories_exist clean

ensure_directories_exist:
	mkdir -p $(binary_path) $(artifact_path)

clean:
	rm -f $(objects) $(program_name) 
