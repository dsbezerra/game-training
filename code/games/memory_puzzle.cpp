
internal void
memory_puzzle_game_update_and_render(game_memory *memory, game_input *input) {
    
    memory_puzzle_state *state = (memory_puzzle_state *) memory->permanent_storage;
    if (!memory->initialized) {
        // TODO(diego): Init game state for Dogdger here.
        OutputDebugString("Memory for memory_puzzle not initialized.\n");
        memory->initialized = true;
    } else {
        OutputDebugString("Memory for memory_puzzle initialized.\n");
    }
    
}