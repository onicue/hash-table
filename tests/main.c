#include <hmap.h>
#include "tests.h"
#include <stdio.h>



TestUnit units[] = {
  {"INSERT",insert_test},
  {"GET", get_test},
  {"CHANGE", change_test},
  {"CHANGE_CHECK", change_check_test},
  {"DELETE", delete_test}
};

int main(void){
  hmap* table = hmap_init(1000, sizeof(int));
  TestSetup setup = initTestSetup(table, TXT_FILE_PATH, units, sizeof(units) / sizeof(units[0]));
  testing(&setup);
  hmap_free(table);
}
