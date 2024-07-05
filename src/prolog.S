.text

// RDI: entry
// RSI: stack
// RDX: argc
// RCX: argv
// R8: envv
.globl prolog
prolog:
  endbr64
  mov %rsi, %rsp // switch to new 
  mov %rdx, %rsi
  mov %rcx, %rdx
  mov %r8, %rcx
  jmp *%rdi
  ret
