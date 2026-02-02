#include "Terrain.h"
#include <cmath>

Terrain::Terrain() {
    // Initialize runway on origin chunk
    mainRunway.startX = -200.0f;
    mainRunway.startZ = 0.0f;
    mainRunway.endX = 200.0f;
    mainRunway.endZ = 0.0f;
    mainRunway.width = 50.0f;
    mainRunway.height = 0.0f;
}

void Terrain::generate(int size, float scale) {
    chunkSize = size;
    terrainScale = scale;
    
    // Generate initial chunks around origin
    loadChunksAroundPlayer({0.0f, 0.0f, 0.0f});
}

void Terrain::update(float deltaTime, const Vector3& playerPosition) {
    int playerChunkX = (int)std::floor(playerPosition.x / (chunkSize * terrainScale));
    int playerChunkZ = (int)std::floor(playerPosition.z / (chunkSize * terrainScale));
    
    std::pair<int, int> currentChunk = {playerChunkX, playerChunkZ};
    
    // Only update if player moved to a new chunk
    if (currentChunk != lastPlayerChunk) {
        lastPlayerChunk = currentChunk;
        loadChunksAroundPlayer(playerPosition);
        unloadDistantChunks(playerPosition);
    }
}

void Terrain::loadChunksAroundPlayer(const Vector3& playerPosition) {
    int playerChunkX = (int)std::floor(playerPosition.x / (chunkSize * terrainScale));
    int playerChunkZ = (int)std::floor(playerPosition.z / (chunkSize * terrainScale));
    
    // Load chunks in render distance
    for (int x = playerChunkX - renderDistance; x <= playerChunkX + renderDistance; ++x) {
        for (int z = playerChunkZ - renderDistance; z <= playerChunkZ + renderDistance; ++z) {
            std::pair<int, int> coord = {x, z};
            if (activeChunks.find(coord) == activeChunks.end()) {
                auto chunk = std::make_shared<TerrainChunk>(x, z);
                activeChunks[coord] = chunk;
                generateChunk(x, z);
            }
        }
    }
}

void Terrain::unloadDistantChunks(const Vector3& playerPosition) {
    int playerChunkX = (int)std::floor(playerPosition.x / (chunkSize * terrainScale));
    int playerChunkZ = (int)std::floor(playerPosition.z / (chunkSize * terrainScale));
    
    int unloadDistance = renderDistance + 2;
    std::vector<std::pair<int, int>> toRemove;
    
    // Find chunks to unload
    for (auto& [coord, chunk] : activeChunks) {
        int dx = std::abs(coord.first - playerChunkX);
        int dz = std::abs(coord.second - playerChunkZ);
        if (dx > unloadDistance || dz > unloadDistance) {
            toRemove.push_back(coord);
        }
    }
    
    // Remove distant chunks
    for (auto& coord : toRemove) {
        activeChunks.erase(coord);
    }
}

