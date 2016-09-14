#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common/Angel.h"
#include "shader.h"
#include "common/tulane.h"

#include <stdlib.h>
#include <stdio.h>
#include <fstream>

using namespace Angel;

GLuint program;
GLuint sq_vao;
GLuint PM_location;
GLuint texId;


static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void init(){
  
  GLuint vertex_shader, fragment_shader;
  GLint vpos_location, vtex_location;
  GLuint sq_buffer;
  
  //Do not change
  vec3 square[16] = {vec3( 0.25,  0.75, 0.0),
                     vec3( 0.75,  0.75, 0.0),
                     vec3( 0.25,  0.25, 0.0),
                     vec3( 0.75,  0.25, 0.0),
                     vec3( 1.125,  1.125, 0.0),
                     vec3( 1.125,  -0.125, 0.0),
                     vec3( -0.125,  1.125, 0.0),
                     vec3( -0.125,  -0.125, 0.0),
                     vec3( 0.5,  -0.207107, 0.0),
                     vec3( -0.207107,  0.5, 0.0),
                     vec3( 1.20711,  0.5, 0.0),
                     vec3( 0.5,  1.20711, 0.0),
                     vec3( 0.375,  0.375, 0.0),
                     vec3( 0.375,  0.625, 0.0),
                     vec3( 0.625,  0.375, 0.0),
                     vec3( 0.625,  0.625, 0.0)};

  
  vec2 square_st[16]  = { vec2( 0.0, 1.0),
                          vec2( 0.0, 0.5),
                          vec2( 0.5, 1.0),
                          vec2( 0.5, 0.5),
                          vec2( 0.5, 1.0),
                          vec2( 0.5, 0.5),
                          vec2( 1.0, 1.0),
                          vec2( 1.0, 0.5),
                          vec2( 0.0, 0.5),
                          vec2( 0.0, 0.0),
                          vec2( 0.5, 0.5),
                          vec2( 0.5, 0.0),
                          vec2( 0.5, 0.5),
                          vec2( 0.5, 0.0),
                          vec2( 1.0, 0.5),
                          vec2( 1.0, 0.0)};
  
  
  std::string vshader = shader_path + "vshader_tex.glsl";
  std::string fshader = shader_path + "fshader_tex.glsl";
  
  GLchar* vertex_shader_source = readShaderSource(vshader.c_str());
  GLchar* fragment_shader_source = readShaderSource(fshader.c_str());
  
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, (const GLchar**) &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  check_shader_compilation(vshader, vertex_shader);
  
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, (const GLchar**) &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  check_shader_compilation(fshader, fragment_shader);
  
  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  
  glLinkProgram(program);
  check_program_link(program);
  
  glBindFragDataLocation(program, 0, "fragColor");
  
  vpos_location   = glGetAttribLocation(program, "vPos");
  vtex_location = glGetAttribLocation( program, "vTexCoord" );
  PM_location = glGetUniformLocation( program, "ProjectionModelview" );
  glUniform1i( glGetUniformLocation(program, "tulane_texture"), 0 );
  
  // Create a vertex array object
  glGenVertexArrays( 1, &sq_vao );
  //Set GL state to use vertex array object
  glBindVertexArray( sq_vao );
  
  //Generate buffer to hold our vertex data
  glGenBuffers( 1, &sq_buffer );
  //Set GL state to use this buffer
  glBindBuffer( GL_ARRAY_BUFFER, sq_buffer );
  
  //Create GPU buffer to hold vertices and color
  glBufferData( GL_ARRAY_BUFFER, sizeof(square)+sizeof(square_st), NULL, GL_STATIC_DRAW );
  //First part of array holds vertices
  glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(square), square );
  //Second part of array hold colors (offset by sizeof(triangle))
  glBufferSubData( GL_ARRAY_BUFFER, sizeof(square), sizeof(square_st), square_st );
  
  glEnableVertexAttribArray(vpos_location);
  glEnableVertexAttribArray(vtex_location);
  
  glVertexAttribPointer( vpos_location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
  glVertexAttribPointer( vtex_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(square)) );
  
  glGenTextures(1, &texId);
  glBindTexture( GL_TEXTURE_2D, texId );
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tulane.width, tulane.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tulane.pixel_data);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindVertexArray(0);
  
}


int main(void)
{
  GLFWwindow* window;
  
  glfwSetErrorCallback(error_callback);
  
  if (!glfwInit())
    exit(EXIT_FAILURE);
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  glfwWindowHint(GLFW_SAMPLES, 4);
  
  
  window = glfwCreateWindow(512, 512, "Tulane Puzzle", NULL, NULL);
  if (!window){
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
  glfwSetKeyCallback(window, key_callback);
  
  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
  glfwSwapInterval(1);
  
  init();
  
  glClearColor(1.0, 1.0, 1.0, 1.0);
  
  while (!glfwWindowShouldClose(window)){
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program);
    
    //Do not change
    mat4 P = Ortho2D(-1.0, 1.0, -1.0, 1.0);
  
    glBindTexture( GL_TEXTURE_2D, texId );
    
    glBindVertexArray( sq_vao );

    glUniformMatrix4fv( PM_location, 1, GL_TRUE, P);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUniformMatrix4fv( PM_location, 1, GL_TRUE, P);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

    glUniformMatrix4fv( PM_location, 1, GL_TRUE, P);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

    glUniformMatrix4fv( PM_location, 1, GL_TRUE, P);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glBindVertexArray( 0 );

    glBindTexture( GL_TEXTURE_2D, 0 );


    glfwSwapBuffers(window);
    glfwPollEvents();
    
  }
  
  glfwDestroyWindow(window);
  
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
