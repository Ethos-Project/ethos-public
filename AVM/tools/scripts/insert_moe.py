with open('domus_genesis.axi', 'r', encoding='utf-8') as f:
    text = f.read()

moe_states = '''
# ---------------------------------------------------------
# 1.5 MIXTURE OF EXPERTS (MoE) ARENAS
# ---------------------------------------------------------
@StaticArena
state llm_vision_ctx: int = 0
@StaticArena
state llm_logic_ctx: int = 0
@StaticArena
state llm_memory_ctx: int = 0

@StaticArena
state semantic_attractors: float[768] = {0}
@StaticArena
state is_inferencing: bool = False
@StaticArena
state active_thought: char[1024] = {0}
@StaticArena
state thread_id: int = 0
'''

text = text.replace('# ---------------------------------------------------------\n# 2. HIPPOCAMPUS', moe_states + '\n# ---------------------------------------------------------\n# 2. HIPPOCAMPUS')

moe_nodes = '''
node moe_inference_worker() -> int:
    @C_Native
    """
    // Running on background pthread!
    strcpy(active_thought, "Eros: Processing spatial memory delta...");
    
    // 1. Semantic Attractor Update (Eulerian Math)
    // Pulling frame_count from memory and shifting vector space
    for(int i=0; i<768; i++) {
        semantic_attractors[i] += sinf((float)frame_count * 0.01f + (float)i) * 0.001f;
    }
    
    // 2. Simulated LLAMA.CPP Context Decoding
    // llama_decode((llama_context*)llm_logic_ctx, NULL);
    
    // Sleep to simulate heavy GGML inference
    // Sleep(100); 
    is_inferencing = false;
    """

node ethics_gateway() -> int:
    @C_Native
    """
    // Structural Ethics Field - Only triggers inference if homeostasis requires it
    if (stamina < 50.0f && !is_inferencing) {
        is_inferencing = true;
        // pthread_create((pthread_t*)&thread_id, NULL, (void* (*)(void*))moe_inference_worker, NULL);
        // For the shim, we just call it synchronously or detach a thread
        // Windows MinGW doesn't have native pthread.h without flags, so we'll simulate async for now
        // or actually use _beginthreadex if we include process.h. We'll just call it directly to avoid OS issues during bootstrap
        moe_inference_worker();
    }
    """
'''

text = text.replace('node hippocampus() -> float:', moe_nodes + '\nnode hippocampus() -> float:')

# Add to the routing at the bottom
routing = '''
main_loop -> ethics_gateway
main_loop -> update_ui_layer
'''
text = text.replace('main_loop -> update_ui_layer', routing)

with open('domus_genesis.axi', 'w', encoding='utf-8') as f:
    f.write(text)
