// Assignment 03: Lighting

/*References
  Trackball: http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
*/


/*
    NOTE: only hyperhelicoidal is used for lighting demo

    Lightings implemented:
    1. Normal Mapping Shaders
    2. Gouraud Diffused Shaders
    3. Phong Implemented at vertex shader
    4. Phong at Fragment Shader
*/


#include "utils.h"
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace glm;

#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL


// Globals and Uniforms
int screen_width = 1280, screen_height = 720;
GLint vModel_uniform, vView_uniform, vProjection_uniform, camera_pos_uniform, light_pos_uniform, vColor_uniform, lColor_uniform;
glm::mat4 modelT, viewT, projectionT; // The model, view and projection transformations
double oldX, oldY, currentX, currentY;
bool isDragging = false;

// Global Attribs
unsigned int shaderProgram;
int vertices = 0;
bool surface = false;
float zoom = 1.0;
int H_SCALE = 30;
int W_SCALE = 20;
int V_COUNT = 150;
int U_COUNT = 150;
GLfloat U_MAX = 3.14;
GLfloat U_MIN = -3.14;
GLfloat V_MAX = 3.14;
GLfloat V_MIN = -3.14;
char shaderType[30] = "Normal";

void createParametricObject(unsigned int &, unsigned int &);
void setupModelTransformation(unsigned int &);
void setupViewTransformation(unsigned int &);
void setupProjectionTransformation(unsigned int &);
void setParams();
void loadShader();
glm::vec3 getTrackBallVector(double x, double y);
void updateXYZ(GLfloat &x, GLfloat &y, GLfloat &z, float u, float v);
glm::vec3 dFdV(float u, float v);
glm::vec3 dFdU(float u, float v);


int main(int, char **)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO &io = ImGui::GetIO(); // Create IO object

    // set bgr white
    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);
    
    // assign shader programs to variables
    unsigned int phongShader = createProgram("./shaders/vshaderPhong.vs", "./shaders/fshaderPhong.fs");
    unsigned int gouraudShader = createProgram("./shaders/vshader.vs", "./shaders/fshader.fs");
    unsigned int phong2Shader = createProgram("./shaders/vshaderPhong2.vs", "./shaders/fshaderPhong2.fs");
    unsigned int normalShader = createProgram("./shaders/vshaderNormal.vs", "./shaders/fshaderNormal.fs");

    shaderProgram = normalShader;

    loadShader();

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
        
    createParametricObject(shaderProgram, VAO);         // plot method called
    setParams(); 

    oldX = oldY = currentX = currentY = 0.0;
    int prevLeftButtonState = GLFW_RELEASE;
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Get current mouse position
        int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        if (leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_RELEASE)
        {
            isDragging = true;
            currentX = oldX = x;
            currentY = oldY = y;
        }
        else if (leftButtonState == GLFW_PRESS && prevLeftButtonState == GLFW_PRESS)
        {
            currentX = x;
            currentY = y;
        }
        else if (leftButtonState == GLFW_RELEASE && prevLeftButtonState == GLFW_PRESS)
        {
            isDragging = false;
        }

        // Rotate based on mouse drag movement
        prevLeftButtonState = leftButtonState;
        if (isDragging && (currentX != oldX || currentY != oldY))
        {
            glm::vec3 va = getTrackBallVector(oldX, oldY);
            glm::vec3 vb = getTrackBallVector(currentX, currentY);

            float angle = acos(std::min(1.0f, glm::dot(va, vb)));
            glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
            glm::mat3 camera2object = glm::inverse(glm::mat3(viewT * modelT));
            glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
            modelT = glm::rotate(modelT, angle, axis_in_object_coord);
            glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));

            oldX = currentX;
            oldY = currentY;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glUseProgram(shaderProgram);
        ImGui::SetNextWindowSize(ImVec2(200, 200));
        {
            ImGui::Begin("Information");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            if (ImGui::Button("Normal Mapping")) {
                shaderProgram = normalShader;
                sprintf(shaderType, "Normal");
                loadShader();
            }
            if (ImGui::Button("Gouraud Shaders")) {
                shaderProgram = gouraudShader;
                sprintf(shaderType, "Gourad");
                loadShader();
            }
            if (ImGui::Button("Phong Vertex Shaders")) {
                shaderProgram = phong2Shader;
                sprintf(shaderType, "Phong Vertex");
                loadShader();
            }
            if (ImGui::Button("Phong Fragment Shaders")) {
                shaderProgram = phongShader;
                sprintf(shaderType, "Phong Fragment");
                loadShader();
            }
            if (ImGui::SliderFloat("Zoom", &zoom, 3.0, 0.1))
                setupViewTransformation(shaderProgram);
            ImGui::Text("%s", shaderType);
            ImGui::End();
        }
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);

        glUniform3f(light_pos_uniform, 100.0, -150.0, 200.0);
        glUniform3f(lColor_uniform, 1.0, 1.0, 1.0);
        glUniform3f(camera_pos_uniform, 40.0, -40.0, 40.0);

        glUniform3f(vColor_uniform, 1, 0.5, 0.5);
        glDrawArrays(GL_TRIANGLES, 0, vertices);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    cleanup(window);

    return 0;
}

