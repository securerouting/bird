/*
 *	BIRD -- The Resource Public Key Infrastructure (RPKI) to Router Protocol
 *
 *	Using RTRLib: http://rpki.realmv6.org/
 *
 *	(c) 2015 CZ.NIC
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

/**
 * DOC: The Resource Public Key Infrastructure (RPKI) to Router Protocol
 */

#define LOCAL_DEBUG

#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dlfcn.h>

#include "proto/rpki/rpki.h"
#include "lib/socket.h"
#include "lib/ip.h"
#include "nest/route.h"

#define RPKI_LOG_ADD "add"
#define RPKI_LOG_DEL "del"
#define RPKI_LOG_ENTRY_FMT(ip_fmt) " roa " ip_fmt "/%u max %u as %u"
#define RPKI_LOG_FMT(operation_name) operation_name RPKI_LOG_ENTRY_FMT("%I")
#define RPKI_LOG_SKIP_FMT(operation_name) "skipped (other IP version than BIRD) " operation_name RPKI_LOG_ENTRY_FMT("%s")

static inline const char *
get_rtr_socket_ident(const struct rtr_socket *socket)
{
  return socket->tr_socket->ident_fp(socket->tr_socket->socket);
}
#define RPKI_CACHE_TRACE(p, rtr_socket, msg, args...) RPKI_TRACE(p, "%s " msg, get_rtr_socket_ident(rtr_socket), ## args);
#define RPKI_CACHE_ERROR(p, rtr_socket, msg, args...) RPKI_ERROR(p, "%s " msg, get_rtr_socket_ident(rtr_socket), ## args);

struct rpki_entry {
  node n;
  u32 asn;
  ip_addr ip;
  u8 pxlen;
  u8 maxlen;
  u8 added;
};

void pipe_drain(int fd); 	/* implementation in io.c */
void pipe_kick(int fd); 	/* implementation in io.c */

static list rpki_proto_list;

/* RTRLib and function pointers */
static void *rtrlib;
static struct rtr_mgr_config * (*rtr_mgr_init_fp)(
    struct rtr_mgr_group groups[], const unsigned int groups_len,
    const unsigned int refresh_interval, const unsigned int expire_interval,
    const void *update_fp,
    const void *spki_update_fp,
    const void *status_fp,
    void *status_fp_data);
static int (*rtr_mgr_start_fp)(struct rtr_mgr_config *config);
static const char * (*rtr_state_to_str_fp)(enum rtr_socket_state state);
static const char * (*rtr_mgr_status_to_str_fp)(enum rtr_mgr_status status);
static int (*tr_tcp_init_fp)(const struct tr_tcp_config *config, struct tr_socket *socket);
static void (*tr_free_fp)(struct tr_socket *tr_sock);
static void (*rtr_mgr_stop_fp)(struct rtr_mgr_config *config);
static void (*rtr_mgr_free_fp)(struct rtr_mgr_config *config);

/*
 * Try load system shared library RTRLib
 * Return NULL pointer if successful
 * Otherwise return a pointer to a description of the error
 */
