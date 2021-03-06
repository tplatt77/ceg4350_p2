/*
 * simdisk.C -- simulated disk of File Sys Project WSU/CEG4350/Mateti/
 * The code in this file creates a simulated disk, on one binary file.
 * This code is complete and will change the least as we progress
 * toward P4.

 * prabhaker.mateti@wright.edu
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fstypes.h"

/* pre:: none;; post:: On success, a binary file will be created in
 * the current working directory.  It will have nSectorsPerDisk
 * sectors in it.  Each sector has nBytesPerSector. 
 * "Failure" is defined as one or more of the following happening:
 * invalid arguments, a file create/write/close fails */

SimDisk::SimDisk(const char *name, uint iSectorsPerDisk, uint iBytesPerSector)
{
  if (name == 0 || *name == 0)
    name = "D0";
  strncpy(labelOfTheDisk, name, LabelSZ);
  nSectorsPerDisk = iSectorsPerDisk;
  nBytesPerSector = iBytesPerSector;
  if (paramsOK() == 0)  // If params are not good, set to half the max.
  {
	  nSectorsPerDisk = SectorsMAX/2;
	  nBytesPerSector = BytesPerSectorMAX/2;
  }

  int exists = 0, fd = openDiskImage(O_RDONLY);	// Does it already exist?
  if (fd > 0) {
    struct stat statBuf;
    fstat(fd, &statBuf);
    close(fd);
    exists = (uint) statBuf.st_size == nSectorsPerDisk * nBytesPerSector;
  }
  if (exists == 0)
    makeDiskImage();
}

uint SimDisk::paramsOK()
{
  return
      0 < nSectorsPerDisk && nSectorsPerDisk <= SectorsMAX &&
      0 < nBytesPerSector && nBytesPerSector <= BytesPerSectorMAX;
}

/* pre:: mode == O_RDONLY or mode == O_WRONLY | O_CREAT;; post::
 * Systematically make up a name for the disk image file, open the/a file
 * with that pathname, and return its file descriptor.  Returned value
 * can be 0, or negative.;; */

int SimDisk::openDiskImage(uint mode)
{
  char tfnm[1024];
  sprintf(tfnm, "%s.dsk", labelOfTheDisk);
  return open(tfnm, mode, 0755);
}

/* pre:: none;; post:: A file named "%s.dsk", where %s stands for the
 * labelOfTheDisk is created.  This is will be of size
 * nBytesPerSector* nSectorsPerDisk in bytes, all set to zero.
 */

int SimDisk::makeDiskImage()
{
  int fd = openDiskImage(O_WRONLY | O_CREAT);
  if (fd < 3)
    return fd;

  char *buf = new char[nBytesPerSector];
  memset(buf, '3', nBytesPerSector);
  for (uint i = nSectorsPerDisk; i--;)
    write(fd, buf, nBytesPerSector);
  close(fd);
  delete buf;
  return 0;
}

uint SimDisk::readSector(uint nBlock, void *p)
{
  if (p == 0 || nBlock >= nSectorsPerDisk)
    return 0;
  int fd = openDiskImage(O_RDONLY);
  lseek(fd, nBytesPerSector * nBlock, SEEK_SET);
  read(fd, p, nBytesPerSector);
  close(fd);
  return nBytesPerSector;
}

uint SimDisk::writeSector(uint nBlock, void *p)
{
  if (p == 0 || nBlock >= nSectorsPerDisk)
    return 0;
  int fd = openDiskImage(O_WRONLY);
  lseek(fd, nBytesPerSector * nBlock, SEEK_SET);
  write(fd, p, nBytesPerSector);
  close(fd);
  return nBytesPerSector;
}

/* "Find" a file volume previously made. */

FileVolume *SimDisk::findfs()
{
  return new FileVolume(this);
}

FileVolume *SimDisk::makefs(uint nInodes, uint htInode)
{
  return new FileVolume(this, nInodes, htInode);
}

/* -eof- */
