// FOR VSCODE - actual definitions are passed as arguments at compile time
#ifdef __VSCODE__

#define REALMODE_SECTORS -1
#define KERNEL_LOAD_ADDRESS -1
#define KERNEL_SIZE -1

#endif

#define KERNEL_SECTORS (KERNEL_SIZE / 512)

void load_kernel();