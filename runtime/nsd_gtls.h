/* An implementation of the nsd interface for GnuTLS.
 *
 * Copyright 2008 Rainer Gerhards and Adiscon GmbH.
 *
 * This file is part of the rsyslog runtime library.
 *
 * The rsyslog runtime library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The rsyslog runtime library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the rsyslog runtime library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * A copy of the GPL can be found in the file "COPYING" in this distribution.
 * A copy of the LGPL can be found in the file "COPYING.LESSER" in this distribution.
 */

#ifndef INCLUDED_NSD_GTLS_H
#define INCLUDED_NSD_GTLS_H

#include "nsd.h"

typedef enum {
	gtlsRtry_None = 0,	/**< no call needs to be retried */
	gtlsRtry_handshake = 1
} gtlsRtryCall_t;		/**< IDs of calls that needs to be retried */

typedef nsd_if_t nsd_gtls_if_t; /* we just *implement* this interface */

/* the nsd_gtls object */
struct nsd_gtls_s {
	BEGINobjInstance;	/* Data to implement generic object - MUST be the first data element! */
	nsd_t *pTcp;		/**< our aggregated nsd_ptcp data */
	uchar *pszConnectHost;	/**< hostname used for connect - may be used to authenticate peer if no other name given */
	int iMode;		/* 0 - plain tcp, 1 - TLS */
	int bAbortConn;		/* if set, abort conncection (fatal error had happened) */
	enum { 
		GTLS_AUTH_CERTNAME = 0,
		GTLS_AUTH_CERTFINGERPRINT = 1,
		GTLS_AUTH_CERTVALID = 2,
		GTLS_AUTH_CERTANON = 3
	} authMode;
	gtlsRtryCall_t rtryCall;/**< what must we retry? */
	int bIsInitiator;	/**< 0 if socket is the server end (listener), 1 if it is the initiator */
	gnutls_session sess;
	int bHaveSess;		/* as we don't know exactly which gnutls_session values are invalid, we use this one
				   to flag whether or not we are in a session (same as -1 for a socket meaning no sess) */
	int bReportAuthErr;	/* only the first auth error is to be reported, this var triggers it. Initially, it is
				 * set to 1 and changed to 0 after the first report. It is changed back to 1 after
				 * one successful authentication. */
	permittedPeers_t *pPermPeers; /* permitted peers */
	gnutls_x509_crt ourCert;	/**< our certificate, if in client mode (unused in server mode) */
	gnutls_x509_privkey ourKey;	/**< our private key, if in client mode (unused in server mode) */
	short	bOurCertIsInit;	/**< 1 if our certificate is initialized and must be deinit on destruction */
	short	bOurKeyIsInit;	/**< 1 if our private key is initialized and must be deinit on destruction */
};

/* interface is defined in nsd.h, we just implement it! */
#define nsd_gtlsCURR_IF_VERSION nsdCURR_IF_VERSION

/* prototypes */
PROTOTYPEObj(nsd_gtls);
/* some prototypes for things used by our nsdsel_gtls helper class */
uchar *gtlsStrerror(int error);
rsRetVal gtlsChkPeerAuth(nsd_gtls_t *pThis);

/* the name of our library binary */
#define LM_NSD_GTLS_FILENAME "lmnsd_gtls"

#endif /* #ifndef INCLUDED_NSD_GTLS_H */
