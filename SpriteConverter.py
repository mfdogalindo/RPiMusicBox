#!/usr/bin/env python3
#pip3 install pillow numpy pathlib 
#python3 SpriteConverter.py Michi.gif --name gladis --output sprites

#!/usr/bin/env python3
import argparse
from PIL import Image
import numpy as np
from pathlib import Path
import colorsys

class SpriteConverter:
    def __init__(self):
        self.color_cache = {}
        
    def rgb_to_565(self, r, g, b):
        """Convierte RGB a formato de color 565 (16 bits)"""
        r = (r >> 3) & 0x1F
        g = (g >> 2) & 0x3F
        b = (b >> 3) & 0x1F
        return (r << 11) | (g << 5) | b
    
    def color_to_565(self, color):
        """Convierte un color (RGB o RGBA) a formato 565"""
        if len(color) == 4:  # RGBA
            r, g, b, a = color
            if a < 128:  # Si es muy transparente
                return 0  # Color transparente
        else:  # RGB
            r, g, b = color
        return self.rgb_to_565(r, g, b)

    def process_gif(self, gif_path):
        """Procesa un archivo GIF y retorna los datos de sprites y paleta"""
        img = Image.open(gif_path)
        frames = []
        try:
            while True:
                # Convertir a RGBA si no lo está ya
                if img.mode != 'RGBA':
                    current = img.convert('RGBA')
                else:
                    current = img.copy()
                
                frames.append(current)
                img.seek(img.tell() + 1)
        except EOFError:
            pass

        # Obtener dimensiones
        width, height = frames[0].size
        
        # Extraer y procesar todos los colores únicos
        color_to_565_map = {}  # Mapeo de colores RGBA a valores 565
        frame_data_565 = []    # Datos de frames en formato 565
        
        for frame in frames:
            frame_pixels_565 = []
            for y in range(height):
                for x in range(width):
                    pixel = frame.getpixel((x, y))
                    
                    # Convertir el pixel a 565 si no está en el caché
                    if pixel not in color_to_565_map:
                        color_to_565_map[pixel] = self.color_to_565(pixel)
                    
                    # Guardar el valor 565
                    frame_pixels_565.append(color_to_565_map[pixel])
            
            frame_data_565.append(frame_pixels_565)
        
        return {
            'width': width,
            'height': height,
            'frame_count': len(frames),
            'frames': frame_data_565
        }
    
    def generate_code(self, sprite_data, sprite_name):
        """Genera el código C++ para el sprite"""
        width = sprite_data['width']
        height = sprite_data['height']
        frames = sprite_data['frames']
        
        # Generar datos de frames
        frame_data_lines = []
        for frame_idx, frame in enumerate(frames):
            frame_data_lines.append(f"    // Frame {frame_idx}")
            chunk_size = 8  # Valores por línea
            for i in range(0, len(frame), chunk_size):
                chunk = frame[i:i + chunk_size]
                line_data = [f"0x{x:04X}" for x in chunk]
                frame_data_lines.append(f"    {', '.join(line_data)}")
                # coma al final de la linea
                if i + chunk_size < len(frame):
                    frame_data_lines[-1] += ","
            frame_data_lines.append("")  # Línea en blanco entre frames
        
        # Construir el código
        code = []
        code.append(f"// Generado automáticamente - Sprite: {sprite_name}")
        code.append("#include <stdint.h>")
        code.append("")
        
        code.append(f"#define {sprite_name.upper()}_WIDTH {width}")
        code.append(f"#define {sprite_name.upper()}_HEIGHT {height}")
        code.append(f"#define {sprite_name.upper()}_FRAMES {len(frames)}")
        code.append("")
        
        code.append("// Datos de frames (formato RGB565)")
        code.append(f"const uint16_t {sprite_name}_frame_data[] = {{")
        code.extend(frame_data_lines)
        code.append("};")
        code.append("")
        
        code.append("// Estructura de sprite")
        code.append(f"const SpriteData {sprite_name}_data = {{")
        code.append(f"    .width = {sprite_name.upper()}_WIDTH,")
        code.append(f"    .height = {sprite_name.upper()}_HEIGHT,")
        code.append(f"    .frame_count = {sprite_name.upper()}_FRAMES,")
        code.append(f"    .frame_data = {sprite_name}_frame_data")
        code.append("};")
        
        return '\n'.join(code)

def main():
    parser = argparse.ArgumentParser(description='Convierte GIFs en datos de sprite para microcontroladores')
    parser.add_argument('gif_path', help='Ruta al archivo GIF')
    parser.add_argument('--name', help='Nombre base para el sprite', default='sprite')
    parser.add_argument('--output', help='Directorio de salida', default='.')
    
    args = parser.parse_args()
    
    converter = SpriteConverter()
    
    try:
        sprite_data = converter.process_gif(args.gif_path)
        code = converter.generate_code(sprite_data, args.name)
        
        output_path = Path(args.output) / f"{args.name}_sprite.h"
        with open(output_path, 'w') as f:
            f.write(code)
            
        print(f"Sprite generado exitosamente: {output_path}")
        print(f"Dimensiones: {sprite_data['width']}x{sprite_data['height']}")
        print(f"Frames: {sprite_data['frame_count']}")
        
    except Exception as e:
        print(f"Error procesando el GIF: {e}")

if __name__ == "__main__":
    main()