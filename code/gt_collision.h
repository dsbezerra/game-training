
struct Box {
    real32 x;
    real32 y;
    real32 width;
    real32 height;
};

internal b32
aabb_vs_aabb(Box a, Box b) {
    b32 result = 0;
    
    result = (a.x < b.x + b.width && 
              a.x + a.width > b.x && 
              a.y < b.y + b.height &&
              a.y + a.height > b.y);
    
    return result;
}