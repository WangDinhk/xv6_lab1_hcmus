#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define MAX_PAGES 64  // Limit page number can check

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 start_address; // Start virtual addr of page to be checked
  int num_pages;        // Number of pages to be checked
  uint64 user_result_address; // Addr in userspace us to store bitmask

  // Get args from userspace 
  argaddr(0, &start_address);
  argint(1, &num_pages);      
  argaddr(2, &user_result_address);

  // Make sure numbers of pages in limination
  if (num_pages < 0 || num_pages > MAX_PAGES)
    return -1;

  uint64 bitmask = 0; // Var to store result
  struct proc *current_proc = myproc(); // Get curr process
  pagetable_t pagetable = current_proc->pagetable; // Page table of curr process

  // Loop for each page
  for (int i = 0; i < num_pages; i++) {
    // Calc virtual addr of curr page
    uint64 current_va = start_address + i * PGSIZE;

    // Get PTE (Page Table Entry) of virtual addr
    pte_t *pte = walk(pagetable, current_va, 0);
    if (pte == 0) // No PTE
      continue;

    // Check bit PTE_A in PTE (bit 6th)
    // If the page has been accessed, bit 6th equal 1, else 0
    if (*pte & PTE_A) {
      bitmask |= (1L << i); // Store to bit i in bitmask mark page i has been accseed 
      *pte &= ~PTE_A; // Clear the PTE_A bit so can check again next time
    }
  }

  // Copy bitmask from kernel space to userspace 
  if (copyout(pagetable, user_result_address, (char *)&bitmask, sizeof(bitmask)) < 0)
    return -1;
    
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
