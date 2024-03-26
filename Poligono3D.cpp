#include <iostream>
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <vector>
#include <cmath>


std::vector<std::vector<double>> matrizRotacaoX(double angulo) {
    double c = cos(angulo);
    double s = sin(angulo);
    return {
        {1, 0, 0},
        {0, c, -s},
        {0, s, c}
    };
}

std::vector<std::vector<double>> matrizRotacaoY(double angulo) {
    double c = cos(angulo);
    double s = sin(angulo);
    return {
        {c, 0, s},
        {0, 1, 0},
        {-s, 0, c}
    };
}

std::vector<std::vector<double>> matrizRotacaoZ(double angulo) {
    double c = cos(angulo);
    double s = sin(angulo);
    return {
        {c, -s, 0},
        {s, c, 0},
        {0, 0, 1}
    };
}

struct Vertice {
    double x, y, z;
};
using lista_vertices = std::vector<Vertice>;

struct Aresta {
    int verticeOrigem, verticeDestino;
};
using lista_arestas = std::vector<Aresta>;

struct Cubo {
    double tamanhoLado;
	Vertice posicao;
	Vertice escala;
	double rotacao;
	lista_vertices vertices;
	lista_arestas arestas;
};
Cubo cubo;

Cubo criar_cubo(double px, double py, double pz, double tamanhoLado) {
    Cubo novo_cubo;
    novo_cubo.tamanhoLado = tamanhoLado;
    novo_cubo.posicao = {px, py, pz};
    novo_cubo.escala = {1, 1, 1};
    novo_cubo.rotacao = 0;
    novo_cubo.vertices = {
        {-1, -1, -1}, // 0
        {1, -1, -1},  // 1
        {1, 1, -1},   // 2
        {-1, 1, -1},  // 3
        {-1, -1, 1},  // 4
        {1, -1, 1},   // 5
        {1, 1, 1},    // 6
        {-1, 1, 1}    // 7
    };
    novo_cubo.arestas = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Face frontal
        {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Face traseira
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Arestas laterais
    };
    return novo_cubo;
}

void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Coloca a cor de background para preto e opaco
    glClearDepth(1.0f);                   // Define o buffer de profundidade para o mais distante possível
    glEnable(GL_DEPTH_TEST);   // Habilita o culling de profundidade
    glDepthFunc(GL_LEQUAL);    // Define o tipo de teste de profundidade
}

void desenhar(Cubo cubo) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa o buffer de cor e o de profundidade
    glMatrixMode(GL_MODELVIEW);     //Operar na matriz de ModelView

    // Renderiza um cubo com 6 quads diferentes
    glLoadIdentity();                 // Reseta para a matriz identidade
    glTranslatef(1.5f, 0.0f, -7.0f);
    glBegin(GL_LINES);
    for (const Aresta& aresta : cubo.arestas) {
        const Vertice& origem = cubo.vertices[aresta.verticeOrigem];
        const Vertice& destino = cubo.vertices[aresta.verticeDestino];
        glVertex3d(origem.x, origem.y, origem.z);
        glVertex3d(destino.x, destino.y, destino.z);
    }
    glEnd();
    glutSwapBuffers();
}

void display(void) {
   desenhar(cubo);
}

//Função de redesenhou prioriza o aspecto da projeção
void reshape(GLsizei width, GLsizei height) {  
    if (height == 0) height = 1;                
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();             
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void escalar(Cubo& cubo, double escala_x, double escala_y, double escala_z) {
    lista_vertices vertices_escaladas;

    for (const Vertice& vertice : cubo.vertices) {
        double x = (vertice.x - cubo.posicao.x) * escala_x + cubo.posicao.x;
        double y = (vertice.y - cubo.posicao.y) * escala_y + cubo.posicao.y;
        double z = (vertice.z - cubo.posicao.z) * escala_z + cubo.posicao.z;
        vertices_escaladas.push_back({x, y, z});
    }

    cubo.vertices = vertices_escaladas;
    cubo.escala.x *= escala_x;
    cubo.escala.y *= escala_y;
    cubo.escala.z *= escala_z;
}

void transladar(Cubo& cubo, double tx, double ty, double tz) {
    for (Vertice& vertice : cubo.vertices) {
        vertice.x += tx;
        vertice.y += ty;
        vertice.z += tz;
    }

    cubo.posicao.x += tx;
    cubo.posicao.y += ty;
    cubo.posicao.z += tz;
}

void rotacionar(Cubo& cubo, double angulo, char eixo) {
    std::vector<std::vector<double>> matrizRotacao;

    switch (eixo) {
        case 'x':
            matrizRotacao = matrizRotacaoX(angulo);
            break;
        case 'y':
            matrizRotacao = matrizRotacaoY(angulo);
            break;
        case 'z':
            matrizRotacao = matrizRotacaoZ(angulo);
            break;
    }

    lista_vertices vertices_rotacionadas;

    for (const Vertice& vertice : cubo.vertices) {
        double x = vertice.x - cubo.posicao.x;
        double y = vertice.y - cubo.posicao.y;
        double z = vertice.z - cubo.posicao.z;

        double x_rot = x * matrizRotacao[0][0] + y * matrizRotacao[0][1] + z * matrizRotacao[0][2];
        double y_rot = x * matrizRotacao[1][0] + y * matrizRotacao[1][1] + z * matrizRotacao[1][2];
        double z_rot = x * matrizRotacao[2][0] + y * matrizRotacao[2][1] + z * matrizRotacao[2][2];

        x_rot += cubo.posicao.x;
        y_rot += cubo.posicao.y;
        z_rot += cubo.posicao.z;

        vertices_rotacionadas.push_back({x_rot, y_rot, z_rot});
    }

    cubo.vertices = vertices_rotacionadas;
    cubo.rotacao += angulo;
}

void keyboard(unsigned char key, int x, int y) {
	std::cout << key;
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'c':
		escalar(cubo, 1.1, 1.1, 1.1);
		break;
	case 'v':
		escalar(cubo, 0.9, 0.9, 0.9);
		break;
    case 'x':
        rotacionar(cubo, 3.14159 / 18.0, 'x');
        break;
    case 'y':
        rotacionar(cubo, 3.14159 / 18.0, 'y');
        break;
    case 'z':
        rotacionar(cubo, 3.14159 / 18.0, 'z');
        break;
	}
    glutPostRedisplay();
}

void keyboard_special(int key, int x, int y) {
    
    switch (key) {
        case GLUT_KEY_UP:
            transladar(cubo, 0.0, 0.1, 0.0);
            break;
        case GLUT_KEY_DOWN:
            transladar(cubo, 0.0, -0.1, 0.0);
            break;
        case GLUT_KEY_LEFT:
            transladar(cubo, -0.1, 0.0, 0.0);
            break;
        case GLUT_KEY_RIGHT:
            transladar(cubo, 0.1, 0.0, 0.0);
            break;
    }
    glutPostRedisplay();
}

int delay = 10;
void redraw(int value) {
	glutPostRedisplay();
	glutTimerFunc(delay, redraw, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE);

    glutInitWindowSize(640, 480); 
    glutInitWindowPosition(50, 50); 

    glutCreateWindow("3D Shapes");          
    cubo = criar_cubo(-1, -1, -1, 1);
    glutDisplayFunc(display);     
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboard_special);
    glutTimerFunc(10, redraw, 0);
    initGL();                       
    glutMainLoop();                
    return 0;
}
