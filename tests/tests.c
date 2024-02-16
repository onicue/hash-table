#include "tests.h"
#include "tools.h"

#include <time.h>
#include <stdio.h>

static void
outputTestInfo(int status, double time,
               int counter, const char* name){
  if(status){
    printf("%i Test %s was completed successfully in time %f\n",
            counter, name, time);
  }else {
    printf("%i Test %s was failed in time %f\n",
            counter, name, time);
  }
}

static int
start_test(TestUnit* unit, hmap* table, const char* filePath){
  TextFileReader* reader = initializeTextFileReader(filePath);
  const char* word;
  while((word = readWord(reader)) != NULL){
    if(!unit->TestFunction(table, word)){
      closeAndFree(reader);
      return 0;
    }
  }
  closeAndFree(reader);
  return 1;
}

void
testing(TestSetup *setup){
  for(int i = 0; i< setup->unitsSize; i++){
    clock_t startTime = clock();

    int status = start_test(&(setup->units[i]), setup->table, setup->filePath);

    clock_t endTime = clock();
    double time = ((double)(endTime - startTime) * 1000.0) / CLOCKS_PER_SEC;

    outputTestInfo(status, time, i+1, setup->units[i].TestName);
  }
}


TestSetup
initTestSetup(hmap* table, const char* filePath, TestUnit* units, int unitsSize){
  TestSetup setup = {
    .table = table,
    .filePath = filePath,
    .unitsSize = unitsSize,
    .units = units
  };
  return setup;
}

int
insert_test(hmap*table, const char* word){
  if(hmap_insert(table, word, &(TestType){TestValue})){
    return 1;
  }
  return 0;
}

int
change_test(hmap*table, const char* word){
  if(hmap_change(table, word, &(TestType){ChangeTestValue})){
    return 1;
  }
  return 0;
}

int
change_check_test(hmap*table, const char* word){
  TestType* var = (TestType*)hmap_get(table, word);
  if(var != NULL){
    if(*var == ChangeTestValue){
      return 1;
    }
  }
  return 0;
}


int
delete_test(hmap*table, const char* word){
  if(hmap_delete(table, word)){
    return 1;
  }
  return 0;
}

int
get_test(hmap *table, const char *word){
  if(hmap_get(table, word) != NULL ){
    return 1;
  }
  return 0;
}
