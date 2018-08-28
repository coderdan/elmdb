#include "lmdb.h"
#include <stdio.h>
#include <stdlib.h>

#define E(expr) CHECK((rc = (expr)) == MDB_SUCCESS, #expr)
#define RES(err, expr) ((rc = expr) == (err) || (CHECK(!rc, #expr), 0))
#define CHECK(test, msg) ((test) ? (void)0 : ((void)fprintf(stderr, \
	"%s:%d: %s: %s\n", __FILE__, __LINE__, msg, mdb_strerror(rc)), abort()))

int main(int argc,char * argv[]) {
  int rc, i, j;
  MDB_env *env;
  MDB_dbi dbi;
  MDB_val key, put_data, get_data;
  MDB_txn *txn;
  MDB_cursor *mc;
  MDB_stat stat;
  MDB_envinfo info;
  char *kval = NULL;
  char *sval = NULL;

  size_t gb = 100;

  E(mdb_env_create(&env));
  E(mdb_env_set_maxreaders(env, 1));
  E(mdb_env_set_mapsize(env, (size_t)(gb * 1073741824)));
  E(mdb_env_open(env, "./testdb", MDB_NORDAHEAD, 0664));
  printf("ENV CREATED\n");


  for (j = 0; j < 100; j++) {
    printf("SECTION %d-%d\n", j, i);

    E(mdb_txn_begin(env, NULL, 0, &txn));
    E(mdb_dbi_open(txn, NULL, 0, &dbi));
    E(mdb_cursor_open(txn, dbi, &mc));

    for (i = 0; i < 100000; i++) {
      kval = malloc(64 * sizeof(char));
      key.mv_size = 64 * sizeof(char);
      key.mv_data = kval;
      sprintf(kval, "%d-%d", j, i);

      sval = malloc(64 * sizeof(char));
      put_data.mv_size = 64 * sizeof(char);
      put_data.mv_data = sval;
      sprintf(sval, "%d-%d%--d", j, i, rand());

      //printf("key[%s]=%s\n", kval, sval);

      E(mdb_cursor_put(mc, &key, &put_data, 0));

      /*if (mdb_get(txn, dbi, &key, &get_data) == 0) {
        printf("%s => %s\n", key.mv_data, get_data.mv_data);
      } else {
        printf("%s => X\n", key.mv_data);
        abort();
      }*/
    }

    E(mdb_txn_commit(txn));
    mdb_dbi_close(env, dbi);
  }
  mdb_env_stat(env, &stat);
  mdb_env_info(env, &info);
  printf("ENV entries: %lu\n", stat.ms_entries);
  printf("ENV map size: %lu\n", info.me_mapsize);
  mdb_env_close(env);
}
