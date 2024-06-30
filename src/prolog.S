.text

// RDI: entry
// RSI: stack
.globl prolog
prolog:
  mov %rsi, %rsp // switch to new stack
  call *%rdi
  ret