char *
rpki_load_rtrlib(void)
{
  char *err_buf = NULL;

  if (rtrlib != NULL)
    return NULL; /* OK, rtrlib is loaded already */

  const char *rtrlib_name = RPKI_LIBRTR_DEFAULT;

#ifdef LIBRTR
  rtrlib_name = LIBRTR; /* use a compile variable */
#endif

  rtrlib = dlopen(rtrlib_name, RTLD_LAZY);
  if (!rtrlib)
  {
    /* This could be pretty frequent problem */
    char *help_msg = "Try recompile BIRD with CFLAGS='-DLIBRTR=\\\"/path/to/librtr.so\\\"' or see BIRD User's Guide for more information.";
    err_buf = mb_alloc(&root_pool, 512);
    bsnprintf(err_buf, 512, "%s. %s", dlerror(), help_msg);
    return err_buf;
  }

  dlerror();    /* clear any existing error */

  rtr_mgr_init_fp = (struct rtr_mgr_config * (*)(
      struct rtr_mgr_group groups[], const unsigned int groups_len,
      const unsigned int refresh_interval, const unsigned int expire_interval,
      const void *update_fp,
      const void *spki_update_fp,
      const void *status_fp,
      void *status_fp_data)) dlsym(rtrlib, "rtr_mgr_init");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  rtr_mgr_start_fp = (int (*)(struct rtr_mgr_config *)) dlsym(rtrlib, "rtr_mgr_start");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  rtr_state_to_str_fp = (const char * (*)(enum rtr_socket_state state)) dlsym(rtrlib, "rtr_state_to_str");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  rtr_mgr_status_to_str_fp = (const char * (*)(enum rtr_mgr_status status)) dlsym(rtrlib, "rtr_mgr_status_to_str");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  tr_tcp_init_fp = (int (*)(const struct tr_tcp_config *config, struct tr_socket *socket)) dlsym(rtrlib, "tr_tcp_init");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  tr_free_fp = (void (*)(struct tr_socket *)) dlsym(rtrlib, "tr_free");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  rtr_mgr_stop_fp = (void (*)(struct rtr_mgr_config *config)) dlsym(rtrlib, "rtr_mgr_stop");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  rtr_mgr_free_fp = (void (*)(struct rtr_mgr_config *config)) dlsym(rtrlib, "rtr_mgr_free");
  if ((err_buf = dlerror()) != NULL)
    return err_buf;

  return NULL; /* OK */
}

void
rpki_init_all(void)
{
  init_list(&rpki_proto_list);
  rtrlib = NULL;
}


static void
rtr_thread_status_hook(const struct rtr_mgr_group *group, enum rtr_mgr_status status, const struct rtr_socket *socket, void *data)
{
  struct rpki_proto *p = data;

#define RPKI_STATUS_CB_LOG_FMT "%s - %s"

  if (status == RTR_MGR_ERROR)
  {
    RPKI_CACHE_ERROR(p, socket, RPKI_STATUS_CB_LOG_FMT, (*rtr_mgr_status_to_str_fp)(status), (*rtr_state_to_str_fp)(socket->state));
  }
  else
  {
    RPKI_CACHE_TRACE(p, socket, RPKI_STATUS_CB_LOG_FMT, (*rtr_mgr_status_to_str_fp)(status), (*rtr_state_to_str_fp)(socket->state));
  }
}

static void
log_skipped_entry(struct rpki_proto *p, const struct pfx_record *rec, const bool added)
{
  char ip_buf[INET6_ADDRSTRLEN];
  ip4_addr ip4;
  ip6_addr ip6;

  if (rec->prefix.ver == RTRLIB_IPV4)
  {
    ip4 = ip4_from_u32(rec->prefix.u.addr4.addr);
    ip4_ntop(ip4, ip_buf);
  }
  else
  {
    ip6 = ip6_build(rec->prefix.u.addr6.addr[0], rec->prefix.u.addr6.addr[1], rec->prefix.u.addr6.addr[2], rec->prefix.u.addr6.addr[3]);
    ip6_ntop(ip6, ip_buf);
  }

  if (added)
  {
    RPKI_CACHE_TRACE(p, rec->socket, RPKI_LOG_SKIP_FMT(RPKI_LOG_ADD), ip_buf, rec->min_len, rec->max_len, rec->asn);
  }
  else
  {
    RPKI_CACHE_TRACE(p, rec->socket, RPKI_LOG_SKIP_FMT(RPKI_LOG_DEL), ip_buf, rec->min_len, rec->max_len, rec->asn);
  }
}

/*
 * Return (struct rpki_proto *) or NULL
 */
