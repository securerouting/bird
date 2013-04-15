/*
 *   BIRD -- The Border Gateway Protocol
 *
 *
 *      Parsons, Inc.
 *      (c) 2013-2013
 *
 *	Code can be used under either license:
 *      - Freely distributed and used under the terms of the GNU GPLv2.
 *      - Freely distributed and used under a BSD license, See README.bgpsec.
 */

CF_HDR

#include "proto/bgp/bgpsec/bgpsec.h"

CF_DEFINES

#define BGPSEC_CFG ((struct bgpsec_config *) this_proto)

CF_DECLS

CF_KEYWORDS(BGPSEC)

CF_GRAMMAR

CF_ADDTO(proto, bgpsec_proto '}' { bgpsec_check_config(BGPSEC_CFG); } )

bgpsec_proto_start: proto_start BGPSEC {
     this_proto = proto_config_new(&proto_bgpsec, sizeof(struct bgpsec_config), $1);
}
;

bgpsec_proto:
   bgpsec_proto_start proto_name '{'
 ;

CF_CODE

CF_END
