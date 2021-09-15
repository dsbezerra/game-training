/* date = January 8th 2021 4:18 pm */

//
// Borrowed from jblow streams
//

#define NOTIFY_LEN 8000

#define RELOAD_DELAY 0.1f

//
// Directory_Info
//
struct Directory_Info {
    char *name;
    HANDLE handle;
    
    s32 bytes_returned;
    OVERLAPPED overlapped;
    FILE_NOTIFY_INFORMATION *notify_information;
    
    b32 read_issue_failed;
};

internal void issue_one_read(Directory_Info *info);
internal void release(Directory_Info *info);

//
// Asset_Change
//

struct Asset_Change {
    char *short_name;
    char *full_name;
    char *extension;
    
    real32 time_of_last_change;
};

internal void add_changed_asset(char *short_name, char *full_name, char *extension);
internal void release(Asset_Change *change);

//
// Hotloader
//
typedef void Hotloader_Callback(Asset_Change *change, b32 handled);

internal void hotloader_register_catalog(Catalog_Base *base);
internal void hotloader_register_callback(Hotloader_Callback *callback);
internal void hotloader_init();
internal b32 hotloader_process_change();
internal void hotloader_shutdown();