static struct rpki_proto *
get_rpki_proto_by_rtr_socket(const struct rtr_socket *socket)
{
  struct rpki_proto *p_not_skipped_back;
  unsigned int i, j;

  WALK_LIST(p_not_skipped_back, rpki_proto_list)
  {
    struct rpki_proto *p = SKIP_BACK(struct rpki_proto, rpki_node, p_not_skipped_back);

    for(i = 0; i < p->rtr_conf->len; i++)
    {
      for(j = 0; j < p->rtr_conf->groups[i].sockets_len; j++)
      {
	if (socket == p->rtr_conf->groups[i].sockets[j])
	  return p;
      }
    }
  }
  return NULL; /* FAIL */
}

static void
send_data_to_main_thread(struct rpki_proto *p, struct rpki_entry *e)
{
  rpki_lock_notify(p);
  add_tail(&p->notify_list, &e->n);
  rpki_unlock_notify(p);
  pipe_kick(p->notify_write_sk->fd);
}

static void
rtr_thread_update_hook(void *pfx_table, const struct pfx_record rec, const bool added)
{
  struct rpki_proto *p = get_rpki_proto_by_rtr_socket(rec.socket);

  /* process only records that are the same with BIRD IP version */
#ifdef IPV6
  if (rec.prefix.ver != RTRLIB_IPV6)
  {
    log_skipped_entry(p, &rec, added);
    return;
  }

  ip_addr ip = ip6_build(rec.prefix.u.addr6.addr[0], rec.prefix.u.addr6.addr[1], rec.prefix.u.addr6.addr[2], rec.prefix.u.addr6.addr[3]);
#else
  if (rec.prefix.ver != RTRLIB_IPV4)
  {
    log_skipped_entry(p, &rec, added);
    return;
  }

  ip_addr ip = ip4_from_u32(rec.prefix.u.addr4.addr);
#endif

  /* TODO: Make more effective solution with thread-safe pool/queue of rpki_entry structures
   *       without endless allocations and frees */
  struct rpki_entry *e = mb_allocz(p->p.pool, sizeof(struct rpki_entry));
  e->added = added;
  e->asn = rec.asn;
  e->ip = ip;
  e->pxlen = rec.min_len;
  e->maxlen = rec.max_len;

  if (e->added)
  {
    RPKI_CACHE_TRACE(p, rec.socket, RPKI_LOG_FMT(RPKI_LOG_ADD), e->ip, e->pxlen, e->maxlen, e->asn);
  }
  else
  {
    RPKI_CACHE_TRACE(p, rec.socket, RPKI_LOG_FMT(RPKI_LOG_DEL), e->ip, e->pxlen, e->maxlen, e->asn);
  }

  send_data_to_main_thread(p, e);
}

static struct proto *
rpki_init(struct proto_config *C)
{
  struct proto *P = proto_new(C, sizeof(struct rpki_proto));
  struct rpki_proto *p = (struct rpki_proto *) P;
  p->cf = (struct rpki_config *) C;

  RPKI_TRACE(p, "------------- rpki_init -------------");

  return P;
}

struct rpki_cache *
rpki_new_cache(void)
{
  struct rpki_cache *cache = (struct rpki_cache *)cfg_allocz(sizeof(struct rpki_cache));
  strcpy(cache->port, RPKI_PORT);
  cache->preference = RPKI_DEFAULT_CACHE_PREFERENCE;
  return cache;
}

static int
recv_data_in_main_thread(struct birdsock *sk, int size)
{
  struct rpki_proto *p = sk->data;
  struct rpki_entry *e;
  list tmp_list;

  pipe_drain(sk->fd);

  rpki_lock_notify(p);
  init_list(&tmp_list);
  add_tail_list(&tmp_list, &p->notify_list);
  init_list(&p->notify_list);
  rpki_unlock_notify(p);

  WALK_LIST_FIRST(e, tmp_list)
  {
    rpki_lock_notify(p);
    rem2_node(&e->n);
    rpki_unlock_notify(p);

    if (e->added)
      roa_add_item(p->cf->roa_table_cf->table, e->ip, e->pxlen, e->maxlen, e->asn, ROA_SRC_RPKI);
    else
      roa_delete_item(p->cf->roa_table_cf->table, e->ip, e->pxlen, e->maxlen, e->asn, ROA_SRC_RPKI);
    mb_free(e);
  }
}

