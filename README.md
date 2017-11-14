# ARMarkerEditor
Assistant program for an [Android Augmented Reality app](https://github.com/Machinezero/ARMarker) which uses Vuforia for multiple marker recognition to view large 3D virtual scenes. This assistant app is used to build AR projects that can be read by the Android App.

![](pc_ui1.png)


## Tools Used
* OpenGL 
* [Assimp](http://www.assimp.org/)
* [Qt](https://www.qt.io/) 

## Requirements
* Qt 5.8.0

## Installation
Open the Project with Qt. The compiler used in development was MinGW 32 bit.

## Usage

The application contains multiple features:
* Saving and opening projects, loading 3D Models;
* An intuitive and easy way to arrange markers similar to the arrangement of 3D objects in other 3D editors or building video games.  Also editing properties of rotation and translation for these entities, ID assignment and texture image display.
*  The creation of cameras in the 3D scene correspond to possible user positions, and a feature is available in which a simulation is ran, painting the 3D landscape with colors representing the quality of the current marker arrangement. This feature also allows the user to define the camera's parameters, like vertical and horizontal angle of view, and also height. 

### Key Binds
* __W:__ Walk Forward;
* __S:__ Walk Backwards;
* __Shift:__ Walking units per frame is increased;
* __LMB:__ Hold to rotate camera; Double click to select entity; Also used in the creation of entities;
* __RMB:__ Used in the creation of entities;
* __C:__ Begin Camera Creation Context;
* __M:__ Begin Marker Creation Context;
* __F:__ When a camera is selected, assumes that camera's position with the determined Android camera's parameters (Angle of Vision and Height);
* __E:__ Top-Down View;
* __P:__ Switch between Real 3D Model and Virtual 3D Model;
* __Backspace:__ Delete entity;

### Camera Creation

After pressing the Camera creation key, holding and dragging the Right Mouse Button selects the walkable region. Releasing the RMB will create the entity.

### Marker Creation
When pressing the Marker creation key, holding and dragging the Right Mouse Button sticks the marker to the surface the pointer is on. Pressing LMB sets the marker at the apointed location. The middle mouse button can also be used to increase or decrease the distance between the surface and the Marker.

### Simulation
The simulation feature will paint the 3D model according to a qualitative analysis of the surface in relation to the user created camera.  There are four colors which represent the following:
* __Green, Orange and Red:__ Represents the quality of the surface normals in relation to the placed camera's position, from good to bad. This means that a surface that is being viewed from the side by the proposed camera, is considered a bad surface (red color) and a surface that is viewed from the front is considered a good surface (green color). These colors also signify that there are no markers nearby.
* __Blue__ Signifies that the current surface has a marker nearby, meaning that another marker is not required at that location. 

### Saving Projects
When saving, projects, a Vuforia API key and a Vuforia binary database is required.

## Author
Pedro Carvalho

## About
This work was done in the scope of a Thesis for Master Degree in Informatic Engineering (Computer Science), at University of Minho.
