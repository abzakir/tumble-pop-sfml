#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <cstdlib>   // For rand()
#include <ctime>     // For time()
//haider shampy

using namespace sf;
using namespace std;

// Helper function to convert int to char array
void intToChar(int num, char* buffer, int bufferSize)
{
    if (num == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    int i = 0;
    bool negative = false;
    if (num < 0)
    {
        negative = true;
        num = -num;
    }
    
    char temp[20];
    int tempIdx = 0;
    while (num > 0 && tempIdx < 19)
    {
        temp[tempIdx++] = '0' + (num % 10);
        num /= 10;
    }
    
    int idx = 0;
    if (negative && idx < bufferSize - 1)
    {
        buffer[idx++] = '-';
    }
    
    for (int j = tempIdx - 1; j >= 0 && idx < bufferSize - 1; j--)
    {
        buffer[idx++] = temp[j];
    }
    buffer[idx] = '\0';
}

// Helper function to build score string
void buildScoreString(int score, char* buffer, int bufferSize)
{
    buffer[0] = 'S';
    buffer[1] = 'C';
    buffer[2] = 'O';
    buffer[3] = 'R';
    buffer[4] = 'E';
    buffer[5] = ':';
    buffer[6] = ' ';
    int idx = 7;
    
    char numStr[20];
    intToChar(score, numStr, 20);
    
    int numIdx = 0;
    while (numStr[numIdx] != '\0' && idx < bufferSize - 1)
    {
        buffer[idx++] = numStr[numIdx++];
    }
    buffer[idx] = '\0';
}

// Helper function to build combo string
void buildComboString(int combo, char* buffer, int bufferSize)
{
    buffer[0] = 'C';
    buffer[1] = 'O';
    buffer[2] = 'M';
    buffer[3] = 'B';
    buffer[4] = 'O';
    buffer[5] = ' ';
    buffer[6] = 'x';
    int idx = 7;
    
    char numStr[20];
    intToChar(combo, numStr, 20);
    
    int numIdx = 0;
    while (numStr[numIdx] != '\0' && idx < bufferSize - 1)
    {
        buffer[idx++] = numStr[numIdx++];
    }
    buffer[idx++] = '!';
    buffer[idx] = '\0';
}

// Helper function to build captured string
void buildCapturedString(int captured, int limit, char* buffer, int bufferSize)
{
    buffer[0] = 'C';
    buffer[1] = 'A';
    buffer[2] = 'P';
    buffer[3] = 'T';
    buffer[4] = 'U';
    buffer[5] = 'R';
    buffer[6] = 'E';
    buffer[7] = 'D';
    buffer[8] = ':';
    buffer[9] = ' ';
    int idx = 10;
    
    char numStr1[20];
    intToChar(captured, numStr1, 20);
    int numIdx = 0;
    while (numStr1[numIdx] != '\0' && idx < bufferSize - 1)
    {
        buffer[idx++] = numStr1[numIdx++];
    }
    
    if (idx < bufferSize - 1) buffer[idx++] = '/';
    
    char numStr2[20];
    intToChar(limit, numStr2, 20);
    numIdx = 0;
    while (numStr2[numIdx] != '\0' && idx < bufferSize - 1)
    {
        buffer[idx++] = numStr2[numIdx++];
    }
    buffer[idx] = '\0';
}

// Helper to check if line starts with prefix
bool lineStartsWith(const char* line, const char* prefix, int prefixLen)
{
    for (int i = 0; i < prefixLen; i++)
    {
        if (line[i] != prefix[i] || line[i] == '\0')
            return false;
    }
    return true;
}

// Helper to parse unsigned int from line
unsigned parseUIntFromLine(const char* line, int startIdx)
{
    unsigned result = 0;
    int idx = startIdx;
    while (line[idx] >= '0' && line[idx] <= '9')
    {
        result = result * 10 + (line[idx] - '0');
        idx++;
    }
    return result;
}

// Helper to parse int from line
int parseIntFromLine(const char* line, int startIdx)
{
    int result = 0;
    int idx = startIdx;
    bool negative = false;
    if (line[idx] == '-')
    {
        negative = true;
        idx++;
    }
    while (line[idx] >= '0' && line[idx] <= '9')
    {
        result = result * 10 + (line[idx] - '0');
        idx++;
    }
    return negative ? -result : result;
}

// Helper to parse float from line
float parseFloatFromLine(const char* line, int startIdx)
{
    float result = 0.0f;
    int idx = startIdx;
    bool negative = false;
    if (line[idx] == '-')
    {
        negative = true;
        idx++;
    }
    
    // Parse integer part
    while (line[idx] >= '0' && line[idx] <= '9')
    {
        result = result * 10.0f + (line[idx] - '0');
        idx++;
    }
    
    // Parse decimal parts 
    if (line[idx] == '.')
    {
        idx++;
        float decimal = 0.1f;
        while (line[idx] >= '0' && line[idx] <= '9')
        {
            result += (line[idx] - '0') * decimal;
            decimal *= 0.1f;
            idx++;
        }
    }
    
    return negative ? -result : result;
}

int maxInt(int a, int b)
{
    return (a > b) ? a : b;
}
int minInt(int a, int b)
{
    return (a < b) ? a : b;
}
float clampFloat(float v, float lo, float hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// Push enemy to captured stack using static array
void pushCapturedEnemy(int capturedStack[], int capturedEnemyType[], int& capturedStackSize, int enemyIndex, int enemyType, const int MAX_CAPTURED)
{
    // Check if stack is full
    if (capturedStackSize >= MAX_CAPTURED) return;
    
    // Add new enemy
    capturedStack[capturedStackSize] = enemyIndex;
    capturedEnemyType[capturedStackSize] = enemyType;
    capturedStackSize++;
}

// Poping enemy from captures stacks
void popCapturedEnemy(int capturedStack[], int capturedEnemyType[], int& capturedStackSize, int& enemyIndex, int& enemyType)
{
    if (capturedStackSize == 0) return;
    
    // Get top element
    enemyIndex = capturedStack[capturedStackSize - 1];
    enemyType = capturedEnemyType[capturedStackSize - 1];
    
    // Decrement size (LIFO - last in, first out)
    capturedStackSize--;
}

// Dot product helper
float dotProduct(float ax, float ay, float bx, float by)
{
    return ax * bx + ay * by;
}

// Helper: Calculate IntRect for frame index
IntRect frameRect(int idx, int frameW, int frameH) {
    return IntRect(idx * frameW, 0, frameW, frameH);
}

// Helper: Update animation with deltaTime
void updateAnimation(float& animTimer, float dt, float frameDuration, 
                     int& frameIndex, int firstFrame, int lastFrame) {
    animTimer += dt;
    if (animTimer >= frameDuration) {
        animTimer -= frameDuration;
        frameIndex++;
        if (frameIndex > lastFrame) 
            frameIndex = firstFrame;
    }
}

// Update Invisible Man state and teleport logic
void updateInvisibleMan(int i, float dt, float& invis_x, float& invis_y,
                         int& invis_state, float& invis_timer, 
                         float& invis_reappearTimer, int& invis_currentFrame,
                         float cell_size, int width, int height, char lvl[][18]) {
    // States: 0=walking visible, 1=disappearing, 2=invisible, 3=appearing
    
    if (invis_state == 0) { // Walking visible
        invis_timer += dt;
        if (invis_timer >= 3.0f + (rand() % 200) / 100.0f) { // 3-5 seconds
            invis_state = 1; // Start disappearing
            invis_timer = 0.0f;
        }
    }
    else if (invis_state == 1) { // Disappearing
        invis_timer += dt;
        if (invis_timer >= 0.5f) { // Disappear animation duration
            invis_state = 2; // Invisible
            invis_timer = 0.0f;
            invis_reappearTimer = 1.0f + (rand() % 100) / 100.0f; // 1-2 seconds
        }
    }
    else if (invis_state == 2) { // Invisible
        invis_reappearTimer -= dt;
        if (invis_reappearTimer <= 0.0f) {
            // Teleport to random platform
            int validRows[] = {2, 5, 8, 11, 14};
            int row = validRows[rand() % 5];
            int col = 2 + rand() % (width - 4);
            
            // Find platform position
            while (col < width && lvl[row][col] != '#') col++;
            if (col < width) {
                invis_x = col * cell_size;
                invis_y = row * cell_size - 192; // Adjust for sprite height
            }
            
            invis_state = 3; // Start appearing
            invis_timer = 0.0f;
        }
    }
    else if (invis_state == 3) { // Appearing
        invis_timer += dt;
        if (invis_timer >= 0.5f) { // Appear animation duration
            invis_state = 0; // Back to walking
            invis_timer = 0.0f;
        }
    }
}

// Update Chelnov shooting behavior
void updateChelnovShooting(int i, float dt, float chelnov_x, float chelnov_y,
                            float player_x, float player_y,
                            float& chelnov_shootTimer, int& chelnov_shootState,
                            int& chelnov_uncapturable, float& chelnov_uncapturableTimer, // 0 = false, 1 = true
                            int& chelnov_currentFrame) {
    // States: 0=walking, 1=shooting
    
    if (chelnov_shootState == 0) { // Walking
        chelnov_shootTimer += dt;
        if (chelnov_shootTimer >= 4.0f) { // Every 4 seconds
            chelnov_shootState = 1; // Start shooting
            chelnov_shootTimer = 0.0f;
            chelnov_uncapturable = 1; // Cannot be captured while shooting
            chelnov_uncapturableTimer = 1.0f; // Shooting lasts 1 second
        }
    }
    else if (chelnov_shootState == 1) { // Shooting
        chelnov_uncapturableTimer -= dt;
        if (chelnov_uncapturableTimer <= 0.0f) {
            chelnov_shootState = 0; // Back to walking
            chelnov_uncapturable = 0;
        }
    }
}

// Point-to-segment collision detection for slanted platforms
bool pointOnSegmentCollision(float px, float py, float x1, float y1, float x2, float y2, 
                             float threshold, float& t, float& qx, float& qy)
{
    // Vector from A to B
    float abx = x2 - x1;
    float aby = y2 - y1;
    
    // Vector from A to P
    float apx = px - x1;
    float apy = py - y1;
    
    // Project t = (AP·AB) / (AB·AB)
    float abDot = dotProduct(abx, aby, abx, aby);
    if (abDot < 0.0001f) return false; // Degenerate segment
    
    t = dotProduct(apx, apy, abx, aby) / abDot;
    
    // Check if point is within segment bounds
    if (t < 0.0f || t > 1.0f) return false;
    
    // Compute closest point Q on segment
    qx = x1 + abx * t;
    qy = y1 + aby * t;
    
    // Compute distance from P to Q
    float dx = px - qx;
    float dy = py - qy;
    float dist = sqrt(dx * dx + dy * dy);
    
    return (dist <= threshold);
}

// Generate slanted platforms for Level 2
void generateSlantedPlatforms(unsigned seed, int width, int height, float cell_size,
                               float* p_x1, float* p_y1, float* p_x2, float* p_y2,
                               int* p_isDescendingRight, int* p_active, int max_slants, // 0 = false, 1 = true
                               char lvl[][18])
{
    srand(seed);
    
    // Start with all platforms inactive
    for (int i = 0; i < max_slants; i++)
    {
        p_active[i] = 0;
    }
    
    // We want exactly 1 slanted platform
    int numSlants = 1;
    // Rows with 3-row gaps between platforms
    int validRows[] = {4, 7, 10, 13};
    int numRows = 4;
    
    int platformsGenerated = 0;
    int maxAttempts = 50;
    int attemptCount = 0;
    int platformIndex = 0;
    
    while (platformsGenerated < 1 && attemptCount < maxAttempts)
    {
        attemptCount++;
        
        // Pick length between 4 or 5 cells
        int lengthCells = (rand() % 2 == 0) ? 5 : 4;
        float slopePx = lengthCells * cell_size;
        
        // Pick direction: down-right or down-left
        int dir = rand() % 2;
        
        // Pick a row
        int row = validRows[rand() % numRows];
        
        // Pick starting X position
        float minX = cell_size * 1;
        float maxX = width * cell_size - slopePx - cell_size * 1;
        if (maxX <= minX) maxX = minX + slopePx; // Ensure valid range
        float startX;
        if (maxX > minX)
            startX = minX + (rand() % (int)(maxX - minX + 1));
        else
            startX = minX;
        
        float x1, y1, x2, y2;
        int startCol, endCol, startRow, endRow;
        
        if (dir == 0) // down-right
        {
            x1 = startX;
            y1 = row * cell_size;
            x2 = startX + slopePx;
            y2 = row * cell_size + slopePx;
            p_isDescendingRight[platformIndex] = 1;
            startCol = (int)(x1 / cell_size);
            endCol = (int)(x2 / cell_size);
            startRow = row;
            endRow = row + lengthCells;
        }
        else // down-left
        {
            x1 = startX + slopePx;
            y1 = row * cell_size;
            x2 = startX;
            y2 = row * cell_size + slopePx;
            p_isDescendingRight[platformIndex] = 0;
            startCol = (int)(x2 / cell_size);
            endCol = (int)(x1 / cell_size);
            startRow = row;
            endRow = row + lengthCells;
        }
        
        // Make sure endpoints are in bounds
        if (x1 >= 0 && x1 < width * cell_size && x2 >= 0 && x2 < width * cell_size &&
            y1 >= 0 && y1 < height * cell_size && y2 >= 0 && y2 <= height * cell_size) // Allow y2 at ground level
        {
            // Check if slant path overlaps any blocks
            bool overlapsWithBlocks = false;
            
            // Check cells along the slant path for blocks
            int steps = lengthCells * 4; // More points for better accuracy
            float endpointTolerance = 0.08f; // Skip first and last 8% of path (endpoints)
            
            for (int step = 0; step <= steps; step++)
            {
                float t = (float)step / (float)steps;
                
                // Skip endpoints since we'll add connection blocks there
                if (t <= endpointTolerance || t >= (1.0f - endpointTolerance))
                    continue;
                
                float checkX = x1 + t * (x2 - x1);
                float checkY = y1 + t * (y2 - y1);
                int checkCol = (int)(checkX / cell_size);
                int checkRow = (int)(checkY / cell_size);
                
                // Check if this point hits a block
                for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
                {
                    for (int colOffset = -1; colOffset <= 1; colOffset++)
                    {
                        int testRow = checkRow + rowOffset;
                        int testCol = checkCol + colOffset;
                        
                        if (testRow >= 0 && testRow < height && testCol >= 0 && testCol < width)
                        {
                            if (lvl[testRow][testCol] == '#')
                            {
                                overlapsWithBlocks = true;
                                break;
                            }
                        }
                    }
                    if (overlapsWithBlocks) break;
                }
                if (overlapsWithBlocks) break;
            }
            
            // Only place platform if the path is clear
            if (!overlapsWithBlocks)
            {
                p_x1[platformIndex] = x1;
                p_y1[platformIndex] = y1;
                p_x2[platformIndex] = x2;
                p_y2[platformIndex] = y2;
                p_active[platformIndex] = 1;
                platformsGenerated++;
                
                // Clear blocks along the slant path to make room for it
                int clearSteps = lengthCells * 8; // More steps for thorough clearing
                for (int step = 0; step <= clearSteps; step++)
                {
                    float t = (float)step / (float)clearSteps;
                    float clearX = x1 + t * (x2 - x1);
                    float clearY = y1 + t * (y2 - y1);
                    
                    // Clear blocks around each point on the slant path
                    int clearCol = (int)(clearX / cell_size);
                    int clearRow = (int)(clearY / cell_size);
                    
                    // Clear the cell and adjacent cells to create a clean gap
                    for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
                    {
                        for (int colOffset = -1; colOffset <= 1; colOffset++)
                        {
                            int clearCellRow = clearRow + rowOffset;
                            int clearCellCol = clearCol + colOffset;
                            
                            if (clearCellRow >= 0 && clearCellRow < height && 
                                clearCellCol >= 0 && clearCellCol < width)
                            {
                                // Clear blocks along the slant path (but keep endpoint connection areas)
                                // Only clear if we're not at the very endpoints
                                float distToStart = sqrt((clearX - x1) * (clearX - x1) + (clearY - y1) * (clearY - y1));
                                float distToEnd = sqrt((clearX - x2) * (clearX - x2) + (clearY - y2) * (clearY - y2));
                                float totalLength = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
                                
                                // Keep blocks at endpoints (first and last 10% of path)
                                if (distToStart > totalLength * 0.1f && distToEnd > totalLength * 0.1f)
                                {
                                    lvl[clearCellRow][clearCellCol] = '.'; // Clear block
                                }
                            }
                        }
                    }
                }
                
                // Place horizontal platform blocks at both ends of the slant for connection
                // Top end (start point) - ensure connection platform exists
                int topCol = (int)(x1 / cell_size);
                int topRow = startRow;
                if (topCol >= 0 && topCol < width && topRow >= 0 && topRow < height)
                {
                    // Place 3-4 blocks at top end for connection (wider platform)
                    for (int i = -1; i <= 2; i++)
                    {
                        int col = topCol + i;
                        if (col >= 0 && col < width && topRow >= 0 && topRow < height)
                        {
                            lvl[topRow][col] = '#'; // Place connection block
                        }
                    }
                }
                
                // Bottom end (end point)  ,we ensure connection platform exists
                int bottomCol = (int)(x2 / cell_size);
                int bottomRow = endRow;
                if (bottomCol >= 0 && bottomCol < width && bottomRow >= 0 && bottomRow < height)
                {
                    // Place 3-4 blocks at bottom end for connection (wider platform)
                    for (int i = -1; i <= 2; i++)
                    {
                        int col = bottomCol + i;
                        if (col >= 0 && col < width && bottomRow >= 0 && bottomRow < height)
                        {
                            lvl[bottomRow][col] = '#'; // Place connection block
                        }
                    }
                }
                
                break; // Successfully generated, exit loop
            }
        }
    }
    
    // If no platforms were generated after all attempts, force at least one simple platform
    if (platformsGenerated == 0)
    {
        // Generate a simple platform at row 7, middle of screen
        float midX = width * cell_size / 2.0f;
        float midY = 7 * cell_size;
        float slantLength = 4 * cell_size;
        p_x1[0] = midX - slantLength / 2.0f;
        p_y1[0] = midY;
        p_x2[0] = midX + slantLength / 2.0f;
        p_y2[0] = midY + slantLength;
        p_isDescendingRight[0] = 1;
        p_active[0] = 1;
    }
}

// sliding physics
void applySlopePhysicsToPlayer(int slopeIndex, float& player_x, float& player_y, 
                                float& velocityY, float playerCenterX, float playerCenterY,
                                float x1, float y1, float x2, float y2, bool isDescendingRight,
                                int PlayerHeight, float slideSpeed)
{
    // Snap player to slope
    float t, qx, qy;
    if (pointOnSegmentCollision(playerCenterX, playerCenterY, x1, y1, x2, y2, 16.0f, t, qx, qy))
    {
        // Set player feet on slope
        player_y = qy - PlayerHeight - 0.01f;
        velocityY = 0.0f;
        
        // Apply sliding
        if (isDescendingRight)
        {
            player_x += slideSpeed;
        }
        else
        {
            player_x -= slideSpeed;
        }
    }
}
void display_level(RenderWindow& window, char lvl[][18], Texture& bgTex, Sprite& bgSprite,
                   Texture& blockTexture, Sprite& blockSprite,
                   Texture& slantTexture, Sprite& slantSprite,
                   const int height, const int width, const int cell_size,
                   float* p_x1, float* p_y1, float* p_x2, float* p_y2,
                   int* p_isDescendingRight, int* p_active, int max_slants, // 0 = false, 1 = true
                   int currentLevel)
{
    // Draw background first
    window.draw(bgSprite);

    // Draw horizontal platforms (blocks) - drawn before slanted platforms
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (lvl[i][j] == '#')
            {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
        }
    }
    
    // Draw slanted platforms using slant.png image (Level 2 only)
    // Drawn AFTER blocks but BEFORE entities to ensure proper layering
    if (currentLevel >= 2)
    {
        for (int i = 0; i < max_slants; i++)
        {
            if (p_active[i] == 0) continue;
            
            // Calculate slope vector
            float dx = p_x2[i] - p_x1[i];
            float dy = p_y2[i] - p_y1[i];
            float length = sqrt(dx * dx + dy * dy);
            
            // Calculate angle for rotation (in degrees) - typically 45 degrees for diagonal
            float angle = atan2(dy, dx) * 180.0f / 3.14159265f;
            
            // Get slant texture size (width only, no Vector2u)
            unsigned int slantWidth = slantTexture.getSize().x;
            
            // Set origin to top-left (0, 0) for proper rotation
            slantSprite.setOrigin(0.0f, 0.0f);
            
            // Scale sprite to match the length of the slant
            float scaleX = (length / slantWidth);
            slantSprite.setScale(scaleX, 1.0f);
            
            // Set position to start point
            slantSprite.setPosition(p_x1[i], p_y1[i]);
            
            // Set rotation for diagonal placement
            slantSprite.setRotation(angle);
            
            // Draw slant sprite (clean diagonal bridge between platforms)
            window.draw(slantSprite);
            
            // Reset rotation, scale, and origin for next iteration
            slantSprite.setRotation(0.0f);
            slantSprite.setScale(1.0f, 1.0f);
            slantSprite.setOrigin(0.0f, 0.0f);
        }
    }
}

void player_gravity(char lvl[][18], float& offset_y, float& velocityY, bool& onGround,
                    const float& gravity, float& terminal_Velocity, float& player_x,
                    float& player_y, const int cell_size, int& Pheight, int& Pwidth,
                    const int height, const int width, bool& canDropThrough, int& dropThroughRow,
                    float* p_x1, float* p_y1, float* p_x2, float* p_y2, int* p_isDescendingRight, 
                    int* p_active, int max_slants, int currentLevel, int& currentSlopeIndex, float slideSpeed)
{
    bool onSlope = false;
    if (currentLevel >= 2 && velocityY >= 0)
    {
        float playerCenterX = player_x + Pwidth / 2.0f;
        float playerFootY = player_y + Pheight;
        
        for (int i = 0; i < max_slants; i++)
        {
            if (p_active[i] == 0) continue;
            
            float t, qx, qy;
            if (pointOnSegmentCollision(playerCenterX, playerFootY, 
                                       p_x1[i], p_y1[i], p_x2[i], p_y2[i], 16.0f, t, qx, qy))
            {
                if (playerFootY <= qy + 10.0f && playerFootY >= qy - 10.0f)
                {
                    player_y = qy - Pheight - 0.01f;
                    velocityY = 0.0f;
                    onGround = true;
                    onSlope = true;
                    currentSlopeIndex = i;
                    
                    if (p_isDescendingRight[i])
                        player_x += slideSpeed;
                    else
                        player_x -= slideSpeed;
                    
                    if (t <= 0.01f || t >= 0.99f)
                    {
                        onSlope = false;
                        currentSlopeIndex = -1;
                        onGround = false;
                    }
                    break;
                }
            }
        }
    }
    
    if (!onSlope)
    {
        currentSlopeIndex = -1;
        offset_y = player_y;
        offset_y += velocityY;

        // check columns are clamped within valid range 
        int leftCol = maxInt(0, minInt(width - 1, static_cast<int>(player_x) / cell_size));
        int rightCol = maxInt(0, minInt(width - 1, static_cast<int>(player_x + Pwidth) / cell_size));
        int midCol = maxInt(0, minInt(width - 1, static_cast<int>(player_x + Pwidth / 2) / cell_size));

        if (velocityY >= 0)
        {
            int footRow = minInt(height - 1, static_cast<int>(offset_y + Pheight) / cell_size);

            // Verify footRow is positive before using array array
            if (footRow >= 0 && footRow < height)
            {
                char bottom_left_down = lvl[footRow][leftCol];
                char bottom_right_down = lvl[footRow][rightCol];
                char bottom_mid_down = lvl[footRow][midCol];

                bool isFloor = (footRow == height - 1);
               
                bool shouldCollide = (!canDropThrough || isFloor || footRow > dropThroughRow);
               
                if (canDropThrough && footRow == dropThroughRow)
                {
                    shouldCollide = false; 
                }

                if (shouldCollide && (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#'))
                {
                    onGround = true;
                    velocityY = 0;
                    player_y = footRow * cell_size - Pheight - 0.01f;
                    canDropThrough = false; 
                }
                else
                {
                    player_y = offset_y;
                    onGround = false;
                }
            }
            else
            {
                // If out of bounds , just move
                player_y = offset_y;
                onGround = false;
            }
        }
        else
        {
            player_y = offset_y;
            onGround = false;
        }

        if (!onGround)
        {
            velocityY += gravity;
            if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
        }
    }
}
void updateGhost(
    float& gx, float& gy, float& gvx, float& gvy, int& movingRight, // 0 = false, 1 = true
    float patrolLeft, float patrolRight,
    char lvl[][18], const int cell_size, const int height, const int width,
    const float gravity, const int ghostWidth, const int ghostHeight
)
{
    // Horizontal movement
    if (movingRight == 1)
    {
        gx += gvx;
        if (gx + ghostWidth >= patrolRight)
        {
            movingRight = 0;
            gx = patrolRight - ghostWidth;  // Clamp position
        }
    }
    else
    {
        gx -= gvx;
        if (gx <= patrolLeft)
        {
            movingRight = 1;
            gx = patrolLeft;  // Clamp position
        }
    }

   
    if (gx < 0)
    {
        gx = 0;
        movingRight = 1;
    }
    if (gx + ghostWidth > width * cell_size)
    {
        gx = width * cell_size - ghostWidth;
        movingRight = 0;
    }

    // Apply gravity
    gy += gvy;
    gvy += gravity;

    // Ground collision
    int leftCol  = maxInt(0, static_cast<int>(gx) / cell_size);
    int rightCol = minInt(width - 1, static_cast<int>(gx + ghostWidth - 1) / cell_size);
    int footRow  = minInt(height - 1, static_cast<int>(gy + ghostHeight) / cell_size);

    if (lvl[footRow][leftCol] == '#' || lvl[footRow][rightCol] == '#')
    {
        gvy = 0;
        gy = footRow * cell_size - ghostHeight - 0.01f;
    }
}

bool checkCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    return !(x1 + w1 < x2 || x2 + w2 < x1 || y1 + h1 < y2 || y2 + h2 < y1);
}

bool render_menu(RenderWindow& window, Texture& menuBgTex, Sprite& menuBgSprite, Sprite& player1Sprite, Sprite& player2Sprite, Font& font){
 bool menu=true;
 window.clear();
 window.draw(menuBgSprite);
 
   window.draw(player1Sprite);
    window.draw(player2Sprite);
    
    // Add selection instructions
    Text instructionText;
    instructionText.setFont(font);
    char inst1[] = "Press 1 for Player 1";
    instructionText.setString(inst1);
    instructionText.setCharacterSize(30);
    instructionText.setFillColor(Color::White);
    instructionText.setPosition(150, 550);
    window.draw(instructionText);
    
    Text instructionText2;
    instructionText2.setFont(font);
    char inst2[] = "Press 2 for Player 2";
    instructionText2.setString(inst2);
    instructionText2.setCharacterSize(30);
    instructionText2.setFillColor(Color::White);
    instructionText2.setPosition(550, 550);
    window.draw(instructionText2);
    
 window.display();
 
 
 return menu;

}



int main()
{  
    // Initialize random seed
    srand((unsigned)time(NULL));
    unsigned gameSeed = (unsigned)time(NULL);
    
    int playerlives=3;
    bool isGameOver=false;
    bool isPaused = false;
    int currentLevel = 1;
    int score = 0;
    int comboCount = 0;
    float comboTime = 0.0f;
    const float COMBO_TIMEOUT = 2.0f;
    
    // Capture limit: All levels = 3
    const int CAPTURE_LIMIT_LEVEL1 = 3;
    const int CAPTURE_LIMIT_LEVEL2 = 3;
    int captureLimit = CAPTURE_LIMIT_LEVEL1;
    
    // Captured enemies stack - using static array now
    const int MAX_CAPTURED = 10; // Maximum captured enemies
    int capturedStack[MAX_CAPTURED];
    int capturedEnemyType[MAX_CAPTURED]; // 0=ghost, 1=skeleton, 2=chelnov, 3=invisible
    int capturedStackSize = 0;
    
    // Projectiles
    const int MAX_PROJECTILES = 20;
    float proj_x[MAX_PROJECTILES];
    float proj_y[MAX_PROJECTILES];
    float proj_vx[MAX_PROJECTILES];
    float proj_vy[MAX_PROJECTILES];
    int proj_active[MAX_PROJECTILES];
    int proj_type[MAX_PROJECTILES]; // 0=ghost, 1=skeleton, 2=chelnov, 3=invisible, 4=chelnov_fireball
    int proj_bounced[MAX_PROJECTILES]; // Track if projectile has bounced once
    float proj_lifetime[MAX_PROJECTILES]; // Lifetime timer for fireballs (10 seconds)
    int proj_count = 0;
    
    // Enemy counts: Level 1 vs Level 2
    const int MAX_GHOSTS = 8; // Level 1: 8, Level 2: 4
    const int MAX_SKELETONS = 9; // Level 1: 4, Level 2: 9
    const int MAX_CHELNOV = 4; // Level 2 only: 4
    const int MAX_INVISIBLE = 3; // Level 2 only: 3
    
    int number_of_ghosts = 8; // Will change to 4 in Level 2
    int number_of_skeletons = 4; // Will change to 9 in Level 2
    int number_of_chelnov = 0; // Will change to 4 in Level 2
    int number_of_invisible = 0; // Will change to 3 in Level 2
    
    // Chelnov enemy (max size for Level 2)
    float chelnov_x[MAX_CHELNOV];
    float chelnov_y[MAX_CHELNOV];
    float chelnov_vx[MAX_CHELNOV];
    float chelnov_vy[MAX_CHELNOV];
    int chelnov_movingRight[MAX_CHELNOV]; // 0 = false, 1 = true
    float chelnov_patrolLeft[MAX_CHELNOV];
    float chelnov_patrolRight[MAX_CHELNOV];
    int chelnov_captured[MAX_CHELNOV];
    int chelnov_sucked[MAX_CHELNOV];
    float chelnov_shootTimer[MAX_CHELNOV];
    int chelnov_uncapturable[MAX_CHELNOV]; // 0 = false, 1 = true
    float chelnov_uncapturableTimer[MAX_CHELNOV];
    int chelnov_shootState[MAX_CHELNOV]; // 0=walking, 1=shooting
    int chelnov_currentFrame[MAX_CHELNOV];
    
    // Invisible Man enemy (Level 2 only)
    float invis_x[MAX_INVISIBLE];
    float invis_y[MAX_INVISIBLE];
    float invis_vx[MAX_INVISIBLE];
    float invis_vy[MAX_INVISIBLE];
    int invis_movingRight[MAX_INVISIBLE]; // 0 = false, 1 = true
    float invis_patrolLeft[MAX_INVISIBLE];
    float invis_patrolRight[MAX_INVISIBLE];
    int invis_captured[MAX_INVISIBLE];
    int invis_sucked[MAX_INVISIBLE];
    int invis_state[MAX_INVISIBLE]; // 0=walking, 1=disappearing, 2=invisible, 3=appearing
    float invis_timer[MAX_INVISIBLE];
    float invis_reappearTimer[MAX_INVISIBLE];
    int invis_currentFrame[MAX_INVISIBLE];
    
    // Clock for timing
    Clock gameClock;
    Clock comboClock;
    
    // Slanted platforms for Level 2 
    const int MAX_SLANTS = 3;
    float p_x1[MAX_SLANTS];
    float p_y1[MAX_SLANTS];
    float p_x2[MAX_SLANTS];
    float p_y2[MAX_SLANTS];
    int p_isDescendingRight[MAX_SLANTS]; // 0 = false, 1 = true
    int p_active[MAX_SLANTS];
    float slideSpeed = 1.2f; // Player slide speed on slopes
    int currentSlopeIndex = -1; // Track which slope player is on
    
    // Level 2 random enemy spawning
    int activeEnemies = 0;
    bool enemy_active[MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + MAX_INVISIBLE];
    for (int i = 0; i < MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + MAX_INVISIBLE; i++)
        enemy_active[i] = false;
 
  Texture gameOverTexture;
  gameOverTexture.loadFromFile("Data/gameover.png");
 
  Sprite gameOverSprite;
gameOverSprite.setTexture(gameOverTexture);
gameOverSprite.setPosition(300, 350);

int screen_x = 1136;
int screen_y = 896;
   
    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    const int cell_size = 64;
    const int height = 14;
    const int width = 18;
    char lvl[14][18]; // Now we made level array as static

    Texture bgTex;
    Sprite bgSprite;
    Texture blockTexture;
    Sprite blockSprite;

    // Level 1 uses bg.png, Level 2 uses bg2.png
    bgTex.loadFromFile("Data/bg.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0, 0);
    bgSprite.setScale(2.0,2.0);

    blockTexture.loadFromFile("Data/block1.png");
    blockSprite.setTexture(blockTexture);
    
    // Level 2 slanted platform texture
    // Slanted platform textures and sprites (from tumble2.cpp)
    Texture slantTexture;
    Sprite slantSprite;
    bool slantTextureLoaded = false;
    
    // Load vacuum textures (vacuum.png removed - vacuum is shown in sprite image)
    Texture vacuumSuctionTexture; // p2suction.png for when vacuum is active
    vacuumSuctionTexture.loadFromFile("Data/P2sunction.png");
    Sprite vacuumSprite(vacuumSuctionTexture);
    const int VACUUM_FRAME_WIDTH = 64; // Adjust based on actual sprite sheet
    const int VACUUM_FRAME_HEIGHT = 64;
    const float VACUUM_SCALE = 2.0f;
    int vacuumFrameIndex = 0;
    float vacuumAnimationTimer = 0.0f;
    const float VACUUM_ANIMATION_SPEED = 0.15f; // Time per frame
    bool vacuumWasActive = false; // Track previous frame state
    bool vacuumTurningOff = false; // Track if vacuum is turning off

    Music lvlMusic;
    lvlMusic.openFromFile("Data/mus.ogg");
    lvlMusic.setVolume(20);
    lvlMusic.play();
    lvlMusic.setLoop(true);
   
    Texture menuBgTex;
    menuBgTex.loadFromFile("Data/menu.png");
    Sprite menuBgSprite(menuBgTex);
    menuBgSprite.setScale(6.0,6.0);
   
    Texture player1Texture, player2Texture;
    player1Texture.loadFromFile("Data/player1.png");
    player2Texture.loadFromFile("Data/P2standing.png");

    Sprite player1Sprite(player1Texture);
    Sprite player2Sprite(player2Texture);

   
    player1Sprite.setPosition(200, 400);
    player2Sprite.setPosition(600, 400);
   
    player1Sprite.setScale(4.0f, 4.0f);
    player2Sprite.setScale(4.0f, 4.0f);
    // Load font for menu 
    Font menuFont;
    if (!menuFont.loadFromFile("Data/arcade.ttf"))
    {
        // Font loading failed
    }
    bool inMainMenu = true;
    int selectedMenuOption = 0; // 0: Play, 1: Instructions, 2: Credits, 3: Quit
    
    while(inMainMenu && window.isOpen()) {
        Event ev;
        while(window.pollEvent(ev)){
            if(ev.type == Event::Closed) {
                window.close();
                inMainMenu = false;
            }
            if(ev.type == Event::KeyPressed) {
                if(ev.key.code == Keyboard::Up) {
                    selectedMenuOption--;
                    if(selectedMenuOption < 0) selectedMenuOption = 3;
                }
                if(ev.key.code == Keyboard::Down) {
                    selectedMenuOption++;
                    if(selectedMenuOption > 3) selectedMenuOption = 0;              }
                if(ev.key.code == Keyboard::Return) {
                    if(selectedMenuOption == 0) { // Play Game
                        inMainMenu = false; 
                    }
                    else if(selectedMenuOption == 1) { // Instructions
                        bool showInst = true;
                        while(showInst && window.isOpen()) {
                            Event instEv;
                            while(window.pollEvent(instEv)) {
                                if(instEv.type == Event::Closed) window.close();
                                if(instEv.type == Event::KeyPressed && instEv.key.code == Keyboard::Escape) showInst = false;
                            }
                            window.clear();
                            window.draw(menuBgSprite);
                            Text instText;
                            instText.setFont(menuFont);
                            instText.setString("INSTRUCTIONS\n\nUse Arrow Keys to Move\nUp Arrow to Jump\nDown Arrow to Drop/Crouch\nZ to use Vacuum\nX to Shoot Enemy\n\nPress ESC to Return");
                            instText.setCharacterSize(30);
                            instText.setFillColor(Color::Yellow);
                            instText.setPosition(300, 300);
                            window.draw(instText);
                            window.display();
                        }
                    }
                    else if(selectedMenuOption == 2) { // Credits
                        bool showCred = true;
                        while(showCred && window.isOpen()) {
                            Event credEv;
                            while(window.pollEvent(credEv)) {
                                if(credEv.type == Event::Closed) window.close();
                                if(credEv.type == Event::KeyPressed && credEv.key.code == Keyboard::Escape) showCred = false;
                            }
                            window.clear();
                            window.draw(menuBgSprite);
                            Text credText;
                            credText.setFont(menuFont);
                            credText.setString("CREDITS\n\nCreated by:\nAbdul Zakir and Haider Shampy\n\nBased on Tumble Pop\n\nPress ESC to Return");
                            credText.setCharacterSize(30);
                            credText.setFillColor(Color::Cyan);
                            credText.setPosition(350, 300);
                            window.draw(credText);
                            window.display();
                        }
                    }
                    else if(selectedMenuOption == 3) { // Quit
                        window.close();
                        return 0;
                    }
                }
            }
        }
        
        if(!window.isOpen()) break;

        window.clear();
        window.draw(menuBgSprite);
        
        Text titleText;
        titleText.setFont(menuFont);
        titleText.setString("TUMBLE POP");
        titleText.setCharacterSize(80);
        titleText.setFillColor(Color::Red);
        titleText.setPosition(350, 100);
        window.draw(titleText);

        Text menuOptions[4];
        const char* options[] = {"Play Game", "Instructions", "Credits", "Quit"};
        
        for(int i = 0; i < 4; i++) {
            menuOptions[i].setFont(menuFont);
            menuOptions[i].setString(options[i]);
            menuOptions[i].setCharacterSize(40);
            menuOptions[i].setPosition(450, 300 + (i * 70));
            if(i == selectedMenuOption) {
                menuOptions[i].setFillColor(Color::Yellow);
                menuOptions[i].setStyle(Text::Bold | Text::Underlined);
            } else {
                menuOptions[i].setFillColor(Color::White);
            }
            window.draw(menuOptions[i]);
        }
        
        window.display();
    }
   
    bool menu=true;
    bool player_selection=false;
    int selection_choice=0;
    menuBgTex.loadFromFile("Data/menu.png");
    
    // Character selection loop
    while(menu && !player_selection && window.isOpen()){
      if(!render_menu(window, menuBgTex, menuBgSprite, player1Sprite, player2Sprite, menuFont))
      break;
     
      Event ev;
      while(window.pollEvent(ev)){
       if(ev.type==Event::Closed)
       {
           window.close();
           menu = false;
           break;
       }
       
       if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape)
       {
            window.close();
            menu = false;
            break;
       }
       
       // Use event based key checking for fast processing
       if(ev.type == Event::KeyPressed && ev.key.code == Keyboard::Num1){
           selection_choice=1;
           player_selection=true;
           menu=false;
           break;
       }
       
       if(ev.type == Event::KeyPressed && ev.key.code == Keyboard::Num2){
           selection_choice=2;
           player_selection=true;
           menu=false;
           break;
       }
      }
    }

    float player_x = 800;
    float player_y = 150;
    float speed = 5;

    const float jumpStrength = -20;
    const float gravity = 1;

    bool facingRight = true;

    Sprite PlayerSprite;

    bool onGround = false;

    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;
    float terminal_Velocity = 20;

    int PlayerHeight = 102;
    int PlayerWidth = 96;

    bool downKeyPressed = false;
    bool canDropThrough = false;
    int dropThroughRow = 0;
    bool vaccum = false;
    float vaccum_distance = 300.0f;
    float vaccum_sucking_power = 3.0f;
   
    // Vacuum position 
    float vacuum_x = player_x + PlayerWidth / 2;
    float vacuum_y = player_y + PlayerHeight / 2;
    
    // Vacuum direction are controlled by WASD keys
    float vacuumDirX = 1.0f; 
    float vacuumDirY = 0.0f;
    
    // Invincibility frames
    bool isInvincible = false;
    float invincibilityTimer = 0.0f;
    const float INVINCIBILITY_DURATION = 1.0f;
    bool playerVisible = true;
    float flickerTimer = 0.0f;
    const float FLICKER_RATE = 0.1f;
    
    // Respawn invincibility (3 seconds after respawning)
    float respawnInvincibilityTimer = 0.0f;
    const float RESPAWN_INVINCIBILITY_DURATION = 3.0f;
    bool isRespawnInvincible = false;
   
   
    // Load player textures from Project folder
    Texture PlayerJumpingTexture;
    Texture PlayerSitTexture;
    
    // Player 1 textures
    const int P1_WALK_FRAMES = 9; 
    Texture P1WalkTextures[P1_WALK_FRAMES];
    for (int i = 0; i < P1_WALK_FRAMES; i++)
    {
        char filename[100];
        int idx = 0;
        
       
        const char* prefix = "Project/proj/greenplayer";
        for(int k=0; prefix[k] != '\0'; k++) filename[idx++] = prefix[k];

        // 2. Add Number
        char numStr[20];
        intToChar(i + 4, numStr, 20);
        for(int k=0; numStr[k] != '\0'; k++) filename[idx++] = numStr[k];
        
        // 3. Add ".png"
        const char* ext = ".png";
        for(int k=0; ext[k] != '\0'; k++) filename[idx++] = ext[k];
        
        // 4. Null terminate
        filename[idx] = '\0';

        P1WalkTextures[i].loadFromFile(filename);
    }
    
    // Player 1 jumping
    Texture P1JumpTexture;
    P1JumpTexture.loadFromFile("Project/proj/greenplayer3.png");
    
    // Player 1 collision
    const int P1_COLLISION_FRAMES = 6;
    Texture P1CollisionTextures[P1_COLLISION_FRAMES];
    for (int i = 0; i < P1_COLLISION_FRAMES; i++)
    {
        char filename[100];
        int idx = 0;
        
        const char* prefix = "Project/proj/greenplayer";
        for(int k=0; prefix[k] != '\0'; k++) filename[idx++] = prefix[k];

        char numStr[20];
        intToChar(i + 30, numStr, 20);
        for(int k=0; numStr[k] != '\0'; k++) filename[idx++] = numStr[k];
        
        const char* ext = ".png";
        for(int k=0; ext[k] != '\0'; k++) filename[idx++] = ext[k];
        
        filename[idx] = '\0';
        
        P1CollisionTextures[i].loadFromFile(filename);
    }
    
    // Player 2 textures
    const int P2_WALK_FRAMES = 10; 
    Texture P2WalkTextures[P2_WALK_FRAMES];
    for (int i = 0; i < P2_WALK_FRAMES; i++)
    {
        char filename[100];
        int idx = 0;
        
        const char* prefix = "Project/proj/yellowplayer";
        for(int k=0; prefix[k] != '\0'; k++) filename[idx++] = prefix[k];

        char numStr[20];
        intToChar(i + 1, numStr, 20);
        for(int k=0; numStr[k] != '\0'; k++) filename[idx++] = numStr[k];
        
        const char* ext = ".png";
        for(int k=0; ext[k] != '\0'; k++) filename[idx++] = ext[k];
        
        filename[idx] = '\0';
        
        P2WalkTextures[i].loadFromFile(filename);
    }
    
    // Player 2 jumping
    Texture P2JumpTexture;
    P2JumpTexture.loadFromFile("Project/proj/yellowplayer3.png");
    
    // Player 2 touch
    const int P2_DEATH_FRAMES = 19; 
    Texture P2DeathTextures[P2_DEATH_FRAMES];
    int deathFrameIndex = 0;
    for (int i = 0; i < 21; i++)
    {
        int frameNum = i + 19;
        if (frameNum == 35 || frameNum == 36)
            continue;
        
        char filename[100];
        int idx = 0;
        
        const char* prefix = "Project/proj/yellowplayer";
        for(int k=0; prefix[k] != '\0'; k++) filename[idx++] = prefix[k];

        char numStr[20];
        intToChar(frameNum, numStr, 20);
        for(int k=0; numStr[k] != '\0'; k++) filename[idx++] = numStr[k];
        
        const char* ext = ".png";
        for(int k=0; ext[k] != '\0'; k++) filename[idx++] = ext[k];
        
        filename[idx] = '\0';
        
        P2DeathTextures[deathFrameIndex].loadFromFile(filename);
        deathFrameIndex++;
    }
    // Set initial texture based on player selection
    if(selection_choice==1)
    {
        PlayerJumpingTexture = P1JumpTexture;
        PlayerSitTexture.loadFromFile("Data/P1sit.png"); // Keep sit texture from Data
    }
    else
    {
        PlayerJumpingTexture = P2JumpTexture; // Use yellowplayer3.png for P2 jumping
        PlayerSitTexture.loadFromFile("Data/P1sit.png");
    }
    
    // Set initial texture and scale based on player selection
    float PLAYER_SCALE;
    if (selection_choice == 1)
    {
        PlayerSprite.setTexture(P1WalkTextures[0]);
        PLAYER_SCALE = 1.875f; // Player 1 scale
    }
    else
    {
        PlayerSprite.setTexture(P2WalkTextures[0]);
        PLAYER_SCALE = 2.25f; // Player 2 scale 
    }
    PlayerSprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    PlayerSprite.setPosition(player_x, player_y);
    
    // Player state: 0=standing/walking, 1=running, 2=jumping, 3=sitting/crouching, 4=collision/death
    int playerState = 0;
    int playerCurrentFrame = 0;
    int playerFrameCounter = 0;
    const int playerFrameSpeed = 10; // Slower animation 
    int playerCollisionFrame = 0; // Frame index for collision/death animation
    bool showingCollisionAnimation = false; // Track if showing collision animation
    int collisionAnimationCounter = 0;
    const int COLLISION_ANIMATION_DURATION = 30; // Show collision animation for 30 frames
    
    
    // Track previous onGround state to detect landing
    bool previousOnGround = true;
    bool showingLandingFrame = false;
    int landingFrameCounter = 0;
    const int LANDING_FRAME_DURATION = 10; // Show landing frame for 10 frames
   

    Sprite GhostSprite;
    Sprite SkeletonSprite;
   
    const int GHOST_FRAME_WIDTH  = 50;
    const int GHOST_FRAME_HEIGHT = 50;
    const float GHOST_SCALE = 1.5f; 

    int ghostWidth  = (int)(GHOST_FRAME_WIDTH*GHOST_SCALE);
    int ghostHeight = (int)(GHOST_FRAME_HEIGHT*GHOST_SCALE);
   
    const int SKELETON_FRAME_WIDTH = 58;
    const int SKELETON_FRAME_HEIGHT = 72;
    const float SKELETON_SCALE = 1.625f; 
   
    int skeletonWidth = (int)(SKELETON_FRAME_WIDTH*SKELETON_SCALE);
    int skeletonHeight = (int)(SKELETON_FRAME_HEIGHT*SKELETON_SCALE);
   
    // Load ghost textures from Project folder
    // Ghost walking: ghost1.png, ghost2.png, ghost6.png, ghost7.png, ghost8.png (5 images)
    const int GHOST_WALK_FRAMES = 5;
    Texture GhostWalkTextures[GHOST_WALK_FRAMES];
    GhostWalkTextures[0].loadFromFile("Project/Ghost/ghost1.png");
    GhostWalkTextures[1].loadFromFile("Project/Ghost/ghost2.png");
    GhostWalkTextures[2].loadFromFile("Project/Ghost/ghost6.png");
    GhostWalkTextures[3].loadFromFile("Project/Ghost/ghost7.png");
    GhostWalkTextures[4].loadFromFile("Project/Ghost/ghost8.png");
    
    // Ghost capture and throw logic from Data folder
    Texture GhostSuckTexture;
    Texture GhostThrowTexture;
    GhostSuckTexture.loadFromFile("Data/Ghostsuck.png");
    GhostThrowTexture.loadFromFile("Data/Ghostthrow.png");
    
    GhostSprite.setTexture(GhostWalkTextures[0]);
    GhostSprite.setScale(GHOST_SCALE, GHOST_SCALE);
    
    // Ghost walking animation frame index
    int ghostWalkFrameIndex = 0;
    
    // Load skeleton textures from Project folder
    // Skeleton walking: skeleton1.png to skeleton8.png 
    const int SKELETON_WALK_FRAMES = 8;
    Texture SkeletonWalkTextures[SKELETON_WALK_FRAMES];
    for (int i = 0; i < SKELETON_WALK_FRAMES; i++)
    {
        char filename[100];
        sprintf(filename, "Project/skeleton/skeleton%d.png", i + 1);
        SkeletonWalkTextures[i].loadFromFile(filename);
    }
    
    // Skeleton throw texture (keep from Data folder for now)
    Texture SkeletonThrowTexture;
    SkeletonThrowTexture.loadFromFile("Data/skeletonthrow.png");
    
    SkeletonSprite.setTexture(SkeletonWalkTextures[0]);
    SkeletonSprite.setScale(SKELETON_SCALE, SKELETON_SCALE);
    
    // Skeleton animation texture tracking 
    int skeletonTextureType[MAX_SKELETONS]; // 0=normal, 1=walking, 2=jumping, 3=throw
    for (int i = 0; i < MAX_SKELETONS; i++)
    {
        skeletonTextureType[i] = 1; // Start with walking
    }
    
        // Ghost animation texture tracking
        int ghostTextureType[MAX_GHOSTS]; // 0=walking, 1=suck, 2=throw
        for (int i = 0; i < MAX_GHOSTS; i++)
        {
            ghostTextureType[i] = 0; // Start with walking
        }
    
    // Load Chelnov textures from Project folder
    // Walking, chelnov1.png to chelnov6.png 
    Sprite ChelnovSprite;
    const int CHELNOV_FRAME_WIDTH = 64;
    const int CHELNOV_FRAME_HEIGHT = 64;
    const float CHELNOV_SCALE = 1.1f; 
    int chelnovWidth = (int)(CHELNOV_FRAME_WIDTH*CHELNOV_SCALE);
    int chelnovHeight = (int)(CHELNOV_FRAME_HEIGHT*CHELNOV_SCALE);
    
    const int CHELNOV_WALK_FRAMES = 6;
    Texture ChelnovWalkTextures[CHELNOV_WALK_FRAMES];
    for (int i = 0; i < 6; i++)
    {
        char filename[100];
        sprintf(filename, "Project/chelnov/chelnov%d.png", i);
        ChelnovWalkTextures[i].loadFromFile(filename);
    }
    
    
    // Suction, chelnov10.png to chelnov18.png )
    const int CHELNOV_SUCTION_FRAMES = 8; // Reduced from 9 to skip chelnov14.png
    Texture ChelnovSuctionTextures[CHELNOV_SUCTION_FRAMES];
    int suctionFrameIndex = 0;
    for (int i = 0; i < 9; i++)
    {
        int frameNum = i + 10;
        // Skip chelnov14.png
        if (frameNum == 14)
            continue;
        
        char filename[100];
        sprintf(filename, "Project/chelnov/chelnov%d.png", frameNum);
        ChelnovSuctionTextures[suctionFrameIndex].loadFromFile(filename);
        suctionFrameIndex++;
    }
    
    // Firing, chelnov19.png to chelnov21.png 
    const int CHELNOV_FIRE_FRAMES = 3;
    Texture ChelnovFireTextures[CHELNOV_FIRE_FRAMES];
    for (int i = 0; i < CHELNOV_FIRE_FRAMES; i++)
    {
        char filename[100];
        sprintf(filename, "Project/chelnov/chelnov%d.png", i + 19);
        ChelnovFireTextures[i].loadFromFile(filename);
    }
    
    // Throwing, chelnov22.png to chelnov29.png 
    const int CHELNOV_THROW_FRAMES = 8;
    Texture ChelnovThrowTextures[CHELNOV_THROW_FRAMES];
    for (int i = 0; i < CHELNOV_THROW_FRAMES; i++)
    {
        char filename[100];
        sprintf(filename, "Project/chelnov/chelnov%d.png", i + 22);
        ChelnovThrowTextures[i].loadFromFile(filename);
    }
    
    // Chelnov animation texture tracking (1=walk, 2=jump, 3=suction, 4=fire, 5=throw)
    int chelnovTextureType[MAX_CHELNOV];
    int chelnovFrameIndex[MAX_CHELNOV]; // Current frame index for each chelnov
    for (int i = 0; i < MAX_CHELNOV; i++)
    {
        chelnovTextureType[i] = 1; // Start with walking
        chelnovFrameIndex[i] = 0;
    }
    
    // Load Invisible Man textures from Project folder
    // Walking, invisible1.png to invisible8.png 
    const int INVIS_WALK_FRAMES = 6;
    Texture InvisibleWalkTextures[INVIS_WALK_FRAMES];
    for (int i = 0; i < INVIS_WALK_FRAMES; i++)
    {
        char filename[100];
        sprintf(filename, "Project/invisible/invisible%d.png", i + 1);
        InvisibleWalkTextures[i].loadFromFile(filename);
    }
    
    // Suction: invisible15.png to invisible20.png (6 images)
    const int INVIS_SUCTION_FRAMES = 6;
    Texture InvisibleSuctionTextures[INVIS_SUCTION_FRAMES];
    for (int i = 0; i < INVIS_SUCTION_FRAMES; i++)
    {
        char filename[100];
        sprintf(filename, "Project/invisible/invisible%d.png", i + 15);
        InvisibleSuctionTextures[i].loadFromFile(filename);
    }
    
    // Throwing: invisible21.png to invisible29.png (9 images)
    const int INVIS_THROW_FRAMES = 9;
    Texture InvisibleThrowTextures[INVIS_THROW_FRAMES];
    for (int i = 0; i < INVIS_THROW_FRAMES; i++)
    {
        char filename[100];
        sprintf(filename, "Project/invisible/invisible%d.png", i + 21);
        InvisibleThrowTextures[i].loadFromFile(filename);
    }
    
    Sprite InvisibleManSprite;
    const int INVIS_FRAME_WIDTH = 64;
    const int INVIS_FRAME_HEIGHT = 96;
    const float INVIS_SCALE = 1.5f; 
      int invisWidth = (int)(INVIS_FRAME_WIDTH*INVIS_SCALE);
    int invisHeight = (int)(INVIS_FRAME_HEIGHT*INVIS_SCALE);
    
    InvisibleManSprite.setTexture(InvisibleWalkTextures[0]);
    InvisibleManSprite.setScale(INVIS_SCALE, INVIS_SCALE);
    
    // Invisible Man animation texture tracking (1=walk, 2=suction, 3=throw)
    int invisTextureType[MAX_INVISIBLE];
    int invisFrameIndex[MAX_INVISIBLE]; // Current frame index for each invisible man
    for (int i = 0; i < MAX_INVISIBLE; i++)
    {
        invisTextureType[i] = 1; // Start with walking
        invisFrameIndex[i] = 0;
    }
    
    // Initialize projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        proj_active[i] = 0;
        proj_lifetime[i] = 0.0f;
    }
    
    // Initialize Chelnov 
    for (int i = 0; i < MAX_CHELNOV; i++)
    {
        chelnov_captured[i] = 0;
        chelnov_sucked[i] = 0;
        chelnov_shootTimer[i] = 0.0f;
        chelnov_uncapturable[i] = 0;
        chelnov_uncapturableTimer[i] = 0.0f;
        chelnov_shootState[i] = 0; // Start walking
        chelnov_currentFrame[i] = 0;
    }
    
    // Initialize Invisible Man 
    for (int i = 0; i < MAX_INVISIBLE; i++)
    {
        invis_captured[i] = 0;
        invis_sucked[i] = 0;
        invis_state[i] = 0; // Start walking
        invis_timer[i] = 0.0f;
        invis_reappearTimer[i] = 0.0f;
        invis_currentFrame[i] = 0;
    }

    // Arrays to store ghosts 
    float ghost_x[MAX_GHOSTS];
    float ghost_y[MAX_GHOSTS];
    float ghost_vx[MAX_GHOSTS];
    float ghost_vy[MAX_GHOSTS];
    int  ghost_movingRight[MAX_GHOSTS]; // 0 = false, 1 = true
    float ghost_patrolLeft[MAX_GHOSTS];
    float ghost_patrolRight[MAX_GHOSTS];
    int ghost_captured[MAX_GHOSTS];
    int ghost_sucked[MAX_GHOSTS];
   
    // Possible Y positions for ghosts
    int ghostYPositions[] = {
       4 * cell_size - ghostHeight,   // On platform 4
       7 * cell_size - ghostHeight,   // On platform 7
       10 * cell_size - ghostHeight,  // On platform 10
   };
    int numGhostYPositions = 3;

   // Initialize all 8 ghosts with random positions
   for (int i = 0; i < number_of_ghosts; i++)
   {
        ghost_x[i] = 100.0f + rand() % 800;
       
        int yIndex = rand() % numGhostYPositions;
        ghost_y[i] = ghostYPositions[yIndex];
       
        ghost_vx[i] = 0.5f + (rand() % 1) / 15.0f;
        ghost_vy[i] = 0.0f;
       
        ghost_movingRight[i] = (rand() % 2 == 0) ? 1 : 0;
       
        ghost_patrolLeft[i] = ghost_x[i] - 150.0f;
        ghost_patrolRight[i] = ghost_x[i] + 150.0f;
       
        if (ghost_patrolLeft[i] < 0)
            ghost_patrolLeft[i] = 0;
        if (ghost_patrolRight[i] > width * cell_size - ghostWidth)
            ghost_patrolRight[i] = width * cell_size - ghostWidth;
           
        ghost_captured[i] = 0;
        ghost_sucked[i] = 0;
    }

    // Arrays to store skeletons 
    float skeleton_x[MAX_SKELETONS];
    float skeleton_y[MAX_SKELETONS];
    float skeleton_vx[MAX_SKELETONS];
    float skeleton_vy[MAX_SKELETONS];
    int  skeleton_movingRight[MAX_SKELETONS]; // 0 = false, 1 = true
    float skeleton_patrolLeft[MAX_SKELETONS];
    float skeleton_patrolRight[MAX_SKELETONS];
   
    int skeleton_captured[MAX_SKELETONS];
    int skeleton_sucked[MAX_SKELETONS];
   
    int skeletonYPositions[] = {
       4 * cell_size - skeletonHeight,
       7 * cell_size - skeletonHeight,
       10 * cell_size - skeletonHeight,
   };
    int numSkeletonYPositions = 3;

   for (int i = 0; i < number_of_skeletons; i++)
   {
        skeleton_x[i] = 100.0f + rand() % 800;
       
        int yIndex = rand() % numSkeletonYPositions;
        skeleton_y[i] = skeletonYPositions[yIndex];
       
        skeleton_vx[i] = 0.5f + (rand() % 5) / 10.0f;
        skeleton_vy[i] = 0.5f+ (rand()% 800)/10.0f;
       
        skeleton_movingRight[i] = (rand() % 2 == 0) ? 1 : 0;
       
        skeleton_patrolLeft[i] = skeleton_x[i] - 150.0f;
        skeleton_patrolRight[i] = skeleton_x[i] + 150.0f;
       
        if (skeleton_patrolLeft[i] < 0)
            skeleton_patrolLeft[i] = 0;
        if (skeleton_patrolRight[i] > width * cell_size - skeletonWidth)
            skeleton_patrolRight[i] = width * cell_size - skeletonWidth;
       
        skeleton_captured[i] = 0;
        skeleton_sucked[i] = 0;
    }

    int ghostCurrentFrame  = 0; // For suck animation
    int ghostFrameCounter  = 0;
   
    // Skeleton animation states: 0=walking, 1=jumping, 2=falling, 3=thrown
    int skeletonState[MAX_SKELETONS]; // 0=walking, 1=jumping, 2=falling
    int skeletonCurrentFrame[MAX_SKELETONS];
    int skeletonFrameCounter[MAX_SKELETONS];
    float skeletonJumpTimer[MAX_SKELETONS];
    float skeletonJumpInterval[MAX_SKELETONS]; // Individual jump intervals
    const float skeletonJumpForce = -8.0f;
    float skeletonTargetX[MAX_SKELETONS]; // Target X position for platform jump
    float skeletonTargetY[MAX_SKELETONS]; // Target Y position for platform jump
    int skeletonHasTarget[MAX_SKELETONS]; // 1 if skeleton has a target platform, 0 otherwise
    float skeletonJumpVx[MAX_SKELETONS]; // Horizontal velocity for platform jump
    
    // Initialize skeleton states , give each skeleton different starting frames 
    for (int i = 0; i < MAX_SKELETONS; i++)
    {
        skeletonState[i] = 0; // Start walking
        // Start different skeletons at different frames in the walk cycle for visual variety
        skeletonCurrentFrame[i] = 1 + (i % 4); 
        if (skeletonCurrentFrame[i] > 4) skeletonCurrentFrame[i] = 1;
        skeletonFrameCounter[i] = (i * 2) % 10; // Offset frame counters so they don't all animate in sync
        skeletonJumpTimer[i] = (rand() % 300) / 100.0f; // Random initial jump timer
        skeletonJumpInterval[i] = 2.0f + (rand() % 300) / 100.0f; // 2-5 seconds
        skeletonTargetX[i] = 0.0f;
        skeletonTargetY[i] = 0.0f;
        skeletonHasTarget[i] = 0;
        skeletonJumpVx[i] = 0.0f;
    }
   
    GhostSprite.setTextureRect(IntRect(0, 0, GHOST_FRAME_WIDTH, GHOST_FRAME_HEIGHT));
    SkeletonSprite.setTextureRect(IntRect(0, 0, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT));

    // Level array is now static

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            lvl[i][j] = '.';

    for (int j = 0; j < width; j++)
        lvl[height - 1][j] = '#';

    lvl[4][2] = '#';  
    lvl[4][3] = '#';  
    lvl[4][4] = '#';  
    lvl[4][5] = '#';
    lvl[4][6] = '#';  
    lvl[4][7] = '#';  
    lvl[4][8] = '#';  
    lvl[4][9] = '#';
    lvl[4][10] = '#';
    lvl[4][11] = '#';
    lvl[4][12] = '#';
    lvl[4][13] = '#';
    lvl[4][14] = '#';
    lvl[4][15] = '#';

    lvl[7][0] = '#';  
    lvl[7][1] = '#';  
    lvl[7][2] = '#';  
    lvl[7][3] = '#';
    lvl[7][4] = '#';  
    lvl[7][7] = '#';  
    lvl[7][8] = '#';  
    lvl[7][9] = '#';
    lvl[7][10] = '#';
    lvl[7][13] = '#';
    lvl[7][14] = '#';
    lvl[7][15] = '#';
    lvl[7][16] = '#';
    lvl[7][17] = '#';

    lvl[10][0] = '#';
    lvl[10][1] = '#';
    lvl[10][2] = '#';
    lvl[10][3] = '#';
    lvl[10][4] = '#';
    lvl[10][5] = '#';
    lvl[10][12] = '#';
    lvl[10][13] = '#';
    lvl[10][14] = '#';
    lvl[10][15] = '#';
    lvl[10][16] = '#';
    lvl[10][17] = '#';

    Texture heartTexture;
    heartTexture.loadFromFile("Data/heart_full.png");

    Sprite hearts[3];
    for (int i = 0; i < 3; i++)
    {
        hearts[i].setTexture(heartTexture);
        hearts[i].setPosition(10 + i * 40, 10);
        hearts[i].setScale(0.1f,0.1f);
    }
    
    // Font and text for score
    Font scoreFont;
    if (!scoreFont.loadFromFile("Data/arcade.ttf"))
    {
        // If font doesn't exist, create a simple text without font
    }
    Text scoreText;
    scoreText.setFont(scoreFont);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(800, 10);
    
    char scoreStr[50];
    buildScoreString(score, scoreStr, 50);
    scoreText.setString(scoreStr);
    
    Text comboText;
    comboText.setFont(scoreFont);
    comboText.setCharacterSize(20);
    comboText.setFillColor(Color::Yellow);
    comboText.setPosition(800, 40);
    
    Text capturedText;
    capturedText.setFont(scoreFont);
    capturedText.setCharacterSize(20);
    capturedText.setFillColor(Color::Cyan);
    capturedText.setPosition(800, 70);
    
    // Projectile sprites
    Sprite projSprites[MAX_PROJECTILES];
    for (int i = 0; i < MAX_PROJECTILES; i++)
    {
        projSprites[i].setTexture(GhostThrowTexture);
        projSprites[i].setScale(1.5f, 1.5f);
    }

   
    Event ev;

    float deltaTime = 0.016f; // ~60 FPS
    
    while (window.isOpen())
    {
        float frameTime = gameClock.getElapsedTime().asSeconds();
        gameClock.restart();
        deltaTime = frameTime;
        
        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
                window.close();
            
            // Pause toggle
            if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::P)
            {
                isPaused = !isPaused;
            }
            
            // Save game
            if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::S && Keyboard::isKeyPressed(Keyboard::LControl))
            {
                // Save game state
                ofstream saveFile("savegame.txt");
                if (saveFile.is_open())
                {
                    saveFile << "SAVE_VERSION 1"<<endl;
                    saveFile << "level " << currentLevel << endl;
                    saveFile << "seed " << gameSeed << endl;
                    saveFile << "player_x " << player_x << endl;
                    saveFile << "player_y " << player_y << endl;
                    saveFile << "player_vx 0"<<endl;
                    saveFile << "player_vy " << velocityY << endl;
                    saveFile << "score " << score << endl;
                    saveFile << "playerlives " << playerlives << endl;
                    saveFile << "captured_count " << capturedStackSize << endl;
                    
                    for (int i = 0; i < capturedStackSize; i++)
                    {
                        saveFile << "captured_" << i << " ";
                        if (capturedEnemyType[i] == 0) saveFile << "GHOST ";
                        else if (capturedEnemyType[i] == 1) saveFile << "SKELETON ";
                        else if (capturedEnemyType[i] == 2) saveFile << "CHELNOV ";
                        else saveFile << "INVISIBLE ";
                        saveFile << capturedStack[i] << endl;
                    }
                    
                    // Save slanted platforms 
                    if (currentLevel >= 2)
                    {
                        saveFile << "slants_count " << MAX_SLANTS << endl;
                        for (int i = 0; i < MAX_SLANTS; i++)
                        {
                            saveFile << "slant_" << i << " " << p_active[i] << " " 
                                    << p_x1[i] << " " << p_y1[i] << " " 
                                    << p_x2[i] << " " << p_y2[i] << " " 
                                    << (p_isDescendingRight[i] ? 1 : 0) << "\n";
                        }
                    }
                    
                    saveFile.close();
                }
            }
            
            // Load game
            if (ev.type == Event::KeyPressed && ev.key.code == Keyboard::L && Keyboard::isKeyPressed(Keyboard::LControl))
            {
                ifstream loadFile("savegame.txt");
                if (loadFile.is_open())
                {
                    char line[256];
                    while (loadFile.getline(line, 256))
                    {
                        if (lineStartsWith(line, "seed ", 5))
                        {
                            gameSeed = parseUIntFromLine(line, 5);
                            srand(gameSeed);
                        }
                        else if (lineStartsWith(line, "player_x ", 9))
                        {
                            player_x = parseFloatFromLine(line, 9);
                        }
                        else if (lineStartsWith(line, "player_y ", 9))
                        {
                            player_y = parseFloatFromLine(line, 9);
                        }
                        else if (lineStartsWith(line, "score ", 6))
                        {
                            score = parseIntFromLine(line, 6);
                        }
                        else if (lineStartsWith(line, "playerlives ", 12))
                        {
                            playerlives = parseIntFromLine(line, 12);
                        }
                    }
                    loadFile.close();
                }
            }
        }
       

         
        if(isGameOver){
            window.clear();
            window.draw(gameOverSprite);
            
            // Display Game Over menu text 
            Text gameOverText;
            gameOverText.setFont(scoreFont);
            char gameOverStr[] = "GAME OVER";
            gameOverText.setString(gameOverStr);
            gameOverText.setCharacterSize(60);
            gameOverText.setFillColor(Color::Red);
            gameOverText.setPosition(400, 200);
            window.draw(gameOverText);
            
            // Display score
            Text scoreDisplayText;
            scoreDisplayText.setFont(scoreFont);
            char scoreDisplayStr[100];
            scoreDisplayStr[0] = 'Y';
            scoreDisplayStr[1] = 'o';
            scoreDisplayStr[2] = 'u';
            scoreDisplayStr[3] = 'r';
            scoreDisplayStr[4] = ' ';
            scoreDisplayStr[5] = 'S';
            scoreDisplayStr[6] = 'c';
            scoreDisplayStr[7] = 'o';
            scoreDisplayStr[8] = 'r';
            scoreDisplayStr[9] = 'e';
            scoreDisplayStr[10] = ':';
            scoreDisplayStr[11] = ' ';
            int idx = 12;
            char scoreNumStr[20];
            intToChar(score, scoreNumStr, 20);
            int numIdx = 0;
            while (scoreNumStr[numIdx] != '\0' && idx < 99)
            {
                scoreDisplayStr[idx++] = scoreNumStr[numIdx++];
            }
            scoreDisplayStr[idx] = '\0';
            scoreDisplayText.setString(scoreDisplayStr);
            scoreDisplayText.setCharacterSize(30);
            scoreDisplayText.setFillColor(Color::White);
            scoreDisplayText.setPosition(400, 300);
            window.draw(scoreDisplayText);
            
            // Instructions
            Text inst1Text;
            inst1Text.setFont(scoreFont);
            char inst1Str[] = "Press ENTER to Restart";
            inst1Text.setString(inst1Str);
            inst1Text.setCharacterSize(24);
            inst1Text.setFillColor(Color::Yellow);
            inst1Text.setPosition(400, 400);
            window.draw(inst1Text);
            
            Text inst2Text;
            inst2Text.setFont(scoreFont);
            char inst2Str[] = "Press M for Main Menu";
            inst2Text.setString(inst2Str);
            inst2Text.setCharacterSize(24);
            inst2Text.setFillColor(Color::Yellow);
            inst2Text.setPosition(400, 450);
            window.draw(inst2Text);
            
            Text inst3Text;
            inst3Text.setFont(scoreFont);
            char inst3Str[] = "Press ESC to Exit Game";
            inst3Text.setString(inst3Str);
            inst3Text.setCharacterSize(24);
            inst3Text.setFillColor(Color::Yellow);
            inst3Text.setPosition(400, 500);
            window.draw(inst3Text);
            
        window.display();
         
         while(window.pollEvent(ev)){
       if(ev.type==Event::Closed)
       window.close();
       
       if(ev.type==Event::KeyPressed && ev.key.code==Keyboard::Escape)
       window.close();
       
                // Restart game 
                if(ev.type==Event::KeyPressed && ev.key.code==Keyboard::Return)
                {
                    // Reset game state
                    playerlives = 3;
                    score = 0;
                    currentLevel = 1;
                    captureLimit = CAPTURE_LIMIT_LEVEL1;
                    isGameOver = false;
                    isInvincible = false;
                    
                    // Reset captured array
                    capturedStackSize = 0;
                    
                    // Reset player
                    player_x = cell_size * 2.0f;
                    player_y = (height - 1) * cell_size - PlayerHeight - 0.01f;
                    velocityY = 0.0f;
                    onGround = true;
                    
                    // Reset enemies 
                    for (int i = 0; i < number_of_ghosts; i++)
                        ghost_captured[i] = 0;
                    for (int i = 0; i < number_of_skeletons; i++)
                        skeleton_captured[i] = 0;
                    for (int i = 0; i < number_of_chelnov; i++)
                        chelnov_captured[i] = 0;
                }
                
                // Main menu 
                if(ev.type==Event::KeyPressed && ev.key.code==Keyboard::M)
                {
                    // Return to main menu
                 
                    playerlives = 3;
                    score = 0;
                    currentLevel = 1;
                    captureLimit = CAPTURE_LIMIT_LEVEL1;
                    isGameOver = false;
                }
            }
            continue; // Skip rest of loop when game over
         }
        
        if (isPaused)
        {
            window.clear();
            Text pauseText;
            pauseText.setFont(scoreFont);
            char pauseStr[] = "PAUSED - Press P to resume";
            pauseText.setString(pauseStr);
            pauseText.setCharacterSize(40);
            pauseText.setFillColor(Color::White);
            pauseText.setPosition(400, 400);
            window.draw(pauseText);
            window.display();
            continue;  
        }
         
        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();
        
        // Update combo timer
        comboTime += deltaTime;
        if (comboTime > COMBO_TIMEOUT)
        {
            comboCount = 0;
            comboTime = 0.0f;
        }

        // Update respawn invincibility of 3 seconds after respawning
        if (isRespawnInvincible)
        {
            respawnInvincibilityTimer += deltaTime;
            flickerTimer += deltaTime;
            
            // Flickering of player during respawn invincibility
            if (flickerTimer >= FLICKER_RATE)
            {
                playerVisible = !playerVisible;
                flickerTimer = 0.0f;
            }
            
            if (respawnInvincibilityTimer >= RESPAWN_INVINCIBILITY_DURATION)
            {
                isRespawnInvincible = false;
                respawnInvincibilityTimer = 0.0f;
                playerVisible = true;
                flickerTimer = 0.0f;
            }
        }
        
        // Update regular invincibility frames 
        if (isInvincible && !isRespawnInvincible)
        {
            invincibilityTimer += deltaTime;
            flickerTimer += deltaTime;
            
            if (flickerTimer >= FLICKER_RATE)
            {
                playerVisible = !playerVisible;
                flickerTimer = 0.0f;
            }
            
            if (invincibilityTimer >= INVINCIBILITY_DURATION)
            {
                isInvincible = false;
                invincibilityTimer = 0.0f;
                playerVisible = true;
                flickerTimer = 0.0f;
            }
        }

