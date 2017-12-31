int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Enter command\n");
    exit(1);
  } else if (!strcmp(argv[1], "init")) {
    flog_init();
  } else {
    make_blob(argv[1]);
  }
  return 0;
    
}
