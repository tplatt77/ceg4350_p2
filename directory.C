/*
 * directory.C of CEG 433/633 File Sys Project; This is extremely
 * incomplete.

 * prabhaker.mateti@wright.edu
 */

#include "fstypes.h"
#include <cstring>

/* pre:: pfv mostly constructed;; post:: The root dir is created. */

uint Directory::create(FileVolume * pfv)
{
  fv = pfv;
  nInode = 1;
  fv->fbvInodes.setBit(1, 0);	// inode #1 no longer free
  char dirEntry[] = ".\00001..\00001"; // suspicious!

  dirf = new File(fv, nInode);
  dirf->appendBytes(dirEntry, 2 + 4 + 3 + 4);
  delete dirf;
  return 1;
}

uint Directory::reCreate(FileVolume * pfv)
{
  fv = pfv;
  return nInode = 1;
}

// TODO for projects

/*
	Find the name in dirEntry and returns 1 if successful and
	0 if fails.
*/
uint Directory::findName(char *name)
{
	char *found = strstr(dirEntry, name);
	if(found != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
	Add the name dirEntry and returns a 1 if successful and 0 otherwise.
	Add the inode number after the name value.
*/
uint Directory::addName(char *name, uint in)
{
	char *nameEntry;
	char *inode;
	inode = (char*) in;
	nameEntry = name;
	strcat(nameEntry, inode);
	strcat(dirEntry, nameEntry);
	char *loc;
	loc = strstr(dirEntry, name);
	if(loc != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint Directory::deleteName(char *name, uint in)
{
	char *pch;
	pch = strstr(dirEntry, name);
	strncpy(pch, "", 1);
	pch = pch + strlen(name);
	strncpy(pch, "", 1);
	if(pch == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint Directory::createFile(char *name)
{
	//STUB
	return 0;
}

uint Directory::ls()
{
	//STUB
	return 0;
}

uint Directory::rm(char *name)
{
	return deleteName(name, 0);
}

uint Directory::addNamePrivate(char *newName, uint in)
{
	//STUB
	return 0;
}

// -eof-

