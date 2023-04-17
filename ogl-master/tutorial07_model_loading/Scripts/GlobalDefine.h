#pragma once

#define Chunk_Size (16)
#define Chunk_Max_Size (15)
#define Chunk_Min_World_Height (-1)
#define Chunk_Zero_World_Height (0)
#define Chunk_Max_World_Height (16)

/*If this function can be use in tread*/
#define Main_Thread
#define Threaded
#define Thread_Func unsigned long(__stdcall*)(void*)
#define Thread_Used_Qty (16)

#define Max_FPS (60)
#define Tick_Time (0.05f)
//#define Fps_Time (0.017f) //60fps
#define Fps_Time (0.00694f) //144fps

#define Render_Distance_Min (2)
#define Render_Distance_Current (5)
#define Render_Distance_Max (16)

#define Block_Total_Shapes (36)
#define Block_Total_Possible_Shapes (64)

#define Perlin_Noise_Octave (6)
#define Perlin_Noise_Frequency (0.0390625) // 10.0 / Perlin_Noise_Resolution
#define Perlin_Noise_Resolution (256.0)
#define Perlin_Noise_DefaultZ (0.34567)