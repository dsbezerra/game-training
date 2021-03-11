internal void
init_game(Flood_It_State *state) {
    state->game_mode = GameMode_Playing;
    state->filled = 0;
    
    generate_grid(state);
    
    platform_show_cursor(true);
}

internal void
update_game(Flood_It_State *state, Game_Input *input) {
    
    if (check_finished(state)) {
        state->game_mode = GameMode_Menu;
        return;
    }
    
    Vector2i mouse_position;
    platform_get_cursor_position(&mouse_position);
    state->mouse_position = mouse_position;
    
    update_colors(state);
    
    if (pressed(Button_Mouse1)) {
        if (state->hovered_color) {
            flood_fill(state);
        }
    }
}

internal void
update_colors(Flood_It_State *state) {
    Vector2i dim = state->dimensions;
    
    real32 tile_size = dim.height * .1f;
    real32 center_x = dim.width * .5f;
    
    real32 colors_width = tile_size * FloodItTileKind_Count;
    real32 sx = center_x - colors_width * .5f;
    real32 pad = 5.f;
    
    real32 margin = tile_size * .2f;
    real32 y = dim.height - tile_size - margin;
    
    Flood_It_Color *hovered_color = 0;
    
    for (u32 color_index = 0; color_index < (u32) FloodItTileKind_Count; ++color_index) {
        Flood_It_Color *color = &state->colors[color_index];
        color->kind = (Flood_It_Tile_Kind) (FloodItTileKind_Red + color_index);
        color->color = get_color_for_tile(color->kind);
        color->x = sx + color_index * tile_size + pad;
        color->y = y + pad;
        color->w = (sx + color_index * tile_size + tile_size - pad) - color->x;
        color->h = (y + tile_size - pad) - color->y;
        color->hover = is_mouse_over(state, color);
        if (color->hover) hovered_color = color;
    }
    
    state->hovered_color = hovered_color;
}

internal void
flood_fill(Flood_It_State *state) {
    Flood_It_Tile first = state->grid.tiles[0][0];
    if (first.kind == state->hovered_color->kind) return;
    
    flood_tile(state, state->hovered_color, 0, 0, first.kind, true);
    
    state->filled = 0;
    
    Flood_It_Grid *grid = &state->grid;
    for (u32 x = 0; x < FLOOD_IT_GRID_SIZE; ++x) {
        for (u32 y = 0; y < FLOOD_IT_GRID_SIZE; ++y) {
            Flood_It_Tile *tile = &state->grid.tiles[x][y];
            if (tile->kind == state->hovered_color->kind)
                state->filled++;
        }
    }
}

internal void
flood_tile(Flood_It_State *state, Flood_It_Color *color, u32 x, u32 y, Flood_It_Tile_Kind first, b32 first_tile) {
    assert(color);
    assert(color->kind >= FloodItTileKind_Red && color->kind < FloodItTileKind_Count);
    
    if (x < 0 || x >= FLOOD_IT_GRID_SIZE) return;
    if (y < 0 || y >= FLOOD_IT_GRID_SIZE) return;
    
    Flood_It_Tile *tile = &state->grid.tiles[x][y];
    if (tile->kind == first || first_tile) state->filled++;
    if (!first_tile && first != tile->kind) return;
    
    set_color(color, tile);
    
    flood_tile(state, color, x + 1, y,     first, false);
    flood_tile(state, color, x    , y + 1, first, false);
    flood_tile(state, color, x - 1, y,     first, false);
    flood_tile(state, color, x    , y - 1, first, false);
}

internal void
generate_grid(Flood_It_State *state) {
    Flood_It_Grid *grid = &state->grid;
    for (u32 x = 0; x < FLOOD_IT_GRID_SIZE; ++x) {
        for (u32 y = 0; y < FLOOD_IT_GRID_SIZE; ++y) {
            grid->tiles[x][y] = make_random_tile(x, y);
        }
    }
}

internal Flood_It_Tile
make_random_tile(u32 x, u32 y) {
    assert(x >= 0 && x < FLOOD_IT_GRID_SIZE);
    assert(y >= 0 && y < FLOOD_IT_GRID_SIZE);
    
    Flood_It_Tile result = {};
    
    Flood_It_Tile_Kind kind;
    
    real32 t = random_real32_in_range(0.f, 2.f);
    if (t >= 2.f) {
        kind = FloodItTileKind_Red;
    } else if (t >= 1.6f) {
        kind = FloodItTileKind_Green;
    } else if (t >= 1.3f) {
        kind = FloodItTileKind_Blue;
    } else if (t >= 0.9f) {
        kind = FloodItTileKind_Yellow;
    } else if (t >= 0.4f) {
        kind = FloodItTileKind_Orange;
    } else {
        kind = FloodItTileKind_Pink;
    }
    result.x = x;
    result.y = y;
    result.kind = kind;
    result.color = get_color_for_tile(kind);
    
    return result;
}

