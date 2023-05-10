#pragma once

#define ENABLE_DEBUG_NEW_OVERRIDE 0

#define ENABLE_DEBUG_MEMORY_LEAK 1
#define ENABLE_DEBUG_BREAK_ALLOC 0
#define BREAK_ALLOC 2862

#define Chunk_Size (16)
#define Chunk_Max_Size (15)
#define Chunk_Min_World_Height (-1)
#define Chunk_Zero_World_Height (0)
#define Chunk_Max_World_Height (16)

constexpr size_t Chunk_Min_Limit_World_Height = (Chunk_Min_World_Height + 1);
constexpr size_t Chunk_Max_Limit_World_Height = (Chunk_Max_World_Height - 1);

/*If this function can be use in tread*/
#define Main_Thread
#define Threaded
#define Thread_Func unsigned long(__stdcall*)(void*)
#define Thread_Used_Qty (16)
#define Thread_Max_ChunkCreation (16)

#define Max_FPS (60)
#define Tick_Time (0.05f)
//#define Fps_Time (0.017f) //60fps
#define Fps_Time (0.00694f) //144fps

#define Render_Distance_Min (2)
#define Render_Distance_Current (16)
#define Render_Distance_Max (16)

constexpr size_t Render_Distance_Total = ((Render_Distance_Current * 2) - 1);
constexpr size_t Render_Distance_Total_Limit = (Render_Distance_Total - 1);

#define Block_Total_Shapes (36)
#define Block_Total_Possible_Shapes (64)

#define Perlin_Noise_Octave (6)
#define Perlin_Noise_Frequency (0.0390625) // 10.0 / Perlin_Noise_Resolution
#define Perlin_Noise_Resolution (256.0)
#define Perlin_Noise_DefaultZ (0.34567)