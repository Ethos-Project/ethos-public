#include <stdio.h>
#include <string.h>
#include "dag_native.h"
int main(void) {
 const char *payload = "AXI_NATIVE_ROUNDTRIP_73912";
 const char *result = __native_dag_commit("probe", "verification", payload);
 struct DagNode node = __native_dag_query("probe");
 printf("commit=%s\nquery=%s\n", result, node.toon_content ? node.toon_content : "NULL");
 if (!node.toon_content || !strstr(node.toon_content, payload)) return 2;
 return 0;
}
