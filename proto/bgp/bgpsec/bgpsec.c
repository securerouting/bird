/*
 *	BIRD -- The Border Gateway Protocol
 *
 *
 *      Parsons, Inc.
 *      (c) 2013-2013
 *
 *	Can be used under either license:
 *      - Freely distributed and used under the terms of the GNU GPLv2.
 *      - Freely distributed and used under a BSD license, See README.bgpsec.
 */

#include "bgpsec.h"

void bgpsec_init(void) { }
void bgpsec_start(void) { }
void bgpsec_shutdown(void) { }
void bgpsec_cleanup(void) { }
void bgpsec_reconfigure(void) { }
void bgpsec_copy_config(void) { }
void bgpsec_get_status(void) { }
void bgpsec_get_attr(void) { }
void bgpsec_get_route_info(void) { }
void bgpsec_show_proto_info(void) { }

void
bgpsec_check_config(struct bgpsec_config *c)
{
}

struct protocol proto_bgpsec = {
  name:			"BGPSEC",
  template:		"bgpsec%d",
  attr_class:		EAP_BGPSEC,
  init:			bgpsec_init,
  start:		bgpsec_start,
  shutdown:		bgpsec_shutdown,
  cleanup:		bgpsec_cleanup,
  reconfigure:		bgpsec_reconfigure,
  copy_config:		bgpsec_copy_config,
  get_status:		bgpsec_get_status,
  get_attr:		bgpsec_get_attr,
  get_route_info:	bgpsec_get_route_info,
  show_proto_info:	bgpsec_show_proto_info
};
