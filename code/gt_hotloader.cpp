
global_variable StringArray *directory_names;
global_variable Directory_Info *directories = 0;

global_variable b32 hotloader_initted = false;
global_variable Hotloader_Callback *hotloader_callback;

global_variable Asset_Change *asset_changes = 0;
global_variable Catalog_Base **catalogs = 0;


//
// Directory_Info
//

internal void
issue_one_read(Directory_Info *info) {
    DWORD result = ReadDirectoryChangesW(info->handle, info->notify_information, NOTIFY_LEN, TRUE, FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_SIZE|FILE_NOTIFY_CHANGE_LAST_WRITE, 0, &info->overlapped, 0);
    if (!result) {
        info->read_issue_failed = true;
    } else {
        info->read_issue_failed = false;
    }
}

internal void
release(Directory_Info *info) {
    platform_free(info->name);
    CloseHandle(info->handle);
    platform_free(info->notify_information);
}

//
// Asset_Change
//

internal void
add_changed_asset(char *short_name, char *full_name, char *extension) {
    Asset_Change result = {};
    result.short_name = copy_string(short_name);
    result.full_name = copy_string(full_name);
    result.extension = copy_string(extension);
    result.time_of_last_change = get_time();
    sb_push(asset_changes, result);
}

internal void
release(Asset_Change *change) {
    platform_free(change->short_name);
    platform_free(change->full_name);
    platform_free(change->extension);
}

//
// Hotloader
//

internal void
hotloader_register_catalog(Catalog_Base *base) {
    sb_push(catalogs, base);
}

internal void
hotloader_register_callback(Hotloader_Callback *callback) {
    hotloader_callback = callback;
}

internal void
hotloader_init() {
    assert(!hotloader_initted);
    hotloader_initted = true;
    
    directory_names = make_string_array(32);
    
    char *root_data_directory = "data";
    array_add_if_unique(directory_names, root_data_directory);
    
    for (int i = 0; i < directory_names->count; ++i) {
        Directory_Info info = {};
        
        info.notify_information = (FILE_NOTIFY_INFORMATION *) platform_alloc(NOTIFY_LEN);
        
#define DIR_LEN 512
        char dir_buffer[DIR_LEN];
        
        DWORD success = GetCurrentDirectory(DIR_LEN, dir_buffer);
        if (success == 0) return; // @Incomplete: Log?
        
        HANDLE event = CreateEventA(0, FALSE, FALSE, 0);
        if (event == INVALID_HANDLE_VALUE) return;
        
        info.overlapped.hEvent = event;
        info.overlapped.Offset = 0;
        info.read_issue_failed = false;
        
        char *dir_name = get_item(directory_names, i);
        if (!dir_name) return;
        
        HANDLE handle = CreateFileA(dir_name, FILE_LIST_DIRECTORY,
                                    FILE_SHARE_READ|FILE_SHARE_DELETE|FILE_SHARE_WRITE,
                                    0,
                                    OPEN_EXISTING,
                                    FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED,
                                    0);
        if (handle == INVALID_HANDLE_VALUE) {
            DWORD error_code = GetLastError();
            
            char *lpMsgBuf = 0;
            if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                0,
                                error_code,
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                                lpMsgBuf,
                                0, 0)) {
                // Handle ther error
                return;
            }
            
            continue;
        }
        
        if (handle == INVALID_HANDLE_VALUE) {
            handle = 0;
        }
        
        info.handle = handle;
        info.name = copy_string(dir_name);
        sb_push(directories, info);
    }
    
    int count = sb_count(directories);
    for (int i = 0; i < count; i++) {
        issue_one_read(&directories[i]);
    }
}

internal FILE_NOTIFY_INFORMATION *
move_info_forward(FILE_NOTIFY_INFORMATION *info) {
    if (info->NextEntryOffset == 0) return 0;
    return (FILE_NOTIFY_INFORMATION *) (((u8 *) info) + info->NextEntryOffset);
}

internal b32
extension_is_supported(StringArray *extensions, char *extension) {
    if (!extensions) return false;
    
    // Catalog extensions are presumed to be in lowercase.
    b32 found = array_find(extensions, extension);
    return found;
}

