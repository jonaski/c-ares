#include "ares-test.h"
#include "dns-proto.h"

extern "C" {
// Remove command-line defines of package variables for the test project...
#undef PACKAGE_NAME
#undef PACKAGE_BUGREPORT
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
// ... so we can include the library's config without symbol redefinitions.
#include "ares_config.h"
#include "ares_nowarn.h"
#include "ares_inet_net_pton.h"
#include "ares_data.h"
#include "bitncmp.h"
char *ares_strdup(const char*);

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
}

#include <string>
#include <vector>

namespace ares {
namespace test {

TEST_F(LibraryTest, InetPtoN) {
  struct in_addr a4;
  struct in6_addr a6;

#ifndef CARES_SYMBOL_HIDING
  uint32_t expected;
  a4.s_addr = 0;
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "1.2.3.4", &a4, sizeof(a4)));
  expected = htonl(0x01020304);
  EXPECT_EQ(expected, a4.s_addr);
  a4.s_addr = 0;
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "129.1.1.1", &a4, sizeof(a4)));
  expected = htonl(0x81010101);
  EXPECT_EQ(expected, a4.s_addr);
  a4.s_addr = 0;
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "192.1.1.1", &a4, sizeof(a4)));
  expected = htonl(0xC0010101);
  EXPECT_EQ(expected, a4.s_addr);
  a4.s_addr = 0;
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "224.1.1.1", &a4, sizeof(a4)));
  expected = htonl(0xE0010101);
  EXPECT_EQ(expected, a4.s_addr);
  a4.s_addr = 0;
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "225.1.1.1", &a4, sizeof(a4)));
  expected = htonl(0xE1010101);
  EXPECT_EQ(expected, a4.s_addr);
  a4.s_addr = 0;
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "240.1.1.1", &a4, sizeof(a4)));
  expected = htonl(0xF0010101);
  EXPECT_EQ(expected, a4.s_addr);

  EXPECT_EQ(16 * 8, ares_inet_net_pton(AF_INET6, "12:34::ff", &a6, sizeof(a6)));
  EXPECT_EQ(16 * 8, ares_inet_net_pton(AF_INET6, "12:34::ffff:1.2.3.4", &a6, sizeof(a6)));
  a4.s_addr = 0;
  EXPECT_EQ(3 * 8, ares_inet_net_pton(AF_INET, "1.2.3.4/24", &a4, sizeof(a4)));
  expected = htonl(0x01020304);
  EXPECT_EQ(expected, a4.s_addr);
  a4.s_addr = 0;
  EXPECT_EQ(3 * 8, ares_inet_net_pton(AF_INET, "1.2.3/24", &a4, sizeof(a4)));
  expected = htonl(0x01020300);
  EXPECT_EQ(expected, a4.s_addr);
  a4.s_addr = 0;
  EXPECT_EQ(2 * 8, ares_inet_net_pton(AF_INET, "0x0102/16", &a4, sizeof(a4)));
  expected = htonl(0x01020000);
  EXPECT_EQ(expected, a4.s_addr);
  EXPECT_EQ(3 * 8, ares_inet_net_pton(AF_INET6, "12:34::ff/24", &a6, sizeof(a6)));

  // Various malformed versions
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "", &a4, sizeof(a4)));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET6, "", &a6, sizeof(a6)));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "xyzzy", &a4, sizeof(a4)));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET+AF_INET6, "1.2.3.4", &a4, sizeof(a4)));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "257.2.3.4", &a4, sizeof(a4)));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "2.3.4.x", &a4, sizeof(a4)));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "2.3.4.5.6", &a4, sizeof(a4)));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "2.3.4:5", &a4, sizeof(a4)));

  // Hex constants are allowed.
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "0x01020304", &a4, sizeof(a4)));
  expected = htonl(0x01020304);
  EXPECT_EQ(expected, a4.s_addr);
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "0x0a0b0c0d", &a4, sizeof(a4)));
  expected = htonl(0x0a0b0c0d);
  EXPECT_EQ(expected, a4.s_addr);
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "0x0A0B0C0D", &a4, sizeof(a4)));
  expected = htonl(0x0a0b0c0d);
  EXPECT_EQ(expected, a4.s_addr);
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "0x0xyz", &a4, sizeof(a4)));
  EXPECT_EQ(4 * 8, ares_inet_net_pton(AF_INET, "0x1122334", &a4, sizeof(a4)));
  expected = htonl(0x11223340);
  EXPECT_EQ(expected, a4.s_addr);  // huh?

  // No room, no room.
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "1.2.3.4", &a4, sizeof(a4) - 1));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET6, "12:34::ff", &a6, sizeof(a6) - 1));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "0x01020304", &a4, 2));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "0x01020304", &a4, 0));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "0x0a0b0c0d", &a4, 0));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "0x0xyz", &a4, 0));
  EXPECT_EQ(-1, ares_inet_net_pton(AF_INET, "0x1122334", &a4, sizeof(a4) - 1));
