#include <Windows.h>
#include <stdio.h>

int main()
{
  DWORD pid = GetCurrentProcessId();
  char testStr[] = "Katya";

  printf("+---------------------+------------------+\n");
  printf("| Process Information |      Value       |\n");
  printf("+---------------------+------------------+\n");
  printf("| %-19s | %-16s |\n", "Test String", testStr);
  printf("|---------------------|------------------|\n");
  printf("| %-19s | %-16lu |\n", "Process ID", pid);
  printf("+---------------------+------------------+\n");

  int a;
  scanf("%d", &a);

  printf("Result: %s\n", testStr);
  system("pause");

  return 0;
}