#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dag_native.h"

// FOSS Edition - Directed Acyclic Graph File I/O Backend
// This provides the C implementation for the @C_Native calls in core::dag.axi

struct DagNode __native_dag_query(const char* uri) {
    struct DagNode node;
    memset(&node, 0, sizeof(struct DagNode));
    
    // uri is typically the hash (e.g. "a1b2c3d4...")
    char filepath[512];
    snprintf(filepath, sizeof(filepath), ".axi/objects/%s", uri);
    
    FILE* file = fopen(filepath, "r");
    if (!file) {
        printf("[DAG Error] Could not find object %s\n", uri);
        node.hash = "ERROR_NOT_FOUND";
        return node;
    }
    
    // In a full implementation, we parse the YAML file here.
    // For now, we simulate reading the basic metadata.
    node.hash = strdup(uri);
    node.timestamp = (float)time(NULL);
    node.yaml_content = strdup("{ \"status\": \"loaded from YAML ledger\" }");
    node.spatial_x = 0.0f;
    node.spatial_y = 0.0f;
    node.spatial_z = 0.0f;
    
    fclose(file);
    return node;
}

const char* __native_dag_commit(const char* hash, const char* namespace_name, const char* toon_payload) {
    char filepath[512];
    snprintf(filepath, sizeof(filepath), ".axi/objects/%s", hash);
    
    FILE* file = fopen(filepath, "w");
    if (!file) {
        printf("[DAG Error] Could not write object %s to ledger.\n", hash);
        return "ERROR_WRITE_FAILED";
    }
    
    fprintf(file, "commit_node:\n");
    fprintf(file, "  type: commit\n");
    fprintf(file, "  hash: \"%s\"\n", hash);
    fprintf(file, "  namespace: \"%s\"\n", namespace_name);
    fprintf(file, "semantic_layer[1]:\n");
    fprintf(file, "  %s\n", toon_payload);
    
    fclose(file);
    
    // Update HEAD ref
    FILE* head = fopen(".axi/refs/heads/main", "w");
    if (head) {
        fprintf(head, "%s", hash);
        fclose(head);
    }
    
    return strdup(hash);
}

const char* __native_json_to_toon(const char* raw_json) {
    // FOSS version just wraps it as a raw string for now
    // Advanced AST tokenization is handled by the internal engine
    return strdup(raw_json);
}

