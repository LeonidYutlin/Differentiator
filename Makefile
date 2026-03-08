COMPILER      := g++
INCLUDE_FLAGS := -I src/
DEFINE_FLAGS  := -D _DEBUG -D DISABLE_NEWLINES 

ARTIFACT_PATH := build
BINARY_PATH   := bin

PROGRAM_NAME  := $(BINARY_PATH)/diff

define to_object
	$(patsubst %.cpp, $(ARTIFACT_PATH)/%.o, $(notdir $(1)))
endef

SOURCES := $(shell find src/ -type f -name '*.cpp')
OBJECTS := $(call to_object,$(SOURCES))

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
	@echo -e "•Linking the project together"
	@$(COMPILER) $(INCLUDE_FLAGS) $(C_FLAGS) $^ -o $@

define declare_recipe
$(call to_object,$(1)): $(1)
endef

$(foreach src,$(SOURCES),$(eval $(strip $(call declare_recipe,$(src)))))

%.o:
	@echo -e "•Compiling" $<
	@$(COMPILER) -c $(DEFINE_FLAGS) $(INCLUDE_FLAGS) $(C_FLAGS) $< -o $@

.PHONY: ensure_directories_exist clean

ensure_directories_exist:
	mkdir -p $(BINARY_PATH) $(ARTIFACT_PATH)

clean:
	rm -f $(OBJECTS) $(PROGRAM_NAME) 
