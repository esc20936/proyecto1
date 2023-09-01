#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <random>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    // Define the center and radius of the bubble
    vec2 center = vec2(0.5, 0.5);
    float radius = 0.5;

    // Calculate the distance from the fragment to the center
    vec2 fragPos = gl_FragCoord.xy / 800.0; // Assuming a resolution of 800x600
    float distance = length(fragPos - center);

    // Define bubble colors
    vec3 bubbleColor = vec3(0.0, 0.5, 1.0); // Bubble color (blue)

    // Add a shimmering effect based on distance and time
    float shimmer = 0.1 * sin(distance * 20.0 + 2.0 * 3.14159265359 * gl_FragCoord.x / 800.0);

    // Combine the bubble color and shimmer effect
    vec3 finalColor = bubbleColor + vec3(shimmer);

    // Set the alpha value based on distance from the center
    float alpha = smoothstep(radius - 0.02, radius + 0.02, distance);

    // Add transparency to the bubble
    alpha *= 0.5; // You can adjust this value for the desired level of transparency

    FragColor = vec4(finalColor, alpha);
}
)";





int main(int argc, char** argv)
{
    // should read how many circles to draw from command line
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <number of circles>" << std::endl;
        return -1;
    }

    // validate the input is a number
    
    if(!isdigit(*argv[1])) {
        std::cout << "Usage: " << argv[0] << " <number of circles>" << std::endl;
        return -1;
    }

    if(atoi(argv[1]) < 1) {
        std::cout << "Usage: " << argv[0] << " <number of circles>" << std::endl;
        return -1;
    }

    int numCircles = atoi(argv[1]);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Fullscreen OpenGL", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const float radius = 0.10f;
    const int segments = 360; // Number of triangle fan segments

    
    

    const int spaceForVertices = 3 * (segments + 2); // (x, y, z) for each vertex

    // Calculate vertices for the circle
    float* vertices = new float[spaceForVertices * numCircles]; // (x, y, z) for each vertex

    for (int circle = 0; circle < numCircles; circle++) {
        float theta = 1.50f * 3.1415926f * float(circle) / float(numCircles);
        int offset = spaceForVertices * circle; // Start from index 3

        // float centerX = 
        // float centerY = 2.0f;
        // float centerX should be a random number between -1.0 and 1.0
        // float centerY should be a random number between -1.0 and 1.0
        float centerX = 2.0f * (float)rand() / (float)RAND_MAX - 1.0f;
        float centerY = 2.0f * (float)rand() / (float)RAND_MAX - 1.0f;

        vertices[offset] = centerX;
        vertices[offset + 1] = centerY;
        vertices[offset + 2] = 0.0f;

        for (int i = 0; i <= segments; ++i) {
            float theta = 2.0f * 3.1415926f * float(i) / float(segments);
            int offset = spaceForVertices * circle + 3 * (i + 1); // Start from index 3

            vertices[offset] = centerX + radius * cos(theta);
            vertices[offset + 1] = centerY + radius * sin(theta);
            vertices[offset + 2] = 0.0f;
        }
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * spaceForVertices * numCircles, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        for (int circle = 0; circle < numCircles; circle++)
        {
            glDrawArrays(GL_TRIANGLE_FAN, circle * (segments + 2), segments + 2);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    delete[] vertices;

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
