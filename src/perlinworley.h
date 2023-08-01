
#ifndef D_TILEABLE3DNOISE
#define D_TILEABLE3DNOISE

#include "../external/glm/gtc/noise.hpp"
#include "./libtarga.h"
#include <math.h>

void writeTGA(const char *fileName, int width, int height, /*const*/ unsigned char *data)
{
    if (!tga_write_raw(fileName, width, height, data, TGA_TRUECOLOR_32))
    {
        printf("Failed to write image!\n");
        printf(tga_error_string(tga_get_last_error()));
    }
}

float remap(float originalValue, float originalMin, float originalMax, float newMin, float newMax)
{
    return newMin + (((originalValue - originalMin) / (originalMax - originalMin)) * (newMax - newMin));
}

class Tileable3dNoise
{
public:
    /// @return Tileable Worley noise value in [0, 1].
    /// @param p 3d coordinate in [0, 1], being the range of the repeatable pattern.
    /// @param cellCount the number of cell for the repetitive pattern.
    static float WorleyNoise(const glm::vec3 &p, float cellCount)
    {
        return Cells(p, cellCount);
    };

    /// @return Tileable Perlin noise value in [0, 1].
    /// @param p 3d coordinate in [0, 1], being the range of the repeatable pattern.
    static float PerlinNoise(const glm::vec3 &pIn, float frequency, int octaveCount)
    {
        const float octaveFrenquencyFactor = 2; // noise frequency factor between octave, forced to 2

        // Compute the sum for each octave
        float sum = 0.0f;
        float weightSum = 0.0f;
        float weight = 0.5f;
        for (int oct = 0; oct < octaveCount; oct++)
        {
            // Perlin vec3 is bugged in GLM on the Z axis :(, black stripes are visible
            // So instead we use 4d Perlin and only use xyz...
            // glm::vec3 p(x * freq, y * freq, z * freq);
            // float val = glm::perlin(p, glm::vec3(freq)) *0.5 + 0.5;

            glm::vec4 p = glm::vec4(pIn.x, pIn.y, pIn.z, 0.0f) * glm::vec4(frequency);
            float val = glm::perlin(p, glm::vec4(frequency));

            sum += val * weight;
            weightSum += weight;

            weight *= weight;
            frequency *= octaveFrenquencyFactor;
        }

        float noise = (sum / weightSum) * 0.5f + 0.5f;
        noise = std::fminf(noise, 1.0f);
        noise = std::fmaxf(noise, 0.0f);
        return noise;
    };