void setParams() {
    if (surface) {
        // Hyperhelicoidal
        // parameters
        U_MAX = 3.14;
        U_MIN = -3.14;
        V_MAX = 3.14;
        V_MIN = -3.14;
    } else {
        // dini's spiral
        //parameters
        U_MAX = 12.4;
        U_MIN = 0;
        V_MAX = 2;
        V_MIN = 0.1;
    }
}

void loadShader(){
    // bind uniforms
    vColor_uniform = glGetUniformLocation(shaderProgram, "vColor");
    lColor_uniform = glGetUniformLocation(shaderProgram, "lColor");
    light_pos_uniform = glGetUniformLocation(shaderProgram, "lpos_world");
    camera_pos_uniform = glGetUniformLocation(shaderProgram, "camera");

    // bind shader
    glUseProgram(shaderProgram);

    // bind MVP matrices
    setupModelTransformation(shaderProgram);
    setupViewTransformation(shaderProgram);
    setupProjectionTransformation(shaderProgram);
}

void updateXYZ(GLfloat &x, GLfloat &y, GLfloat &z, float u, float v) {
    // parametric equation of the curve
    if (surface) {
        // dini's spiral
        // equation
        x = W_SCALE * glm::cos(u) * glm::sin(v);
        y = W_SCALE * glm::sin(u) * glm::sin(v);
        z = H_SCALE * (glm::cos(v) + std::log(glm::tan(v/2))) + 2*u;
    } else {
        // Hyperhelicoidal
        // equation
        x = W_SCALE * ((glm::sinh(v)*glm::cos(3*u)) / (1 + glm::cosh(u)*glm::cosh(v)));
        y = W_SCALE * ((glm::sinh(v)*glm::sin(3*u)) / (1 + glm::cosh(u)*glm::cosh(v)));
        z = H_SCALE * ((glm::cosh(v)*glm::sinh(u)) / (1 + glm::cosh(u)*glm::cosh(v)));
    }
}

glm::vec3 dFdV(float u, float v) {
    float a = cos(3*u);
    float b = cosh(u);

    float dx = (a*(cosh(v)+b))/pow((b*cosh(v)+1), 2);
    a = sin(3*u);
    float dy = (a*(cosh(v)+b))/pow((b*cosh(v)+1), 2);
    a = sinh(u);
    float dz = (a*sinh(v))/pow((b*cosh(v)+1), 2);

    return vec3(dx, dy, dz);
}
glm::vec3 dFdU(float u, float v) {
    float a = sinh(v);
    float b = cosh(v);
    float denom = (b*cosh(u) + 1) * (b*cosh(u) + 1);

    float dx = -(a * ( (3*b*cosh(u) + 3)*sin(3*u) + b*sinh(u)*cos(3*u) ) / denom);
    float dy = -(a * (( b*sinh(u)*sin(3*u) + (-3*b*cosh(u) - 3)*cos(3*u)) / denom ));
    float dz = -(b * (( b*sinh(u)*sin(3*u) + (-3*b*cosh(u) - 3)*cos(3*u)) / denom ));

    return vec3(dx, dy, dz);
}

/* Function for older Normal Finding Method */
glm::vec3 getNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {     // p1 p2 p3 in order (RHTR)
    return glm::cross(glm::vec3(p3 - p2), glm::vec3(p1 - p2));
}

