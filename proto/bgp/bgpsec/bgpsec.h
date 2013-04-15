/*
 *	BIRD -- The Border Gateway Protocol
 *
 *
 *      Parsons, Inc.
 *      (c) 2013-2013
 *
 *	Code can be used under either license:
 *      - Freely distributed and used under the terms of the GNU GPLv2.
 *      - Freely distributed and used under a BSD license, See README.bgpsec.
 */

#ifndef _BIRD_BGPSEC_H_
#define _BIRD_BGPSEC_H_

#include <stdint.h>
#include "nest/route.h"
#include "nest/protocol.h"

struct bgpsec_config {
  struct proto_config c;
};

struct bgpsec_proto {
   struct proto p;
   struct bgpsec_config *cf;
};

#endif

