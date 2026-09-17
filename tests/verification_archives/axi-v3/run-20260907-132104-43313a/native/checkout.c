#include "dag_native.h"
extern struct DagNode __native_dag_checkout(const char*);
int main(void) { struct DagNode n = __native_dag_checkout("probe"); return n.hash == 0; }