static void
recv_err_in_main_thread(struct birdsock *sk, int err)
{
  struct rpki_proto *p = sk->data;
  RPKI_ERROR(p, "Notify socket error: %m", err);
}

static sock *
create_socket(struct rpki_proto *p, int fd)
{
  sock *sk = sk_new(p->p.pool);
  sk->type = SK_MAGIC;
  sk->fd = fd;
  sk->data = p;
  return sk;
}

static void
create_read_socket(struct rpki_proto *p, int fd)
{
  sock *sk = create_socket(p, fd);
  sk->rx_hook = recv_data_in_main_thread;
  sk->err_hook = recv_err_in_main_thread;
  if (sk_open(sk) < 0)
    RPKI_DIE(p, "read socket sk_open() failed");
  p->notify_read_sk = sk;
}

static void
create_write_socket(struct rpki_proto *p, int fd)
{
  sock *sk = create_socket(p, fd);
  sk->flags = SKF_THREAD;
  if (sk_open(sk) < 0)
    RPKI_DIE(p, "write socket sk_open() failed");
  p->notify_write_sk = sk;
}

static void
create_rw_sockets(struct rpki_proto *p)
{
  int pipe_fildes[2];

  int rv = pipe(pipe_fildes);
  if (rv < 0)
    RPKI_DIE(p, "pipe: %m");

  create_read_socket (p, pipe_fildes[0]);
  create_write_socket(p, pipe_fildes[1]);
}

static uint
count_number_of_various_preferences(list *cache_list)
{
  uint i;
  u8 preference[256];
  bzero(preference, sizeof(preference));

  struct rpki_cache *cache;
  WALK_LIST(cache, *cache_list)
  {
    preference[cache->preference]++;
  }

  uint count = 0;
  for (i = 0; i < 256; i++)
  {
    if (preference[i])
      count++;
  }
  return count;
}

static uint
count_number_of_caches_with_specific_preference(list *cache_list, uint preference)
{
  uint count = 0;

  struct rpki_cache *cache;
  WALK_LIST(cache, *cache_list)
  {
    if (cache->preference == preference)
      count++;
  }

  return count;
}

static struct rtr_socket *
create_rtrlib_tcp_socket(struct rpki_cache *cache, pool *pool)
{
  struct rtr_socket *rtrlib_tcp = mb_allocz(pool, sizeof(struct rtr_socket));
  rtrlib_tcp->tr_socket = mb_allocz(pool, sizeof(struct tr_socket));
  struct tr_tcp_config tcp_config = {
      .host = cache->host,
      .port = cache->port
  };

  (*tr_tcp_init_fp)(&tcp_config, rtrlib_tcp->tr_socket);

  return rtrlib_tcp;
}

struct rtr_mgr_group_crate {
  struct rtr_mgr_group *groups;
  uint groups_len;
};

