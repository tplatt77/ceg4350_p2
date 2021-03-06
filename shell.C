/*
 * shell.C -- CEG4350 File Sys Project shell; This is a simple shell,
 * invoking various methods of the CEG 4350/6350 project as the project
 * develops through the term.  It uses a well known C std library
 * function strtok().  This code is almost complete and will change in the
 * command table as we progress toward Pn.

 * prabhaker.mateti@wright.edu
 */

#include "fstypes.h"

SimDisk *simDisk = 0;
FileVolume *cfv = 0;
Directory *currentDir = 0;
void releaseSimDisk()
{
  if (simDisk)
    delete simDisk;
  simDisk = 0;
}

void releaseCfv()
{
  if (cfv)
    delete cfv;
  cfv = 0;
}


uint TODO()
{
  printf("to be done!\n");
  return 0;
}

uint TODO(const char *p)
{
  printf("%s to be done!\n", p);
  return 0;
}

/* An Arg-ument for one of our commands is either a "word" (a null
 * terminated string), or an unsigned integer.  We store both
 * representations of the argument. */

class Arg {
public:
  char *s;
  uint u;
};

uint isDigit(char c)
{
  return ('0' <= c) && (c <= '9');
}

uint isAlphaNumDot(char c)
{
  return c == '.' || (('a' <= c) && (c <= 'z'))
    || (('A' <= c) && (c <= 'Z')) || (('0' <= c) && (c <= '9'));
}

int toNum(const char *p)
{
  return (p != 0) && ('0' <= *p) && (*p <= '9' ? atoi(p) : 0);
}

uint mkSimDisk()
{
  char name[1024] = "";
  uint nblocks = 0, nbytes = 0;

  FILE *f = fopen("diskParams.dat", "r");
  if (f == 0)
    return 0;
  fscanf(f, "%s %u %u\n", name, &nblocks, &nbytes);
  fclose(f);

  releaseSimDisk();
  simDisk = new SimDisk(name, nblocks, nbytes);
  printf("simDisk %lx = new SimDisk(%s, %d, %d)\n",
	 (ulong) simDisk, name, nblocks, nbytes);
  return (ulong) simDisk;
}

/* Also does "findDisk". */

void doMakeDisk(Arg * a)
{
  uint r = mkSimDisk();
  if (r != 0)
    releaseCfv();
  printf("mkSimDisk() == %d\n", r);
}

void doWriteDisk(Arg * a)
{
  const char *st = a[1].s;		// arbitrary word
  if (st == 0)			// if it is NULL, we use ...
    st = "CEG4350/6350/Mateti";
  char buf[1024];		// assuming nBytesPerSectorMAX < 1024
  for (uint m = strlen(st), n = 0; n < 1024 - m; n += m)
    memcpy(buf + n, st, m);	// fill with several copies of st
  uint r = simDisk->writeSector(a[0].u, buf);
  printf("write4350disk(%d, %s...) == %d\n", a[0].u, st, r);
}

void doReadDisk(Arg * a)
{
  char buf[1024];		// assuming nBytesPerSectorMAX < 1024
  uint r = simDisk->readSector(a[0].u, buf);
  buf[10] = 0;			// sentinel
  printf("read4350disk(%d, %s...) == %d\n", a[0].u, buf, r);
}

void doQuit(Arg * a)
{
  releaseCfv();
  releaseSimDisk();
  exit(0);
}

void doEcho(Arg * a)
{
  printf("%s#%d, %s#%d, %s#%d, %s#%d\n", a[0].s, a[0].u,
	 a[1].s, a[1].u, a[2].s, a[2].u, a[3].s, a[3].u);
}

void doMakeFS(Arg * a)
{
  uint ni, ht;
  if(a[1].u == 0)
  {
     ni = 100;
     ht = 8;
  }
  else
  {
    ni = a[0].u;
    ht = a[1].u;
  }
  releaseCfv();
  cfv = simDisk->makefs(ni, ht);
  printf("makefs(%d, %d) == %px\n", ni, ht, (void *) cfv);
}

void doFindFS(Arg * a)
{
  releaseCfv();
  cfv = simDisk->findfs();
  printf("findfs() == %px\n", (void *) cfv);
}

void doInode(Arg * a)
{
  uint ni = a[0].u;
  cfv->inodes.show(ni);
}

// Implement in P1
//-----------------
void doCP(Arg * a) 
{
  //uint src = a[0].u;
  //uint des = a[1].u;
  
}

void doMV(Arg * a) 
{
	
}

void doLS(Arg * a) 
{
	currentDir->ls();
}

void doRM(Arg * a) 
{
  char* pnm = a[0].s;
  currentDir->rm(pnm);
}

void doMkDir(Arg * a) 
{

}

void doRmDir(Arg * a) 
{

}

