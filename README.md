# v-cloud
 A volumetric cloud render based on OpenGL.
Implementing:
 - Preetham model to render the sun and sky
 - Pre-computed Perlin-worley noise to calculate cloud density
 - Simplex 3D noise to calculate curl
 - Ray marching to calculate cloud lighting 

## Compile
Compile and build using `CMake`. Configure using IDE or build with command line
> `cmake --build build --config Release --target all -j 12 `

## Instructions

| Key | Operation |
| -------- | ------- |
| Drag mouse | Tilt camera |
| W | Forward |
| A | Backward |
| S | Left |
| D | Right |
| Q | Up |
| Z | Down |
| &uarr; | Rise sun |
| &uarr; | Fall sun |
| &larr; | Decrease coverage (less rain cloud) |
| &rarr; | Increase coverage (more rain cloud) |

Frame rate is outputed to `stdout` per 5s.

Please don't go above the clouds, below the surface, or outside of radius as it triggers bugs!

## Objectives 

- [x] Sample Perlin-Worley noise to create cloud models. (See [perlinworley.h](src/perlinworley.h))
- [x] Calculate light intensity going through clounds based on Beer-Lambert law. (Line 271 in [sky.frag](shaders/sky.frag#217))
- [x] Account for scattering. (Line 265 in [sky.frag](/shaders/sky.frag#265))
- [x] Account for absorption. (Line 279 in [sky.frag](shaders/sky.frag#279))
- [x] Calculate sky color in four discrete times of a day: sunrise, noon, sunset and night, using Preetham model. (Line 55 in [sky.frag](shaders/sky.frag#55))
- [x] Use ray marching to render clouds. (Line 242 in [sky.frag](shaders/sky.frag#242))
- [x] FPS ≥ 3. (See `stdout`)
- [x] Use a user-interface to determine rain cloud density. (See above)

## Dependancies
- C++ 20
- OpenGL 3
- GLFW, GLEW, GLM(included in [external](external/))

## Acknowledgements
### Models
[The Real Time Volumetric Cloudscapes of Horizon Zero Dawn](https://www.guerrilla-games.com/read/the-real-time-volumetric-cloudscapes-of-horizon-zero-dawn)

[NUBIS: AUTHORING REAL-TIME VOLUMETRIC CLOUDSCAPES WITH THE DECIMA ENGINE](https://www.guerrilla-games.com/read/nubis-authoring-real-time-volumetric-cloudscapes-with-the-decima-engine)

[A Practical Analytic Model for Daylight](https://courses.cs.duke.edu/fall01/cps124/resources/p91-preetham.pdf)

### Noises

[Simplex 3D Noise](https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83)

[Perlin Worley Noise](https://github.com/sebh/TileableVolumeNoise)

### Codes
[clayjohn/realtime_clouds](https://github.com/clayjohn/realtime_clouds)

[OfenPower/RealtimeVolumetricCloudRenderer](https://github.com/OfenPower/RealtimeVolumetricCloudRenderer)

[CS 488 - Toshiya Hachisuka](https://cs.uwaterloo.ca/~thachisu/CS488_S23/)