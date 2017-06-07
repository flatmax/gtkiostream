/* xermsg.f -- translated by f2c (version 20061008).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ int xermsg_(char *libname, char *subname, char *errmsg, 
	integer *errcode, integer *retcode, ftnlen libname_len, ftnlen 
	subname_len, ftnlen errmsg_len)
{
    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 6, 0, "(1X, A, ':', A, ':', 1X, A)", 0 };
    static cilist io___2 = { 0, 6, 0, "(1X, 'ERRORCODE = ', I)", 0 };
    static cilist io___3 = { 0, 6, 0, "(1X, 'RETCODE   = ', I)", 0 };



/*     only print fatal errors */

    if (*retcode > 0) {
	s_wsfe(&io___1);
	do_fio(&c__1, libname, libname_len);
	do_fio(&c__1, subname, subname_len);
	do_fio(&c__1, errmsg, errmsg_len);
	e_wsfe();
	s_wsfe(&io___2);
	do_fio(&c__1, (char *)&(*errcode), (ftnlen)sizeof(integer));
	e_wsfe();
	s_wsfe(&io___3);
	do_fio(&c__1, (char *)&(*retcode), (ftnlen)sizeof(integer));
	e_wsfe();
    }

    return 0;
} /* xermsg_ */

