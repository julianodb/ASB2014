/* file: main.c			J. Barboza		26 March 2014
				Last revised:		26 March 2014   
-------------------------------------------------------------------------------
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.

You may contact the author by e-mail (george@mit.edu) or postal mail
(MIT Room E25-505A, Cambridge, MA 02139 USA).  For updates to this software,
please visit PhysioNet (http://www.physionet.org/).
_______________________________________________________________________________
*/

#include <stdio.h>
#include <stddef.h>
#include <wfdb/wfdb.h>
#include <wfdb/ecgcodes.h>

void *gcalloc(size_t nmemb, size_t size);
void cleanup(int status);

char auxbuf[1+255+1];		/* 'aux' string buffer for annotations */
char *pname;			/* name of this program, used in messages */
char *record = NULL;		/* name of input record */
FILE *config = NULL;		/* configuration file, if any */
int debug;			/* if non-zero, generate debugging output */
int minutes = 0;		/* minutes elapsed in the current hour */
int nsig;			/* number of signals */
WFDB_Annotation annot;		/* most recently written annotation */
WFDB_Sample *v;			/* latest sample of each input signal */
WFDB_Siginfo *si;		/* characteristics of each input signal */
WFDB_Signal sig;		/* signal number of signal to be analyzed */
WFDB_Time next_minute;		/* sample number of start of next minute */
WFDB_Time spm;			/* sample intervals per minute */
WFDB_Time sps;			/* sample intervals per second */
WFDB_Time t;			/* time of the current sample being analyzed */
WFDB_Time t0;			/* time of the start of the analysis period */
WFDB_Time tf;			/* time of the end of the analysis period */
WFDB_Time tf_learn;		/* time of the end of the learning period */

main(int argc, char **argv)
{
    char *p;
    int gvmode = 0, i, isiglist = 0, j, nisig, s;
    WFDB_Anninfo a;

    pname = prog_name(argv[0]);

    // get record name (-r option)
    for (i = 1; i < argc; i++) {
        if (*argv[i] == '-') switch (*(argv[i]+1)) {
          case 'r':     /* record name */
            if (++i >= argc) {
                (void)fprintf(stderr, "%s: input record name must follow -r\n",
                              pname);
                cleanup(1);
            }
            record = argv[i];
            break;
          default:
            (void)fprintf(stderr, "%s: unrecognized option %s\n", pname,
                          argv[i]);
            cleanup(1);
        }
        else {
            (void)fprintf(stderr, "%s: unrecognized argument %s\n", pname,
                          argv[i]);
            cleanup(1);
        }
    }

    if (record == NULL) {
	cleanup(1);
    }

    // magic words
    if (gvmode == 0 && (p = getenv("WFDBGVMODE")))
	gvmode = atoi(p);
    setgvmode(gvmode|WFDB_GVPAD);

    // check number of signals
    if ((nsig = isigopen(record, NULL, 0)) < 1) cleanup(2);

    // allocate signal info si and last sample v
    si = (WFDB_Siginfo *)gcalloc((size_t)nsig, sizeof(WFDB_Siginfo));
    v = (WFDB_Sample *)gcalloc((size_t)nsig, sizeof(WFDB_Sample));

    // wfdb init with the record and annotation qrs
    a.name = "qrs"; a.stat = WFDB_WRITE;
    if ((nsig = wfdbinit(record, &a, 1, si, nsig)) < 1) cleanup(2);

    // verify sample freq
    if (sampfreq((char *)NULL) < 50.) {
	(void)fprintf(stderr, "%s: sampling frequency (%g Hz) is too low%s",
		      pname, sampfreq((char *)NULL),
		      gvmode & WFDB_HIGHRES ? "\n" : ", try -H option\n");
	cleanup(3);
    }

    sps = strtim("1");
    spm = strtim("1:0");
    next_minute = t + spm;

    /*
    if (sig > 0) {
	int i = findsig(argv[sig]);
	if (i < 0) {
	    (void)fprintf(stderr,
			  "%s: (warning) no signal %d in record %s\n",
			  pname, s, record);
	    cleanup(4);
	}
	sig = i;
    }
    */

    // algorithm specific calls
    state = RUNNING;
    t = t0 - dt4;
    gqrs(t0, tf);		/* run the detector and collect output */

    printf(" %s\n", timstr(strtim("i")));
    cleanup(0);
}

void gqrs(WFDB_Time from, WFDB_Time to)
{
			    annot.subtyp = 1;
			if (state == RUNNING) {
			    int qsize;

			    annot.time = p->time - dt2;
			    annot.anntyp = NORMAL;
			    annot.chan = sig;
			    qsize = p->amp * 10.0 / qthr;
			    if (qsize > 127) qsize = 127;
			    annot.num = qsize;
			    putann(0, &annot);
			    annot.time += dt2;
			}
			if (tw) {
			    static WFDB_Annotation tann;

			    tann.time = tw->time - dt2;
			    if (debug && state == RUNNING) {
				tann.anntyp = TWAVE;
				tann.chan = sig+1;
				tann.num = rtdmin;
				tann.subtyp = (tann.time > annot.time + rtmean);
				tann.aux = NULL;
				putann(0, &tann);
			    }
			r = p; q = NULL; qamp = 0; annot.subtyp = 0;
	    annot.anntyp = NORMAL;
	    annot.chan = sig;
	    annot.time = p->time;
	    putann(0, &annot);
}

/* prog_name() extracts this program's name from argv[0], for use in error and
   warning messages. */

char *prog_name(char *s)
{
    char *p = s + strlen(s);

#ifdef MSDOS
    while (p >= s && *p != '\\' && *p != ':') {
	if (*p == '.')
	    *p = '\0';		/* strip off extension */
	if ('A' <= *p && *p <= 'Z')
	    *p += 'a' - 'A';	/* convert to lower case */
	p--;
    }
#else
    while (p >= s && *p != '/')
	p--;
#endif
    return (p+1);
}

/* gcalloc() is a wrapper for calloc() that handles errors and maintains
   a list of allocated buffers for automatic on-exit deallocation via
   gfreeall(). */

size_t gcn = 0, gcnmax = 0;
void **gclist = NULL;

void *gcalloc(size_t nmemb, size_t size)
{
    void *p = calloc(nmemb, size), **q = NULL;

    if ((p == NULL) ||
	((gcn >= gcnmax) &&
	 (q = realloc(gclist, (gcnmax += 32)*(sizeof(void *)))) == NULL)) {
	fprintf(stderr, "%s: insufficient memory\n", pname);
	cleanup(3);
    }
    if (q) gclist = q;
    return (gclist[gcn++] = p);
}

void gfreeall()			/* free memory allocated using gcalloc() */
{
   while (gcn-- > 0)
	if (gclist[gcn]) free(gclist[gcn]);
    free(gclist);
}

void cleanup(int status)	/* close files and free allocated memory */
{
    if (record) wfdbquit();
    gfreeall();
    exit(status);
}
