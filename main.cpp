#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "headers/cyTriMesh.h"
#include "Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize trimesh
    cyTriMesh trimesh;

    // Load model
    std::string model_path = "models/utah_teapot.obj";
    trimesh.LoadFromFileObj(model_path.c_str());
    const unsigned int noOfVertices = trimesh.NV();
    float vertices[100000];
    int count = 0;
    int maxX = 0, maxY = 0, maxZ = 0;
    for (int i = 0; i < noOfVertices; i++) {
        cyVec3f vertex = trimesh.V(i);
        vertices[i * 3] = vertex[0];
        vertices[i * 3 + 1] = vertex[1];
        vertices[i * 3 + 2] = vertex[2];
        if (maxX < abs(vertex[0])) {
            maxX = abs(vertex[0]);
        }
        if (maxY < abs(vertex[1])) {
            maxY = abs(vertex[1]);
        }
        if (maxZ < abs(vertex[2])) {
            maxZ = abs(vertex[2]);
        }
        
        //std::cout << vertex[0] << ", " << vertex[1] << ", " << vertex[2] << std::endl;
    }

    for (int i = 0; i < noOfVertices; i++) {
        vertices[i * 3] /= maxX*1.5;
        vertices[i * 3 + 1] /= maxY*1.5;
        vertices[i * 3 + 2] /= maxZ*1.5;
        //std::cout << vertices[i * 3] << ", " << vertices[i * 3 + 1] << ", " << vertices[i * 3 + 2] << std::endl;
    }

    Shader shaderBasic = Shader("shaders/vertexShader.vs", "shaders/fragmentShader.fs");

    /*float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };*/

    unsigned int VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    shaderBasic.activate();
    shaderBasic.set4Float("ourColor", 1, 1, 1, 1);

    glm::mat4 transformation(1.0f);

    //transformation = glm::translate(transformation, glm::vec3(0.5f, -0.5f, 0.0f));
    transformation = glm::rotate(transformation, 90.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    shaderBasic.setMat4("transform", transformation);


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Activate Shader
        glBindVertexArray(VAO);
        //glLineWidth(5.0f);
        glPointSize(3.0f);
        glDrawArrays(GL_POINTS, 0, noOfVertices);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}