    static void GeneratePerlinWorleyTexture()
    {
        const float frequenceMul[6] = {2.0f, 8.0f, 14.0f, 20.0f, 26.0f, 32.0f}; // special weight for perling worley

        // Cloud base shape (will be used to generate PerlingWorley noise in he shader)
        // Note: all channels could be combined once here to reduce memory bandwith requirements.
        int cloudBaseShapeTextureSize = 128; // !!! If this is reduce, you hsould also reduce the number of frequency in the fmb noise  !!!
        int cloudBaseShapeRowBytes = cloudBaseShapeTextureSize * sizeof(unsigned char) * 4;
        int cloudBaseShapeSliceBytes = cloudBaseShapeRowBytes * cloudBaseShapeTextureSize;
        int cloudBaseShapeVolumeBytes = cloudBaseShapeSliceBytes * cloudBaseShapeTextureSize;
        unsigned char *cloudBaseShapeTexels = (unsigned char *)malloc(cloudBaseShapeVolumeBytes);
        unsigned char *cloudBaseShapeTexelsPacked = (unsigned char *)malloc(cloudBaseShapeVolumeBytes);
        for (int s = 0; s < cloudBaseShapeTextureSize; s++)
        {
            const glm::vec3 normFact = glm::vec3(1.0f / float(cloudBaseShapeTextureSize));
            for (int t = 0; t < cloudBaseShapeTextureSize; t++)
            {
                for (int r = 0; r < cloudBaseShapeTextureSize; r++)
                {
                    glm::vec3 coord = glm::vec3(s, t, r) * normFact;

                    // Perlin FBM noise
                    const int octaveCount = 3;
                    const float frequency = 8.0f;
                    float perlinNoise = Tileable3dNoise::PerlinNoise(coord, frequency, octaveCount);

                    float PerlinWorleyNoise = 0.0f;
                    {
                        const float cellCount = 4;
                        const float worleyNoise0 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * frequenceMul[0]));
                        const float worleyNoise1 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * frequenceMul[1]));
                        const float worleyNoise2 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * frequenceMul[2]));
                        const float worleyNoise3 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * frequenceMul[3]));
                        const float worleyNoise4 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * frequenceMul[4]));
                        const float worleyNoise5 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * frequenceMul[5])); // half the frequency of texel, we should not go further (with cellCount = 32 and texture size = 64)

                        float worleyFBM = worleyNoise0 * 0.625f + worleyNoise1 * 0.25f + worleyNoise2 * 0.125f;

                        // Perlin Worley is based on description in GPU Pro 7: Real Time Volumetric Cloudscapes.
                        // However it is not clear the text and the image are matching: images does not seem to match what the result  from the description in text would give.
                        // Also there are a lot of fudge factor in the code, e.g. *0.2, so it is really up to you to fine the formula you like.
                        // PerlinWorleyNoise = remap(worleyFBM, 0.0, 1.0, 0.0, perlinNoise);	// Matches better what figure 4.7 (not the following up text description p.101). Maps worley between newMin as 0 and
                        PerlinWorleyNoise = remap(perlinNoise, 0.0f, 1.0f, worleyFBM, 1.0f); // mapping perlin noise in between worley as minimum and 1.0 as maximum (as described in text of p.101 of GPU Pro 7)
                    }

                    const float cellCount = 4;
                    float worleyNoise0 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 1));
                    float worleyNoise1 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 2));
                    float worleyNoise2 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 4));
                    float worleyNoise3 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 8));
                    float worleyNoise4 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 16));
                    float worleyNoise5 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 32));	//cellCount=2 -> half the frequency of texel, we should not go further (with cellCount = 32 and texture size = 64)

                    // Three frequency of Worley FBM noise
                    float worleyFBM0 = worleyNoise1 * 0.625f + worleyNoise2 * 0.25f + worleyNoise3 * 0.125f;
                    float worleyFBM1 = worleyNoise2 * 0.625f + worleyNoise3 * 0.25f + worleyNoise4 * 0.125f;
                    float worleyFBM2 = worleyNoise3*0.625f + worleyNoise4*0.25f + worleyNoise5*0.125f;
                    //float worleyFBM2 = worleyNoise3 * 0.75f + worleyNoise4 * 0.25f; // cellCount=4 -> worleyNoise5 is just noise due to sampling frequency=texel frequency. So only take into account 2 frequencies for FBM

                    int addr = r * cloudBaseShapeTextureSize * cloudBaseShapeTextureSize + t * cloudBaseShapeTextureSize + s;

                    addr *= 4;
                    cloudBaseShapeTexels[addr] =  (unsigned char)(255.0f * PerlinWorleyNoise);
                    cloudBaseShapeTexels[addr + 1] =  (unsigned char)(255.0f * worleyFBM0);
                    cloudBaseShapeTexels[addr + 2] =  (unsigned char)(255.0f * worleyFBM1);
                    cloudBaseShapeTexels[addr + 3] =  (unsigned char)(255.0f * worleyFBM2);

                    float value = 0.0;
                    {
                        // pack the channels for direct usage in shader
                        float lowFreqFBM = worleyFBM0 * 0.625f + worleyFBM1 * 0.25f + worleyFBM2 * 0.125f;
                        float baseCloud = PerlinWorleyNoise;
                        value = remap(baseCloud, -(1.0f - lowFreqFBM), 1.0f, 0.0f, 1.0f);
                        // Saturate
                        value = std::fminf(value, 1.0f);
                        value = std::fmaxf(value, 0.0f);
                    }
                    cloudBaseShapeTexelsPacked[addr] =  (unsigned char)(255.0f * value);
                    cloudBaseShapeTexelsPacked[addr + 1] =  (unsigned char)(255.0f * value);
                    cloudBaseShapeTexelsPacked[addr + 2] =  (unsigned char)(255.0f * value);
                    cloudBaseShapeTexelsPacked[addr + 3] =  (unsigned char)(255.0f);
                }
            }
        }; // end parallel_for
        {
            int width = cloudBaseShapeTextureSize * cloudBaseShapeTextureSize;
            int height = cloudBaseShapeTextureSize;
            writeTGA("assets/noiseShape.tga", width, height, cloudBaseShapeTexels);
            writeTGA("assets/noiseShapePacked.tga", width, height, cloudBaseShapeTexelsPacked);
        }

        int cloudErosionTextureSize = 32;
        int cloudErosionRowBytes = cloudErosionTextureSize * sizeof(unsigned char) * 4;
        int cloudErosionSliceBytes = cloudErosionRowBytes * cloudErosionTextureSize;
        int cloudErosionVolumeBytes = cloudErosionSliceBytes * cloudErosionTextureSize;
        unsigned char *cloudErosionTexels = (unsigned char *)malloc(cloudErosionVolumeBytes);
        unsigned char *cloudErosionTexelsPacked = (unsigned char *)malloc(cloudErosionVolumeBytes);
        for (int s = 0; s < cloudErosionTextureSize; s++)
        {
            const glm::vec3 normFact = glm::vec3(1.0f / float(cloudErosionTextureSize));
            for (int t = 0; t < cloudErosionTextureSize; t++)
            {
                for (int r = 0; r < cloudErosionTextureSize; r++)
                {
                    glm::vec3 coord = glm::vec3(s, t, r) * normFact;

                    // 3 octaves
                    const float cellCount = 2;
                    float worleyNoise0 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 1));
                    float worleyNoise1 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 2));
                    float worleyNoise2 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 4));
                    float worleyNoise3 = (1.0f - Tileable3dNoise::WorleyNoise(coord, cellCount * 8));
                    float worleyFBM0 = worleyNoise0 * 0.625f + worleyNoise1 * 0.25f + worleyNoise2 * 0.125f;
                    float worleyFBM1 = worleyNoise1 * 0.625f + worleyNoise2 * 0.25f + worleyNoise3 * 0.125f;
                    float worleyFBM2 = worleyNoise2 * 0.75f + worleyNoise3 * 0.25f;
                    int addr = r * cloudErosionTextureSize * cloudErosionTextureSize + t * cloudErosionTextureSize + s;
                    addr *= 4;
                    cloudErosionTexels[addr] = (unsigned char)(255.0f * worleyFBM0);
                    cloudErosionTexels[addr + 1] =  (unsigned char)(255.0f * worleyFBM1);
                    cloudErosionTexels[addr + 2] =  (unsigned char)(255.0f * worleyFBM2);
                    cloudErosionTexels[addr + 3] =  (unsigned char)(255.0f);

                    float value = 0.0;
                    {
                        value = worleyFBM0 * 0.625f + worleyFBM1 * 0.25f + worleyFBM2 * 0.125f;
                    }
                    cloudErosionTexelsPacked[addr] =  (unsigned char)(255.0f * value);
                    cloudErosionTexelsPacked[addr + 1] =  (unsigned char)(255.0f * value);
                    cloudErosionTexelsPacked[addr + 2] =  (unsigned char)(255.0f * value);
                    cloudErosionTexelsPacked[addr + 3] =  (unsigned char)(255.0f);
                }
            }
        };

        {
            int width = cloudErosionTextureSize * cloudErosionTextureSize;
            int height = cloudErosionTextureSize;
            writeTGA("assets/noiseErosion.tga", width, height, cloudErosionTexels);
            writeTGA("assets/noiseErosionPacked.tga", width, height, cloudErosionTexelsPacked);
        }

        free(cloudErosionTexels);
        free(cloudErosionTexelsPacked);
        free(cloudBaseShapeTexels);
        free(cloudBaseShapeTexelsPacked);
    }

