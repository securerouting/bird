/*
 *	BIRD -- The Border Gateway Protocol
 *
 *	(c) 2011 SPARTA, Inc.
 *
 *	XXX: License
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

