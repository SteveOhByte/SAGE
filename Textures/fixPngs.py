import os
from PIL import Image

def premultiply_alpha(png_file_path):
    """Premultiply alpha channel of a PNG image."""
    with Image.open(png_file_path) as image:
        if image.mode in ['RGBA', 'LA'] or (image.mode == 'P' and 'transparency' in image.info):
            # Convert to RGBA if necessary
            if image.mode != 'RGBA':
                image = image.convert('RGBA')
            
            # Premultiply alpha channel
            premultiplied = Image.alpha_composite(Image.new('RGBA', image.size, (0, 0, 0, 0)), image)
            premultiplied.save(png_file_path, 'PNG')

def scan_and_premultiply(directory):
    """Scan through the directory and its subdirectories for PNG files and premultiply their alpha."""
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.png'):
                file_path = os.path.join(root, file)
                print(f"Processing {file_path}")
                try:
                    premultiply_alpha(file_path)
                except Exception as e:
                    print(f"Error processing {file_path}: {e}")

# Main
directory_path = "C:\\Users\\miste\\OneDrive - NC\\GraphicsProgramming\\SAGE\\Textures"
scan_and_premultiply(directory_path)