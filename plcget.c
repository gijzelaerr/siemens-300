/*
 * (c) 2005 Anywi Technologies B.V., All Rights Reserved.
 * 
 * \file
 *
 * Interface to get values from Siemens S7 PLC.
 */

/*
 * PLC Get configuration file format
 *
 * Example:
 * 	#DB	address	bt	label	datatype	statuslen (optional)
 *	10	4	0	Temp1	temp		2
 *	10	10	0	Temp2	temp
 *
 * where
 * 	DB		the PLC DB to address
 *	address		address within the db
 *	bt		bit offset (currently must be 0)
 *	label		label to use in output
 *	datatype	see below
 *	statuslen	length of optional status field following the data
 *
 * Data types:
 *
 *       name    content                         interpretation
 *       ------  ------------------------------- ----------------------------------------
 *       real    real(4)			 floating point value
 *       byte    byte(1)			 byte
 *       int     2 byte word			 aka word
 *       dint	 4 byte word                     aka dword
 *       smu     byte(1), byte(1), word(2)       (sec, min, hour) returned as (%.5f) hours
 *       smus    byte(1), byte(1), word(2)       (sec, min, hour) returned as (%d) seconds
 *       puls    real(4)                         rounded down to int
 */

#define PROGRAM_VERSION	"Siemens S7 PLC Get, (C) AnyWi Technologies (Rev. " SVNVERSION ", " BUILDDATE ")"

#include <sys/cdefs.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/time.h>

#include <nodave.h>
#include <openSocket.h>

#include <clib.h>

int plcport = 1099;
char *plchost = NULL;

static void
usage(void)
{
	fprintf(stderr, PROGRAM_VERSION "\n"
		"Usage: %s <ip> <configfile> <outputfile>\n"
		, getprogname());
	exit(1);
}

static unsigned char *
readData(daveConnection * dc, int area, int db, int start, int len, int statuslen,
	 char *infile, int lineno,
	 char *label, char *datatype)
{
	static unsigned char buffer[10];

	if (daveReadBytes(dc, area, db, start, len+statuslen, buffer) != daveResOK) {
		fprintf(stderr, "%s:%d: failed to read %s (%s)\n", infile, lineno, label, datatype);
		return NULL;	/* error */
	} else if (statuslen == 0
	           || (statuslen == 1 && daveGetU8from(buffer+len) != 0)
	           || (statuslen == 2 && daveGetU16from(buffer+len) != 0)
	           || (statuslen == 4 && daveGetU32from(buffer+len) != 0)) {
		return buffer;	/* data valid */
	} else {
		fprintf(stderr, "%s:%d: data not valid for %s (%s), skipped\n", infile, lineno, label, datatype);
		return NULL;	/* data invalid */
	}
}

