# (c) 2005 AnyWi Technologies B.V., All Rights Reserved.
#
# Commercial in Confidence.
#
# $Id: Makefile 15536 2014-03-27 20:13:54Z nick $
#
LIBNODAVE=	../../../third-party/libnodave-0.8.4.4

PROG=		plcget
SRCS=		plcget.c
NDOBJS=		${LIBNODAVE}/nodave.o ${LIBNODAVE}/openSocket.o
OBJS=		${NDOBJS}

FILESGROUPS=	AWMSG
AWMSG=		plcsend.sh
AWMSGOWN=	root
AWMSGGRP=	wheel
AWMSGMODE=	0755
AWMSGDIR=	${BINDIR}

# For libnodave
CFLAGS+=	-I${LIBNODAVE} -DLINUX
LDADD+=		${SWBASEDIR}/clib/libanywi.a

SCRIPTSDIR=	${BINDIR}

# Compile these using their own Makefile
.for o in ${NDOBJS}
$o:
	cd ${LIBNODAVE}; ${MAKE} ${o:T}
.endfor

test: ${PROG} plcget.cnf
	./plcget 10.0.0.10 plcget.cnf

plcget.cnf:
	@echo '** Please create the file plcget.cnf. See' 1>&2
	@echo '** projects/Koole/Naval/koole.ship/Files/usr/local/etc/plcget.cnf' 1>&2
	@echo '** for an example.' 1>&2
	@false

.include <bsd.prog.mk>

# Note: exclude a few flags from WARNS for libnodave header files.
CWARNFLAGS+=	-Wno-redundant-decls -Wno-strict-prototypes
TAGSADD+=	${LIBNODAVE}/nodave.h ${LIBNODAVE}/nodave.c ${LIBNODAVE}/openSocket.c