// Down key: Go down on ground in Level 1, drop through platforms in Level 2+
if (Keyboard::isKeyPressed(Keyboard::Down))
{
    if (currentLevel == 1 && onGround)
    {
   
        playerState = 3; 
        playerCurrentFrame = 0;
        playerFrameCounter = 0;
        // Small downward movement for visual effect
        player_y += 2.0f; // Small downward movement
        // Clamp to ground
        int footRow = minInt(height - 1, static_cast<int>(player_y + PlayerHeight) / cell_size);
        if (footRow < height - 1)
        {
            player_y = footRow * cell_size - PlayerHeight - 0.01f;
        }
    }
    else if (currentLevel >= 2 && !canDropThrough && onGround)
    {
        // Level 2: Drop through platforms
        int footRow = minInt(height - 1, static_cast<int>(player_y + PlayerHeight) / cell_size);
    if (footRow < height - 1) {
        dropThroughRow = static_cast<int>((player_y + PlayerHeight) / cell_size);
        canDropThrough = true;
        onGround = false;
        velocityY = 1.0f;
        }
    }
}




        // Player movement -- Arrow keys (Left/Right/Up )
        // On slopes, player can resist slide but not stop completely
        float targetX = player_x;
        float moveSpeed = speed;
        
        if (currentSlopeIndex >= 0 && currentSlopeIndex < MAX_SLANTS && p_active[currentSlopeIndex] == 1)
        {
            // allow resistance but still slide on slanted platform
        if (Keyboard::isKeyPressed(Keyboard::Left))
        {
                if (p_isDescendingRight[currentSlopeIndex] == 1)
                {
                    // Resisting slide 
                    targetX -= slideSpeed * 0.4f;
                }
                else
                {
                    // Same direction just increase slide speed by 30%
                    targetX -= slideSpeed * 1.3f;
                }
                facingRight = false;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                if (p_isDescendingRight[currentSlopeIndex] == 1)
                {
                    // Same direction just increase slide speed by 30%
                    targetX += slideSpeed * 1.3f;
                }
                else
                {
                    // Resisting slide just reduce slide speed by 60%
                    targetX += slideSpeed * 0.4f;
                }
                facingRight = true;
            }
            // If no input, slide continues 
        }
        else
        {
            // Normal movement when not on slope
            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                targetX -= moveSpeed;
            facingRight = false;
        }
        if (Keyboard::isKeyPressed(Keyboard::Right))
        {
                targetX += moveSpeed;
            facingRight = true;
            }
        }

        float minX = 0.f;
        float maxX = width * cell_size - PlayerWidth - 1.f;
        targetX = clampFloat(targetX, minX, maxX);

        int topRow = maxInt(0, static_cast<int>(player_y) / cell_size);
        int bottomRow = minInt(height - 1, static_cast<int>(player_y + PlayerHeight - 1) / cell_size);
        int leftCol = maxInt(0, static_cast<int>(targetX) / cell_size);
        int rightCol = minInt(width - 1, static_cast<int>(targetX + PlayerWidth) / cell_size);

        bool blocked = false;
        for (int r = topRow; r <= bottomRow; r++)
        {
            if (lvl[r][leftCol] == '#' || lvl[r][rightCol] == '#')
            {
                blocked = true;
                break;
            }
        }

        if (!blocked)
            player_x = targetX;

        // Vacuum position follows player character 
        vacuum_x = player_x + PlayerWidth / 2;
        vacuum_y = player_y + PlayerHeight / 2;
        
        // Vacuum direction control: WASD keys 
        static bool wPressed = false, aPressed = false, sPressed = false, dPressed = false;
        
        if (Keyboard::isKeyPressed(Keyboard::W) && !wPressed)
        {
            vacuumDirX = 0.0f;
            vacuumDirY = -1.0f; // Up
            wPressed = true;
            aPressed = sPressed = dPressed = false;
        }
        else if (Keyboard::isKeyPressed(Keyboard::A) && !aPressed)
        {
            vacuumDirX = -1.0f; // Left
            vacuumDirY = 0.0f;
            aPressed = true;
            wPressed = sPressed = dPressed = false;
        }
        else if (Keyboard::isKeyPressed(Keyboard::S) && !sPressed)
        {
            vacuumDirX = 0.0f;
            vacuumDirY = 1.0f; // Down
            sPressed = true;
            wPressed = aPressed = dPressed = false;
        }
        else if (Keyboard::isKeyPressed(Keyboard::D) && !dPressed)
        {
            vacuumDirX = 1.0f; // Right
            vacuumDirY = 0.0f;
            dPressed = true;
            wPressed = aPressed = sPressed = false;
        }
        
         // Reset key states when released
        if (!Keyboard::isKeyPressed(Keyboard::W)) wPressed = false;
        if (!Keyboard::isKeyPressed(Keyboard::A)) aPressed = false;
        if (!Keyboard::isKeyPressed(Keyboard::S)) sPressed = false;
        if (!Keyboard::isKeyPressed(Keyboard::D)) dPressed = false;
        
        // Z - Hold to suck
        bool vaccumThisFrame = Keyboard::isKeyPressed(Keyboard::Z);
        
        // Track vacuum state changes
        if (!vacuumWasActive && vaccumThisFrame)
        {
            // Vacuum just turned on - start with first frame
            vacuumFrameIndex = 0;
            vacuumAnimationTimer = 0.0f;
            vacuumTurningOff = false;
        }
        else if (vacuumWasActive && !vaccumThisFrame)
        {
            // start turning off animation if vaccum in this frame is of
            vacuumFrameIndex = 4;
            vacuumAnimationTimer = 0.0f;
            vacuumTurningOff = true;
        }
        
        vaccum = vaccumThisFrame;
        vacuumWasActive = vaccumThisFrame;
        
        // Skip all game logic if game is over to prevent segmentation fault
        // This check must be early to prevent any array access or game logic
        if (isGameOver)
        {
            continue; // Skip to game over screen rendering 
        }

       // Jump with Up arrow key 
       if (Keyboard::isKeyPressed(Keyboard::Up) && onGround)
       {
        velocityY = jumpStrength;
        onGround = false;
        playerState = 2; // Jumping
        playerCurrentFrame = 0; // Reset to first frame
        playerFrameCounter = 0;
       }

      // Check if just landed
       bool justLanded = (!previousOnGround && onGround);
      if (justLanded)
      { 
       showingLandingFrame = true;
       landingFrameCounter = 0;
      }
 
     // Update landing frame counter
     if (showingLandingFrame)
    {
     landingFrameCounter++;
     if (landingFrameCounter >= LANDING_FRAME_DURATION)
     {
        showingLandingFrame = false;
        landingFrameCounter = 0;
    }
   }

   // Update player state based on movement
   if (showingLandingFrame)
    {
     // Keep jumping state while showing landing frame
      playerState = 2;
    }
   else if (Keyboard::isKeyPressed(Keyboard::Down) && currentLevel == 1 && onGround)
   {
    // Down key pressed in Level 1 = sitting/crouching
    playerState = 3;
   }
   else if (!onGround)
    {
     // In air = jumping
     playerState = 2;
    }
   else if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::Right))
    {
     // Moving left or right, use standing texture 
     playerState = 0; // Use standing state
    }
   else
    {
     // Not moving = standing
     playerState = 0;
    }

    // Update collision animation
    if (showingCollisionAnimation)
    {
     collisionAnimationCounter++;
     if (collisionAnimationCounter >= COLLISION_ANIMATION_DURATION)
     {
        showingCollisionAnimation = false;
        collisionAnimationCounter = 0;
        playerCollisionFrame = 0;
     }
     else
     {
        // Animate collision frames
        if (collisionAnimationCounter % 8 == 0) // Change frame every 8 frames 
        {
            if (selection_choice == 1)
            {
                playerCollisionFrame++;
                if (playerCollisionFrame >= P1_COLLISION_FRAMES)
                    playerCollisionFrame = 0;
            }
            else
            {
                playerCollisionFrame++;
                if (playerCollisionFrame >= P2_DEATH_FRAMES)
                    playerCollisionFrame = 0;
            }
         }
      }
   }

