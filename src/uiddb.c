/* vi: set sw=4 ts=4: */
/*
 * Copyright (C) 2001 Christian Hohnstaedt.
 *
 *  All rights reserved.
 *
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  - Neither the name of the author nor the names of its contributors may be 
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 *
 * http://www.hohnstaedt.de/e2fsimage
 * email: christian@hohnstaedt.de
 *
 * $Id: uiddb.c,v 1.2 2004/03/12 14:20:17 chris2511 Exp $ 
 *
 */                           

#include "e2fsimage.h"
#include <string.h>
#include <unistd.h>

#define RESIZE 1024

/* 
 * The uid database (uidb) is a lookup table to find uids to filenames
 * from the source filesystem  and their corresponding inode number
 * in the target ext2 filesystem.
 * It is a simple resizeable unsorted linked list of a structure.
 */


/* 
 * we get a pointer to a maintainance structure and initialize it
 */
int uiddb_init(uiddb_t *db)
{
	memset(db, 0, sizeof(uiddb_t));
	return 0;
}

/* 
 * adds an inode pair to the stack 
 * and resizes the stack if nessecary
 */
int uiddb_add(uiddb_t *db, const char* name, int uid, int gid)
{
	int needed_size, namelen;
	struct uidentry *new;
	char *nameptr;
	/* 
	 * the needed size of the whole thing:
	 * the structure + the length of the filename
	 */
	namelen = strnlen(name, 79);
	needed_size = sizeof(struct uidentry) + (namelen * sizeof(char));
	/* 
	 * first look if the available place is sufficent
	 * end resize it if not
	 */ 
	if (db->size + needed_size > db->maxsize) {
		struct uidentry *ptr;
		ptr = realloc(db->first, db->maxsize + RESIZE );
		if (ptr == NULL) {
			fprintf(stderr, "realloc() failed\n");
			return -1;
		}
		/* update maxsize and the start */
		db->maxsize += RESIZE;
		db->first = ptr;
	}
	
	/* make the new one sit at the end of the chain */
	new = (struct uidentry *)(((unsigned char *)db->first) + db->size);
	new->uid = uid;
	new->gid = gid;
	new->namelen = namelen;
	/*
	 * the name begins after the uidentry struct 
	 * Warning: Pointer arith. ! 
	 */
	nameptr = (char *)(new + 1);
	memcpy(nameptr, name, namelen * sizeof(char)); 
	
	db->size += needed_size;
	printf("new:%p, namep:%p, size:%d, first:%p, structs:%d soc:%d\n",
		  new, new+1, db->size, db->first, sizeof(struct uidentry), sizeof(char));
	return 0;
}

static struct uidentry *next_ptr(struct uidentry *ptr) {
	return (struct uidentry *)((unsigned char *)ptr  + sizeof(struct uidentry) + ptr->namelen*sizeof(char));
}

/*
 * iterates over the uidentry stack and searches for the supplied filename.
 * Returns the uidentry or 0 if it was not found.
 */
int uiddb_search(uiddb_t *db, const char *name, int *uid, int *gid)
{
	struct uidentry *ptr;
	if (!db->first) return 0;
	for (ptr = db->first; (unsigned char *)ptr - (unsigned char *)db->first < db->size; ptr = next_ptr(ptr)) {
		if (memcmp(name, ptr+1, ptr->namelen) == 0) {
			*uid = ptr->uid;
			*gid = ptr->gid;
			return 1;
		}
	}
	return 0;
}

/* 
 * release all the occupied memory
 */
void uiddb_free(uiddb_t *db)
{
	if (db) {
		free(db->first);
	}
}
