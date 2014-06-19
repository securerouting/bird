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

struct proto *	bgpsec_init(struct proto_config *c)					{}
int		bgpsec_start(struct proto *p)						{}
int		bgpsec_shutdown(struct proto *p)					{}
void		bgpsec_cleanup(struct proto *p)						{}
int		bgpsec_reconfigure(struct proto *p, struct proto_config *pc)		{}
void		bgpsec_copy_config(struct proto_config *pc1, struct proto_config *pc2)	{}
void		bgpsec_get_status(struct proto *p, byte *buf)				{}
int		bgpsec_get_attr(struct eattr *a, byte *buf, int buflen)			{}
void		bgpsec_get_route_info(struct rte *r, byte *buf, struct ea_list *attrs)	{}
void		bgpsec_show_proto_info(struct proto *p) {}

void		bgpsec_check_config(struct bgpsec_config *c)				{}

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
