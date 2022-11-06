# Parametric Surface generation and shading

To do lighting and shading computations, we need to use the normals at every point in the shaders and manipulate them using the light and camera position to display a certain color at a pixel.

The parametric surface used is HyperHelicoidal.<br>
HyperHelicoidal parametric equation is:<br>
` x = (sinh(v)*cos(3*u))/(1+cosh(u)*cosh(v)) `<br>
` y = (sinh(v)*sin(3*u))/(1+cosh(u)*cosh(v)) `<br>
` z = (cosh(v)*sinh(u))/(1+cosh(u)*cosh(v)) `<br>


For info on HyperHelicoidal, reference:  [scientificlib.com - Hyperhelicoidal](http://www.scientificlib.com/en/Mathematics/Surfaces/Hyperhelicoidal.html)

<br>

### Some important functions used in the code:

` createParametricObject() `<br>
Discretizes and triangulates an equation (Hyper helicoidal in this case)
- Adds vertex to vertex array object
- Adds normals to vertex array object

` setParams() `<br>
Sets parameters for the parametric object

` loadShader() `<br>
Binds uniforms to the shaders

` updateXYZ() `<br>
Called to update x, y, z coordinates of the vertex by passing u and v values to it

` dFdU() `<br>
calculates the df/du vector at a vertex

` dFdV() `<br>
calculates the df/dv vector at a vertex

<br>

### Four types of Shaders have been created:
* Normal Mapping
  - Vertex
  - Fragment
* Gouraud Shader (Diffused)
  - Vertex
  - Fragment
* Phong Shader (in Vertex)
  - Vertex
  - Fragment
* Phong Shader (in Fragment)
  - Vertex
  - Fragment

All four shader files are pre compiled and can be switched using on screen ImGui buttons.<br>
Three Matrices Model, View and Projection are passed to the shaders as uniforms.<br>
Along with those,<br>
- Camera Position
- Light Position
- Light Color
- Object Color
are also passed to the shaders as uniforms.
<br><br>
## Objectives Completed in the Code:
1. In Gouraud, we will compute the lighting in the vertex shader.<br>
![Gouraud](./Images/gouraud.png)<br>
2. In this variant of Phong we are calculating the lighting in the vertex shader only.<br>
![Phong - vertex](./Images/phong-v.png)<br>
3. In Phong, we will compute the lighting in the fragment shader.<br>
![Phong - fragment](./Images/phong-f.png)<br>
4. We will compute the normals in the fragment shader and pass their modification as color.<br>
![Normal Map](./Images/normal-map.png)<br>
