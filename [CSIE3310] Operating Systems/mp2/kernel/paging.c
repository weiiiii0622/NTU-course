#include "param.h"
#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"

/* NTU OS 2022 */
/* Page fault handler */
int handle_pgfault() {
  /* Find the address that caused the fault */
  /* TODO */
  uint64 *pa;
  uint64 va = r_stval();
  struct proc *p = myproc();
  if(va > p->sz) return -1;

  va = PGROUNDDOWN(va);
  pte_t *pte = walk(p->pagetable, va, 0);
  if(*pte & PTE_S){
    uint64 blk;
    *pte |= PTE_V;
    *pte &= ~PTE_S;
    // write from disk to free physical pg
    begin_op();
    blk = PTE2BLOCKNO(*pte);
    pa = (uint64*) kalloc();
    memset(pa, 0, PGSIZE);
    read_page_from_disk(ROOTDEV, (char*)pa, blk); 
    bfree_page(ROOTDEV, blk);
    end_op();

    // redirect to physicak pg
    *pte = PA2PTE(pa) | PTE_FLAGS(*pte); 
  }
  else if(!(*pte & PTE_V)){
    pa = (uint64*) kalloc();
    memset(pa, 0, PGSIZE);
    if(mappages(p->pagetable, va, PGSIZE, (uint64)pa, PTE_U|PTE_R|PTE_X|PTE_W) < 0){
      kfree((void*)pa);
      return -1;
    }
  }
  return 0;
  //panic("not implemented yet\n");
}