// Update player animation
if (showingCollisionAnimation)
{
    // Show death animation
    playerState = 4;
    if (selection_choice == 1)
    {
        PlayerSprite.setTexture(P1CollisionTextures[playerCollisionFrame]);
    }
    else
    {
        PlayerSprite.setTexture(P2DeathTextures[playerCollisionFrame]);
    }
}
else
{
    playerFrameCounter++;
    if (playerState == 2) // Jumping
    {
        // Use jumping texture (single frame)
        if (selection_choice == 1)
        {
            PlayerSprite.setTexture(P1JumpTexture);
        }
        else
        {
            PlayerSprite.setTexture(PlayerJumpingTexture);
        }
    }
    else if (playerState == 3) // Sitting/Crouching
    {
        playerCurrentFrame = 0;
        playerFrameCounter = 0;
        PlayerSprite.setTexture(PlayerSitTexture);
    }
    else // Standing/Walking
    {
        // Animate walking
        if (playerFrameCounter >= playerFrameSpeed)
        {
            playerFrameCounter = 0;
            playerCurrentFrame++;
            if (selection_choice == 1)
            {
                if (playerCurrentFrame >= P1_WALK_FRAMES)
                    playerCurrentFrame = 0;
                PlayerSprite.setTexture(P1WalkTextures[playerCurrentFrame]);
            }
            else
            {
                if (playerCurrentFrame >= P2_WALK_FRAMES)
                    playerCurrentFrame = 0;
                PlayerSprite.setTexture(P2WalkTextures[playerCurrentFrame]);
            }
        }
        else
        {
            // Keep current frame
            if (selection_choice == 1)
            {
                PlayerSprite.setTexture(P1WalkTextures[playerCurrentFrame]);
            }
            else
            {
                PlayerSprite.setTexture(P2WalkTextures[playerCurrentFrame]);
            }
        }
    }
}

    // Update previous onGround state for next frame
     previousOnGround = onGround ;

    // Shooting projectiles, X = throw single enemy 
    static bool shootXKeyPressed = false;

     // X key= Throw single captured enemy from vacuum 
      if (Keyboard::isKeyPressed(Keyboard::X) && !shootXKeyPressed && capturedStackSize > 0)
       {
        int enemyIndex = 0;
        int enemyType = 0;
        popCapturedEnemy(capturedStack, capturedEnemyType, capturedStackSize, enemyIndex, enemyType);
    
     // Find inactive projectile slot
      for (int i = 0; i < MAX_PROJECTILES; i++)
      {
         if (!proj_active[i])
          {
            proj_active[i] = 1;
            proj_x[i] = vacuum_x;
            proj_y[i] = vacuum_y;
            proj_type[i] = enemyType;
            proj_bounced[i] = 0; // Reset bounce counter when throwing
            
            // Set velocity based on sprite facing direction in Level 1, vacuum direction in Level 2
            float shootSpeed = 8.0f;
            if (currentLevel == 1)
            {
                //  throw in direction sprite is facing
            proj_vx[i] = facingRight ? shootSpeed : -shootSpeed;
            proj_vy[i] = 0.0f;
            }
            else
            {
                //use vacuum direction
                proj_vx[i] = vacuumDirX * shootSpeed;
                proj_vy[i] = vacuumDirY * shootSpeed;
            }
            
            // Set sprite based on type (use throw textures)
            if (enemyType == 0)
            {
                // Ghost thrown: use Ghostthrow.png
                projSprites[i].setTexture(GhostThrowTexture);
                projSprites[i].setTextureRect(IntRect(0, 0, GHOST_FRAME_WIDTH, GHOST_FRAME_HEIGHT));
            }
            else if (enemyType == 1)
            {
                // Skeleton thrown: use skeletonthrow.png
                projSprites[i].setTexture(SkeletonThrowTexture);
                projSprites[i].setTextureRect(IntRect(0, 0, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT));
                // Store projectile frame index for animation
                proj_type[i] = enemyType; // Already set, but ensure it's set
            }
            else if (enemyType == 2)
            {
                // Chelnov thrown: use Chelnovthrow.png
                projSprites[i].setTexture(ChelnovThrowTextures[0]);
                projSprites[i].setTextureRect(IntRect(0, 0, CHELNOV_FRAME_WIDTH, CHELNOV_FRAME_HEIGHT));
            }
            else if (enemyType == 3)
            {
                // Invisible Man thrown: use Invisiblethrow.png
                projSprites[i].setTexture(InvisibleThrowTextures[0]);
                projSprites[i].setTextureRect(IntRect(0, 0, INVIS_FRAME_WIDTH, INVIS_FRAME_HEIGHT));
            }
            
            break;
        }
    }
    shootXKeyPressed = true;
}
else if (!Keyboard::isKeyPressed(Keyboard::X))
{
    shootXKeyPressed = false;
}

        window.clear();

        // Load slant texture if not already loaded
        if (!slantTextureLoaded && currentLevel >= 2)
        {
            slantTexture.loadFromFile("slant.png");
            slantSprite.setTexture(slantTexture);
            slantTextureLoaded = true;
        }
        
        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, slantTexture, slantSprite, height, width, cell_size,
                     p_x1, p_y1, p_x2, p_y2, p_isDescendingRight, p_active, MAX_SLANTS, currentLevel);

        player_gravity(lvl, offset_y, velocityY, onGround, gravity, terminal_Velocity,
                       player_x, player_y, cell_size, PlayerHeight, PlayerWidth, height, width, canDropThrough, dropThroughRow,
                       p_x1, p_y1, p_x2, p_y2, p_isDescendingRight, p_active, MAX_SLANTS, currentLevel, currentSlopeIndex, slideSpeed);
        
        // Sliding physics is now handled in player_gravity function
        
        // Level 2 random enemy spawning - maintain exactly 4 active enemies (randomly selected from pool)
        if (currentLevel == 2)
        {
            // Count active enemies (not captured)
            activeEnemies = 0;
            
            for (int i = 0; i < number_of_ghosts; i++)
            {
                if (!ghost_captured[i] && enemy_active[i])
                    activeEnemies++;
            }
            for (int i = 0; i < number_of_skeletons; i++)
            {
                if (!skeleton_captured[i] && enemy_active[MAX_GHOSTS + i])
                    activeEnemies++;
            }
            for (int i = 0; i < number_of_chelnov; i++)
            {
                if (!chelnov_captured[i] && enemy_active[MAX_GHOSTS + MAX_SKELETONS + i])
                    activeEnemies++;
            }
            for (int i = 0; i < number_of_invisible; i++)
            {
                if (!invis_captured[i] && enemy_active[MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + i])
                    activeEnemies++;
            }
            
            // Spawn enemies to maintain exactly 4 active at all times (random selection from pool)
            // Count how many enemies are still available to spawn (not yet captured)
            int availableGhosts = 0, availableSkeletons = 0, availableChelnov = 0, availableInvisible = 0;
            for (int i = 0; i < number_of_ghosts; i++)
                if (ghost_captured[i]) availableGhosts++;
            for (int i = 0; i < number_of_skeletons; i++)
                if (skeleton_captured[i]) availableSkeletons++;
            for (int i = 0; i < number_of_chelnov; i++)
                if (chelnov_captured[i]) availableChelnov++;
            for (int i = 0; i < number_of_invisible; i++)
                if (invis_captured[i]) availableInvisible++;
            
            int totalAvailable = availableGhosts + availableSkeletons + availableChelnov + availableInvisible;
            
            // Only spawn if we have less than 4 active AND there are enemies available
            while (activeEnemies < 4 && totalAvailable > 0)
            {
                // Randomly select enemy type from available pool
                int enemyType = -1;
                int randomChoice = rand() % 20; // 0-19
                
                if (randomChoice < 3 && availableInvisible > 0)
                    enemyType = 3; // Invisible Man
                else if (randomChoice < 7 && availableChelnov > 0)
                    enemyType = 2; // Chelnov
                else if (randomChoice < 16 && availableSkeletons > 0)
                    enemyType = 1; // Skeleton
                else if (availableGhosts > 0)
                    enemyType = 0; // Ghost
                else
                {
                    // No available enemies of selected type, try others
                    if (availableInvisible > 0) enemyType = 3;
                    else if (availableChelnov > 0) enemyType = 2;
                    else if (availableSkeletons > 0) enemyType = 1;
                    else if (availableGhosts > 0) enemyType = 0;
                    else break; // No enemies available
                }
                
                int ghostIdx = 0;
                int skeletonIdx = 0;
                int chelnovIdx = 0;
                int invisIdx = 0;
                bool spawned = false;
                
                if (enemyType == 0) // Ghost
                {
                    // Find inactive ghost 
                    while (ghostIdx < number_of_ghosts && enemy_active[ghostIdx])
                        ghostIdx++;
                    if (ghostIdx < number_of_ghosts)
                    {
                        enemy_active[ghostIdx] = true;
                        ghost_captured[ghostIdx] = 0; // Reset captured flag to reuse
                        ghost_x[ghostIdx] = 100.0f + rand() % 800;
                        int yIndex = rand() % 5;
                        int ghostYPositions[] = {
                            2 * cell_size - ghostHeight,
                            5 * cell_size - ghostHeight,
                            8 * cell_size - ghostHeight,
                            11 * cell_size - ghostHeight,
                            14 * cell_size - ghostHeight
                        };
                        ghost_y[ghostIdx] = ghostYPositions[yIndex];
                        ghost_vx[ghostIdx] = 0.5f + (rand() % 1) / 15.0f;
                        ghost_vy[ghostIdx] = 0.0f;
                        ghost_movingRight[ghostIdx] = (rand() % 2 == 0) ? 1 : 0;
                        ghost_patrolLeft[ghostIdx] = ghost_x[ghostIdx] - 150.0f;
                        ghost_patrolRight[ghostIdx] = ghost_x[ghostIdx] + 150.0f;
                        if (ghost_patrolLeft[ghostIdx] < 0) ghost_patrolLeft[ghostIdx] = 0;
                        if (ghost_patrolRight[ghostIdx] > width * cell_size - ghostWidth)
                            ghost_patrolRight[ghostIdx] = width * cell_size - ghostWidth;
                        ghost_sucked[ghostIdx] = 0;
                        activeEnemies++;
                        availableSkeletons--; // One less available
                        totalAvailable--;
                        spawned = true;
                    }
                }
                else if (enemyType == 1) // Skeleton
                {
                    // Find inactive skeleton 
                    while (skeletonIdx < number_of_skeletons && enemy_active[MAX_GHOSTS + skeletonIdx])
                        skeletonIdx++;
                    if (skeletonIdx < number_of_skeletons)
                    {
                        enemy_active[MAX_GHOSTS + skeletonIdx] = true;
                        skeleton_captured[skeletonIdx] = 0; // Reset captured flag to reuse
                        skeleton_x[skeletonIdx] = 100.0f + rand() % 800;
                        int yIndex = rand() % 5;
                        int skeletonYPositions[] = {
                            2 * cell_size - skeletonHeight,
                            5 * cell_size - skeletonHeight,
                            8 * cell_size - skeletonHeight,
                            11 * cell_size - skeletonHeight,
                            14 * cell_size - skeletonHeight
                        };
                        skeleton_y[skeletonIdx] = skeletonYPositions[yIndex];
                        skeleton_vx[skeletonIdx] = 0.8f;
                        skeleton_vy[skeletonIdx] = 0.0f;
                        skeleton_movingRight[skeletonIdx] = (rand() % 2 == 0) ? 1 : 0;
                        skeleton_patrolLeft[skeletonIdx] = skeleton_x[skeletonIdx] - 150.0f;
                        skeleton_patrolRight[skeletonIdx] = skeleton_x[skeletonIdx] + 150.0f;
                        if (skeleton_patrolLeft[skeletonIdx] < 0) skeleton_patrolLeft[skeletonIdx] = 0;
                        if (skeleton_patrolRight[skeletonIdx] > width * cell_size - skeletonWidth)
                            skeleton_patrolRight[skeletonIdx] = width * cell_size - skeletonWidth;
                        skeleton_sucked[skeletonIdx] = 0;
                        skeletonState[skeletonIdx] = 0;
                        skeletonCurrentFrame[skeletonIdx] = 1 + (skeletonIdx % 4);
                        if (skeletonCurrentFrame[skeletonIdx] > 4) skeletonCurrentFrame[skeletonIdx] = 1;
                        skeletonFrameCounter[skeletonIdx] = (skeletonIdx * 2) % 3;
                        skeletonJumpTimer[skeletonIdx] = (rand() % 300) / 100.0f;
                        skeletonJumpInterval[skeletonIdx] = 2.0f + (rand() % 300) / 100.0f;
                        activeEnemies++;
                        availableChelnov--; // One less available
                        totalAvailable--;
                        spawned = true;
                    }
                }
                else if (enemyType == 2) // Chelnov
                {
                    // Find inactive chelnov 
                    while (chelnovIdx < number_of_chelnov && enemy_active[MAX_GHOSTS + MAX_SKELETONS + chelnovIdx])
                        chelnovIdx++;
                    if (chelnovIdx < number_of_chelnov)
                    {
                        enemy_active[MAX_GHOSTS + MAX_SKELETONS + chelnovIdx] = true;
                        chelnov_captured[chelnovIdx] = 0; // Reset captured flag to reuse
                        chelnov_x[chelnovIdx] = 100.0f + rand() % 800;
                        int yIndex = rand() % 5;
                        int chelnovYPositions[] = {
                            2 * cell_size - chelnovHeight,
                            5 * cell_size - chelnovHeight,
                            8 * cell_size - chelnovHeight,
                            11 * cell_size - chelnovHeight,
                            14 * cell_size - chelnovHeight
                        };
                        chelnov_y[chelnovIdx] = chelnovYPositions[yIndex];
                        chelnov_vx[chelnovIdx] = 0.5f;
                        chelnov_vy[chelnovIdx] = 0.0f;
                        chelnov_movingRight[chelnovIdx] = 1;
                        chelnov_patrolLeft[chelnovIdx] = 100.0f;
                        chelnov_patrolRight[chelnovIdx] = 900.0f;
                        chelnov_sucked[chelnovIdx] = 0;
                        chelnov_shootTimer[chelnovIdx] = (rand() % 400) / 100.0f;
                        chelnov_uncapturable[chelnovIdx] = 0;
                        chelnov_uncapturableTimer[chelnovIdx] = 0.0f;
                        chelnov_shootState[chelnovIdx] = 0;
                        chelnov_currentFrame[chelnovIdx] = 0;
                        activeEnemies++;
                        spawned = true;
                    }
                }
                else if (enemyType == 3) // Invisible Man
                {
                    // Find inactive invisible man 
                    while (invisIdx < number_of_invisible && enemy_active[MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + invisIdx])
                        invisIdx++;
                    if (invisIdx < number_of_invisible)
                    {
                        enemy_active[MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + invisIdx] = true;
                        invis_captured[invisIdx] = 0; // Reset captured flag to reuse
                        invis_x[invisIdx] = 100.0f + rand() % 800;
                        int yIndex = rand() % 5;
                        int invisYPositions[] = {
                            2 * cell_size - invisHeight,
                            5 * cell_size - invisHeight,
                            8 * cell_size - invisHeight,
                            11 * cell_size - invisHeight,
                            14 * cell_size - invisHeight
                        };
                        invis_y[invisIdx] = invisYPositions[yIndex];
                        invis_vx[invisIdx] = 0.8f;
                        invis_vy[invisIdx] = 0.0f;
                        invis_movingRight[invisIdx] = 1;
                        invis_patrolLeft[invisIdx] = 100.0f;
                        invis_patrolRight[invisIdx] = 900.0f;
                        invis_sucked[invisIdx] = 0;
                        invis_state[invisIdx] = 0; // Walking state
                        invis_timer[invisIdx] = 0.0f;
                        invis_reappearTimer[invisIdx] = 0.0f;
                        invis_currentFrame[invisIdx] = 0;
                        activeEnemies++;
                        availableInvisible--; // One less available
                        totalAvailable--;
                        spawned = true;
                    }
                }
                
                // If couldn't spawn, break to avoid infinite loop
                if (!spawned)
                {
                    // Couldn't spawn this type, try to break if no enemies available
                    if (totalAvailable == 0) break;
                    // Otherwise continue to try other types
                }
            }
        }
        
        // Update vacuum position to follow player 
        vacuum_x = player_x + PlayerWidth / 2;
        vacuum_y = player_y + PlayerHeight / 2;




        // Update all 8 ghosts
       for (int i = 0; i < number_of_ghosts; i++)
       {
         if (!ghost_captured[i])
          {
           //  Only update if enemy is active
           if (currentLevel == 2 && !enemy_active[i])
            {
              // Skip inactive enemies in Level 2
            }
        else
        {
          updateGhost(ghost_x[i], ghost_y[i], ghost_vx[i], ghost_vy[i], ghost_movingRight[i],
                  ghost_patrolLeft[i], ghost_patrolRight[i],
                  lvl, cell_size, height, width, gravity, ghostWidth, ghostHeight);
           }
         }      
       }

