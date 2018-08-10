
#include "lmdb.h"
#include "crypto.h"
#include "ore_blk.h"
#include "aes.h"
#include "errors.h"
#include <stdio.h>

static void print_b64(byte *data, int size) {
  if (DO_DEBUG) {
    BIO *bio, *b64;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(stdout, BIO_NOCLOSE);
    BIO_push(b64, bio);
    BIO_write(b64, data, size);
    BIO_flush(b64);

    BIO_free_all(b64);
  }
}

static void print_ct_b64(ore_blk_ciphertext ct) {
  DEBUG("LEFT: ");
  print_b64(ct->comp_left, ct->left_size);
  DEBUG("RIGHT: ");
  print_b64(ct->comp_right, ct->right_size);
}

int mdb_ore_blk_compare(const MDB_val *a, const MDB_val *b) {
  ore_blk_params params;
  int nbits = 32;
  int block_len = 8;
  int result = 0;

  ore_blk_ciphertext cipher_text_a;
  ore_blk_ciphertext cipher_text_b;

  // TODO: Check the return state (like the CHECK macro)
  init_ore_blk_params(params, nbits, block_len);

  // TODO: Check states
  init_ore_blk_ciphertext(cipher_text_a, params);
  init_ore_blk_ciphertext(cipher_text_b, params);

  // TODO: Check that a->mv_size = left_size + right_size
  
  byte *a_left  = (byte *)a->mv_data;
  byte *a_right = (byte *)a->mv_data + cipher_text_a->left_size;
  byte *b_left  = (byte *)b->mv_data;
  byte *b_right = (byte *)b->mv_data + cipher_text_b->left_size;

  memcpy(cipher_text_a->comp_left, a_left, cipher_text_a->left_size);
  memcpy(cipher_text_a->comp_right, a_right, cipher_text_a->right_size);
  memcpy(cipher_text_b->comp_left, b_left, cipher_text_b->left_size);
  memcpy(cipher_text_b->comp_right, b_right, cipher_text_b->right_size);

  DEBUG("CTS in compare\n");
  print_ct_b64(cipher_text_a);
  print_ct_b64(cipher_text_b);

  /* Initialise the cipher text structs with passed values, probs should create a function to do this in ore_blk */

  // TODO: CHECK(ore_blk...)
  if (ore_blk_compare(&result, cipher_text_a, cipher_text_b) != ERROR_NONE) {
    // TODO: Free here!
    // What else to do??
    return -1;
  }

  DEBUG("Compare result: %d\n", result);

  // TODO: Free the memories!

  return result;
}