void Terrain::generateChunk(int chunkX, int chunkZ) {
    auto chunk_iter = activeChunks.find({chunkX, chunkZ});
    if (chunk_iter == activeChunks.end() || !chunk_iter->second) {
        return;  // Chunk not found or invalid
    }
    
    auto chunk = chunk_iter->second;
    
    float baseX = chunkX * chunkSize * terrainScale;
    float baseZ = chunkZ * chunkSize * terrainScale;
    
    // Generate vertices for this chunk
    for (int z = 0; z < chunkSize; ++z) {
        for (int x = 0; x < chunkSize; ++x) {
            float worldX = baseX + x * terrainScale;
            float worldZ = baseZ + z * terrainScale;
            
            float height = perlinNoise(worldX * 0.01f, worldZ * 0.01f) * heightScale;
            
            // Flatten runway area
            if (std::abs(worldZ - mainRunway.startZ) < mainRunway.width &&
                worldX >= mainRunway.startX && worldX <= mainRunway.endX) {
                height = mainRunway.height;
            }
            
            chunk->vertices.push_back({worldX, height, worldZ});
            chunk->colors.push_back({0.2f, 0.6f, 0.2f, 1.0f}); // Green grass
        }
    }
    
    // Generate normals using neighbor vertices
    for (int z = 0; z < chunkSize; ++z) {
        for (int x = 0; x < chunkSize; ++x) {
            Vector3 normal = {0.0f, 1.0f, 0.0f};
            if (x > 0 && x < chunkSize - 1 && z > 0 && z < chunkSize - 1) {
                Vector3 v0 = chunk->vertices[z * chunkSize + (x - 1)];
                Vector3 v1 = chunk->vertices[z * chunkSize + (x + 1)];
                Vector3 v2 = chunk->vertices[(z - 1) * chunkSize + x];
                Vector3 v3 = chunk->vertices[(z + 1) * chunkSize + x];
                
                Vector3 e1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
                Vector3 e2 = {v3.x - v2.x, v3.y - v2.y, v3.z - v2.z};
                
                // Simplified normal: average of edge directions
                normal.x = -(e1.y * e2.z - e1.z * e2.y);
                normal.y = (e1.x * e2.z - e1.z * e2.x);
                normal.z = -(e1.x * e2.y - e1.y * e2.x);
                
                float len = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                if (len > 0.001f) {
                    normal.x /= len;
                    normal.y /= len;
                    normal.z /= len;
                }
            }
            
            chunk->normals.push_back(normal);
        }
    }
    
    chunk->generated = true;
}

float Terrain::getHeightAt(float x, float z) const {
    return perlinNoise(x * 0.01f, z * 0.01f) * heightScale;
}

Vector3 Terrain::getNormalAt(float x, float z) const {
    float h = 0.1f;
    float h1 = perlinNoise((x - h) * 0.01f, z * 0.01f) * heightScale;
    float h2 = perlinNoise((x + h) * 0.01f, z * 0.01f) * heightScale;
    float h3 = perlinNoise(x * 0.01f, (z - h) * 0.01f) * heightScale;
    float h4 = perlinNoise(x * 0.01f, (z + h) * 0.01f) * heightScale;
    
    Vector3 normal = {
        (h1 - h2) / (2.0f * h),
        1.0f,
        (h3 - h4) / (2.0f * h)
    };
    
    float len = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (len > 0.001f) {
        normal.x /= len;
        normal.y /= len;
        normal.z /= len;
    }
    
    return normal;
}

bool Terrain::isOnRunway(const Vector3& position) const {
    return std::abs(position.z - mainRunway.startZ) < mainRunway.width &&
           position.x >= mainRunway.startX && position.x <= mainRunway.endX;
}

float Terrain::smoothNoise(float x, float z) const {
    int xi = (int)std::floor(x);
    int zi = (int)std::floor(z);
    float xf = x - xi;
    float zf = z - zi;
    
    // Hash function for pseudo-random values
    auto hashNoise = [](int ix, int iz) -> float {
        unsigned int hash = ix * 73856093 ^ iz * 19349663;
        hash = (hash ^ (hash >> 13)) * 1274126177U;
        return ((float)(hash & 0x7FFFFFFF) / 0x7FFFFFFF) * 2.0f - 1.0f;
    };
    
    // Smoothstep interpolation
    float u = xf * xf * (3.0f - 2.0f * xf);
    float v = zf * zf * (3.0f - 2.0f * zf);
    
    float n00 = hashNoise(xi, zi);
    float n10 = hashNoise(xi + 1, zi);
    float n01 = hashNoise(xi, zi + 1);
    float n11 = hashNoise(xi + 1, zi + 1);
    
    float nx0 = n00 * (1.0f - u) + n10 * u;
    float nx1 = n01 * (1.0f - u) + n11 * u;
    
    return nx0 * (1.0f - v) + nx1 * v;
}

float Terrain::perlinNoise(float x, float z) const {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;
    
    // Octaves of Perlin noise
    for (int i = 0; i < 4; ++i) {
        value += smoothNoise(x * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }
    
    return value / maxValue;
}