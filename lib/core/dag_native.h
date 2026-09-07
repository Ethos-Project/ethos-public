#ifndef DAG_NATIVE_H
#define DAG_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif

struct DagNode {
    const char* hash;
    float timestamp;
    const char* toon_content;
    float spatial_x;
    float spatial_y;
    float spatial_z;
    // NOTE: parents and children string arrays omitted for FOSS C layout simplicity for now
};

struct DagNode __native_dag_query(const char* uri);
const char* __native_dag_commit(const char* hash, const char* namespace_name, const char* toon_payload);
const char* __native_json_to_toon(const char* raw_json);

#ifdef __cplusplus
}
#endif

#endif // DAG_NATIVE_H
