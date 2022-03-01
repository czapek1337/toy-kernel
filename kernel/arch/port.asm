global port_out8
global port_out16
global port_out32

global port_in8
global port_in16
global port_in32

port_out8:
  mov rdx, rdi
  mov rax, rsi
  out dx, al
  ret

port_out16:
  mov rdx, rdi
  mov rax, rsi
  out dx, ax
  ret

port_out32:
  mov rdx, rdi
  mov rax, rsi
  out dx, eax
  ret

port_in8:
  mov rdx, rdi
  xor rax, rax
  in al, dx
  ret

port_in16:
  mov rdx, rdi
  xor rax, rax
  in ax, dx
  ret

port_in32:
  mov rdx, rdi
  xor rax, rax
  in eax, dx
  ret
