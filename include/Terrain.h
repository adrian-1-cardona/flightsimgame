#pragma once

#include "Types.h"
#include <unordered_map>
#include <memory>
#include <vector>

// Chunk system for infinite terrain
struct TerrainChunk {
    int chunkX;
    int chunkZ;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Color> colors;
    bool generated;
    
    TerrainChunk(int x, int z) : chunkX(x), chunkZ(z), generated(false) {}
};

// Hash function for chunk coordinates
struct ChunkCoordHash {
    std::size_t operator()(const std::pair<int, int>& coord) const {
        return std::hash<int>()(coord.first) ^ (std::hash<int>()(coord.second) << 1);
    }
};

class Terrain {
public:
    Terrain();
    ~Terrain() = default;
    
    void generate(int chunkSize, float scale);
    void update(float deltaTime, const Vector3& playerPosition);
    
    // Height queries
    float getHeightAt(float x, float z) const;
    Vector3 getNormalAt(float x, float z) const;
    
    // Terrain properties
    int getChunkSize() const { return chunkSize; }
    float getScale() const { return terrainScale; }
    
    // Runway
    const Runway& getRunway() const { return mainRunway; }
    bool isOnRunway(const Vector3& position) const;
    
    // Rendering data - returns all active chunks
    const std::unordered_map<std::pair<int, int>, std::shared_ptr<TerrainChunk>, ChunkCoordHash>& getChunks() const { 
        return activeChunks; 
    }
    
private:
    void generateChunk(int chunkX, int chunkZ);
    void unloadDistantChunks(const Vector3& playerPosition);
    void loadChunksAroundPlayer(const Vector3& playerPosition);
    
    float smoothNoise(float x, float z) const;
    float perlinNoise(float x, float z) const;
    
    // Chunk system
    int chunkSize = 64;                // Vertices per chunk edge
    float terrainScale = 10.0f;        // Meters per vertex
    float heightScale = 400.0f;        // Max height
    int renderDistance = 4;            // Chunks to render in each direction
    
    std::unordered_map<std::pair<int, int>, std::shared_ptr<TerrainChunk>, ChunkCoordHash> activeChunks;
    std::pair<int, int> lastPlayerChunk = {0, 0};
    
    Runway mainRunway;
};