internal Vector4
get_color_for_tile(Flood_It_Tile_Kind kind) {
    Vector4 result = {};
    
    switch (kind) {
        case FloodItTileKind_Red:    result = make_color(0xffff0000); break;
        case FloodItTileKind_Green:  result = make_color(0xff00ff00); break;
        case FloodItTileKind_Blue:   result = make_color(0xff0000ff); break;
        case FloodItTileKind_Yellow: result = make_color(0xffffff00); break;
        case FloodItTileKind_Orange: result = make_color(0xffff8200); break;
        case FloodItTileKind_Pink:   result = make_color(0xffff00ff); break;
        default: invalid_code_path; break;
    }
    
    return result;
}

internal void
set_color(Flood_It_Color *color, Flood_It_Tile *tile) {
    tile->kind = color->kind;
    tile->color = get_color_for_tile(color->kind);
}

internal b32
is_mouse_over(Flood_It_State *state, Flood_It_Color *color) {
    Vector2i p = state->mouse_position;
    if (p.x < color->x) return false;
    if (p.y < color->y) return false;
    if (p.x > (color->x + color->w)) return false;
    if (p.y > (color->y + color->h)) return false;
    
    return true;
}

internal b32
check_finished(Flood_It_State *state) {
    u32 num_tiles = FLOOD_IT_GRID_SIZE * FLOOD_IT_GRID_SIZE;
    return state->filled == num_tiles;
}

internal void
draw_colors(Flood_It_State *state) {
    
    immediate_begin();
    for (u32 color_index = 0; color_index < (u32) FloodItTileKind_Count; ++color_index) {
        Flood_It_Color *color = &state->colors[color_index];
        Vector2 min = make_vector2(color->x, color->y);
        Vector2 max = make_vector2(color->x + color->w, color->y + color->h);
        immediate_quad(min, max, color->color);
    }
    
    if (state->hovered_color) {
        Flood_It_Color *color = state->hovered_color;
        Vector2 min = make_vector2(color->x, color->y);
        Vector2 max = make_vector2(color->x + color->w, color->y + color->h);
        immediate_quad(min, max, make_color(0xaaffffff));
    }
    
    immediate_flush();
}

internal void
draw_grid(Flood_It_State *state) {
    
    Vector2i dim = state->dimensions;
    
    real32 height = dim.height * .7f;
    real32 tile_size = height / (real32) FLOOD_IT_GRID_SIZE;
    
    real32 width  = tile_size * FLOOD_IT_GRID_SIZE;
    
    real32 remaining_wspace = dim.width  - width;
    real32 remaining_hspace = dim.height - height;
    
    real32 start_x = remaining_wspace * .5f;
    real32 start_y = remaining_hspace * .5f;
    
    immediate_begin();
    
    for (u32 tile_x = 0; tile_x < FLOOD_IT_GRID_SIZE; ++tile_x) {
        for (u32 tile_y = 0; tile_y < FLOOD_IT_GRID_SIZE; ++tile_y) {
            Flood_It_Tile tile = state->grid.tiles[tile_x][tile_y];
            if (tile.kind == FloodItTileKind_Count) continue;
            
            real32 sx = start_x + tile_x * tile_size;
            real32 sy = start_y + tile_y * tile_size;
            
            Vector2 min = make_vector2(sx, sy);
            Vector2 max = make_vector2(sx + tile_size, sy + tile_size);
            
            immediate_quad(min, max, tile.color);
        }
    }
    
    immediate_flush();
}

