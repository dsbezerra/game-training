/* date = November 1st 2020 8:50 pm */

enum camera_mode {
    CameraMode_None,
    CameraMode_Free,
    CameraMode_Count,
};

struct camera {
    camera_mode mode;
    v3 position;
    v3 front;
    v3 up;
    v3 right;
    v3 world_up;
    real32 fov;
    real32 yaw;
    real32 pitch;
};

internal void init_camera(camera *cam);
internal void update_vectors(camera *cam);
internal void update_camera(camera *cam, game_input *input);
internal void set_camera_mode(camera *cam, camera_mode mode);
internal mat4 get_view_matrix(camera *cam);