void doChDir(Arg * a) 
{

}

void doPWD(Arg * a) 
{

}

void doFdFS(Arg * a) 
{

}



/* The following describes one entry in our table of commands.  For
 * each cmdName (a null terminated string), we specify the arguments
 * it requires by a sequence of letters.  The letter s stands for
 * "that argument should be a string", the letter u stands for "that
 * argument should be an unsigned int."  The third data member (func)
 * is a pointer to the function in our code that implements that
 * command.
 */

class CmdTable {
public:
  const char *cmdName;
  const char *argsRequired;
  const char *globalsNeeded;		// need d==simDisk, v==cfv?
  void (*func) (Arg * a);
} cmdTable[] = {
  {"quit", "", "", doQuit},
  {"mkdisk", "", "", doMakeDisk},
  {"fddisk", "", "", doMakeDisk},
  {"rddisk", "u", "d", doReadDisk},
  {"wrdisk", "us", "d", doWriteDisk},
  {"echo", "ssss", "", doEcho},
  {"mkfs", "d", "", doMakeFS},// Implement for P1
  {"findfs", "", "v", doFindFS},
  {"inode", "u", "v", doInode},
   // TODO: Implement for P1
   // -------------------------
  {"cp", "d", "d", doCP},
  {"mv", "d", "d", doMV},
  {"ls", "","", doLS},
  {"rm", "s", "", doRM},
  {"mkdir", "", "", doMkDir},
  {"rmdir", "", "", doRmDir},
  {"chdir", "", "", doChDir},
  {"pwd", "", "", doPWD},
  {"fdfs", "", "", doFdFS},
  // ---------------------------
  {"q", "", "", doQuit},
  {"exit", "", "", doQuit}
};

uint ncmds = sizeof(cmdTable) / sizeof(CmdTable);

void usage()
{
  printf("The shell has only the following cmds:\n");
  for (uint i = 0; i < ncmds; i++)
    printf("\t%s\t%s\n", cmdTable[i].cmdName, cmdTable[i].argsRequired);
  printf("Start with ! to invoke a Unix shell cmd\n");
}

/* pre:: k >= 0;; post:: Check that args are ok, and the needed
 * simDisk orcfv exists before invoking the appropriate action. */

void invokeCmd(int k)
{
  uint ok = 1;
  if (cmdTable[k].globalsNeeded[0] == 'd' && simDisk == 0) {
    ok = 0;
    printf("Cmd %s needs the simDisk to be != 0.\n", cmdTable[k].cmdName);
  }
  if (cmdTable[k].globalsNeeded[0] == 'v' && cfv == 0) {
    ok = 0;
    printf("Cmd %s needs the cfv to be != 0.\n", cmdTable[k].cmdName);
  }

  const char *req = cmdTable[k].argsRequired;
  uint na = strlen(req);
  Arg *arg = new Arg[na];
  for (uint i = 0; i < na; i++) {
    arg[i].s = strtok(0, " \t");
    arg[i].u = 0;
    if (req[i] == 's' && (arg[i].s == 0 || arg[i].s[0] == 0)) {
      ok = 0;
      printf("arg #%d must be a non-empty string.\n", i);
    }
    if (req[i] == 'u') {
      if (arg[i].s != 0 && isDigit(arg[i].s[0]))
	arg[i].u = toNum(arg[i].s);
      else {
	ok = 0;
	printf("arg #%d (%s) must be a number.\n", i, arg[i].s);
      }
    }
  }
  if (ok)
    (*cmdTable[k].func) (arg);
  delete arg;
}

/* pre:: none;; post:: Find the row number of the cmd given in p.
 * Return this number if found; return -1 otherwise. */

int findCmd(char *p)
{
  if (p != 0)
    for (uint i = 0; i < ncmds; i++) {
      if (strcmp(p, cmdTable[i].cmdName) == 0) {
	return i;
      }
    }
  return -1;
}

void ourgets(char * buf, uint sz)
{
    fgets(buf, sz, stdin);
    buf[strlen(buf) - 1] = '\0'; // remove the trailing \n
    printf("cmd [%s]\n", buf);	// just print out what we got as-is
}

int main()
{
  char buf[1024];		// do not type > 1023 chars

  usage();
  for (;;) {
    *buf = 0;			// clear old input
    printf("%s", "FS35% ");	// prompt
    ourgets(buf, 1023);
    if (buf[0] == 0)
      continue;
    if (buf[0] == '#')
      continue;			// this is a comment line, do nothing
    if (buf[0] == '!')		// begins with !, execute it as 
      system(buf + 1);		// a normal shell cmd
    else {
      char *p = strtok(buf, " \t");
      int k = findCmd(p);
      if (k >= 0)
	invokeCmd(k);
      else
	usage();
    }
  }
}

// -eof-
