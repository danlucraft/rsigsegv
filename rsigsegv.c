// Include the Ruby headers and goodies
#include "ruby.h"
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>

// Defining a space for information and references about the module to be stored internally
VALUE SigSegv = Qnil;

void bt_sighandler(int sig, struct sigcontext ctx) {
  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;

  if (sig == SIGSEGV)
    printf("Got signal %d, faulty address is %p, from %p\n", sig, ctx.cr2, ctx.eip);
  else
    printf("Got signal %d\n", sig);

  trace_size = backtrace(trace, 16);
  /* overwrite sigaction with caller's address */
  trace[1] = (void *) ctx.eip;
  messages = backtrace_symbols(trace, trace_size);
  /* skip first stack frame (points here) */
  printf("[bt] Execution path:\n");
  for (i=1; i<trace_size; ++i)
    printf("[bt] %s\n", messages[i]);

  exit(0);
}


static VALUE rb_install_sigsegv_handler(VALUE self) {
  /* Install our signal handler */
  struct sigaction sa;

  sa.sa_handler = (void *)bt_sighandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
}

VALUE rb_trigger_sigsegv(VALUE self) {
  char *p = (char *)0xdeadbeef;
  
  *p = 10;	/* CRASH here!! */

  return Qnil;
}

// The initialization method for this module
void Init_rsigsegv() {
	SigSegv = rb_define_module("SigSegv");
	rb_define_singleton_method(SigSegv, "trigger", rb_trigger_sigsegv, 0);
	rb_define_singleton_method(SigSegv, "install_handler", rb_install_sigsegv_handler, 0);
}

