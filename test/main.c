int main() {
  char msg[] = "Hello World from an ELF32!";
  const char COLOR = 11;
  char *vmem = (char *)0xb8000;
  for (int i = 0; i < sizeof(msg) - 1; i++) {
    *(vmem++) = msg[i];
    *(vmem++) = COLOR;
  }

  return 42;
}
