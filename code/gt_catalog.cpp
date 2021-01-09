
internal void
release(Catalog_Base *base) {
    release(base->extensions);
    sb_free(base->short_names_to_reload);
    sb_free(base->full_names_to_reload);
}