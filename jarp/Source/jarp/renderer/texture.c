bool texture_load(const char* path, struct Texture* resource) {
    int texture_width, texture_height, channels;
    stbi_uc* pixels = stbi_load(path, &texture_width, &texture_height, &channels, STBI_rgb_alpha);
    if (!resource->pixels) {
        log_error("Failed to load texture image");
        return false;
    }

    resource->path = path;
    resource->texture_width = texture_width;
    resource->texture_height = texture_height;
    resource->channels = channels;
    resource->pixels = pixels;

    return true;
}

void texture_unload(struct Texture* resource) {
    stbi_image_free(resource->pixels);
    ZERO_STRUCT(*resource);
}
