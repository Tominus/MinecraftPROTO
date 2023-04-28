#pragma once

/*Every things that have to be done.*/

/* Storage of a Big Data for Blocks (initialized before start playing)
* 
* Blocks_Global_Datas
* 
* using enum Block_Type
* 
* with this enum we can access a struct of many *:
* - Shapes (vertexs and uvs)
* - Render (texture)
* - Information (like every different block present in the chunk)
* - ?Collision
* - ?CodeBehaviour (?that can be added dynamically) (like burning, liquid, damage entity, spawner...)
* ...
*/

/*
* - ? faire une pool de chunk créé sur le main thread et récupéré par les threads
* - same than opti_worldChunks but with State enum : None, IsGenerating, Genered
* - mutex priority
* - stock in chunk there array index and decrement them when array is erased (probably more opti)
* - save data
* - test destroy block
* - rendu alpha
* - cull
* - physics
* - player
* - generation
* 
* - toujours load les chunks sous le player
*/

/* Update Render every frame.
* Don't Render if it's a server.
* Every other thing (exept Connection Behaviour) have to be in 20 ticks rate.
*/

/* Stock en .txt le rendu vertexs && uvs d'un chunk.
* pour éviter de le recalculer au rechargement.
* fonctionne qu'en solo.
*/