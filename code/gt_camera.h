/* date = November 1st 2020 8:50 pm */

enum Camera_Mode {
    CameraMode_None,
    CameraMode_Free,
    CameraMode_Count,
};

struct Camera{
    Camera_Mode mode;
    Vector3 position;
    Vector3 front;
    Vector3 up;
    Vector3 right;
    Vector3 world_up;
    real32 fov;
    real32 yaw;
    real32 pitch;
};

internal void init_camera(Camera*cam);
internal void update_vectors(Camera*cam);
internal void update_camera(Camera*cam, Game_Input *input);
internal void set_camera_mode(Camera*cam, Camera_Mode mode);
internal Mat4 get_view_matrix(Camera*cam);