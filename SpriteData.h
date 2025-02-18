#ifndef SPRITE_DATA_H
#define SPRITE_DATA_H

#include <stdint.h>

struct SpriteData {
    uint16_t width;
    uint16_t height;
    uint8_t frame_count;
    const uint16_t* frame_data;  // Datos directamente en formato RGB565
};

class AnimatedSprite {
private:
    const SpriteData& data;
    uint8_t current_frame;
    unsigned long last_frame_time;
    unsigned long frame_duration;
    bool looping;
    bool playing;
    uint8_t scale;
    size_t frame_size;  // Tamaño en pixels (no en bytes) de cada frame

public:
    AnimatedSprite(const SpriteData& sprite_data, unsigned long frame_ms = 100, uint8_t initial_scale = 1)
        : data(sprite_data),
          current_frame(0),
          last_frame_time(0),
          frame_duration(frame_ms),
          looping(true),
          playing(false),
          scale(initial_scale)
    {
        frame_size = data.width * data.height;
    }
    
    void play() {
        playing = true;
        last_frame_time = millis();
    }
    
    void pause() {
        playing = false;
    }
    
    void reset() {
        current_frame = 0;
        last_frame_time = millis();
    }
    
    void setLooping(bool loop) {
        looping = loop;
    }
    
    void setFrameDuration(unsigned long ms) {
        frame_duration = ms;
    }
    
    void setScale(uint8_t new_scale) {
        scale = max(1, new_scale);
    }
    
    uint8_t getScale() const {
        return scale;
    }
    
    const uint16_t* getCurrentFrame() const {
        return &data.frame_data[current_frame * frame_size];
    }
    
    uint16_t getWidth() const {
        return data.width;
    }
    
    uint16_t getHeight() const {
        return data.height;
    }
    
    uint16_t getScaledWidth() const {
        return data.width * scale;
    }
    
    uint16_t getScaledHeight() const {
        return data.height * scale;
    }
    
    bool isPlaying() const {
        return playing;
    }
    
    void update() {
        if (!playing) return;
        
        unsigned long current_time = millis();
        if (current_time - last_frame_time >= frame_duration) {
            current_frame++;
            if (current_frame >= data.frame_count) {
                if (looping) {
                    current_frame = 0;
                } else {
                    current_frame = data.frame_count - 1;
                    playing = false;
                }
            }
            last_frame_time = current_time;
        }
    }
};

#endif // SPRITE_DATA_H