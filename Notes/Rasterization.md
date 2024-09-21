Here's the reformatted markdown using conventional markdown formatting:
# Rasterization Pipeline
- Tries to generate a realistic 2D image on the camera plane from the 3D scene using the light source and camera
## 1. Application Processing
- Handles input
- Handles changes to scene
- Controls AI
- etc.
## 2. Vertex Processing
- Process vertexes based on a variety of [[Transformations]]
  1. Model (from model to world)
  2. View (from world to camera)
  3. Projection (from camera to image)
## 3. Triangle Processing
- Connect points to form triangles
## 4. Rasterization
- Assigning fragments to triangles
## 5. Fragment Processing
- Shade fragments
## 6. Frame-buffer Operations
- Convert fragments to pixels
## 7. Display frame-buffer