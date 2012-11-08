#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argn, char* argv[])
{
  int sysmem_fd;
  char* data;
  const int length = 4096;
  int i;
  unsigned int value;
  int exit = 0;
  int chose;
  sysmem_fd = open("/sys/kernel/debug/lophilo/sysmem", O_RDWR);
  data = mmap(
	  0,  // starting address
	  length, // length
	  PROT_READ | PROT_WRITE,  //prot
	  MAP_SHARED, //flags
	  sysmem_fd, //filedescriptor
	  0x0 // offset
	  );
  close(sysmem_fd);
  printf("FPGA debug tools\n");
  while(!exit)
  {
	  printf("Menu\n");
	  printf("0:Exit\n");
	  printf("1:Dump\n");
	  printf("2:read\n");
	  printf("3:write\n");
  	  printf("4:Step motor driver demo\n");

	  scanf("%d",&chose);
	  switch(chose) {
	  case 0:
		  exit =1;
		  break;
	  case 1:
		  printf("FPGA reg dump\n");
		  for(i=0;i<length;i++) {
			  printf("0x%02x ", data[i]);
			  if(i != 0 && (i+1)%8 ==0)
				  printf("\n");
		  }
		  break;
	  case 2:
		  printf("input reg address\n");
		  scanf("%x",&i);
		  if(i < 0 || i > length)
			  continue;
		  printf("current reg address 0x%x\n",i);
		  printf("current reg value: 0x%x\n", data[i]);
		  break;
	  case 3:
		  printf("input reg address\n");
		  scanf("%x",&i);
		  if(i < 0 || i > length)
			  continue;
		  printf("input reg value\n");
		  scanf("%x",&value);
		  data[i] =value;
		  break;
	  case 4:
		  {
			  unsigned int* PWM_Freg  = data+0x240;
			  unsigned int* PWM_width_A = data+0x244;
			  unsigned int* PWM_width_B = data+0x248;
			  char c;
			  int frequent = 1000;
			  int duty_cycle =50;
			  *PWM_Freg = frequent * 0x100000000 / 200000000;
			  *PWM_width_A = 0xffffffff / 100 * duty_cycle;
			  *PWM_width_B = 0xffffffff / 100 * duty_cycle;
			  printf("%d\n",frequent);
			  printf("%d\n",*PWM_width_A);
			  while(1)
			  {
				  data[0x24c]=1;
				  c = getchar();
				  if(c == 'q')break;
				  data[0x24c]=0;
				  c = getchar();
				  if(c == 'q')break;
			  }
		  }
	  	  break;
	  default:
		  exit =1;
		  break;
	  }
  }

  munmap(data, length);
  return 0;
}
