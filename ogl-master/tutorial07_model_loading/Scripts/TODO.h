#pragma once

/*Every things will have to be done.*/

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
* - donner le side à rework
* - les rendus à changer sont fait par le main thread
* 
* - je dis aux chunks qu'ils doivent s'update 
*/

/* 
* - fix rendu du chunk sur coté
* - test destroy block
* - rendu alpha
* - cull
* - physics
* - player
* - generation
* 
* - toujours load les chunks sous le player
*/

/* Store Chunk in there position instead of an array
* Impossible cause negative value exist
* soluce :
* int negativeOffsetX
* //int negativeOffsetY doesn't exist cause world height is predetermined
* int negativeOffsetZ
* Triple vect
* 
* if we have a new chunk with negative value
* we increment the correspondant offset
* and we redo the Triple vect.
* Or use [][][] instead
*/

/* Update Render every frame.
* Don't Render if it's a server.
* Every other thing (exept Connection Behaviour) have to be in 20 ticks rate.
*/

/* Stock en .txt le rendu vertexs && uvs d'un chunk.
* pour éviter de le recalculer au rechargement.
* fonctionne qu'en solo.
*/