private:
    ///
    /// Worley noise function based on https://www.shadertoy.com/view/Xl2XRR by Marc-Andre Loyer
    ///

    static float hash(float n)
    {
        return glm::fract(sin(n + 1.951f) * 43758.5453f);
    };
    static float noise(const glm::vec3 &x)
    {
        glm::vec3 p = glm::floor(x);
        glm::vec3 f = glm::fract(x);

        f = f * f * (glm::vec3(3.0f) - glm::vec3(2.0f) * f);
        float n = p.x + p.y * 57.0f + 113.0f * p.z;
        return glm::mix(
            glm::mix(
                glm::mix(hash(n + 0.0f), hash(n + 1.0f), f.x),
                glm::mix(hash(n + 57.0f), hash(n + 58.0f), f.x),
                f.y),
            glm::mix(
                glm::mix(hash(n + 113.0f), hash(n + 114.0f), f.x),
                glm::mix(hash(n + 170.0f), hash(n + 171.0f), f.x),
                f.y),
            f.z);
    };
    static float Cells(const glm::vec3 &p, float cellCount)
    {
        const glm::vec3 pCell = p * cellCount;
        float d = 1.0e10;
        for (int xo = -1; xo <= 1; xo++)
        {
            for (int yo = -1; yo <= 1; yo++)
            {
                for (int zo = -1; zo <= 1; zo++)
                {
                    glm::vec3 tp = glm::floor(pCell) + glm::vec3(xo, yo, zo);

                    tp = pCell - tp - noise(glm::mod(tp, cellCount / 1));

                    d = glm::min(d, dot(tp, tp));
                }
            }
        }
        d = std::fminf(d, 1.0f);
        d = std::fmaxf(d, 0.0f);
        return d;
    };
};

#endif // D_TILEABLE3DNOISE
