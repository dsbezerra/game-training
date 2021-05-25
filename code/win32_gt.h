/* date = May 24th 2021 11:49 pm */

struct Platform_Work_Queue_Entry {
    Platform_Work_Queue_Callback *callback;
    void *data;
};

struct Platform_Work_Queue {
    u32 volatile completion_goal;
    u32 volatile completion_count;
    
    u32 volatile next_entry_to_read;
    u32 volatile next_entry_to_write;
    HANDLE semaphore_handle;
    
    Platform_Work_Queue_Entry entries[256];
};