static struct rtr_mgr_group_crate
group_cache_list_by_preferences(list *cache_list, pool *pool)
{
  u8 completed_preference[256];
  bzero(completed_preference, sizeof(completed_preference));

  uint groups_len = count_number_of_various_preferences(cache_list);
  struct rtr_mgr_group *groups = mb_allocz(pool, groups_len * sizeof(struct rtr_mgr_group));

  uint group_idx = 0;
  struct rpki_cache *first_cache_in_group;
  WALK_LIST(first_cache_in_group, *cache_list)
  {
    if (completed_preference[first_cache_in_group->preference])
      continue;
    completed_preference[first_cache_in_group->preference] = 1;

    struct rtr_mgr_group *group = &groups[group_idx];

    group->preference = first_cache_in_group->preference;
    group->sockets_len = count_number_of_caches_with_specific_preference(cache_list, first_cache_in_group->preference);
    group->sockets = mb_allocz(pool, group->sockets_len * sizeof(struct rtr_socket *));

    uint socket_idx = 0;
    struct rpki_cache *cache;
    WALK_LIST(cache, *cache_list)
    {
      if (cache->preference == groups[group_idx].preference)
      {
	group->sockets[socket_idx] = cache->rtr_tcp = create_rtrlib_tcp_socket(cache, pool);
	socket_idx++;
      }
    }
    group_idx++;
  }

  struct rtr_mgr_group_crate grouped_list = {
      .groups = groups,
      .groups_len = groups_len
  };
  return grouped_list;
}

static int
rpki_start(struct proto *P)
{
  struct rpki_proto *p = (struct rpki_proto *) P;
  struct rpki_config *cf = (struct rpki_config *) (P->cf);

  create_rw_sockets(p);
  init_list(&p->notify_list);
  pthread_mutex_init(&p->notify_lock, NULL);

  add_tail(&rpki_proto_list, &p->rpki_node);

  struct rtr_mgr_group_crate grouped_list = group_cache_list_by_preferences(&cf->cache_list, P->pool);

  p->rtr_conf = (*rtr_mgr_init_fp)(grouped_list.groups, grouped_list.groups_len, 30, 520, &rtr_thread_update_hook, NULL, &rtr_thread_status_hook, p);
  (*rtr_mgr_start_fp)(p->rtr_conf);

  return PS_UP;
}

static int
rpki_shutdown(struct proto *P)
{
  struct rpki_proto *p = (struct rpki_proto *) P;

  (*rtr_mgr_stop_fp)(p->rtr_conf);
  (*rtr_mgr_free_fp)(p->rtr_conf);

  struct rpki_cache *cache;
  WALK_LIST(cache, p->cf->cache_list)
  {
    (*tr_free_fp)(cache->rtr_tcp->tr_socket);
    mb_free(cache->rtr_tcp->tr_socket);
    mb_free(cache->rtr_tcp);
    if (cache->ip_buf)
      mb_free(cache->ip_buf);
  }

  mb_free(p->rtr_sockets);
  mb_free(p->rtr_groups);

  pthread_mutex_destroy(&p->notify_lock);

  return PS_DOWN;
}

static int
rpki_reconfigure(struct proto *p, struct proto_config *c)
{
  struct rpki_proto *rpki = (struct rpki_proto *) p;
  struct rpki_config *new = (struct rpki_config *) c;

  log(L_DEBUG "------------- rpki_reconfigure -------------");

  return 1;
}

static void
rpki_copy_config(struct proto_config *dest, struct proto_config *src)
{
  struct rpki_config *d = (struct rpki_config *) dest;
  struct rpki_config *s = (struct rpki_config *) src;

  log(L_DEBUG "------------- rpki_copy_config -------------");
}

static void
rpki_get_status(struct proto *p, byte *buf)
{
  struct proto_rpki *rpki = (struct proto_rpki *) p;

  log(L_DEBUG "------------- rpki_get_status -------------");
}

struct protocol proto_rpki = {
  .name = 		"RPKI",
  .template = 		"rpki%d",
//  .attr_class = 	EAP_BGP,
//  .preference = 	DEF_PREF_BGP,
  .config_size =	sizeof(struct rpki_config),
  .init = 		rpki_init,
  .start = 		rpki_start,
  .shutdown = 		rpki_shutdown,
//  .cleanup = 		rpki_cleanup,
  .reconfigure = 	rpki_reconfigure,
  .copy_config = 	rpki_copy_config,
  .get_status = 	rpki_get_status,
//  .get_attr = 		rpki_get_attr,
//  .get_route_info = 	rpki_get_route_info,
//  .show_proto_info = 	rpki_show_proto_info
};