#endif

  EXPECT_EQ(1, ares_inet_pton(AF_INET, "1.2.3.4", &a4));
  EXPECT_EQ(1, ares_inet_pton(AF_INET6, "12:34::ff", &a6));
  EXPECT_EQ(1, ares_inet_pton(AF_INET6, "12:34::ffff:1.2.3.4", &a6));
  EXPECT_EQ(0, ares_inet_pton(AF_INET, "xyzzy", &a4));
  EXPECT_EQ(-1, ares_inet_pton(AF_INET+AF_INET6, "1.2.3.4", &a4));
}

TEST_F(LibraryTest, FreeCorruptData) {
  // ares_free_data(p) expects that there is a type field and a marker
  // field in the memory before p.  Feed it incorrect versions of each.
  struct ares_data *data = (struct ares_data *)malloc(sizeof(struct ares_data));
  void* p = &(data->data);

  // Invalid type
  data->type = (ares_datatype)99;
  data->mark = ARES_DATATYPE_MARK;
  ares_free_data(p);

  // Invalid marker
  data->type = (ares_datatype)ARES_DATATYPE_MX_REPLY;
  data->mark = ARES_DATATYPE_MARK + 1;
  ares_free_data(p);

  // Null pointer
  ares_free_data(nullptr);

  free(data);
}

#ifndef CARES_SYMBOL_HIDING
TEST(LibraryInit, StrdupFailures) {
  EXPECT_EQ(ARES_SUCCESS, ares_library_init(ARES_LIB_INIT_ALL));
  char* copy = ares_strdup("string");
  EXPECT_NE(nullptr, copy);
  free(copy);
  ares_library_cleanup();
}

TEST_F(LibraryTest, StrdupFailures) {
  SetAllocFail(1);
  char* copy = ares_strdup("string");
  EXPECT_EQ(nullptr, copy);
}

TEST_F(LibraryTest, MallocDataFail) {
  EXPECT_EQ(nullptr, ares_malloc_data((ares_datatype)99));
  SetAllocSizeFail(sizeof(struct ares_data));
  EXPECT_EQ(nullptr, ares_malloc_data(ARES_DATATYPE_MX_REPLY));
}

TEST(Misc, Bitncmp) {
  byte a[4] = {0x80, 0x01, 0x02, 0x03};
  byte b[4] = {0x80, 0x01, 0x02, 0x04};
  byte c[4] = {0x01, 0xFF, 0x80, 0x02};
  EXPECT_GT(0, ares__bitncmp(a, b, sizeof(a)*8));
  EXPECT_LT(0, ares__bitncmp(b, a, sizeof(a)*8));
  EXPECT_EQ(0, ares__bitncmp(a, a, sizeof(a)*8));

  for (int ii = 1; ii < (3*8+5); ii++) {
    EXPECT_EQ(0, ares__bitncmp(a, b, ii));
    EXPECT_EQ(0, ares__bitncmp(b, a, ii));
    EXPECT_LT(0, ares__bitncmp(a, c, ii));
    EXPECT_GT(0, ares__bitncmp(c, a, ii));
  }

  // Last byte differs at 5th bit
  EXPECT_EQ(0, ares__bitncmp(a, b, 3*8 + 3));
  EXPECT_EQ(0, ares__bitncmp(a, b, 3*8 + 4));
  EXPECT_EQ(0, ares__bitncmp(a, b, 3*8 + 5));
  EXPECT_GT(0, ares__bitncmp(a, b, 3*8 + 6));
  EXPECT_GT(0, ares__bitncmp(a, b, 3*8 + 7));
}

TEST_F(LibraryTest, Casts) {
  ssize_t ssz = 100;
  unsigned int u = 100;
  int i = 100;
  long l = 100;

  unsigned int ru = aresx_sztoui(ssz);
  EXPECT_EQ(u, ru);
  int ri = aresx_sztosi(ssz);
  EXPECT_EQ(i, ri);

  ri = aresx_sltosi(l);
  EXPECT_EQ(l, (long)ri);
}
#endif

}  // namespace test
}  // namespace ares