int
main(int argc, char **argv)
{
	int		i, lineno = 0;
	int		e = 0;
	char		buf1      [davePartnerListSize];
	char		line      [1024];
	FILE		*in, *out;
	char		*infile, *outfile;

	daveInterface  *di;
	daveConnection *dc = NULL;
	_daveOSserialType fds;

	openlog(getprogname(), LOG_CONS|LOG_PERROR, LOG_USER);
	INFO(PROGRAM_VERSION);

	if (argc != 4 || strcmp(argv[1], "-h") == 0) {
		usage();
		exit(1);
	}

	infile = argv[2];
	if (!(in = fopen(infile, "r"))) {
		ERROR("Could not open config file '%s' for reading: %s", infile, strerror(errno));
		exit(1);
	}
	outfile = argv[3];
	if (!(out = fopen(outfile, "w"))) {
		ERROR("Could not open output file '%s' for writing: %s", outfile, strerror(errno));
		exit(1);
	}
	plchost = argv[1];

	/* Note: libnodave returns _0_ if the opening of the socket failed. */
	if ((fds.rfd = openSocket(plcport, plchost)) == 0) {	
		/* This is not unusual, for example where there is no PLC.
		 * Therefore reduce the log level.
		 */
		INFO("%s:%d: Could not open connection: %s", plchost, plcport, strerror(errno));
		exit(2);
	}

	fds.wfd = fds.rfd;
	di = daveNewInterface(fds, __DECONST(char *, "IF1"), 0, daveProtoMPI_IBH, daveSpeed1500k);
	if (!di || daveInitAdapter(di) != daveResOK) {
		ERROR("Could not make new Interface");
		exit(2);
	}

	int l = daveListReachablePartners(di, buf1);
	for (i = 0; i < l; i++) {
		if (buf1[i] == daveMPIReachable) {
			if ((dc = daveNewConnection(di, i, 0, 0))
			    && daveConnectPLC(dc) == daveResOK) {
				break;
			} else if (dc) {
				daveDisconnectPLC(dc);
			}
		}
	}

	if (!dc) {
		ERROR("%s:%d: Could not connect to any partner", plchost, plcport);
		daveDisconnectAdapter(di);
		exit(3);
	}
	ERROR("%s:%d: Connected to partner %d", plchost, plcport, i);

	while (fgets(line, sizeof(line) - 1, in)) {
		unsigned int db, adr, bt;
		char label[sizeof(line)];
		char datatype[sizeof(line)];
		char *p = line;
		int statuslen = 0;
		unsigned char *b;

		lineno++;
		while (*p == ' ')
			p++;
		if ((*p == '#') || (*p == '\n') || (*p == '\0'))
			continue;

		if (sscanf(p, "%d %d %d %s %s %d", &db, &adr, &bt, label, datatype, &statuslen) < 5) {
			ERROR("%s:%d: malformed line '%s'", infile, lineno, p);
			e = 4;
		} else if (bt != 0) {
			/* XXX non-byte aligned not implemented yet */
			ERROR("%s:%d: values must be byte aligned", infile, lineno);
			e = 4;
		} else if (strcasecmp(datatype, "smu") == 0) {
			if ((b = readData(dc, daveDB, db, adr, 4, statuslen, infile, lineno, label, datatype))) {
				int s = daveGetU8from(b);
				int m = daveGetU8from(b+1);
				int u = daveGetU16from(b+2);
				fprintf(out, "%s: %.5f\n", label, (double)(u*3600+m*60+s)/3600.0);
			}
		} else if (strcasecmp(datatype, "smus") == 0) {
			if ((b = readData(dc, daveDB, db, adr, 4, statuslen, infile, lineno, label, datatype))) {
				int s = daveGetU8from(b);
				int m = daveGetU8from(b+1);
				int u = daveGetU16from(b+2);
				fprintf(out, "%s: %d\n", label, u*3600+m*60+s);
			}
		} else if (strcasecmp(datatype, "real") == 0
		           || strcasecmp(datatype, "temp") == 0) {	// legacy
			if ((b = readData(dc, daveDB, db, adr, 4, statuslen, infile, lineno, label, datatype)))
				fprintf(out, "%s: %f\n", label, daveGetFloatfrom(b));
		} else if (strcasecmp(datatype, "puls") == 0) {
			if ((b = readData(dc, daveDB, db, adr, 4, statuslen, infile, lineno, label, datatype)))
				fprintf(out, "%s: %.0f\n", label, daveGetFloatfrom(b));
		} else if (strcasecmp(datatype, "byte") == 0) {
			if ((b = readData(dc, daveDB, db, adr, 1, statuslen, infile, lineno, label, datatype)))
				fprintf(out, "%s: %d\n", label, daveGetU8from(b));
		} else if (strcasecmp(datatype, "word") == 0
		           || strcasecmp(datatype, "int") == 0) {
			if ((b = readData(dc, daveDB, db, adr, 2, statuslen, infile, lineno, label, datatype)))
				fprintf(out, "%s: %d\n", label, daveGetU16from(b));
		} else if (strcasecmp(datatype, "dword") == 0
		           || strcasecmp(datatype, "dint") == 0) {
			if ((b = readData(dc, daveDB, db, adr, 4, statuslen, infile, lineno, label, datatype)))
				fprintf(out, "%s: %d\n", label, daveGetU32from(b));
		} else {
			ERROR("%s:%d: unknown type %s", infile, lineno, datatype);
			e = 4;
		}
	};

	daveDisconnectPLC(dc);
	daveDisconnectAdapter(di);

	exit(e);
}
