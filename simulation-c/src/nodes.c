#include "nodes.h"

node * NodeCreate(int id, adaptFunciton * adaptArray, size_t adaptNumber) {
    node * newNode = malloc(sizeof(node));
    newNode->id = id;
    newNode->adaptNumber = adaptNumber;
    newNode->adaptArray = adaptArray;
    // creation of the routign table
    newNode->table = NULL;
    // construction of in and out
    size_t inCount = 0, outCount = 0;
}