internal void
draw_game_view(Flood_It_State *state) {
    
    game_frame_begin(state->dimensions.width, state->dimensions.height);
    
    if (state->game_mode == GameMode_Playing) {
        Vector2i dim = state->dimensions;
        
        // TODO(diego): Replace background
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) dim.width, (real32) dim.height, make_color(0xff2f3242));
        immediate_flush();
        
        draw_grid(state);
        draw_colors(state);
        draw_hud(state);
    } else {
        draw_menu(FLOOD_IT_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
draw_hud(Flood_It_State *state) {
    // TODO(diego): Implement.
    
    Vector2i dim = state->dimensions;
    
#if 0
    immediate_begin();
    
    real32 line_thickness = 1.f / 2.f;
    
    Vector2 min = make_vector2(0.f, dim.height * .5f - line_thickness);
    Vector2 max = make_vector2((real32) dim.width, dim.height * .5f + line_thickness);
    immediate_quad(min, max, make_color(0xffff00ff));
    
    min = make_vector2(dim.width * .5f - line_thickness, 0.f);
    max = make_vector2(dim.width * .5f + line_thickness, (real32) dim.height);
    immediate_quad(min, max, make_color(0xffff00ff));
    
    immediate_flush();
    
#endif
    
#define DRAW_MOUSE_LINES 0
#if DRAW_MOUSE_LINES
    immediate_begin();
    
    real32 line_thickness = 1.f / 2.f;
    
    Vector2i mp = state->mouse_position;
    
    real32 mouse_x = (real32) mp.x;
    real32 mouse_y = (real32) mp.y;
    
    // Horizontal
    Vector2 min = make_vector2(0.f, mouse_y - line_thickness);
    Vector2 max = make_vector2((real32) dim.width, mouse_y + line_thickness);
    immediate_quad(make_vector2(0.f, 0.f), make_vector2((real32) dim.width, (real32) dim.height), make_color(0xaa000000));
    
    immediate_quad(min, max, make_color(0xffff00ff));
    
    // Vertical
    min = make_vector2(mouse_x - line_thickness, 0.f);
    max = make_vector2(mouse_x + line_thickness, (real32) dim.height);
    immediate_quad(min, max, make_color(0xffff00ff));
    immediate_flush();
    
    u32 width_from_left = mp.x;
    u32 height_from_top = mp.y;
    
    u32 width_from_right = dim.width - width_from_left;
    u32 height_from_bottom = dim.height - height_from_top;
    
    char buf[256];
    sprintf(buf, "(%d, %d)", width_from_left, height_from_top);
    
    Vector4 white = make_color(0xffffffff);
    draw_text(mouse_x, mouse_y, (u8 *) buf, &state->assets.debug, white);
    
    sprintf(buf, "(%d, %d)", width_from_right, height_from_bottom);
    draw_text(mouse_x, mouse_y + 12.f, (u8 *) buf, &state->assets.debug, white);
    
#endif
    
}

internal void
flood_it_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_quad(min, max, make_color(0xffff00ff));
    immediate_flush();
}

internal void
flood_it_game_restart(Flood_It_State *state) {
    init_game(state);
}

internal void
flood_it_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    Flood_It_State *state = (Flood_It_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        use_multisampling = false;
        
        // NOTE(diego): Not used.
        Memory_Index total_memory_size = kilobytes(16);
        state = (Flood_It_State *) game_alloc(memory, total_memory_size);
        
        Flood_It_Assets assets = {};
        assets.debug = load_font("./data/fonts/Inconsolata-Regular.ttf", 16.f);
        
        state->assets = assets;
        
        init_game(state);
    }
    
    state->dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    if (state->game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
            update_game(state, input);
        }
    } else if (state->game_mode == GameMode_Menu ||
               state->game_mode == GameMode_GameOver) {
        if (pressed(Button_Down)) {
            advance_menu_choice(&state->menu_selected_item, 1);
        }
        if (pressed(Button_Up)) {
            advance_menu_choice(&state->menu_selected_item, -1);
        }
        if (pressed(Button_Escape)) {
            if (state->game_mode == GameMode_GameOver) {
                memory->asked_to_quit = true;
            } else {
                state->game_mode = GameMode_Playing;
            }
        }
        if (pressed(Button_Enter)) {
            switch (state->menu_selected_item) {
                case 0: {
                    flood_it_game_restart(state);
                } break;
                
                case 1: {
                    if (state->quit_was_selected) {
                        memory->asked_to_quit = true;
                    } else {
                        state->quit_was_selected = true;
                    }
                } break;
                
                default: {
                    assert(!"Should not happen!");
                } break;
            }
        }
        if (state->menu_selected_item != 1) {
            state->quit_was_selected = false;
        } else if (state->quit_was_selected) {
            if (pressed(Button_Escape)) {
                state->quit_was_selected = false;
            }
        }
    }
    
    //
    // Draw
    //
    draw_game_view(state);
}

internal void
flood_it_game_free(Game_Memory *memory) {
    if (!memory) return;
    
    Flood_It_State *state = (Flood_It_State *) memory->permanent_storage;
    if (!state) return;
    
    use_multisampling = true;
    // TODO(diego): If necessary.
}
