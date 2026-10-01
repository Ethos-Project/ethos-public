#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#pragma pack(push, 1)
typedef struct {
    char hash_id[16];           // Short simulated hash for the blueprint
    char parent_hash_id[16];    
    uint64_t timestamp;         
    uint32_t emotion_vector;    
    uint32_t payload_size;      
} ErosMemoryNode;
#pragma pack(pop)

// Simple hash generator for memory nodes
void generate_hash(const char* payload, char* output) {
    uint32_t hash = 5381;
    int c;
    while ((c = *payload++)) hash = ((hash << 5) + hash) + c; 
    sprintf(output, "%08X", hash);
}

// Commits a conversation into the .eros database
void eros_dag_commit(const char* db_path, const char* parent_hash, const char* payload, uint32_t emotion) {
    FILE* file = fopen(db_path, "ab"); // Append binary mode
    if (!file) return;

    ErosMemoryNode node = {0};
    if (parent_hash) strncpy(node.parent_hash_id, parent_hash, 15);
    else strcpy(node.parent_hash_id, "ROOT");

    generate_hash(payload, node.hash_id);
    node.timestamp = (uint64_t)time(NULL);
    node.emotion_vector = emotion;
    node.payload_size = strlen(payload) + 1; // Include null terminator

    // Write the structural node header, then immediately write the memory payload
    fwrite(&node, sizeof(ErosMemoryNode), 1, file);
    fwrite(payload, 1, node.payload_size, file);
    
    fclose(file);
    printf("[DAG COMMIT] Synapse branched. Memory '%s' written to %s\n", node.hash_id, db_path);
}

// Retrieves and traverses the .eros graph
void eros_dag_retrieve(const char* db_path) {
    FILE* file = fopen(db_path, "rb");
    if (!file) {
        printf("[DAG READ] No memory database found.\n");
        return;
    }

    printf("\n--- EROS LONG-TERM MEMORY GRAPH (%s) ---\n", db_path);
    ErosMemoryNode node;
    while (fread(&node, sizeof(ErosMemoryNode), 1, file) == 1) {
        char* payload = malloc(node.payload_size);
        fread(payload, 1, node.payload_size, file);

        printf("├─[NODE: %s] (Parent: %s)\n", node.hash_id, node.parent_hash_id);
        printf("│  Timestamp : %llu\n", node.timestamp);
        printf("│  Emotion   : %u\n", node.emotion_vector);
        printf("│  Memory    : \"%s\"\n", payload);
        printf("│\n");

        free(payload);
    }
    printf("--- END OF MEMORY GRAPH ---\n\n");
    fclose(file);
}

int main() {
    const char* db = "brain.eros";
    
    // Clear old database for the demonstration
    remove(db);
    
    // Simulate a conversation branching over time
    printf("[SYSTEM] Eros is learning and committing to long-term memory...\n");
    
    eros_dag_commit(db, "ROOT", "User is building a world-class AI platform.", 1);
    
    // Branching off the previous memory
    eros_dag_commit(db, "979C7A1D", "User wants to focus on chat and integrations first.", 2);
    
    // Changing the subject (New Branch from ROOT)
    eros_dag_commit(db, "ROOT", "User loves the concept of a DAG memory database.", 3);
    
    // Reading the graph back
    eros_dag_retrieve(db);

    return 0;
}
