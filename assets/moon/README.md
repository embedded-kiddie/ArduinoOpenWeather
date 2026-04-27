# Moon Phase Images

## Source
[Moon Phases Loop - NASA's Scientific Visualization Studio][1]

## Extracting images frame by frame from an MP4 video
```bash
magick convert -coalesce moon_720p30.mp4 moon%03d.png
```

[1]: https://svs.gsfc.nasa.gov/4310/ "NASA SVS - Moon Phases Loop"
