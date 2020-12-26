global_variable Loaded_Font global_camera_font = {};

internal void
init_camera(Camera *cam, real32 yaw, real32 pitch, real32 fov) {
    cam->yaw       = yaw;
    cam->pitch     = pitch;
    cam->fov       = fov;
    cam->world_up  = make_vector3(0.f, 1.f, 0.f);
    cam->front     = make_vector3(0.f, 0.f, -1.f);
    update_vectors(cam);
}

internal void
init_camera(Camera *cam) {
    init_camera(cam, -90.f, 0.f, 45.f);
}

internal void
draw_camera_debug(Camera *cam, Vector2i dim) {
    if (!global_camera_font.texture) {
        global_camera_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 16.f);
    }
    
    render_2d_right_handed(dim.width, dim.height);
    
    char buf[256];
    sprintf(buf, "Camera Values\nXYZ: %.2f, %.2f, %.2f\nPitch: %f\nYaw: %f\n", cam->position.x, cam->position.y, cam->position.z, cam->pitch, cam->yaw);
    
    int line_count;
    real32 width = get_text_width(&global_camera_font, buf, &line_count);
    
    real32 margin = 20.f;
    Vector2 p = make_vector2(dim.width - width - margin, margin);
    Vector4 white = make_color(0xffffffff);
    
    glDisable(GL_DEPTH_TEST);
    draw_text(p.x, p.y + 1.f, (u8*) buf, &global_camera_font, make_color(0));
    draw_text(p.x, p.y, (u8*) buf, &global_camera_font, white);
    glEnable(GL_DEPTH_TEST);
}


internal void
update_vectors(Camera *cam) {
    assert(cam);
    
    real32 yrad = angle_to_radians(cam->yaw);
    real32 prad = angle_to_radians(cam->pitch);
    
    Vector3 front = {};
    
    front.x = cosf(yrad) * cosf(prad);
    front.y = sinf(prad);
    front.z = sinf(yrad) * cosf(prad);
    
    cam->front = normalize(front);
    cam->right = normalize(cross(cam->front, cam->world_up));
    cam->up    = normalize(cross(cam->right, cam->front));
}

internal void
update_camera(Camera *cam, Game_Input *input) {
    
    assert(cam);
    
    switch (cam->mode) {
        case CameraMode_Free: {
            
            Game_Mouse *mouse = &input->mouse;
            
            real32 xoff = mouse->sensitivity * mouse->velocity.x;
            real32 yoff = mouse->sensitivity * mouse->velocity.y;
            
            cam->yaw   += xoff;
            cam->pitch += yoff;
            
            cam->pitch = clampf(-89.f, cam->pitch, 89.f);
            cam->yaw   = fmodf(cam->yaw, 360.f);
            
            update_vectors(cam);
            
            real32 speed = 2.5f * time_info.dt;
            if (is_down(Button_Space)) {
                speed *= 10.f;
            }
            if (is_down(Button_Up) || is_down(Button_W)) {
                cam->position = cam->position + cam->front * speed;
            }
            if (is_down(Button_Down) || is_down(Button_S)) {
                cam->position = cam->position - cam->front * speed;
            }
            if (is_down(Button_Left) || is_down(Button_A)) {
                cam->position = cam->position - cam->right * speed;
            }
            if (is_down(Button_Right) || is_down(Button_D)) {
                cam->position = cam->position + cam->right * speed;
            }
            // TODO(diego): Handle scroll.
        } break;
        
        case CameraMode_LookAt: {
            // Do nothing.
        } break;
        
        default: invalid_code_path;
    }
}

internal void
set_camera_mode(Camera *cam, Camera_Mode mode) {
    assert(cam);
    
    OutputDebugString("setting camera mode to\n");
    if (mode == CameraMode_Free) {
        OutputDebugString("free\n");
    } else {
        OutputDebugString("look_at\n");
    }
    cam->mode = mode;
}

internal Mat4
get_view_matrix(Camera *cam) {
    assert(cam);
    
    Mat4 result;
    
    Vector3 target = cam->target;
    if (cam->mode == CameraMode_Free) {
        target = cam->position + cam->front;
    }
    result = look_at(cam->position, target, cam->up);
    return result;
}