void createParametricObject(unsigned int &program, unsigned int &shape_VAO)
{
    glUseProgram(program);

    // Bind shader variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    int norm_attrib = glGetAttribLocation(program, "vertex_norm");
    
    // vertex count = m * n
    const float V_STEP = ((V_MAX - V_MIN)/V_COUNT);
    const float U_STEP = ((U_MAX - U_MIN)/U_COUNT);
    const int VERTEX_COUNT = (V_COUNT * U_COUNT);

    // size of array = 3 * 3 * 2 * (m-1)*(n-1) 
    std::vector<GLfloat> shape_vertices;
    std::vector<vec3> preciseNormals;
    // shape_vertices.reserve(3 * 3 * 2 * (V_COUNT-1) * (U_COUNT-1));

    for (int i=0; i<U_COUNT; i++) {
        for (int j=0; j<V_COUNT; j++) {
            const float u = U_MIN + ((float)i)*U_STEP;
            const float v = V_MIN + ((float)j)* V_STEP;
            GLfloat x, y, z;
            const int directions[6][2] = {
                {0, 0}, {1, 0}, {1, 1},
                {0, 0}, {1, 1}, {0, 1}
            };

            for (int k=0; k<6; k++) {
                updateXYZ(x, y, z, u + U_STEP*directions[k][0], v + V_STEP*directions[k][1]);

                // std::cout << "x: " << x << " y: " << y << " z: " << z << " | " << i << " " << j <<  std::endl;
                vec3 dfdu = dFdU(u + U_STEP*directions[k][0], v + V_STEP*directions[k][1]);
                vec3 dfdv = dFdV(u + U_STEP*directions[k][0], v + V_STEP*directions[k][1]);
                vec3 nrml = normalize(cross(dfdu, dfdv));

                preciseNormals.push_back(nrml);
                shape_vertices.push_back(x);
                shape_vertices.push_back(y);
                shape_vertices.push_back(z);
            }
        }
    }

    int size = shape_vertices.size();
    // glm::vec3 normals[size/3];
    // for (int i = 0; i < size; i+=3*3) {
    //     glm::vec3 p1 = glm::vec3(shape_vertices[i+0], shape_vertices[i+1], shape_vertices[i+2]);
    //     glm::vec3 p2 = glm::vec3(shape_vertices[i+3], shape_vertices[i+4], shape_vertices[i+5]);
    //     glm::vec3 p3 = glm::vec3(shape_vertices[i+6], shape_vertices[i+7], shape_vertices[i+8]);
    //     glm::vec3 normal = getNormal(p1, p2, p3);

    //     normals[i/3+0] = normal;
    //     normals[i/3+1] = normal;
    //     normals[i/3+2] = normal;
    // }

    vertices = shape_vertices.size()/3;
    
    // Generate VAO object
    glGenVertexArrays(1, &shape_VAO);
    glBindVertexArray(shape_VAO);

    // Create VBOs for the VAO
    GLuint vertex_VBO;
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, shape_vertices.size() * sizeof(GLfloat), shape_vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint normal_VBO;
    glGenBuffers(1, &normal_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, shape_vertices.size() * sizeof(GLfloat), preciseNormals.data() /*&normals[0]*/, GL_STATIC_DRAW);
    glEnableVertexAttribArray(norm_attrib);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) *3, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0); // Unbind the VAO to disable changes outside this function.

}

void setupModelTransformation(unsigned int &program)
{
    // Modelling transformations (Model -> World coordinates)
    modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)); // Model coordinates are the world coordinates

    // Pass on the modelling matrix to the vertex shader
    glUseProgram(program);
    vModel_uniform = glGetUniformLocation(program, "vModel");
    if (vModel_uniform == -1)
    {
        fprintf(stderr, "Could not bind location: vModel\n");
        exit(0);
    }
    glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT));
}

void setupViewTransformation(unsigned int &program)
{
    // Viewing transformations (World -> Camera coordinates
    // Camera at (0, 0, 100) looking down the negative Z-axis in a right handed coordinate system
    glm::vec3 camPos = zoom * glm::vec3(40.0, -40.0, 40.0);
    viewT = glm::lookAt(camPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    // Pass-on the viewing matrix to the vertex shader
    glUseProgram(program);
    vView_uniform = glGetUniformLocation(program, "vView");
    if (vView_uniform == -1)
    {
        fprintf(stderr, "Could not bind location: vView\n");
        exit(0);
    }
    glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT));
}

void setupProjectionTransformation(unsigned int &program)
{
    // Projection transformation
    projectionT = glm::perspective(45.0f, (GLfloat)screen_width / (GLfloat)screen_height, 0.1f, 1000.0f);

    // Pass on the projection matrix to the vertex shader
    glUseProgram(program);
    vProjection_uniform = glGetUniformLocation(program, "vProjection");
    if (vProjection_uniform == -1)
    {
        fprintf(stderr, "Could not bind location: vProjection\n");
        exit(0);
    }
    glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT));
}

glm::vec3 getTrackBallVector(double x, double y)
{
    glm::vec3 p = glm::vec3(2.0 * x / screen_width - 1.0, 2.0 * y / screen_height - 1.0, 0.0); // Normalize to [-1, +1]
    p.y = -p.y;                                                                                // Invert Y since screen coordinate and OpenGL coordinates have different Y directions.

    float mag2 = p.x * p.x + p.y * p.y;
    if (mag2 <= 1.0f)
        p.z = sqrtf(1.0f - mag2);
    else
        p = glm::normalize(p); // Nearest point, close to the sides of the trackball
    return p;
}
