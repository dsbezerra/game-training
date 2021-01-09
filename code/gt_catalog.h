/* date = January 8th 2021 7:46 pm */

enum Catalog_Kind {
    CatalogKind_None,
    CatalogKind_Texture,
    CatalogKind_Shader,
};

struct Catalog_Base {
    Catalog_Kind kind;
    char *my_name;
    StringArray *extensions;
    u32 size;
    
    char **short_names_to_reload;
    char **full_names_to_reload;
};

internal void release(Catalog_Base *base);