struct Texture {
    const char* path;
    int texture_width;
    int texture_height;
    int channels;
    unsigned char* pixels;
};

extern bool texture_load(const char* path, struct Texture* resource);
extern void texture_unload(struct Texture* resource);
