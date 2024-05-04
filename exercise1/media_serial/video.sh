#!/bin/bash

# Directory containing the PGM files
input_dir="static" # or ordered

# Output file name
output_video="output_video.mp4"

# Ensuring FFmpeg is installed
if ! command -v ffmpeg &> /dev/null
then
    echo "FFmpeg could not be found. Please install it to continue."
    exit 1
fi

# Creating the video from the PGM images
ffmpeg -r 10  -pattern_type glob -i "${input_dir}/*.pgm" -c:v libx264 -pix_fmt yuv420p ${output_video}

echo "Video created successfully: ${output_video}"
