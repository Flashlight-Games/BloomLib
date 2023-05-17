# BloomLib
TomerBARZ's Bloom Game Engine

Main source files are at Include/Bloom/  
SteamTools -> for networking/multiplayer , cloud saves and other steam related things  
BoneKinematics -> Forward Kinematics for a bone structure and 2 Bone Inverse Kinematics  
Bones -> Responsible for keeping track of parent and children and automatically updating locations and rotations to keep a structure  
Collision -> Multiple algorithms for checking if rays or polygons intersect each other  
Engine -> Responsible for running the game, taking inputs, and starting events  
Framebuffers -> Record pixels (usually the screen) to a texture so it can be altered or analyzed  
kinematicsBody -> kinematic simulations for rigid bodies  
particles -> multi body (hundreds) sprite rendering and basic code execution  
shaders -> for creating shaders from glsl files, for creating compute shaders and executing general purpose code on the GPU  
shake -> simple camera shake in X & Y  
sprite -> sprite rendering and spritesheets for animations  
text -> for text rendering from font files  
textures -> reading images and using them for sprites  
utils -> miscellaneous functions for general use  
res/engine -> Resources required to use the engine: fonts, textures, shaders, etc...  

![A](https://github.com/Flashlight-Games/BloomLib/assets/25566561/a5a80ef7-c96f-4e8d-990d-820e81c78577)
https://github.com/Flashlight-Games/BloomLib/assets/25566561/aa01cf40-e33e-41b2-8608-edea1077cd68

