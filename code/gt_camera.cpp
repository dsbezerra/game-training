internal void
init_camera(Camera *cam) {
    cam->yaw       = -90.f;
    cam->pitch     = 0.f;
    cam->fov       = 45.f;
    cam->world_up  = make_vector3(0.f, 1.f, 0.f);
    cam->front     = make_vector3(0.f, 0.f, -1.f);
    update_vectors(cam);
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
            if (is_down(Button_Up)) {
                cam->position = cam->position + cam->front * speed;
            }
            if (is_down(Button_Down)) {
                cam->position = cam->position - cam->front * speed;
            }
            if (is_down(Button_Left)) {
                cam->position = cam->position - cam->right * speed;
            }
            if (is_down(Button_Right)) {
                cam->position = cam->position + cam->right * speed;
            }
            
            
            
            // TODO(diego): Handle scroll.
            
        } break;
        default: invalid_code_path;
    }
}

internal void
set_camera_mode(Camera *cam, Camera_Mode mode) {
    assert(cam);
    
    cam->mode = mode;
}

internal Mat4
get_view_matrix(Camera *cam) {
    assert(cam);
    
    Mat4 result;
    
    result = look_at(cam->position, cam->position + cam->front, cam->up);
    
    return result;
}