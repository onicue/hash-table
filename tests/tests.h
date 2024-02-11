#pragma once

#include <hmap.h>

#ifndef TestType
#define TestType int
#endif

#ifndef TestValue
#define TestValue 10
#endif


#ifndef ChangeTestValue
#define ChangeTestValue 100
#endif

typedef struct {
  const char* TestName;
  int(*TestFunction)(hmap* table, const char* word);
}TestUnit;

typedef struct {
  hmap* table;
  const char* filePath;
  TestUnit* units;
  int unitsSize;
} TestSetup;

void
testing(TestSetup*);

TestSetup
initTestSetup(hmap* table, const char* filePath, TestUnit* units, int unitsSize);

int
insert_test(hmap* table, const char* word);

int
change_test(hmap* table, const char* word);

int
change_check_test(hmap* table, const char* word);

int
get_test(hmap* table, const char* word);

int
delete_test(hmap* table, const char* word);