for (int i = 0; i < number_of_skeletons; i++)
{
if (!skeleton_captured[i] && !skeleton_sucked[i])
{
    //  Only update if enemy is active
    if (currentLevel == 2 && !enemy_active[MAX_GHOSTS + i])
    {
        // Skip inactive enemies in Level 2
    }
    else
    {
    // Update jump timer for random jumping to platforms (up or down)
    skeletonJumpTimer[i] += deltaTime;
    if (skeletonState[i] == 0 && skeletonJumpTimer[i] >= skeletonJumpInterval[i])
    {
        // Check if there is a platform above the skeleton
        int currentCol = static_cast<int>(skeleton_x[i] + skeletonWidth / 2) / cell_size;
        bool platformAbove = false;
        
        // Find current platform row
        int currentRow = -1;
        for (int row = height - 1; row >= 0; row--)
        {
            if (currentCol >= 0 && currentCol < width && lvl[row][currentCol] == '#')
            {
                currentRow = row;
                break;
            }
        }
        
        // Check if there's a platform above
        if (currentRow >= 0)
        {
            for (int row = currentRow - 1; row >= 0; row--)
            {
                if (currentCol >= 0 && currentCol < width && lvl[row][currentCol] == '#')
                {
                    platformAbove = true;
                    break;
                }
            }
        }
        
        // If platform is above, jump up 
        if (platformAbove && (rand() % 100 < 50))
        {
            skeletonState[i] = 1; // Start jumping
            skeleton_vy[i] = skeletonJumpForce; // Jump up
            skeletonJumpTimer[i] = 0.0f;
            skeletonJumpInterval[i] = 1.0f + (rand() % 200) / 100.0f; // 1-3 seconds
        }
        else
        {
            // Random chance to jump to any platform 
            if (rand() % 100 < 30)
             {
                // Find current platform row
                int currentRow = -1;
                for (int row = height - 1; row >= 0; row--)
                {
                    int col = static_cast<int>(skeleton_x[i] + skeletonWidth / 2) / cell_size;
                    if (col >= 0 && col < width && lvl[row][col] == '#')
                    {
                        currentRow = row;
                        break;
                    }
                }
                
                // Find available platforms
                int availableRows[20];
                int numAvailable = 0;
                for (int row = 0; row < height - 1; row++)
                {
                    bool hasPlatform = false;
                    for (int col = 0; col < width; col++)
                    {
                        if (lvl[row][col] == '#')
                        {
                            hasPlatform = true;
                            break;
                        }
                    }
                    if (hasPlatform && row != currentRow)
                    {
                        availableRows[numAvailable++] = row;
                    }
                }
                
                if (numAvailable > 0)
                {
                    int targetRow = availableRows[rand() % numAvailable];
                    int platformStart = -1, platformEnd = -1;
                    for (int col = 0; col < width; col++)
                    {
                        if (lvl[targetRow][col] == '#')
                        {
                            if (platformStart == -1) platformStart = col;
                            platformEnd = col;
                        }
                    }
                    
                    if (platformStart >= 0)
                    {
                        int targetCol = platformStart + rand() % (platformEnd - platformStart + 1);
                        skeletonTargetX[i] = targetCol * cell_size;
                        skeletonTargetY[i] = targetRow * cell_size - skeletonHeight;
                        skeletonHasTarget[i] = 1;
                        
                        float distX = skeletonTargetX[i] - skeleton_x[i];
                        float distY = skeletonTargetY[i] - skeleton_y[i];
                        
                        if (distY < 0) // Jumping up
                        {
                            skeletonState[i] = 1;
                            skeleton_vy[i] = skeletonJumpForce;
                            float jumpTime = (-skeletonJumpForce / gravity) * 2.0f;
                            skeletonJumpVx[i] = (jumpTime > 0.1f) ? distX / jumpTime : 2.0f;
                        }
                        else // Falling down
                        {
                            skeletonState[i] = 2;
                            skeleton_vy[i] = 1.0f;
                            float jumpTime = sqrt(2.0f * distY / gravity);
                            if (jumpTime < 0.5f) jumpTime = 0.5f;
                            skeletonJumpVx[i] = (jumpTime > 0.1f) ? distX / jumpTime : 2.0f;
                        }
                    }
                }
            }
            
            skeletonJumpTimer[i] = 0.0f;
            skeletonJumpInterval[i] = 1.0f + (rand() % 200) / 100.0f; // 1-3 seconds
        }
    }
    
    // Update skeleton movement by applying horizontal velocity toward target if jumping or falling
    if (skeletonState[i] == 1 || skeletonState[i] == 2)
    {
        // If skeleton has a target, move horizontally toward it
        if (skeletonHasTarget[i] == 1)
        {
            float distToTarget = skeletonTargetX[i] - skeleton_x[i];
            if (abs(distToTarget) > 5.0f) // Not reached target yet
            {
                // Apply horizontal movement toward target
                skeleton_x[i] += skeletonJumpVx[i] * deltaTime * 60.0f; // Scale by deltaTime
                
                // Update facing direction
                skeleton_movingRight[i] = (skeletonJumpVx[i] > 0) ? 1 : 0;
            }
        }
    }
    
    // Update skeleton movement, gravity and platform collision
    updateGhost(skeleton_x[i], skeleton_y[i], skeleton_vx[i], skeleton_vy[i],
                skeleton_movingRight[i], skeleton_patrolLeft[i], skeleton_patrolRight[i],
                lvl, cell_size, height, width, gravity, skeletonWidth, skeletonHeight);
    
    // Update skeleton state based on velocity
    if (skeletonState[i] == 1 && skeleton_vy[i] >= 0)
    {
        // from jumping to falling, still use jumping texture
        skeletonState[i] = 2; // Falling
        skeletonTextureType[i] = 2; // Keep jumping texture for falling
        skeletonCurrentFrame[i] = 0; // Start falling animation from first frame
        skeletonFrameCounter[i] = 0;
    }
    else if (skeletonState[i] == 2)
    {
        // Check if landed 
        int footRow = minInt(height - 1, static_cast<int>(skeleton_y[i] + skeletonHeight) / cell_size);
        int leftCol = maxInt(0, static_cast<int>(skeleton_x[i]) / cell_size);
        int rightCol = minInt(width - 1, static_cast<int>(skeleton_x[i] + skeletonWidth - 1) / cell_size);
        
        if (footRow < height && (lvl[footRow][leftCol] == '#' || lvl[footRow][rightCol] == '#'))
        {
            // if landed, get back to walking
            skeletonState[i] = 0;
            skeletonTextureType[i] = 1; // Use walking texture
            skeletonCurrentFrame[i] = 0; // Start walking animation from first frame
            skeletonFrameCounter[i] = 0;
            skeletonHasTarget[i] = 0; // Clear target
            skeletonJumpVx[i] = 0.0f; // Reset jump velocity
        }
    }
    }
    }
}

        // Update ghost walking animation  
        ghostFrameCounter++;
        if (ghostFrameCounter >= 10) // Slower animation 
        {
            ghostFrameCounter = 0;
            ghostWalkFrameIndex++;
            if (ghostWalkFrameIndex >= GHOST_WALK_FRAMES)
                ghostWalkFrameIndex = 0;
        }
 
        // Update skeleton walking animation
        for (int i = 0; i < number_of_skeletons; i++)
        {
            if (skeleton_captured[i]) continue;
            
            // Set texture type based on state
            if (skeletonState[i] == 0)
            {
                skeletonTextureType[i] = 1; // Walking texture
                // Animate walking
                skeletonFrameCounter[i]++;
                if (skeletonFrameCounter[i] >= 10) // Slower animation 
                {
                    skeletonFrameCounter[i] = 0;
                    skeletonCurrentFrame[i]++;
                    if (skeletonCurrentFrame[i] >= SKELETON_WALK_FRAMES)
                        skeletonCurrentFrame[i] = 0;
                }
            }
            else if (skeletonState[i] == 1 || skeletonState[i] == 2)
            {
                skeletonTextureType[i] = 2; // Jumping texture
                skeletonCurrentFrame[i] = 0; // First frame only
            }
        }
       
        if (vaccum)
        {
            // Pull ghosts toward vacuum position
       for (int i = 0; i < number_of_ghosts; i++)
        {
         if (ghost_captured[i]) continue;

                // Calculate distance from vacuum center to ghost center
                float ghostCenterX = ghost_x[i] + ghostWidth / 2.0f;
                float ghostCenterY = ghost_y[i] + ghostHeight / 2.0f;
                float dist_x = vacuum_x - ghostCenterX;
                float dist_y = vacuum_y - ghostCenterY;
                float dist = sqrt(dist_x * dist_x + dist_y * dist_y);

                // Check if enemy is in vacuum direction 
                float vacuumDirLen = sqrt(vacuumDirX * vacuumDirX + vacuumDirY * vacuumDirY);
                float normalizedVacuumDirX, normalizedVacuumDirY;
                if (vacuumDirLen < 0.1f)
                {
                    // Default to right if no direction set
                    normalizedVacuumDirX = 1.0f;
                    normalizedVacuumDirY = 0.0f;
                }
                else
                {
                    normalizedVacuumDirX = vacuumDirX / vacuumDirLen;
                    normalizedVacuumDirY = vacuumDirY / vacuumDirLen;
                }
                
                // Calculate dot product to check if enemy is in front of vacuum
                float dotProduct = (dist_x * normalizedVacuumDirX + dist_y * normalizedVacuumDirY) / dist;
                
                // Only suck if enemy is in the direction vacuum is pointing 
                // But also allow if vacuum direction has not been set (
                bool inDirection = (vacuumDirLen < 0.1f) || (dotProduct > 0.3f);
                if (dist < vaccum_distance && dist > 0.1f && inDirection)
                 {
                    ghost_sucked[i] = 1;
                    ghostTextureType[i] = 1; // Use suck texture
                    // Pull toward vacuum in the direction vacuum is pointing
                    ghost_x[i] += normalizedVacuumDirX * vaccum_sucking_power;
                    ghost_y[i] += normalizedVacuumDirY * vaccum_sucking_power;
 
                    // Capture when ghost is very close to vacuum 
                    if (dist < 60.0f && capturedStackSize < captureLimit && !isGameOver)
                      {
                        ghost_captured[i] = 1;
                        ghost_sucked[i] = 0;
                        // Move enemy off screen 
                        ghost_x[i] = -1000.0f;
                        ghost_y[i] = -1000.0f;
                        // Add to captured stack using exact size push
                        pushCapturedEnemy(capturedStack, capturedEnemyType, capturedStackSize, i, 0, MAX_CAPTURED);
                        
                        // Update score
                        score += 50;
                         comboCount++;
                       comboTime = 0.0f;
                       }
                      }
                 else
                    {
                      ghost_sucked[i] = 0;
                      ghostTextureType[i] = 0; // Use walking texture when not sucked
                    }
                   }
           
                   // Pull skeletons toward vacuum position
                for (int i = 0; i < number_of_skeletons; i++)
               {
                 if (skeleton_captured[i]) continue;

                // Calculate distance from vacuum center to skeleton center
                float skeletonCenterX = skeleton_x[i] + skeletonWidth / 2.0f;
                float skeletonCenterY = skeleton_y[i] + skeletonHeight / 2.0f;
                float dist_x = vacuum_x - skeletonCenterX;
                float dist_y = vacuum_y - skeletonCenterY;
                float dist = sqrt(dist_x * dist_x + dist_y * dist_y);

                // Check if enemy is in vacuum direction 
                float vacuumDirLen = sqrt(vacuumDirX * vacuumDirX + vacuumDirY * vacuumDirY);
                float normalizedVacuumDirX, normalizedVacuumDirY;
                if (vacuumDirLen < 0.1f)
                {
                    // Default to right if no direction set
                    normalizedVacuumDirX = 1.0f;
                    normalizedVacuumDirY = 0.0f;
                }
                else
                {
                    normalizedVacuumDirX = vacuumDirX / vacuumDirLen;
                    normalizedVacuumDirY = vacuumDirY / vacuumDirLen;
                }
                
                // Calculate dot product to check if enemy is in front of vacuum
                float dotProduct = (dist_x * normalizedVacuumDirX + dist_y *  normalizedVacuumDirY) / dist;
                
                // Only suck if enemy is in the direction vacuum is pointing
                // But also allow if vacuum direction has not been set 
                bool inDirection = (vacuumDirLen < 0.1f) || (dotProduct > 0.3f);
                if (dist < vaccum_distance && dist > 0.1f && inDirection)
                  { 
                    skeleton_sucked[i] = 1;
 
                    // Pull toward vacuum in the direction vacuum is pointing
                    skeleton_x[i] += normalizedVacuumDirX * vaccum_sucking_power;
                    skeleton_y[i] += normalizedVacuumDirY * vaccum_sucking_power;
                    
                    // Capture when skeleton is very close to vacuum  disappears from screen
                    if (dist < 60.0f && capturedStackSize < captureLimit && !isGameOver)
                     {
                        skeleton_captured[i] = 1;  
                        skeleton_sucked[i] = 0;
                        // Move enemy off screen
                        skeleton_x[i] = -1000.0f;
                        skeleton_y[i] = -1000.0f;
                        // Add to captured stack using exact-size push
                        pushCapturedEnemy(capturedStack, capturedEnemyType, capturedStackSize, i, 1, MAX_CAPTURED);
                        
                        // Update score 
                        score += 75;
                        comboCount++;
                        comboTime = 0.0f;
                     }
                    }
               else
                {
                 skeleton_sucked[i] = 0;
                }
               }
              }
              else
              {
            // Reset if vac is off
               for (int i = 0; i < number_of_ghosts; i++)
               {
                 ghost_sucked[i] = 0;
                 ghostTextureType[i] = 0; // Reset to walking texture
               }
           
              for (int i = 0; i < number_of_skeletons; i++)
                skeleton_sucked[i] = 0;
            
              // Reset Invisible Man suction 
              if (currentLevel >= 2)
              {
                for (int i = 0; i < number_of_invisible; i++)
                    invis_sucked[i] = 0;
              }
            
            // Reset Chelnov suction 
             if (currentLevel >= 2)
              {
                for (int i = 0; i < number_of_chelnov; i++)
                    chelnov_sucked[i] = 0;
              }
           }
        
           // Update Invisible Man 
           if (currentLevel >= 2)
            {
             for (int i = 0; i < number_of_invisible; i++)
             {
                 // Check if invisible man is active and not captured
                 if (enemy_active[MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + i] && !invis_captured[i] && !invis_sucked[i])
                 {
                   
                    updateInvisibleMan(i, deltaTime, invis_x[i], invis_y[i],
                                      invis_state[i], invis_timer[i],
                                      invis_reappearTimer[i], invis_currentFrame[i],
                                      cell_size, width, height, lvl);
                    
                    // Update movement 
                    if (invis_state[i] == 0 || invis_state[i] == 3)
                    {
                        updateGhost(invis_x[i], invis_y[i], invis_vx[i], invis_vy[i],
                                   invis_movingRight[i], invis_patrolLeft[i], invis_patrolRight[i],
                                   lvl, cell_size, height, width, gravity, invisWidth, invisHeight);
                    }
                    
                    // Update animation and texture type
                    if (invis_state[i] == 0) { // Walking
                        invisTextureType[i] = 1; // Use walk texture array
                        invisFrameIndex[i]++;
                        if (invisFrameIndex[i] >= INVIS_WALK_FRAMES * 8) // Slower animation
                        {
                            invisFrameIndex[i] = 0;
                        }
                        invis_currentFrame[i] = invisFrameIndex[i] / 8;
                        if (invis_currentFrame[i] >= INVIS_WALK_FRAMES)
                            invis_currentFrame[i] = 0;
                    }
                    else if (invis_state[i] == 1) { // Disappearing
                        invisTextureType[i] = 1; // Keep walking texture
                        invis_currentFrame[i] = 0;
                    }
                    else if (invis_state[i] == 3) { // Appearing
                        invisTextureType[i] = 1; // Use walking texture
                        invis_currentFrame[i] = 0;
                    }
                    
                    // Vacuum suction for Invisible Man 
                    if (vaccum && invis_state[i] != 2) // Don't suck if invisible
                    {
                        float invisCenterX = invis_x[i] + invisWidth / 2.0f;
                        float invisCenterY = invis_y[i] + invisHeight / 2.0f;
                        float dist_x = vacuum_x - invisCenterX;
                        float dist_y = vacuum_y - invisCenterY;
                        float dist = sqrt(dist_x * dist_x + dist_y * dist_y);
                        
                        float vacuumDirLen = sqrt(vacuumDirX * vacuumDirX + vacuumDirY * vacuumDirY);
                        float normalizedVacuumDirX, normalizedVacuumDirY;
                        if (vacuumDirLen < 0.1f)
                        {
                            normalizedVacuumDirX = 1.0f;
                            normalizedVacuumDirY = 0.0f;
                        }
                        else
                        {
                            normalizedVacuumDirX = vacuumDirX / vacuumDirLen;
                            normalizedVacuumDirY = vacuumDirY / vacuumDirLen;
                        }
                        
                        float dotProduct = (dist_x * normalizedVacuumDirX + dist_y * normalizedVacuumDirY) / dist;
                        bool inDirection = (vacuumDirLen < 0.1f) || (dotProduct > 0.3f);
                        
                        if (dist < vaccum_distance && dist > 1.0f && inDirection)
                        {
                            invis_sucked[i] = 1;
                            invis_x[i] += normalizedVacuumDirX * vaccum_sucking_power;
                            invis_y[i] += normalizedVacuumDirY * vaccum_sucking_power;
                            
                            if (dist < 50.0f && capturedStackSize < captureLimit && !isGameOver)
                            {
                                invis_captured[i] = 1;
                                invis_sucked[i] = 0;
                                invis_x[i] = -1000.0f;
                                invis_y[i] = -1000.0f;
                                pushCapturedEnemy(capturedStack, capturedEnemyType, capturedStackSize, i, 3, MAX_CAPTURED); // Type 3 = invisible
                                score += 150; // Invisible Man: +150 base
                                comboCount++;
                                comboTime = 0.0f;
                            }
                        }
                        else if (!vaccum)
                        {
                            invis_sucked[i] = 0;
                        }
                    }
                }
            }
        }
        
        // Update Chelnov 
        if (currentLevel >= 2)
        {
        for (int i = 0; i < number_of_chelnov; i++)
        {
            // Check if Chelnov is active and not captured
            if (enemy_active[MAX_GHOSTS + MAX_SKELETONS + i] && !chelnov_captured[i])
            {
                    // Update shooting behavior
                    bool justShot = false;
                    if (chelnov_shootState[i] == 0 && chelnov_shootTimer[i] >= 4.0f)
                    {
                        justShot = true; // Will shoot this frame
                    }
                    
                    updateChelnovShooting(i, deltaTime, chelnov_x[i], chelnov_y[i],
                                         player_x, player_y,
                                         chelnov_shootTimer[i], chelnov_shootState[i],
                                         chelnov_uncapturable[i], chelnov_uncapturableTimer[i],
                                         chelnov_currentFrame[i]);
                    
                    // Create fireball when shooting starts
                    if (justShot && chelnov_shootState[i] == 1)
                    {
                        // Find inactive projectile slot for fireball
                        for (int j = 0; j < MAX_PROJECTILES; j++)
                        {
                            if (!proj_active[j])
                            {
                                proj_active[j] = 1;
                                proj_type[j] = 4; // Fireball type
                                proj_x[j] = chelnov_x[i] + chelnovWidth / 2.0f;
                                proj_y[j] = chelnov_y[i] + chelnovHeight / 2.0f;
                                // Fire in direction chelnov is facing
                                float fireballSpeed = 3.0f;
                                proj_vx[j] = chelnov_movingRight[i] ? fireballSpeed : -fireballSpeed;
                                proj_vy[j] = 0.0f;
                                proj_bounced[j] = 0;
                                proj_lifetime[j] = 0.0f; // Start timer
                                
                                // Set fireball sprite (use fireball.png)
                                static Texture fireballTexture;
                                static bool fireballTextureLoaded = false;
                                if (!fireballTextureLoaded)
                                {
                                    fireballTexture.loadFromFile("fireball.png");
                                    fireballTextureLoaded = true;
                                }
                                projSprites[j].setTexture(fireballTexture);
                                projSprites[j].setScale(1.5f, 1.5f);
                                break;
                            }
                        }
                    }
                    
                    // Update animation and texture type
                    float chelnovFrameDuration = 0.15f;
                    if (chelnov_shootState[i] == 0) { // Walking
                        chelnovTextureType[i] = 1; // Use walk texture array
                        chelnovFrameIndex[i]++;
                        if (chelnovFrameIndex[i] >= CHELNOV_WALK_FRAMES * 8) // Change frame every 8 updates 
                        {
                            chelnovFrameIndex[i] = 0;
                        }
                        chelnov_currentFrame[i] = chelnovFrameIndex[i] / 8;
                        if (chelnov_currentFrame[i] >= CHELNOV_WALK_FRAMES)
                            chelnov_currentFrame[i] = 0;
                    }
                    else if (chelnov_shootState[i] == 1) { // Shooting/Firing
                        chelnovTextureType[i] = 4; // Use fire texture array
                        chelnovFrameIndex[i]++;
                        if (chelnovFrameIndex[i] >= CHELNOV_FIRE_FRAMES * 8) // Slower animation
                        {
                            chelnovFrameIndex[i] = 0;
                        }
                        chelnov_currentFrame[i] = chelnovFrameIndex[i] / 8;
                        if (chelnov_currentFrame[i] >= CHELNOV_FIRE_FRAMES)
                            chelnov_currentFrame[i] = 0;
                    }
                
                // Update movement
                if (!chelnov_sucked[i])
                {
                    updateGhost(chelnov_x[i], chelnov_y[i], chelnov_vx[i], chelnov_vy[i],
                               chelnov_movingRight[i], chelnov_patrolLeft[i], chelnov_patrolRight[i],
                               lvl, cell_size, height, width, gravity, chelnovWidth, chelnovHeight);
                }
                
                // Vacuum effect
                if (vaccum && chelnov_uncapturable[i] == 0)
                {
                        float chelnovCenterX = chelnov_x[i] + chelnovWidth / 2.0f;
                        float chelnovCenterY = chelnov_y[i] + chelnovHeight / 2.0f;
                        float dist_x = vacuum_x - chelnovCenterX;
                        float dist_y = vacuum_y - chelnovCenterY;
                    float dist = sqrt(dist_x * dist_x + dist_y * dist_y);
                    
                        // Check if enemy is in vacuum direction 
                        float vacuumDirLen = sqrt(vacuumDirX * vacuumDirX + vacuumDirY * vacuumDirY);
                        float normalizedVacuumDirX, normalizedVacuumDirY;
                        if (vacuumDirLen < 0.1f)
                        {
                            // Default to right if no direction set
                            normalizedVacuumDirX = 1.0f;
                            normalizedVacuumDirY = 0.0f;
                        }
                        else
                        {
                            normalizedVacuumDirX = vacuumDirX / vacuumDirLen;
                            normalizedVacuumDirY = vacuumDirY / vacuumDirLen;
                        }
                        
                        // Calculate dot product to check if enemy is in front of vacuum
                        float dotProduct = (dist_x * normalizedVacuumDirX + dist_y * normalizedVacuumDirY) / dist;
                        
                        // Only suck if enemy is in the direction vacuum is pointing
                        // But also allow if vacuum direction has not been set 
                        bool inDirection = (vacuumDirLen < 0.1f) || (dotProduct > 0.3f);
                        if (dist < vaccum_distance && dist > 1.0f && inDirection)
                    {
                        chelnov_sucked[i] = 1;
                            // Pull toward vacuum in the direction vacuum is pointing
                            chelnov_x[i] += normalizedVacuumDirX * vaccum_sucking_power;
                            chelnov_y[i] += normalizedVacuumDirY * vaccum_sucking_power;
                        
                            if (dist < 50.0f && capturedStackSize < captureLimit && !isGameOver)
                        {
                            chelnov_captured[i] = 1;
                            chelnov_sucked[i] = 0;
                                chelnov_x[i] = -1000.0f;
                                chelnov_y[i] = -1000.0f;
                               
                                pushCapturedEnemy(capturedStack, capturedEnemyType, capturedStackSize, i, 2, MAX_CAPTURED);
                                
                                // Update score 
                                score += 200;
                            comboCount++;
                            comboTime = 0.0f;
                        }
                    }
                    else
                    {
                        chelnov_sucked[i] = 0;
                    }
                }
                else if (!vaccum)
                {
                    chelnov_sucked[i] = 0;
                    }
                }
            }
        }
        
        // Update projectiles
        static int projFrameCounter[MAX_PROJECTILES] = {0};
        static int projCurrentFrame[MAX_PROJECTILES] = {0};
        const int projFrameSpeed = 10;
        
        for (int i = 0; i < MAX_PROJECTILES; i++)
        {
            if (proj_active[i])
            {
                // Update fireball lifetime 
                if (proj_type[i] == 4) // Fireball
                {
                    proj_lifetime[i] += deltaTime;
                    if (proj_lifetime[i] >= 10.0f)
                    {
                        proj_active[i] = 0;
                        proj_lifetime[i] = 0.0f;
                        continue;
                    }
                }
                
                proj_x[i] += proj_vx[i];
                proj_y[i] += proj_vy[i];
                
                // Animate thrown enemies
                projFrameCounter[i]++;
                if (projFrameCounter[i] >= projFrameSpeed)
                {
                    projFrameCounter[i] = 0;
                    projCurrentFrame[i]++;
                    
                    if (proj_type[i] == 0) // Ghost thrown
                    {
                        // Animate ghost throw texture
                        unsigned int texWidth = GhostThrowTexture.getSize().x;
                        int maxFrames = texWidth / GHOST_FRAME_WIDTH;
                        if (projCurrentFrame[i] >= maxFrames)
                            projCurrentFrame[i] = 0;
                        projSprites[i].setTextureRect(IntRect(projCurrentFrame[i] * GHOST_FRAME_WIDTH, 0, GHOST_FRAME_WIDTH, GHOST_FRAME_HEIGHT));
                    }
                    else if (proj_type[i] == 1) // Skeleton thrown
                    {
                        // Animate skeleton throw texture
                        unsigned int texWidth = SkeletonThrowTexture.getSize().x;
                        int maxFrames = texWidth / SKELETON_FRAME_WIDTH;
                        if (projCurrentFrame[i] >= maxFrames)
                            projCurrentFrame[i] = 0;
                        projSprites[i].setTextureRect(IntRect(projCurrentFrame[i] * SKELETON_FRAME_WIDTH, 0, SKELETON_FRAME_WIDTH, SKELETON_FRAME_HEIGHT));
                    }
                    else if (proj_type[i] == 2) // Chelnov thrown
                    {
                        // Animate Chelnov throw texture array
                        projFrameCounter[i]++;
                        if (projFrameCounter[i] >= projFrameSpeed)
                        {
                            projFrameCounter[i] = 0;
                            projCurrentFrame[i]++;
                            if (projCurrentFrame[i] >= CHELNOV_THROW_FRAMES)
                                projCurrentFrame[i] = 0;
                        }
                        projSprites[i].setTexture(ChelnovThrowTextures[projCurrentFrame[i]]);
                    }
                    else if (proj_type[i] == 3) // Invisible Man thrown
                    {
                        // Animate Invisible Man throw texture array
                        projFrameCounter[i]++;
                        if (projFrameCounter[i] >= projFrameSpeed)
                        {
                            projFrameCounter[i] = 0;
                            projCurrentFrame[i]++;
                            if (projCurrentFrame[i] >= INVIS_THROW_FRAMES)
                                projCurrentFrame[i] = 0;
                        }
                        projSprites[i].setTexture(InvisibleThrowTextures[projCurrentFrame[i]]);
                    }
                }
                
                // Bounce off walls fireballs bounce and come back, others bounce once then disappear
                if (proj_x[i] < 0)
                {
                    if (proj_type[i] == 4) // Fireball always bounce
                {
                    proj_vx[i] = -proj_vx[i];
                        proj_x[i] = 0;
                    }
                    else if (proj_bounced[i] == 0)
                    {
                        // First bounce then reverse direction
                        proj_vx[i] = -proj_vx[i];
                        proj_x[i] = 0; // Keep it in bounds
                        proj_bounced[i] = 1;
                    }
                    else
                    {
                        // if already bounced once remove it
                        proj_active[i] = 0;
                        projFrameCounter[i] = 0;
                        projCurrentFrame[i] = 0;
                    }
                }
                else if (proj_x[i] > width * cell_size)
                {
                    if (proj_type[i] == 4) // if fireball always bounce
                    {
                        proj_vx[i] = -proj_vx[i];
                        proj_x[i] = width * cell_size;
                    }
                    else if (proj_bounced[i] == 0)
                    {
                        // First bounce then reverse direction
                        proj_vx[i] = -proj_vx[i];
                        proj_x[i] = width * cell_size; // Keep it in bounds
                        proj_bounced[i] = 1;
                    }
                    else
                    {
                        // if already bounced once, remove it
                        proj_active[i] = 0;
                        projFrameCounter[i] = 0;
                        projCurrentFrame[i] = 0;
                    }
                }
                
                // Remove if out of bounds vertically
                if (proj_y[i] < -100 || proj_y[i] > height * cell_size + 100)
                {
                    proj_active[i] = 0;
                    projFrameCounter[i] = 0;
                    projCurrentFrame[i] = 29;
                }
            }
        }
        
        // Projectile-to-projectile collision 
        for (int i = 0; i < MAX_PROJECTILES; i++)
        {
            if (!proj_active[i]) continue;
            
            for (int j = i + 1; j < MAX_PROJECTILES; j++)
            {
                if (!proj_active[j]) continue;
                
                if (checkCollision(proj_x[i], proj_y[i], 30.0f, 30.0f,
                                  proj_x[j], proj_y[j], 30.0f, 30.0f))
                {
                    // Both projectiles disappear when they hit each other
                    proj_active[i] = 0;
                    proj_active[j] = 0;
                    break;
                }
            }
        }
 
        for (int i = 0; i < number_of_ghosts; i++)
        {
            if (ghost_captured[i]) continue;
            if (ghost_sucked[i]) continue;  // Skip collision if being sucked
           
            if (!isInvincible && !isRespawnInvincible && checkCollision(player_x, player_y, (float)PlayerWidth, (float)PlayerHeight,
                               ghost_x[i], ghost_y[i], (float)ghostWidth, (float)ghostHeight))
            {
                // Take damage
                playerlives--;
                score -= 50; // Damage penalty
                
                // Start collision animation
                showingCollisionAnimation = true;
                collisionAnimationCounter = 0;
                playerCollisionFrame = 0;
                
                // Activate invincibility frames
                isInvincible = true;
                invincibilityTimer = 0.0f;
                flickerTimer = 0.0f;
                playerVisible = true;
               
                // Respawn player
                player_x = cell_size * 2.0f;
                player_y = (height - 1) * cell_size - PlayerHeight - 0.01f;
                velocityY = 0.0f;
                onGround = true;
                facingRight = true;
               
                // Activate 3 second respawn invincibility
                isRespawnInvincible = true;
                respawnInvincibilityTimer = 0.0f;
               
                if(playerlives <= 0)
                {
                    isGameOver = true;
                    score -= 200; // Full death penalty
                    // Reset captured array when game over
                    capturedStackSize = 0;
                    break; // Exit collision loop immediately
                }

            break;
            }
        }  

        for (int i = 0; i < number_of_skeletons; i++)
        {
            if (skeleton_captured[i]) continue;
            if (skeleton_sucked[i]) continue;  // Skip collision if being sucked
           
            if (!isInvincible && !isRespawnInvincible && checkCollision(player_x, player_y, (float)PlayerWidth, (float)PlayerHeight,
                               skeleton_x[i], skeleton_y[i], (float)skeletonWidth, (float)skeletonHeight))
            {
                // Take damage
                playerlives--;
                score -= 50; // Damage penalty
                
                // Start collision animation
                showingCollisionAnimation = true;
                collisionAnimationCounter = 0;
                playerCollisionFrame = 0;
                
                // Activate invincibility frames
                isInvincible = true;
                invincibilityTimer = 0.0f;
                flickerTimer = 0.0f;
                playerVisible = true;
                
                // Respawn player
                player_x = cell_size * 2.0f;
                player_y = (height - 1) * cell_size - PlayerHeight - 0.01f;
                velocityY = 0.0f;
                onGround = true;
                facingRight = true;
               
                // Activate 3 second respawn invincibility
                isRespawnInvincible = true;
                respawnInvincibilityTimer = 0.0f;
               
                if(playerlives <= 0)
                {
                    isGameOver = true;
                    score -= 200; // Full death penalty
                    // Reset captured array when game over
                    capturedStackSize = 0;
                    break; // Exit collision loop immediately
                }
            }
        }
       
        // Skip rest of game logic if game over to prevent segfault
        if (isGameOver)
        {
            continue;
        }
       
       
        // Check collision with Invisible Man 
        if (currentLevel >= 2)
        {
            for (int i = 0; i < number_of_invisible; i++)
            {
                if (invis_captured[i]) continue;
                if (invis_sucked[i]) continue;
                if (invis_state[i] == 2) continue; // Skip collision if invisible
                
                if (!isInvincible && !isRespawnInvincible && checkCollision(player_x, player_y, (float)PlayerWidth, (float)PlayerHeight,
                                   invis_x[i], invis_y[i], (float)invisWidth, (float)invisHeight))
                {
                    // Take damage
                    playerlives--;
                    score -= 50; // Damage penalty
                    
                    // Activate invincibility frames
                    isInvincible = true;
                    invincibilityTimer = 0.0f;
                    flickerTimer = 0.0f;
                    playerVisible = true;
                    
                    // Respawn player
                    player_x = cell_size * 2.0f;
                    player_y = (height - 1) * cell_size - PlayerHeight - 0.01f;
                    velocityY = 0.0f;
                    onGround = true;
                    facingRight = true;
                    
                    // Activate 3 second respawn invincibility
                    isRespawnInvincible = true;
                    respawnInvincibilityTimer = 0.0f;
                   
                    if(playerlives <= 0)
                    {
                        isGameOver = true;
                        score -= 200; // Full death penalty
                        // Reset captured array when game over
                        capturedStackSize = 0;
                        break; // Exit collision loop immediately
                    } 
                 }
              }
           }
        
        // Check collision with Chelnov 
            if (currentLevel >= 2)
             {
              for (int i = 0; i < number_of_chelnov; i++)
             {
            // Only check collision if active and not captured
            if (!enemy_active[MAX_GHOSTS + MAX_SKELETONS + i] || chelnov_captured[i]) continue;
            if (chelnov_sucked[i]) continue;
            
                if (!isInvincible && !isRespawnInvincible && checkCollision(player_x, player_y, (float)PlayerWidth, (float)PlayerHeight,
                               chelnov_x[i], chelnov_y[i], (float)chelnovWidth, (float)chelnovHeight))
            {
                    // Take damage
                playerlives--;
                    score -= 50; // Damage penalty
                    
                    // Activate invincibility frames
                    isInvincible = true;
                    invincibilityTimer = 0.0f;
                    flickerTimer = 0.0f;
                    playerVisible = true;
                    
                    // Respawn player
                player_x = cell_size * 2.0f;
                player_y = (height - 1) * cell_size - PlayerHeight - 0.01f;
                velocityY = 0.0f;
                onGround = true;
                facingRight = true;
                    
                    // Activate 3-second respawn invincibility
                    isRespawnInvincible = true;
                    respawnInvincibilityTimer = 0.0f;
                
                if (playerlives <= 0)
                {
                    isGameOver = true;
                        score -= 200; // Full death penalty
                        // Reset captured array when game over
                        capturedStackSize = 0;
                        break; // Exit collision loop immediately
                }
                break;
            }
        }
        }
        
        // Skip rest of game logic if game over to prevent segfault
        if (isGameOver)
        {
            continue;
        }
        
        // Check collision with Chelnov projectiles 
        // This will be added when implementing Chelnov shooting
        
        // Check projectile collisions with enemies
        for (int i = 0; i < MAX_PROJECTILES; i++)
        {
            if (!proj_active[i]) continue;
            
            // Check collision with ghosts
            for (int j = 0; j < number_of_ghosts; j++)
            {
                if (ghost_captured[j]) continue;
                if (checkCollision(proj_x[i], proj_y[i], 30.0f, 30.0f,
                                   ghost_x[j], ghost_y[j], (float)ghostWidth, (float)ghostHeight))
                {
                    ghost_captured[j] = 1;
                    proj_active[i] = 0;
                    score += 50 * 2; // Projectile kill bonus x2
                    break;
                }
            }
            
            // Check collision with skeletons
            for (int j = 0; j < number_of_skeletons; j++)
            {
                if (skeleton_captured[j]) continue;
                if (checkCollision(proj_x[i], proj_y[i], 30.0f, 30.0f,
                                   skeleton_x[j], skeleton_y[j], (float)skeletonWidth, (float)skeletonHeight))
                {
                    skeleton_captured[j] = 1;
                    proj_active[i] = 0;
                    score += 75 * 2; // Projectile kill bonus x2
                    break;
                }
            }
            
            // Check collision with Chelnov
            if (currentLevel >= 2)
            {
                for (int j = 0; j < number_of_chelnov; j++)
                {
                    if (chelnov_captured[j]) continue;
                    if (checkCollision(proj_x[i], proj_y[i], 30.0f, 30.0f,
                                       chelnov_x[j], chelnov_y[j], (float)chelnovWidth, (float)chelnovHeight))
                    {
                        chelnov_captured[j] = 1;
                        proj_active[i] = 0;
                        score += 200 * 2; // Projectile kill bonus x2
                        break;
                    }
                }
            }
            
            // Check collision with Invisible Man 
            if (currentLevel >= 2)
            {
                for (int j = 0; j < number_of_invisible; j++)
                {
                    if (invis_captured[j]) continue;
                    if (invis_state[j] == 2) continue; // Skip if invisible
                    if (checkCollision(proj_x[i], proj_y[i], 30.0f, 30.0f,
                                       invis_x[j], invis_y[j], (float)invisWidth, (float)invisHeight))
                    {
                        invis_captured[j] = 1;
                        proj_active[i] = 0;
                        score += 150 * 2; // Projectile kill bonus x2
                        break;
                    }
                }
            }
        }
        
        // Check fireball collision with player 
        if (currentLevel >= 2)
        {
            for (int i = 0; i < MAX_PROJECTILES; i++)
            {
                if (!proj_active[i]) continue;
                if (proj_type[i] != 4) continue; // Only fireballs
                
                if (!isInvincible && !isRespawnInvincible && 
                    checkCollision(player_x, player_y, (float)PlayerWidth, (float)PlayerHeight,
                                   proj_x[i], proj_y[i], 30.0f, 30.0f))
                {
                    // Take damage 
                    playerlives--;
                    score -= 50; // Damage penalty
                    
                    // Activate invincibility frames
                    isInvincible = true;
                    invincibilityTimer = 0.0f;
                    flickerTimer = 0.0f;
                    playerVisible = true;
                    
                    // Remove fireball on hit
                    proj_active[i] = 0;
                    
                    // Respawn player if dead
                    if (playerlives <= 0)
                    {
                        isGameOver = true;
                        break;
                    }
                    else
                    {
                        // Respawn player
                        player_x = cell_size * 2.0f;
                        player_y = (height - 1) * cell_size - PlayerHeight - 0.01f;
                        velocityY = 0.0f;
                        onGround = true;
                        
                        // Activate respawn invincibility
                        isRespawnInvincible = true;
                        respawnInvincibilityTimer = 0.0f;
                    }
                    break;
                }
            }
        }
        
        // Check level completion 
        if (currentLevel == 1)
        {
            int defeatedCount = 0;
            for (int i = 0; i < number_of_ghosts; i++)
                if (ghost_captured[i]) defeatedCount++;
            for (int i = 0; i < number_of_skeletons; i++)
                if (skeleton_captured[i]) defeatedCount++;
            
            if (defeatedCount >= 12)
            {
                currentLevel = 2;
                captureLimit = CAPTURE_LIMIT_LEVEL2;
                // Add level completion bonus
                score += 1000; // Level 1 clear bonus
                // Reset player position to top-left in Level 2
                player_x = cell_size * 0.5f; // Left side
                player_y = 1 * cell_size; // Top row
                velocityY = 0.0f;
                onGround = true;
                
                for (int i = 0; i < height - 1; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        lvl[i][j] = '.';
                    }
                }
                
                lvl[4][2] = '#';  
                lvl[4][3] = '#';  
                lvl[4][4] = '#';  
                lvl[4][5] = '#';
                lvl[4][6] = '#';  
                lvl[4][7] = '#';  
                lvl[4][8] = '#';  
                lvl[4][9] = '#';
                lvl[4][10] = '#';
                lvl[4][11] = '#';
                lvl[4][12] = '#';
                lvl[4][13] = '#';
                lvl[4][14] = '#';
                lvl[4][15] = '#';
                
                lvl[10][0] = '#';
                lvl[10][1] = '#';
                lvl[10][2] = '#';
                lvl[10][3] = '#';
                lvl[10][4] = '#';
                lvl[10][5] = '#';
                lvl[10][12] = '#';
                lvl[10][13] = '#';
                lvl[10][14] = '#';
                lvl[10][15] = '#';
                lvl[10][16] = '#';
                lvl[10][17] = '#';
                
                float midX = width * cell_size / 2.0f;
                float midY = 7 * cell_size;
                float slantLength = 5 * cell_size;
                p_x1[0] = midX - slantLength / 2.0f;
                p_y1[0] = midY;
                p_x2[0] = midX + slantLength / 2.0f;
                p_y2[0] = midY + slantLength;
                p_isDescendingRight[0] = 1;
                p_active[0] = 1;
                
                int startCol = (int)(p_x1[0] / cell_size);
                int endCol = (int)(p_x2[0] / cell_size);
                int startRow = 7;
                int endRow = startRow + 5;
                
                int clearSteps = 5 * 8;
                for (int step = 0; step <= clearSteps; step++)
                {
                    float t = (float)step / (float)clearSteps;
                    float clearX = p_x1[0] + t * (p_x2[0] - p_x1[0]);
                    float clearY = p_y1[0] + t * (p_y2[0] - p_y1[0]);
                    
                    int clearCol = (int)(clearX / cell_size);
                    int clearRow = (int)(clearY / cell_size);
                    
                    for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
                    {
                        for (int colOffset = -1; colOffset <= 1; colOffset++)
                        {
                            int clearCellRow = clearRow + rowOffset;
                            int clearCellCol = clearCol + colOffset;
                            
                            if (clearCellRow >= 0 && clearCellRow < height && 
                                clearCellCol >= 0 && clearCellCol < width)
                            {
                                float distToStart = sqrt((clearX - p_x1[0]) * (clearX - p_x1[0]) + (clearY - p_y1[0]) * (clearY - p_y1[0]));
                                float distToEnd = sqrt((clearX - p_x2[0]) * (clearX - p_x2[0]) + (clearY - p_y2[0]) * (clearY - p_y2[0]));
                                float totalLength = sqrt((p_x2[0] - p_x1[0]) * (p_x2[0] - p_x1[0]) + (p_y2[0] - p_y1[0]) * (p_y2[0] - p_y1[0]));
                                
                                if (distToStart > totalLength * 0.1f && distToEnd > totalLength * 0.1f)
                                {
                                    lvl[clearCellRow][clearCellCol] = '.';
                                }
                            }
                        }
                    }
                }
                
                int topCol = (int)(p_x1[0] / cell_size);
                int topRow = startRow;
                if (topCol >= 0 && topCol < width && topRow >= 0 && topRow < height)
                {
                    for (int i = -1; i <= 2; i++)
                    {
                        int col = topCol + i;
                        if (col >= 0 && col < width && topRow >= 0 && topRow < height)
                        {
                            lvl[topRow][col] = '#';
                        }
                    }
                }
                
                int bottomCol = (int)(p_x2[0] / cell_size);
                int bottomRow = endRow;
                if (bottomCol >= 0 && bottomCol < width && bottomRow >= 0 && bottomRow < height)
                {
                    for (int i = -1; i <= 2; i++)
                    {
                        int col = bottomCol + i;
                        if (col >= 0 && col < width && bottomRow >= 0 && bottomRow < height)
                        {
                            lvl[bottomRow][col] = '#';
                        }
                    }
                }
                
                bgTex.loadFromFile("Data/bg2.png");
                bgSprite.setTexture(bgTex);
                
                number_of_ghosts = 4;
                number_of_skeletons = 9;
                number_of_chelnov = 4;
                number_of_invisible = 3;
                
                playerlives = 3;
                
                activeEnemies = 0;
                for (int i = 0; i < MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + MAX_INVISIBLE; i++)
                    enemy_active[i] = false;
                
                for (int i = 0; i < number_of_ghosts; i++) {
                    ghost_captured[i] = 1;
                    ghost_x[i] = -1000.0f;
                    ghost_y[i] = -1000.0f;
                    ghost_sucked[i] = 0;
                }
                
                for (int i = 0; i < number_of_skeletons; i++) {
                    skeleton_captured[i] = 1;
                    skeleton_x[i] = -1000.0f;
                    skeleton_y[i] = -1000.0f;
                    skeleton_sucked[i] = 0;
                    skeletonState[i] = 0;
                }
                
                for (int i = 0; i < number_of_chelnov; i++) {
                    chelnov_captured[i] = 1;
                    chelnov_x[i] = -1000.0f;
                    chelnov_y[i] = -1000.0f;
                    chelnov_sucked[i] = 0;
                    chelnov_shootTimer[i] = 0.0f;
                    chelnov_uncapturable[i] = 0;
                    chelnov_uncapturableTimer[i] = 0.0f;
                    chelnov_shootState[i] = 0;
                    chelnov_currentFrame[i] = 0;
                }
                
                for (int i = 0; i < number_of_invisible; i++) {
                    invis_captured[i] = 1;
                    invis_x[i] = -1000.0f;
                    invis_y[i] = -1000.0f;
                    invis_sucked[i] = 0;
                    invis_state[i] = 0;
                    invis_timer[i] = 0.0f;
                    invis_reappearTimer[i] = 0.0f;
                    invis_currentFrame[i] = 0;
                }
            }
        }
        
        // Check Level 2 completion
        if (currentLevel == 2)
        {
            int defeatedCount = 0;
            for (int i = 0; i < number_of_ghosts; i++)
                if (ghost_captured[i]) defeatedCount++;
            for (int i = 0; i < number_of_skeletons; i++)
                if (skeleton_captured[i]) defeatedCount++;
            for (int i = 0; i < number_of_chelnov; i++)
                if (chelnov_captured[i]) defeatedCount++;
            for (int i = 0; i < number_of_invisible; i++)
                if (invis_captured[i]) defeatedCount++;
            
            if (defeatedCount >= 20) { // 4 ghosts + 9 skeletons + 4 chelnov + 3 invisible = 20
                // Level 2 complete
                score += 2000; // Level 2 clear bonus
               
        }
        }
 
        // Use player specific scale based on selection
        float currentPlayerScale = (selection_choice == 1) ? 1.875f : 2.25f;
        PlayerSprite.setScale(facingRight ? -currentPlayerScale : currentPlayerScale, currentPlayerScale);
        float spriteX = facingRight ? player_x + PlayerWidth : player_x;
        PlayerSprite.setPosition(spriteX, player_y);
        
        // Draw player only if visible 
        if (playerVisible || (!isInvincible && !isRespawnInvincible))
        {
        window.draw(PlayerSprite);
        }
        
        // Draw vacuum sprite at vacuum position 
        if (vaccum || vacuumTurningOff)
        {
            // Update vacuum animation
            vacuumAnimationTimer += deltaTime;
            if (vacuumAnimationTimer >= VACUUM_ANIMATION_SPEED)
            {
                if (vaccum)
                {
                    // use p2suction.png and cycle through frames when vacuum is active
                    vacuumFrameIndex++;
                    // Get frame count from p2suction texture
                    unsigned int texWidth = vacuumSuctionTexture.getSize().x;
                    int maxFrames = texWidth / VACUUM_FRAME_WIDTH;
                    if (vacuumFrameIndex >= maxFrames)
                    {
                        vacuumFrameIndex = 0; 
                    }
                }
                else if (vacuumTurningOff)
                {
                    // Turning off cycle through last 4 frames (4-7)
                    vacuumFrameIndex++;
                    if (vacuumFrameIndex > 7)                     {
                        // Finished turning off animation
                        vacuumTurningOff = false;
                        vacuumFrameIndex = 0;
                    }
                }
                vacuumAnimationTimer = 0.0f;
            }
            
            // Set texture and frame based on state
            if (vaccum)
            {
                // Use p2suction.png when vacuum is active
                vacuumSprite.setTexture(vacuumSuctionTexture);
                // Get frame count from texture
                unsigned int texWidth = vacuumSuctionTexture.getSize().x;
                int maxFrames = texWidth / VACUUM_FRAME_WIDTH;
                // Ensure frame index is in valid range
                if (vacuumFrameIndex < 0 || vacuumFrameIndex >= maxFrames)
                {
                    vacuumFrameIndex = 0;
                }
                vacuumSprite.setTextureRect(IntRect(vacuumFrameIndex * VACUUM_FRAME_WIDTH, 0, VACUUM_FRAME_WIDTH, VACUUM_FRAME_HEIGHT));
            }
            else if (vacuumTurningOff)
            {
                // When turning off use p2suction.png (vacuum.png removed)
                vacuumSprite.setTexture(vacuumSuctionTexture);
                // When turning off ensure frame is 4-7
                if (vacuumFrameIndex < 4)
                {
                    vacuumFrameIndex = 4;
                }
                if (vacuumFrameIndex > 7)
                {
                    vacuumFrameIndex = 7;
                }
                unsigned int texWidth = vacuumSuctionTexture.getSize().x;
                int maxFrames = texWidth / VACUUM_FRAME_WIDTH;
                if (vacuumFrameIndex >= maxFrames) vacuumFrameIndex = maxFrames - 1;
                vacuumSprite.setTextureRect(IntRect(vacuumFrameIndex * VACUUM_FRAME_WIDTH, 0, VACUUM_FRAME_WIDTH, VACUUM_FRAME_HEIGHT));
            }
            
            // Position vacuum at vacuum position 
            float vacuumSpriteX = vacuum_x - VACUUM_FRAME_WIDTH * VACUUM_SCALE / 2.0f;
            float vacuumSpriteY = vacuum_y - VACUUM_FRAME_HEIGHT * VACUUM_SCALE / 2.0f;
            vacuumSprite.setScale(VACUUM_SCALE, VACUUM_SCALE);
            vacuumSprite.setPosition(vacuumSpriteX, vacuumSpriteY);
            
            window.draw(vacuumSprite);
        }

        for (int i = 0; i < number_of_ghosts; i++)
        {
            if (!ghost_captured[i])
            {
                // Set texture based on state
                if (ghostTextureType[i] == 1) // Sucking
                {
                    GhostSprite.setTexture(GhostSuckTexture);
                    // Animate suck texture
                    unsigned int texWidth = GhostSuckTexture.getSize().x;
                    int maxFrames = texWidth / GHOST_FRAME_WIDTH;
                    int suckFrame = ghostCurrentFrame % maxFrames;
                    GhostSprite.setTextureRect(IntRect(suckFrame * GHOST_FRAME_WIDTH, 0, GHOST_FRAME_WIDTH, GHOST_FRAME_HEIGHT));
                }
                else // always use walking texture array
                {
                    ghostTextureType[i] = 0; // walking texture type
                    GhostSprite.setTexture(GhostWalkTextures[ghostWalkFrameIndex]);
                }
                
                GhostSprite.setScale(ghost_movingRight[i] ? -GHOST_SCALE : GHOST_SCALE, GHOST_SCALE);
                float ghostDrawX = ghost_movingRight[i] ? ghost_x[i] + ghostWidth : ghost_x[i];
                GhostSprite.setPosition(ghostDrawX, ghost_y[i]);
                window.draw(GhostSprite);
            }
        }
       
        for (int i = 0; i < number_of_skeletons; i++)
        {
            if (!skeleton_captured[i])
            {
                // Set texture based on state
                if (skeletonTextureType[i] == 1) // Walking
                {
                    SkeletonSprite.setTexture(SkeletonWalkTextures[skeletonCurrentFrame[i]]);
                }
                else if (skeletonTextureType[i] == 2) // Jumping or Falling 
                {
                    SkeletonSprite.setTexture(SkeletonWalkTextures[0]);
                }
                else // Throw (fallback)
                {
                    SkeletonSprite.setTexture(SkeletonThrowTexture);
                }
                
                SkeletonSprite.setScale(skeleton_movingRight[i] ? -SKELETON_SCALE : SKELETON_SCALE, SKELETON_SCALE);
                float skeletonDrawX = skeleton_movingRight[i] ? skeleton_x[i] + skeletonWidth : skeleton_x[i];
                SkeletonSprite.setPosition(skeletonDrawX, skeleton_y[i]);
                
                window.draw(SkeletonSprite);
            }
        }
        
        if (currentLevel >= 2)
        {
            for (int i = 0; i < number_of_chelnov; i++)
            {
                // Only draw if active and not captured
                if (enemy_active[MAX_GHOSTS + MAX_SKELETONS + i] && !chelnov_captured[i])
                {
                    // 1. Select Texture
                    if (chelnov_sucked[i] == 1) {
                        chelnovTextureType[i] = 3; 
                        chelnovFrameIndex[i]++;
                        if (chelnovFrameIndex[i] >= CHELNOV_SUCTION_FRAMES * 6) chelnovFrameIndex[i] = 0;
                        chelnov_currentFrame[i] = chelnovFrameIndex[i] / 6;
                        if (chelnov_currentFrame[i] >= CHELNOV_SUCTION_FRAMES) chelnov_currentFrame[i] = 0;
                        ChelnovSprite.setTexture(ChelnovSuctionTextures[chelnov_currentFrame[i]]);
                    } 
                    else if (chelnovTextureType[i] == 4) { 
                        if (chelnov_currentFrame[i] >= CHELNOV_FIRE_FRAMES) chelnov_currentFrame[i] = 0; 
                        ChelnovSprite.setTexture(ChelnovFireTextures[chelnov_currentFrame[i]]); 
                    }
                    else { 
                        if (chelnov_currentFrame[i] >= CHELNOV_WALK_FRAMES) chelnov_currentFrame[i] = 0; 
                        ChelnovSprite.setTexture(ChelnovWalkTextures[chelnov_currentFrame[i]]); 
                    }
                    
                    float texW = (float)ChelnovSprite.getTexture()->getSize().x;
                    float texH = (float)ChelnovSprite.getTexture()->getSize().y;
                    
                    // Set Origin to Bottom Center 
                    ChelnovSprite.setOrigin(texW / 2.0f, texH);

                    // Calculate Position 
                    float drawX = chelnov_x[i] + chelnovWidth / 2.0f;
                    float drawY = chelnov_y[i] + chelnovHeight;

                    ChelnovSprite.setPosition(drawX, drawY);
                    
                    // 3. Scale ,Flip if moving right, based on sprite orientation
                    if (chelnov_movingRight[i]) 
                        ChelnovSprite.setScale(-CHELNOV_SCALE, CHELNOV_SCALE); 
                    else 
                        ChelnovSprite.setScale(CHELNOV_SCALE, CHELNOV_SCALE);

                    // 4. Color
                    if (chelnov_uncapturable[i] == 1) 
                        ChelnovSprite.setColor(Color(255, 100, 100)); 
                    else 
                        ChelnovSprite.setColor(Color::White);

                    window.draw(ChelnovSprite);
                    
                    // Reset Origin to avoid side effects
                    ChelnovSprite.setOrigin(0, 0); 
                }
            }
        }

  


        // Draw Invisible Man 
        if (currentLevel >= 2)
        {
            for (int i = 0; i < number_of_invisible; i++)
            {
                // Only draw if active, not captured, and not in invisible state
                if (enemy_active[MAX_GHOSTS + MAX_SKELETONS + MAX_CHELNOV + i] && !invis_captured[i] && invis_state[i] != 2) // Don't draw if invisible
                {
                    // Set texture based on state
                    if (invis_sucked[i] == 1) // Suction
                    {
                        invisTextureType[i] = 2; // Use suction texture array
                        invisFrameIndex[i]++;
                        if (invisFrameIndex[i] >= INVIS_SUCTION_FRAMES * 8) // Slower animation
                        {
                            invisFrameIndex[i] = 0;
                        }
                        invis_currentFrame[i] = invisFrameIndex[i] / 8;
                        if (invis_currentFrame[i] >= INVIS_SUCTION_FRAMES)
                            invis_currentFrame[i] = 0;
                        InvisibleManSprite.setTexture(InvisibleSuctionTextures[invis_currentFrame[i]]);
                    }
                    else if (invisTextureType[i] == 1) // Walking
                    {
                        invisFrameIndex[i]++;
                        if (invisFrameIndex[i] >= INVIS_WALK_FRAMES * 8) // Slower animation
                        {
                            invisFrameIndex[i] = 0;
                        }
                        invis_currentFrame[i] = invisFrameIndex[i] / 8;
                        if (invis_currentFrame[i] >= INVIS_WALK_FRAMES)
                            invis_currentFrame[i] = 0;
                        InvisibleManSprite.setTexture(InvisibleWalkTextures[invis_currentFrame[i]]);
                    }
                    else // Default
                    {
                        InvisibleManSprite.setTexture(InvisibleWalkTextures[0]);
                    }
                    
                    InvisibleManSprite.setScale(invis_movingRight[i] ? -INVIS_SCALE : INVIS_SCALE, INVIS_SCALE);
                    float invisDrawX = invis_movingRight[i] ? invis_x[i] + invisWidth : invis_x[i];
                    InvisibleManSprite.setPosition(invisDrawX, invis_y[i]);
                    window.draw(InvisibleManSprite);
                }
            }
        }
        
        // Draw vacuum position indicator with direction arrow
        // Vacuum position 
        Sprite vacuumIndicator;
        vacuumIndicator.setTexture(blockTexture);
        vacuumIndicator.setPosition(vacuum_x - 10.0f, vacuum_y - 10.0f);
        vacuumIndicator.setScale(0.3f, 0.3f);
        if (vaccum)
        {
            vacuumIndicator.setColor(Color(255, 255, 0)); // Yellow when active
        }
        else
        {
            vacuumIndicator.setColor(Color(200, 200, 200)); // Gray when inactive
        }
        window.draw(vacuumIndicator);
        
        // Draw direction indicator
        if (vaccum)
        {
            // Draw a line in the direction the vacuum is pointing
            float arrowLength = 30.0f;
            float arrowEndX = vacuum_x + vacuumDirX * arrowLength;
            float arrowEndY = vacuum_y + vacuumDirY * arrowLength;
            
            // Use a small sprite as direction indicator
            Sprite dirIndicator;
            dirIndicator.setTexture(blockTexture);
            dirIndicator.setPosition(arrowEndX - 5.0f, arrowEndY - 5.0f);
            dirIndicator.setScale(0.2f, 0.2f);
            dirIndicator.setColor(Color(255, 0, 0)); // Red for direction
            window.draw(dirIndicator);
        }
        
        // Draw projectiles
        for (int i = 0; i < MAX_PROJECTILES; i++)
        {
            if (proj_active[i])
            {
                projSprites[i].setPosition(proj_x[i], proj_y[i]);
                window.draw(projSprites[i]);
            }
        }
        
        // Update and draw score text
        buildScoreString(score, scoreStr, 50);
        scoreText.setString(scoreStr);
        window.draw(scoreText);
        
        if (comboCount > 0)
        {
            char comboStr[50];
            buildComboString(comboCount, comboStr, 50);
            comboText.setString(comboStr);
            window.draw(comboText);
        }
        
        char capturedStr[50];
        buildCapturedString(capturedStackSize, captureLimit, capturedStr, 50);
        capturedText.setString(capturedStr);
        window.draw(capturedText);
        
        for (int i = 0; i < playerlives; i++)
{
    window.draw(hearts[i]);
}

        window.display();
    }  // End of main while (window.isOpen()) loop

    lvlMusic.stop();

    return 0;
} 