internal void
win32_pump_notifications() {
    int dir_count = sb_count(directories);
    for (int i = 0; i < dir_count; ++i) {
        Directory_Info *info = &directories[i];
        
        HANDLE handle = info->handle;
        if (!handle) continue;
        
        if (!HasOverlappedIoCompleted(&info->overlapped)) continue;
        
        // Handle the change...
        DWORD bytes_transferred;
        BOOL success = GetOverlappedResult(handle, &info->overlapped, &bytes_transferred, FALSE);
        assert(success != 0);
        
        // Issue next read immediately.
        issue_one_read(info);
        
        // Sometimes, it seems, this call fails. In that case (hopefully always?)
        // bytes_transferred will be 0, indicating that the destination data
        // is uninitalized.
        
        if (bytes_transferred == 0) {
            break;
        }
        
        FILE_NOTIFY_INFORMATION *notify = info->notify_information;
        b32 first = true;
        while (notify) {
            
            char *action_name = "UNKNOWN";
            switch (notify->Action) {
                case FILE_ACTION_ADDED: action_name = "ADDED"; break;
                case FILE_ACTION_MODIFIED: action_name = "MODIFIED"; break;
                case FILE_ACTION_RENAMED_NEW_NAME: action_name = "RENAMED"; break;
                default: {
                    // Ignore REMOVE and RENAMED_OLD_NAME file actions.
                    continue;
                } break;
            }
            
#define NAME_BUFFER_LEN 1000
            char name_buffer[NAME_BUFFER_LEN];
            int result = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, notify->FileName, notify->FileNameLength / sizeof(s16),
                                             name_buffer, NAME_BUFFER_LEN - 1, 0, 0);
            if (result) {
                name_buffer[result] = 0;
            }
            
            if (result == 0) continue;
            
            // Replace slashes
            int name_len = string_length(name_buffer);
            for (int c = 0; c < name_len; ++c) {
                if (name_buffer[c] == '\\') {
                    name_buffer[c] = '/';
                }
            }
            
            // Plus 4 because we need to add "./" at the beginning and a '/' between the dir name 
            // and file changed, and finally the null-terminator '/0'
            
            int full_name_len = name_len + string_length(info->name) + 4; 
            char *full_name = (char *) platform_alloc(sizeof(char) * full_name_len);
            sprintf(full_name, "./%s/%s", info->name, name_buffer);
            
            // TODO(diego): Handle the file name and create a Asset_Change struct.
            Break_String_Result r = break_by_tok(name_buffer, '.');
            // lowercase extension if necessary
            for (int c = 0; c < string_length(r.rhs); ++c) {
                char it = r.rhs[c];
                if (it >= 'A' && it <= 'Z') {
                    char n = it - 'A';
                    r.rhs[c] = n + 'a';
                }
            }
            
            char *short_name = r.lhs;
            char *extension = r.rhs;
            
            int t = find_character_from_right(r.lhs, '/');
            if (t) {
                advance(&short_name, t + 1);
            }
            
            int short_name_count = string_length(short_name);
            if (short_name_count > 0 && extension) {
                add_changed_asset(short_name, full_name, extension);
            }
            
            platform_free(full_name);
            notify = move_info_forward(notify);
        }
    }
}

internal b32
hotloader_process_change() {
    if (!hotloader_initted) hotloader_init();
    
    int dir_count = sb_count(directories);
    for (int i = 0; i < dir_count; ++i) {
        Directory_Info *info = &directories[i];
        if (info->read_issue_failed) {
            info->read_issue_failed = false;
            issue_one_read(info);
            if (info->read_issue_failed) return false;
        }
    }
    
    win32_pump_notifications();
    
    int changes_count = sb_count(asset_changes);
    if (!changes_count) return false;
    
    real32 now = get_time();
    
    local_persist int handled_assets = 0;
    
    int catalog_count = sb_count(catalogs);;
    for (int i = 0; i < changes_count; ++i) {
        Asset_Change *change = &asset_changes[i];
        
        real32 delta = now - change->time_of_last_change;
        if (delta < RELOAD_DELAY) continue;
        
        // Find catalogs that support this extension
        // Also figure out if this file is in an overlay supported by one of them.
        Catalog_Base **matching_catalogs = 0;
        
        for (int y = 0; y < catalog_count; ++y) {
            Catalog_Base *catalog = catalogs[y];
            if (extension_is_supported(catalog->extensions, change->extension)) {
                sb_push(matching_catalogs, catalog);
            }
        }
        
        b32 handled = false;
        
        int matching_count = sb_count(matching_catalogs);
        for (int y = 0; y < matching_count; ++y) {
            Catalog_Base *catalog = matching_catalogs[y];
            
            // We take memory responsibility of the short_name and full_name,
            // so we don't free them.
            
            assert(sb_count(catalog->short_names_to_reload) == sb_count(catalog->full_names_to_reload));
            
            //
            //     log_print("Hotloader: File '%' ssent to catalog '%'.\n", change.short_name, catalog.my_name);
            
            sb_push(catalog->short_names_to_reload, copy_string(change->short_name));
            sb_push(catalog->full_names_to_reload, copy_string(change->full_name));
            
            handled = true;
        }
        
        if (hotloader_callback) {
            hotloader_callback(change, handled);
        }
        
        handled_assets++;
    }
    
    if (handled_assets == changes_count) {
        handled_assets = 0;
        sb_free(asset_changes);
        asset_changes = 0;
        return false;
    }
    
    return true;
}

internal void
hotloader_shutdown() {
    if (!hotloader_initted) return;
    
    // We do not bother freeing all memory; just releasing OS handles/etc.
    for (int i = 0; i < sb_count(directories); ++i) {
        release(&directories[i]);
    }
    sb_free(directories);
    directories = 0;
    
    hotloader_initted = false;
}