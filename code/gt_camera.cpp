internal void
init_camera(camera *cam) {
    cam->yaw       = -90.f;
    cam->pitch     = 0.f;
    cam->fov       = 45.f;
    cam->world_up  = make_v3(0.f, 1.f, 0.f);
    cam->front     = make_v3(0.f, 0.f, -1.f);
    update_vectors(cam);
}

internal void
update_vectors(camera *cam) {
    assert(cam);
    
    real32 yrad = angle_to_radians(cam->yaw);
    real32 prad = angle_to_radians(cam->pitch);
    
    v3 front = {};
    
    front.x = cosf(yrad) * cosf(prad);
    front.y = sinf(prad);
    front.z = sinf(yrad) * cosf(prad);
    
    cam->front = normalize(front);
    cam->right = normalize(cross(cam->front, cam->world_up));
    cam->up    = normalize(cross(cam->right, cam->front));
}

internal void
update_camera(camera *cam, game_input *input) {
    
    assert(cam);
    
    switch (cam->mode) {
        case CameraMode_Free: {
            
            game_mouse *mouse = &input->mouse;
            
            real32 xoff = mouse->sensitivity * mouse->velocity.x;
            real32 yoff = mouse->sensitivity * mouse->velocity.y;
            
            cam->yaw   += xoff;
            cam->pitch += yoff;
            
            cam->pitch = clampf(-89.f, cam->pitch, 89.f);
            cam->yaw   = fmodf(cam->yaw, 360.f);
            
            update_vectors(cam);
            
            real32 speed = 5.f * time_info.dt;
            if (is_down(Button_Up)) {
                cam->position = add(cam->position, mul(cam->front, speed));
            }
            if (is_down(Button_Down)) {
                cam->position = sub(cam->position, mul(cam->front, speed));
            }
            if (is_down(Button_Left)) {
                cam->position = sub(cam->position, mul(cam->right, speed));
            }
            if (is_down(Button_Right)) {
                cam->position = add(cam->position, mul(cam->right, speed));
            }
            
            
            
            // TODO(diego): Handle scroll.
            
        } break;
        default: invalid_code_path;
    }
}

internal void
set_camera_mode(camera *cam, camera_mode mode) {
    assert(cam);
    
    cam->mode = mode;
}

internal mat4
get_view_matrix(camera *cam) {
    assert(cam);
    
    mat4 result;
    
    result = look_at(cam->position, add(cam->position, cam->front), cam->up);
    
    return result;
}