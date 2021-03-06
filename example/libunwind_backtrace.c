#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <stdio.h>

// Call this function to get a backtrace.
void backtrace() {
  unw_cursor_t cursor;
  unw_context_t context;

  // Initialize cursor to current frame for local unwinding.
  unw_getcontext(&context);
  unw_init_local(&cursor, &context); 

  const char* c = unw_regname(&cursor, 14);
  printf("Register name = %s \n", c);

  unw_word_t r14;
  unw_get_reg(&cursor, UNW_PPC64_R14, &r14);
  printf("r14 = %lu \n", r14);

  unw_fpreg_t f14;
  unw_get_fpreg(&cursor, UNW_PPC64_F14, &f14);
  printf("f14 = %lu \n", f14);


/*  printf("----------------------------\n");
  int i = 0;
  for(i = 0; i < 137; i++) {
	printf("[%d] = %lu \n", i, context.data[i]);

  }
*/


  // Unwind frames one by one, going up the frame stack.
  while (unw_step(&cursor) > 0) {
    unw_word_t offset, pc;
    unw_get_reg(&cursor, UNW_REG_IP, &pc);
    if (pc == 0) {
      break;
    }
    printf("instruction pointer = 0x%lx \n", pc);

    char sym[256];
    if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
      printf(" (%s+0x%lx)\n", sym, offset);
    } else {
      printf(" -- error: unable to obtain symbol name for this frame\n");
    }

  }
}

void foo() {
  backtrace(); // backtrace here!
}

void bar() {
  foo();
}

int main(int argc, char **argv) {
  asm("li 14, 200;");
  asm("lfd 14, 0(1)");

  bar();

  return 0;
}

